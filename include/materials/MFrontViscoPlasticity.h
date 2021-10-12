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
 * MFront visco-elastic Maxwell and non associative Drucker-Prager (perfect plasticity) material
 * behaviour
 */
class MFrontViscoPlasticity : public MFrontStressBase
{
public:
  static InputParameters validParams();
  MFrontViscoPlasticity(const InputParameters & parameters);
  virtual ~MFrontViscoPlasticity() {}

protected:
  void initQpStatefulProperties() override;

  virtual void setMaterialProperties(mgis::behaviour::BehaviourData & bd) override;
  virtual void setInternalStateVariables(mgis::behaviour::BehaviourData & bd) override;
  virtual void setExternalStateVariables(mgis::behaviour::BehaviourData & bd) override;
  virtual void setThermodynamicForces(mgis::behaviour::BehaviourData & /*bd*/) override {}
  virtual void updateStateFromMFront(mgis::behaviour::BehaviourData & bd) override;
  void computeEffectiveViscosity();
  // coupled variable temperature
  const VariableValue & _temp;
  const VariableValue & _temp_old;
  // creep
  const Real & _Q_act;
  const Real & _A_creep;
  const Real & _n_creep;
  const Real & _gas_constant;
  // plasticity
  const Real _coh;
  Real _phi_angle;
  Real _psi_angle;
  const bool _convert_to_radians;
  // mfront internal state variables
  MaterialProperty<Real> & _eqvs;
  const MaterialProperty<Real> & _eqvs_old;
  MaterialProperty<Real> & _eqps;
  const MaterialProperty<Real> & _eqps_old;
  const MaterialProperty<RankTwoTensor> & _elastic_strain_old;
  // mfront materials
  MaterialProperty<Real> & _effective_viscosity;
  MaterialProperty<RankTwoTensor> & _inelastic_strain;
};
