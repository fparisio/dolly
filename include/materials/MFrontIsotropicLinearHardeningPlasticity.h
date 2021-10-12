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
 * MFront Isotropic Linear Hardening Plastic material
 */
class MFrontIsotropicLinearHardeningPlasticity : public MFrontStressBase
{
public:
  static InputParameters validParams();

  MFrontIsotropicLinearHardeningPlasticity(const InputParameters & parameters);
  virtual ~MFrontIsotropicLinearHardeningPlasticity() {}

protected:
  void initQpStatefulProperties() override;

  virtual void setMaterialProperties(mgis::behaviour::BehaviourData & bd) override;
  virtual void setInternalStateVariables(mgis::behaviour::BehaviourData & bd) override;
  virtual void setExternalStateVariables(mgis::behaviour::BehaviourData & /*bd*/) override {}
  virtual void setThermodynamicForces(mgis::behaviour::BehaviourData & /*bd*/) override {}
  virtual void updateStateFromMFront(mgis::behaviour::BehaviourData & bd) override;
  // plasticity
  const Real _hardening_slope;
  const Real _yield_strength;
  // mfront internal state variables
  MaterialProperty<Real> & _eqps;
  const MaterialProperty<Real> & _eqps_old;
  const MaterialProperty<RankTwoTensor> & _elastic_strain_old;
};
