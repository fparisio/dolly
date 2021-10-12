//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "libmesh/utility.h"

#include "MFrontRambergOsgoodNonLinearElasticity.h"

registerMooseObject("TensorMechanicsApp", MFrontRambergOsgoodNonLinearElasticity);

InputParameters
MFrontRambergOsgoodNonLinearElasticity::validParams()
{
  InputParameters params = MFrontStressBase::validParams();
  params.addClassDescription("mfront model - Ramberg-Osgood non linear elasticity.");
  params.addRequiredParam<Real>("n", "The plastic exponent.");
  params.addRequiredParam<Real>("alpha", "The alpha parameter.");
  params.addRequiredParam<Real>("yield_strength", "The yield strength");
  return params;
}

MFrontRambergOsgoodNonLinearElasticity::MFrontRambergOsgoodNonLinearElasticity(
    const InputParameters & parameters)
  : MFrontStressBase(parameters),
    _n(getParam<Real>("n")),
    _alpha(getParam<Real>("alpha")),
    _yield_strength(getParam<Real>("yield_strength")),
    _von_mises_stress(declareProperty<Real>("mfront_von_mises_stress")),
    _von_mises_stress_old(getMaterialPropertyOld<Real>("mfront_von_mises_stress"))
{
  std::string behaviour_name = "RambergOsgoodNonLinearElasticity";
  _b = mgis::behaviour::load(
      _mfront_lib_name, behaviour_name, mgis::behaviour::Hypothesis::TRIDIMENSIONAL);
}

void
MFrontRambergOsgoodNonLinearElasticity::initQpStatefulProperties()
{
  _von_mises_stress[_qp] = 0.0;
  ComputeStressBase::initQpStatefulProperties();
}

void
MFrontRambergOsgoodNonLinearElasticity::setMaterialProperties(mgis::behaviour::BehaviourData & bd)
{
  bd.s1.material_properties = {_young_modulus, _poisson_ratio, _n, _alpha, _yield_strength};
}

void
MFrontRambergOsgoodNonLinearElasticity::setInternalStateVariables(
    mgis::behaviour::BehaviourData & bd)
{
  bd.s0.internal_state_variables = {_von_mises_stress[_qp]};
}

void
MFrontRambergOsgoodNonLinearElasticity::updateStateFromMFront(mgis::behaviour::BehaviourData & bd)
{
  // update the stress tensor
  _stress[_qp] = RankTwoTensor(bd.s1.thermodynamic_forces[0],
                               bd.s1.thermodynamic_forces[1],
                               bd.s1.thermodynamic_forces[2],
                               bd.s1.thermodynamic_forces[5],
                               bd.s1.thermodynamic_forces[4],
                               bd.s1.thermodynamic_forces[3]);
  // update the internal state variables
  _von_mises_stress[_qp] = bd.s1.internal_state_variables[0];
}
