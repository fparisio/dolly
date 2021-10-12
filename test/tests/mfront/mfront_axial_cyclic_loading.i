# test: axial loading (cyclic) - no confinement
# material: non associative Drucker-Prager perfect plasticity
# unloading cycles are half of the loading cycles in loading velocity

[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 2
  ny = 2
  nz = 2
  xmin = 0
  xmax = 0.5
  ymin = 0
  ymax = 0.5
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

[Functions]
  [push]
    type = ParsedFunction
    value = 'if(t<=50,load,if(t>50&t<80,unload,if(t>=80&t<=130,load,if(t>130&t<160,unload,load))))'
    vars = 'load unload'
    vals = '-7.5e-6 v_unload'
  []
  [v_unload]
    type = ParsedFunction
    value = 'v0/a'
    vars = 'v0 a'
    vals = '7.5e-6 1'
  []
[]

[BCs]
  [./no_x]
    type = DirichletBC
    variable = disp_x
    boundary = 'left right'
    value = 0.0
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
    boundary = 'back'
    value = 0.0
  [../]
  [./load_z_front]
    type = PresetVelocity
    variable = disp_z
    boundary = 'front'
    function = push
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
    dilation_angle = 20
  []
[]

[AuxVariables]
  [stress_zz]
    order = CONSTANT
    family = MONOMIAL
  []
  [elastic_strain_zz]
    order = CONSTANT
    family = MONOMIAL
  []
  [total_strain_zz]
    order = CONSTANT
    family = MONOMIAL
  []
  [eq_plastic_strain]
    order = CONSTANT
    family = MONOMIAL
  []
[]

[AuxKernels]
  [stress_zz_aux]
    type = RankTwoAux
    variable = stress_zz
    rank_two_tensor = stress
    index_i = 2
    index_j = 2
    execute_on = 'TIMESTEP_END'
  []
  [elastic_strain_zz_aux]
    type = RankTwoAux
    variable = elastic_strain_zz
    rank_two_tensor = elastic_strain
    index_i = 2
    index_j = 2
    execute_on = 'TIMESTEP_END'
  []
  [total_strain_zz_aux]
    type = RankTwoAux
    variable = total_strain_zz
    rank_two_tensor = total_strain
    index_i = 2
    index_j = 2
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
  [./u_z]
    type = SideAverageValue
    variable = disp_z
    boundary = right
    outputs = gnuplot
    execute_on = 'INITIAL TIMESTEP_END'
  [../]
  [./S_zz]
    type = ElementAverageValue
    variable = stress_zz
    outputs = gnuplot
    execute_on = 'INITIAL TIMESTEP_END'
  [../]
  [./Es_zz]
    type = ElementAverageValue
    variable = elastic_strain_zz
    outputs = gnuplot
    execute_on = 'INITIAL TIMESTEP_END'
  [../]
  [./Ts_zz]
    type = ElementAverageValue
    variable = total_strain_zz
    outputs = gnuplot
    execute_on = 'INITIAL TIMESTEP_END'
  [../]
  [./Ep_zz]
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
  end_time = 210.0
  dt = 5.0
  nl_abs_tol = 1e-15
[]

[Outputs]
  file_base = mfront_axial_cyclic_loading
  execute_on = 'initial timestep_end'
  print_linear_residuals = false
  perf_graph = true
  # gnuplot = true
  exodus = true
[]
