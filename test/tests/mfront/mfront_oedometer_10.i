# test: oedometer
# material behaviour:
# non associative Drucker-Prager perfect plasticity
# dilation = 10

[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 2
  ny = 2
  nz = 2
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

[BCs]
  [./no_x]
    type = DirichletBC
    variable = disp_x
    boundary = left
    value = 0.0
  [../]
  [./load_x]
    type = PresetVelocity
    variable = disp_x
    boundary = right
    velocity = -7.5e-6
  [../]
  [./no_y]
    type = DirichletBC
    variable = disp_y
    boundary = 'bottom top'
    value = 0.0
  [../]
  [./no_z_back]
    type = DirichletBC
    variable = disp_z
    boundary = 'back front'
    value = 0.0
  [../]
[]

[Materials]
  [plastic]
    type = MFrontNonAssociativeDruckerPrager
    mfront_lib_name = '/home/cacace/projects/dolly/plugins/mfront/src/libBehaviour.so'
    young_modulus = 4500
    poisson_ratio = 0.125
    cohesion = 1
    friction_angle = 20
    dilation_angle = 10
  []
[]

[AuxVariables]
  [stress_xx]
    order = CONSTANT
    family = MONOMIAL
  []
  [elastic_strain_xx]
    order = CONSTANT
    family = MONOMIAL
  []
  [total_strain_xx]
    order = CONSTANT
    family = MONOMIAL
  []
  [eq_plastic_strain]
    order = CONSTANT
    family = MONOMIAL
  []
[]

[AuxKernels]
  [stress_xx_aux]
    type = RankTwoAux
    variable = stress_xx
    rank_two_tensor = stress
    index_i = 0
    index_j = 0
    execute_on = 'TIMESTEP_END'
  []
  [elastic_strain_xx_aux]
    type = RankTwoAux
    variable = elastic_strain_xx
    rank_two_tensor = elastic_strain
    index_i = 0
    index_j = 0
    execute_on = 'TIMESTEP_END'
  []
  [total_strain_xx_aux]
    type = RankTwoAux
    variable = total_strain_xx
    rank_two_tensor = total_strain
    index_i = 0
    index_j = 0
    execute_on = 'TIMESTEP_END'
  []
  [eq_plastic_strain_aux]
    type = MaterialRealAux
    variable = eq_plastic_strain
    property = eq_plastic_strain
    execute_on = 'TIMESTEP_END'
  []
[]

[Postprocessors]
  [./u_x]
    type = SideAverageValue
    variable = disp_x
    boundary = right
    outputs = gnuplot
    execute_on = 'INITIAL TIMESTEP_END'
  [../]
  [./S_xx]
    type = ElementAverageValue
    variable = stress_xx
    outputs = gnuplot
    execute_on = 'INITIAL TIMESTEP_END'
  [../]
  [./Es_xx]
    type = ElementAverageValue
    variable = elastic_strain_xx
    outputs = gnuplot
    execute_on = 'INITIAL TIMESTEP_END'
  [../]
  [./Ts_xx]
    type = ElementAverageValue
    variable = total_strain_xx
    outputs = gnuplot
    execute_on = 'INITIAL TIMESTEP_END'
  [../]
  [./Ep_xx]
    type = ElementAverageValue
    variable = eq_plastic_strain
    outputs = gnuplot
    execute_on = 'INITIAL TIMESTEP_END'
  [../]
[]

[Preconditioning]
  [./precond]
    type = SMP
    full = true
    petsc_options_iname = '-pc_type -pc_hypre_type'
    petsc_options_value = 'hypre boomeramg'
  [../]
[]

[Executioner]
  type = Transient
  solve_type = 'NEWTON'
  automatic_scaling = true
  start_time = 0.0
  end_time = 100.0
  dt = 4.0
  nl_abs_tol = 1e-15
[]

[Outputs]
  file_base = mfront_oedometer_10
  print_linear_residuals = false
  [./perf_graph]
    type = PerfGraphOutput
    heaviest_branch = true
    heaviest_sections = 3
    execute_on = 'FINAL'
  [../]
  exodus = true
  # gnuplot = true
  execute_on = 'INITIAL TIMESTEP_END'
[]
