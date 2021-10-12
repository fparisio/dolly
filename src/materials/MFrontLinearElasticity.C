//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "MFrontLinearElasticity.h"

registerMooseObject("TensorMechanicsApp", MFrontLinearElasticity);

InputParameters
MFrontLinearElasticity::validParams()
{
  InputParameters params = MFrontStressBase::validParams();
  params.addClassDescription("mfront model - linear elasticity.");
  return params;
}

MFrontLinearElasticity::MFrontLinearElasticity(const InputParameters & parameters)
  : MFrontStressBase(parameters)
{
  std::string behaviour_name = "LinearElasticity";
  _b = mgis::behaviour::load(
      _mfront_lib_name, behaviour_name, mgis::behaviour::Hypothesis::TRIDIMENSIONAL);
}

void
MFrontLinearElasticity::setMaterialProperties(mgis::behaviour::BehaviourData & bd)
{
  bd.s1.material_properties = {_young_modulus, _poisson_ratio};
}

void
MFrontLinearElasticity::updateStateFromMFront(mgis::behaviour::BehaviourData & bd)
{
  // update the new stress state
  _stress[_qp] = RankTwoTensor(bd.s1.thermodynamic_forces[0],
                               bd.s1.thermodynamic_forces[1],
                               bd.s1.thermodynamic_forces[2],
                               bd.s1.thermodynamic_forces[5],
                               bd.s1.thermodynamic_forces[4],
                               bd.s1.thermodynamic_forces[3]);
}
