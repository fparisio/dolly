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
 * MFront Implicit Norton creep material
 */
class MFrontImplicitNorton : public MFrontStressBase
{
public:
  static InputParameters validParams();

  MFrontImplicitNorton(const InputParameters & parameters);
  virtual ~MFrontImplicitNorton() {}

protected:
  virtual void initQpStatefulProperties() override;

  virtual void setMaterialProperties(mgis::behaviour::BehaviourData & bd) override;
  virtual void setInternalStateVariables(mgis::behaviour::BehaviourData & bd) override;
  virtual void setExternalStateVariables(mgis::behaviour::BehaviourData & bd) override;
  virtual void setThermodynamicForces(mgis::behaviour::BehaviourData & /*bd*/) override {}
  virtual void updateStateFromMFront(mgis::behaviour::BehaviourData & bd) override;

  void computeEffectiveViscosity();

  // coupled variable temperature
  const VariableValue & _temp;
  const VariableValue & _temp_old;
  // Norton creep
  const Real & _Q_act;
  const Real & _A_creep;
  const Real & _n_creep;
  const Real & _gas_constant;
  // mfront internal state variables
  MaterialProperty<Real> & _eqvs;
  const MaterialProperty<Real> & _eqvs_old;
  const MaterialProperty<RankTwoTensor> & _elastic_strain_old;
  // additional material output
  MaterialProperty<RankTwoTensor> & _viscous_strain;
  MaterialProperty<Real> & _effective_viscosity;
};
