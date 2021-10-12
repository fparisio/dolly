# Testing the MFront Interface
# linear elastic model using small strain formulation - 1D case.

[GlobalParams]
  displacements = 'disp_x'
[]

[Mesh]
  [gen]
    type = GeneratedMeshGenerator
    dim = 1
    xmin = -0.5
    xmax = 0.5
    nx = 2
  []
[]

[Functions]
  [pull]
    type = ParsedFunction
    value = t/100
  []
[]

[Modules/TensorMechanics/Master]
  [all]
    add_variables = true
    strain = SMALL
    incremental = true
  []
[]

[BCs]
  [pull_function]
    type = FunctionDirichletBC
    variable = disp_x
    boundary = right
    function = pull
  []
  [fixed]
    type = DirichletBC
    variable = disp_x
    boundary = left
    value = 0.0
  []
[]

[Materials]
  [mfront]
    type = MFrontLinearElasticity
    young_modulus = 1000
    poisson_ratio = 0.3
    mfront_lib_name = '/home/cacace/projects/dolly/plugins/mfront/src/libBehaviour.so'
  []
[]

[AuxVariables]
  [stress_00]
    order = CONSTANT
    family = MONOMIAL
  []
  [stress_11]
    order = CONSTANT
    family = MONOMIAL
  []
  [stress_22]
    order = CONSTANT
    family = MONOMIAL
  []
  [stress_01]
    order = CONSTANT
    family = MONOMIAL
  []
  [stress_02]
    order = CONSTANT
    family = MONOMIAL
  []
  [stress_12]
    order = CONSTANT
    family = MONOMIAL
  []
[]

[AuxKernels]
  [stress_00_aux]
    type = RankTwoAux
    variable = stress_00
    rank_two_tensor = stress
    index_i = 0
    index_j = 0
    execute_on = 'TIMESTEP_END'
  []
  [stress_11_aux]
    type = RankTwoAux
    variable = stress_11
    rank_two_tensor = stress
    index_i = 1
    index_j = 1
    execute_on = 'TIMESTEP_END'
  []
  [stress_22_aux]
    type = RankTwoAux
    variable = stress_22
    rank_two_tensor = stress
    index_i = 2
    index_j = 2
    execute_on = 'TIMESTEP_END'
  []
  [stress_01_aux]
    type = RankTwoAux
    variable = stress_01
    rank_two_tensor = stress
    index_i = 0
    index_j = 1
    execute_on = 'TIMESTEP_END'
  []
  [stress_02_aux]
    type = RankTwoAux
    variable = stress_02
    rank_two_tensor = stress
    index_i = 0
    index_j = 2
    execute_on = 'TIMESTEP_END'
  []
  [stress_12_aux]
    type = RankTwoAux
    variable = stress_12
    rank_two_tensor = stress
    index_i = 1
    index_j = 2
    execute_on = 'TIMESTEP_END'
  []
[]

[Executioner]
  type = Transient
  solve_type = 'NEWTON'

  petsc_options = '-snes_ksp_ew'
  petsc_options_iname = '-ksp_gmres_restart'
  petsc_options_value = '101'

  line_search = 'none'

  l_max_its = 100
  nl_max_its = 100
  nl_rel_tol = 1e-12
  nl_abs_tol = 1e-10
  l_tol = 1e-9
  start_time = 0.0
  num_steps = 30
  dt = 1.0
[]

[Preconditioning]
  [smp]
    type = SMP
    full = true
  []
[]

[Outputs]
  exodus = true
  file_base = mfront_elastic_1D
  print_linear_residuals = false
  [perf_graph]
    type = PerfGraphOutput
    heaviest_branch = true
    level = 2
    heaviest_sections = 2
    execute_on = 'FINAL'
  []
[]
