//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "MFrontViscoPlasticity.h"

registerMooseObject("TensorMechanicsApp", MFrontViscoPlasticity);

InputParameters
MFrontViscoPlasticity::validParams()
{
  InputParameters params = MFrontStressBase::validParams();
  params.addClassDescription(
      "mfront model - viscoelastis Maxwell and non associative DP perfect plastic material.");
  params.addCoupledVar("temperature", "The temperature variable.");
  params.addRequiredParam<Real>("Q_act", "The activation enthalpy.");
  params.addRequiredParam<Real>("A_creep", "The pre-exponential factor.");
  params.addRequiredParam<Real>("n_creep", "The exponent.");
  params.addParam<Real>("gas_constant", 8.3144621, "The universal gas constant");
  params.addRequiredParam<Real>("coh", "The cohesion.");
  params.addRequiredParam<Real>("phi_angle", "The friction angle.");
  params.addRequiredParam<Real>("psi_angle", "The dilation angle.");
  params.addParam<bool>(
      "convert_to_radians", true, "Whether to convert the friction and dilation angle to radians");
  return params;
}

MFrontViscoPlasticity::MFrontViscoPlasticity(const InputParameters & parameters)
  : MFrontStressBase(parameters),
    _temp(isCoupled("temperature") ? coupledValue("temperature") : _zero),
    _temp_old(isCoupled("temperature") ? coupledValueOld("temperature") : _zero),
    _Q_act(getParam<Real>("Q_act")),
    _A_creep(getParam<Real>("A_creep")),
    _n_creep(getParam<Real>("n_creep")),
    _gas_constant(getParam<Real>("gas_constant")),
    _coh(getParam<Real>("coh")),
    _phi_angle(getParam<Real>("phi_angle")),
    _psi_angle(getParam<Real>("psi_angle")),
    _convert_to_radians(getParam<bool>("convert_to_radians")),
    _eqvs(declareProperty<Real>("eq_viscous_strain")),
    _eqvs_old(getMaterialPropertyOld<Real>("eq_viscous_strain")),
    _eqps(declareProperty<Real>("eq_plastic_strain")),
    _eqps_old(getMaterialPropertyOld<Real>("eq_plastic_strain")),
    _elastic_strain_old(getMaterialPropertyOld<RankTwoTensor>(_base_name + "elastic_strain")),
    _effective_viscosity(declareProperty<Real>("effective_viscosity")),
    _inelastic_strain(declareProperty<RankTwoTensor>("inelastic_strain"))
{
  // load bejaviour object
  std::string behaviour_name = "ViscoPlasticity";
  _b = mgis::behaviour::load(
      _mfront_lib_name, behaviour_name, mgis::behaviour::Hypothesis::TRIDIMENSIONAL);
  if (_convert_to_radians)
  {
    _phi_angle *= libMesh::pi / 180;
    _psi_angle *= libMesh::pi / 180;
  }
  if (_phi_angle < 0.0 || _psi_angle < 0.0 || _phi_angle > libMesh::pi / 2.0 ||
      _psi_angle > libMesh::pi / 2.0)
    mooseError(name(), ": both friction and dilation angles must lie in [0, Pi/2]... aborting.");
  if (_phi_angle < _psi_angle)
    mooseError(name(), ": friction angle should not be smaller than dilation angle ... aborting.");
  if (_coh < 0.0)
    mooseError(name(), ": cohesion should not be negative ... aborting.");
}

void
MFrontViscoPlasticity::initQpStatefulProperties()
{
  _eqvs[_qp] = 0.0;
  _eqps[_qp] = 0.0;
  MFrontStressBase::initQpStatefulProperties();
}

void
MFrontViscoPlasticity::setMaterialProperties(mgis::behaviour::BehaviourData & bd)
{
  bd.s1.material_properties = {_young_modulus,
                               _poisson_ratio,
                               _Q_act,
                               _A_creep,
                               _n_creep,
                               _gas_constant,
                               _coh,
                               _phi_angle,
                               _psi_angle};
}

void
MFrontViscoPlasticity::setInternalStateVariables(mgis::behaviour::BehaviourData & bd)
{
  bd.s0.internal_state_variables = {_elastic_strain_old[_qp](0, 0),
                                    _elastic_strain_old[_qp](1, 1),
                                    _elastic_strain_old[_qp](2, 2),
                                    _elastic_strain_old[_qp](0, 1),
                                    _elastic_strain_old[_qp](0, 2),
                                    _elastic_strain_old[_qp](1, 2),
                                    _eqvs_old[_qp],
                                    _eqps_old[_qp]};
}

void
MFrontViscoPlasticity::setExternalStateVariables(mgis::behaviour::BehaviourData & bd)
{
  bd.s0.external_state_variables = {_temp_old[_qp]};
  bd.s1.external_state_variables = {_temp[_qp]};
}

void
MFrontViscoPlasticity::updateStateFromMFront(mgis::behaviour::BehaviourData & bd)
{
  // update the new stress state
  _stress[_qp] = RankTwoTensor(bd.s1.thermodynamic_forces[0],
                               bd.s1.thermodynamic_forces[1],
                               bd.s1.thermodynamic_forces[2],
                               bd.s1.thermodynamic_forces[5],
                               bd.s1.thermodynamic_forces[4],
                               bd.s1.thermodynamic_forces[3]);
  // update the internal state variables
  _elastic_strain[_qp] = RankTwoTensor(bd.s1.internal_state_variables[0],
                                       bd.s1.internal_state_variables[1],
                                       bd.s1.internal_state_variables[2],
                                       bd.s1.internal_state_variables[5],
                                       bd.s1.internal_state_variables[4],
                                       bd.s1.internal_state_variables[3]);
  _eqvs[_qp] = bd.s1.internal_state_variables[6];
  _eqps[_qp] = bd.s1.internal_state_variables[7];
  _inelastic_strain[_qp] = _total_strain[_qp] - _elastic_strain[_qp];
  computeEffectiveViscosity();
}

void
MFrontViscoPlasticity::computeEffectiveViscosity()
{
  Real G = _young_modulus / (2.0 * (1 - _poisson_ratio));
  RankTwoTensor tau = 2.0 * G * _elastic_strain[_qp].deviatoric();
  Real s_II = std::sqrt(1.5) * tau.L2norm();
  Real RT = 8.3144598 * _temp[_qp];
  Real Q = (RT > 0) ? _A_creep * std::exp(-_Q_act / RT) : _A_creep;
  Real creep_rate = 2.0 / 3.0 * Q * std::pow(s_II, _n_creep);
  _effective_viscosity[_qp] = (creep_rate != 0.0) ? s_II / (3.0 * creep_rate) : 0.0;
}
