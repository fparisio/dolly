//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "MFrontIsotropicLinearHardeningPlasticity.h"

registerMooseObject("TensorMechanicsApp", MFrontIsotropicLinearHardeningPlasticity);

InputParameters
MFrontIsotropicLinearHardeningPlasticity::validParams()
{
  InputParameters params = MFrontStressBase::validParams();
  params.addClassDescription(
      "mfront model - isotropic plasticity with linear hardening - implicit implementation.");
  params.addRequiredParam<Real>("hardening_slope", "The hardening slope.");
  params.addRequiredParam<Real>("yield_strength", "The yield strength.");
  return params;
}

MFrontIsotropicLinearHardeningPlasticity::MFrontIsotropicLinearHardeningPlasticity(
    const InputParameters & parameters)
  : MFrontStressBase(parameters),
    _hardening_slope(getParam<Real>("hardening_slope")),
    _yield_strength(getParam<Real>("yield_strength")),
    _eqps(declareProperty<Real>("eq_plastic_strain")),
    _eqps_old(getMaterialPropertyOld<Real>("eq_plastic_strain")),
    _elastic_strain_old(getMaterialPropertyOld<RankTwoTensor>(_base_name + "elastic_strain"))
{
  std::string behaviour_name = "IsotropicLinearHardeningPlasticity";
  _b = mgis::behaviour::load(
      _mfront_lib_name, behaviour_name, mgis::behaviour::Hypothesis::TRIDIMENSIONAL);
}

void
MFrontIsotropicLinearHardeningPlasticity::initQpStatefulProperties()
{
  _eqps[_qp] = 0.0;
  ComputeStressBase::initQpStatefulProperties();
}

void
MFrontIsotropicLinearHardeningPlasticity::setMaterialProperties(mgis::behaviour::BehaviourData & bd)
{
  bd.s1.material_properties = {_young_modulus, _poisson_ratio, _hardening_slope, _yield_strength};
}

void
MFrontIsotropicLinearHardeningPlasticity::setInternalStateVariables(
    mgis::behaviour::BehaviourData & bd)
{
  bd.s0.internal_state_variables = {_elastic_strain_old[_qp](0, 0),
                                    _elastic_strain_old[_qp](1, 1),
                                    _elastic_strain_old[_qp](2, 2),
                                    _elastic_strain_old[_qp](0, 1),
                                    _elastic_strain_old[_qp](0, 2),
                                    _elastic_strain_old[_qp](1, 2),
                                    _eqps_old[_qp]};
}

void
MFrontIsotropicLinearHardeningPlasticity::updateStateFromMFront(mgis::behaviour::BehaviourData & bd)
{
  // update the stress tensor
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
  _eqps[_qp] = bd.s1.internal_state_variables[6];
}
