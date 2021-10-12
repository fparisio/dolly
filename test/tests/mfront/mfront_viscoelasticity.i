# benchmark: quasi-static stress build-up for a Maxwell rheology
# set up from Gerya and Yuen[2007] first example
# the original set up requires an incompressible material
# here the poisson ratio is set close to 0.5 to fake incompressibility
# analytical solution:
# sigma = 2 * eps_dot * eta * (1 - exp(-G*t/eta))

[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 2
  ny = 2
  xmin = 0
  xmax = 1
  ymin = 0
  ymax = 1
[]

[GlobalParams]
  displacements = 'disp_x disp_y'
[]

[Modules/TensorMechanics/Master]
  [all]
    add_variables = true
    strain = SMALL
    incremental = true
  []
[]

[AuxVariables]
  [./Se]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./E_eqv]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./E_eqv_creep]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./eta_e]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[AuxKernels]
  [./Se_aux]
    type = MaterialRealAux
    variable = Se
    property = equivalent_stress
    execute_on = 'INITIAL TIMESTEP_END'
  [../]
  [./E_eqv_aux]
    type = MaterialRealAux
    variable = E_eqv
    property = equivalent_strain
    execute_on = 'INITIAL TIMESTEP_END'
  [../]
  [./E_eqv_creep_aux]
    type = MaterialRealAux
    variable = E_eqv_creep
    property = eq_viscous_strain
    execute_on = 'INITIAL TIMESTEP_END'
  [../]
  [./eta_e_aux]
    type = MaterialRealAux
    variable = eta_e
    property = effective_viscosity
    execute_on = 'TIMESTEP_END'
  [../]
[]

[BCs]
  [./no_ux]
    type = DirichletBC
    variable = disp_x
    boundary = left
    value = 0.0
  [../]
  [./ux_right]
    type = PresetVelocity
    variable = disp_x
    boundary = right
    velocity = -1.0e-14
  [../]
  [./no_uy]
    type = DirichletBC
    variable = disp_y
    boundary = top
    value = 0.0
  [../]
  [./uy_bottom]
    type = PresetVelocity
    variable = disp_y
    boundary = bottom
    velocity = -1.0e-14
  [../]
[]

[Materials]
  [./visco_elastic]
    type = MFrontImplicitNorton
    mfront_lib_name = '/home/cacace/projects/dolly/plugins/mfront/src/libBehaviour.so'
    # bulk_modulus = 1.0e+12
    # shear_modulus = 1.0e+10
    young_modulus = 2.99e10
    poisson_ratio = 0.495
    A_creep = 5.0e-23
    n_creep = 1.0
    Q_act = 0.0
  [../]
[]

[Postprocessors]
  [./S]
    type = ElementAverageValue
    variable = Se
    outputs = gnuplot
  [../]
  [./Eeqv]
    type = ElementAverageValue
    variable = E_eqv
    outputs = gnuplot
  [../]
  [./Eveqv]
    type = ElementAverageValue
    variable = E_eqv_creep
    outputs = gnuplot
  [../]
[]

[Preconditioning]
  active = 'mine'
  [./precond]
    type = SMP
    full = true
    petsc_options = '-snes_ksp_ew'
    petsc_options_iname = '-ksp_type -pc_type -snes_atol -snes_rtol -snes_max_it -ksp_max_it -sub_pc_type -sub_pc_factor_shift_type'
    petsc_options_value = 'gmres asm 1e-15 1e-10 20 50 ilu NONZERO'
  [../]
  [./mine]
    type = SMP
    full = true
    petsc_options = '-snes_ksp_ew'
    petsc_options_iname = '-snes_atol -snes_rtol -snes_max_it
                           -ksp_type -ksp_max_it
                           -pc_type
                           -sub_pc_type -sub_pc_factor_shift_type'
    petsc_options_value = '1e-15 1e-10 20
                          fgmres 1000
                          asm ilu NONZERO'
  [../]
[]

[Executioner]
  type = Transient
  solve_type = 'NEWTON'
  start_time = 0.0
  end_time = 3.1536e+13
  num_steps = 100 #5000
  automatic_scaling = true
  compute_scaling_once = false
[]

[Outputs]
  file_base = mfront_viscoelasticity
  execute_on = 'INITIAL TIMESTEP_END'
  print_linear_residuals = false
  perf_graph = true
  # gnuplot = true
  exodus = true
[]
