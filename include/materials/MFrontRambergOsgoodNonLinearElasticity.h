//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "MFrontStressBase.h"

/**
 * MFront Ramberg-Osgood non linear elastic material behaviour
 */
class MFrontRambergOsgoodNonLinearElasticity : public MFrontStressBase
{
public:
  static InputParameters validParams();
  MFrontRambergOsgoodNonLinearElasticity(const InputParameters & parameters);
  virtual ~MFrontRambergOsgoodNonLinearElasticity() {}

protected:
  void initQpStatefulProperties() override;

  virtual void setMaterialProperties(mgis::behaviour::BehaviourData & bd) override;
  virtual void setInternalStateVariables(mgis::behaviour::BehaviourData & bd) override;
  virtual void setExternalStateVariables(mgis::behaviour::BehaviourData & /*bd*/) override {}
  virtual void setThermodynamicForces(mgis::behaviour::BehaviourData & /*bd*/) override {}
  virtual void updateStateFromMFront(mgis::behaviour::BehaviourData & bd) override;
  // non linear elasticity
  const Real _n;
  const Real _alpha;
  const Real _yield_strength;
  // mfront internal state variable
  MaterialProperty<Real> & _von_mises_stress;
  const MaterialProperty<Real> & _von_mises_stress_old;
};
