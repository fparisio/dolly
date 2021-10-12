//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "MFrontStressBase.h"
#include "Factory.h"
#include "MooseMesh.h"
#include "RankTwoTensor.h"
#include "RankFourTensor.h"

InputParameters
MFrontStressBase::validParams()
{
  InputParameters params = ComputeStressBase::validParams();
  params.addClassDescription(
      "Base class for coupling material to use mfront behaviour models in MOOSE");
  params.addRequiredParam<std::string>("mfront_lib_name",
                                       "The name (full path) of the m_front library.");
  params.addRequiredParam<Real>("young_modulus", "The Young modulus.");
  params.addRequiredParam<Real>("poisson_ratio", "The Poisson ratio.");
  return params;
}

MFrontStressBase::MFrontStressBase(const InputParameters & parameters)
  : ComputeStressBase(parameters),
    _mfront_lib_name(getParam<std::string>("mfront_lib_name")),
    _young_modulus(getParam<Real>("young_modulus")),
    _poisson_ratio(getParam<Real>("poisson_ratio")),
    _total_strain(getMaterialProperty<RankTwoTensor>(_base_name + "total_strain")),
    _total_strain_old(getMaterialPropertyOld<RankTwoTensor>(_base_name + "total_strain")),
    _eq_stress(declareProperty<Real>("equivalent_stress")),
    _eq_strain(declareProperty<Real>("equivalent_strain"))
{
}

void
MFrontStressBase::computeProperties()
{
  // initialize the behaviour data on the current element
  auto bd = mgis::behaviour::BehaviourData{_b};
  // loop @ quadrature points
  for (_qp = 0; _qp < _qrule->n_points(); ++_qp)
  {
    // fill in the behaviour data
    bd.dt = _dt;
    bd.rdt = 1.0;
    bd.K[0] = 4.0; // require consistent tangent operator
    // set material properties
    setMaterialProperties(bd);
    // set internal state variables
    setInternalStateVariables(bd);
    // set external state variables
    setExternalStateVariables(bd);
    // set gradients
    setGradients(bd);
    // set thermodynamic forces
    setThermodynamicForces(bd);
    // initialize the material data view
    auto mv = mgis::behaviour::make_view(bd);
    // do the actual integration
    auto const status = mgis::behaviour::integrate(mv, _b);
    // here I do not know what is best
    // mfront enables substepping - therefore it would
    // be OK to get out of the current step and cut the next
    // this implementation does not do that
    if (status != 1)
      throw MooseException(
          name(), ": MFront: integration failed with status ", std::to_string(status));
    // mooseError(name(), ": MFront: integration failed with status ", std::to_string(status));
    // update the new state
    updateStateFromMFront(bd);
    // compute the Jacobian contribution
    // I assumed it is done in the same way for all mfront models
    if (_fe_problem.currentlyComputingJacobian())
      computeJacobianFromMFront(bd);
    // add in extra stress - since we do not call
    // computeQpProperties in the base material
    _stress[_qp] += _extra_stress[_qp];
    // compute additional materials
    _eq_stress[_qp] = std::sqrt(3.0 / 2.0) * _stress[_qp].deviatoric().L2norm();
    _eq_strain[_qp] = std::sqrt(2.0 / 3.0) * _total_strain[_qp].deviatoric().L2norm();
  }
}

void
MFrontStressBase::setGradients(mgis::behaviour::BehaviourData & bd)
{
  bd.s0.gradients = {_total_strain_old[_qp](0, 0),
                     _total_strain_old[_qp](1, 1),
                     _total_strain_old[_qp](2, 2),
                     _total_strain_old[_qp](0, 1),
                     _total_strain_old[_qp](0, 2),
                     _total_strain_old[_qp](1, 2)};
  bd.s1.gradients = {_total_strain[_qp](0, 0),
                     _total_strain[_qp](1, 1),
                     _total_strain[_qp](2, 2),
                     _total_strain[_qp](0, 1),
                     _total_strain[_qp](0, 2),
                     _total_strain[_qp](1, 2)};
}

void
MFrontStressBase::computeJacobianFromMFront(mgis::behaviour::BehaviourData & bd)
{
  _Jacobian_mult[_qp].fillSymmetric21FromInputVector(std::array<Real, 21>{{
      bd.K[0],        // C1111
      bd.K[1],        // C1122
      bd.K[2],        // C1133
      bd.K[3],        // C1123
      bd.K[4],        // C1113
      bd.K[5],        // C1112
      bd.K[7],        // C2222
      bd.K[8],        // C2233
      bd.K[9],        // C2223
      bd.K[10],       // C2213
      bd.K[11],       // C2212
      bd.K[14],       // C3333
      bd.K[15],       // C3323
      bd.K[16],       // C3313
      bd.K[17],       // C3312
      0.5 * bd.K[21], // C2323
      0.5 * bd.K[22], // C2313
      0.5 * bd.K[23], // C2312
      0.5 * bd.K[28], // C1313
      0.5 * bd.K[29], // C1312
      0.5 * bd.K[35]  // C1212
  }});
}
