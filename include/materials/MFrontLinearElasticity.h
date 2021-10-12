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
 * MFront Linear Elastic material
 */
class MFrontLinearElasticity : public MFrontStressBase
{
public:
  static InputParameters validParams();

  MFrontLinearElasticity(const InputParameters & parameters);
  virtual ~MFrontLinearElasticity() {}

protected:
  virtual void setMaterialProperties(mgis::behaviour::BehaviourData & bd) override;
  virtual void setInternalStateVariables(mgis::behaviour::BehaviourData & /*bd*/) override {}
  virtual void setExternalStateVariables(mgis::behaviour::BehaviourData & /*bd*/) override {}
  virtual void setThermodynamicForces(mgis::behaviour::BehaviourData & /*bd*/) override {}
  virtual void updateStateFromMFront(mgis::behaviour::BehaviourData & bd) override;
};
