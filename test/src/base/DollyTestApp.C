//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
#include "DollyTestApp.h"
#include "DollyApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "MooseSyntax.h"
#include "ModulesApp.h"

InputParameters
DollyTestApp::validParams()
{
  InputParameters params = DollyApp::validParams();
  return params;
}

DollyTestApp::DollyTestApp(InputParameters parameters) : MooseApp(parameters)
{
  DollyTestApp::registerAll(
      _factory, _action_factory, _syntax, getParam<bool>("allow_test_objects"));
}

DollyTestApp::~DollyTestApp() {}

void
DollyTestApp::registerAll(Factory & f, ActionFactory & af, Syntax & s, bool use_test_objs)
{
  DollyApp::registerAll(f, af, s);
  if (use_test_objs)
  {
    Registry::registerObjectsTo(f, {"DollyTestApp"});
    Registry::registerActionsTo(af, {"DollyTestApp"});
  }
}

void
DollyTestApp::registerApps()
{
  registerApp(DollyApp);
  registerApp(DollyTestApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
// External entry point for dynamic application loading
extern "C" void
DollyTestApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  DollyTestApp::registerAll(f, af, s);
}
extern "C" void
DollyTestApp__registerApps()
{
  DollyTestApp::registerApps();
}
