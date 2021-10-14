# benchmark: quasi-static stress build-up for a Maxwell rheology
# it is the same of the mfront_viscoelastic case but we add plasticity
# test that we enter the plastic NR loop with the correct (viscoelastic) trial stress

[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 1
  ny = 1
  nz = 1
  xmin = 0
  xmax = 1
  ymin = 0
  ymax = 1
  zmin = 0
  zmax = 1
[]

[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
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
  [./E_eqv_plastic]
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
  [./E_eqv_plastic_aux]
    type = MaterialRealAux
    variable = E_eqv_plastic
    property = eq_plastic_strain
    execute_on = 'INITIAL TIMESTEP_END'
  [../]
  [./eta_e_aux]
    type = MaterialRealAux
    variable = eta_e
    property = effective_viscosity
    execute_on = 'TIMESTEP_END'
  [../]
[]

[Functions]
  [./pressure]
    type = ParsedFunction
    value = 'sqrt(t)'
  [../]

  [./pressure_ramp]
    type = PiecewiseLinear
    y = '1e-2 1e-1 1e0 1e1 1e2'
    x = '0    7e-1 7e0 7e1 1e2'
  [../]
  [./confinement]
    type = ParsedFunction
    vars = 'a b'
    vals = '1e3 1e6'
    value = 'if(t<1e3,a*t,b)'
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
    variable = disp_y
    boundary = top
    velocity = -1.0e-16
  [../]
  
  [./uy_bottom]
    type = DirichletBC
    variable = disp_y
    boundary = bottom
    value = 0.0
  [../]
  [./uz_back]
    type = DirichletBC
    variable = disp_z
    boundary = back
    value = 0.0
  [../]
  
  [./outerPressure_right]
    type = Pressure
    boundary = right
    variable = disp_x
    component = 0
    function = confinement
  [../]

    [./outerPressure_front]
    type = Pressure
    boundary = front
    variable = disp_z
    component = 2
    function = confinement
  [../]
#  [./outerPressure_top]
#    type = Pressure
#    boundary = top
#    variable = disp_y
#    component = 1
#    function = pressure
#  [../]

[]

[Materials]
  [./visco_elastic_plastic]
    type = MFrontViscoPlasticity
    mfront_lib_name = '/home/francesco/projects/moose-mfront/moose/modules/tensor_mechanics/plugins/mfront/src/libBehaviour.so'
    # bulk_modulus = 1.0e+12
    # shear_modulus = 1.0e+10
    young_modulus = 2.99e10
    poisson_ratio = 0.25
    phi_angle = 0
    psi_angle = 0
    coh = 38.0e26
    convert_to_radians = true
    A_creep = 0.05e-23
    n_creep = 1.1
    Q_act = 0.0
  [../] 
[]

[Postprocessors]
  [./S_TX_1MPa]
    type = ElementAverageValue
    variable = Se
    outputs = gnuplot
  [../]
  [./Eeqv_TX_1MPa]
    type = ElementAverageValue
    variable = E_eqv
    outputs = gnuplot
  [../]
  [./Eeqv_v_TX_1MPa]
    type = ElementAverageValue
    variable = E_eqv_creep
    outputs = gnuplot
  [../]
  [./Eeqv_p_TX_1MPa]
    type = ElementAverageValue
    variable = E_eqv_plastic
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
  end_time = 8.1536e+13
#  num_steps = 10000
  [TimeStepper]
    type = IterationAdaptiveDT
    dt = 1
    growth_factor = 1.2
  []

  automatic_scaling = true
  compute_scaling_once = false
[]

[Outputs]
  execute_on = 'INITIAL TIMESTEP_END'
  print_linear_residuals = false
  perf_graph = true
  gnuplot = true
  exodus = true
[]
