#include "DollyApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
#include "MooseSyntax.h"

InputParameters
DollyApp::validParams()
{
  InputParameters params = MooseApp::validParams();

  // Do not use legacy material output, i.e., output properties on INITIAL as well as TIMESTEP_END
  params.set<bool>("use_legacy_material_output") = false;

  return params;
}

DollyApp::DollyApp(InputParameters parameters) : MooseApp(parameters)
{
  DollyApp::registerAll(_factory, _action_factory, _syntax);
}

DollyApp::~DollyApp() {}

void
DollyApp::registerAll(Factory & f, ActionFactory & af, Syntax & syntax)
{
  ModulesApp::registerAll(f, af, syntax);
  Registry::registerObjectsTo(f, {"DollyApp"});
  Registry::registerActionsTo(af, {"DollyApp"});

  /* register custom execute flags, action syntax, etc. here */
}

void
DollyApp::registerApps()
{
  registerApp(DollyApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
extern "C" void
DollyApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  DollyApp::registerAll(f, af, s);
}
extern "C" void
DollyApp__registerApps()
{
  DollyApp::registerApps();
}
