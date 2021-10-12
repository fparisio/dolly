//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "MFrontImplicitNorton.h"

registerMooseObject("TensorMechanicsApp", MFrontImplicitNorton);

InputParameters
MFrontImplicitNorton::validParams()
{
  InputParameters params = MFrontStressBase::validParams();
  params.addClassDescription("mfront model - implicit Norton creep.");
  params.addCoupledVar("temperature", "The temperature variable.");
  params.addRequiredParam<Real>("Q_act", "The activation enthalpy.");
  params.addRequiredParam<Real>("A_creep", "The pre-exponential factor.");
  params.addRequiredParam<Real>("n_creep", "The exponent.");
  params.addParam<Real>("gas_constant", 8.3144621, "The universal gas constant");
  return params;
}

MFrontImplicitNorton::MFrontImplicitNorton(const InputParameters & parameters)
  : MFrontStressBase(parameters),
    _temp(isCoupled("temperature") ? coupledValue("temperature") : _zero),
    _temp_old((isCoupled("temperature") && _is_transient) ? coupledValueOld("temperature") : _zero),
    _Q_act(getParam<Real>("Q_act")),
    _A_creep(getParam<Real>("A_creep")),
    _n_creep(getParam<Real>("n_creep")),
    _gas_constant(getParam<Real>("gas_constant")),
    _eqvs(declareProperty<Real>("eq_viscous_strain")),
    _eqvs_old(getMaterialPropertyOld<Real>("eq_viscous_strain")),
    _elastic_strain_old(getMaterialPropertyOld<RankTwoTensor>(_base_name + "elastic_strain")),
    _viscous_strain(declareProperty<RankTwoTensor>("viscous_strain")),
    _effective_viscosity(declareProperty<Real>("effective_viscosity"))
{
  // load bejaviour object
  std::string behaviour_name = "ImplicitNorton";
  _b = mgis::behaviour::load(
      _mfront_lib_name, behaviour_name, mgis::behaviour::Hypothesis::TRIDIMENSIONAL);
}

void
MFrontImplicitNorton::initQpStatefulProperties()
{
  _eqvs[_qp] = 0.0;
  MFrontStressBase::initQpStatefulProperties();
}

void
MFrontImplicitNorton::setMaterialProperties(mgis::behaviour::BehaviourData & bd)
{
  bd.s1.material_properties = {
      _young_modulus, _poisson_ratio, _Q_act, _A_creep, _n_creep, _gas_constant};
}

void
MFrontImplicitNorton::setInternalStateVariables(mgis::behaviour::BehaviourData & bd)
{
  bd.s0.internal_state_variables = {_elastic_strain_old[_qp](0, 0),
                                    _elastic_strain_old[_qp](1, 1),
                                    _elastic_strain_old[_qp](2, 2),
                                    _elastic_strain_old[_qp](0, 1),
                                    _elastic_strain_old[_qp](0, 2),
                                    _elastic_strain_old[_qp](1, 2),
                                    _eqvs_old[_qp]};
}

void
MFrontImplicitNorton::setExternalStateVariables(mgis::behaviour::BehaviourData & bd)
{
  bd.s0.external_state_variables = {_temp_old[_qp]};
  bd.s1.external_state_variables = {_temp[_qp]};
}

void
MFrontImplicitNorton::updateStateFromMFront(mgis::behaviour::BehaviourData & bd)
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
  _viscous_strain[_qp] = _total_strain[_qp] - _elastic_strain[_qp];
  computeEffectiveViscosity();
}

void
MFrontImplicitNorton::computeEffectiveViscosity()
{
  Real G = _young_modulus / (2.0 * (1 - _poisson_ratio));
  RankTwoTensor tau = 2.0 * G * _elastic_strain[_qp].deviatoric();
  Real s_II = std::sqrt(1.5) * tau.L2norm();
  Real RT = _gas_constant * _temp[_qp];
  Real Q = (RT > 0) ? _A_creep * std::exp(-_Q_act / RT) : _A_creep;
  Real creep_rate = 2.0 / 3.0 * Q * std::pow(s_II, _n_creep);
  _effective_viscosity[_qp] = (creep_rate != 0.0) ? s_II / (3.0 * creep_rate) : 0.0;
}
