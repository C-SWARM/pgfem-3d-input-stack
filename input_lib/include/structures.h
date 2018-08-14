/* AUTHORS:
 * Aaron Howell
 */
#pragma once

#include <string>
#include <vector>

struct Physics;
struct Basis_Vector;
struct HR_Material;
struct Cohesive_Potential;
struct TypeID;
struct Prob;
struct NodeDomID;
struct Reaction;

//contains only non physics-specific data and a list of physics structs
struct Input_Data{
  /*****************optional file flags**************/
  //Used to determine whether to create BC files
  //(pgfem reads BCs from .in file if there's no BC directory)
  bool bc_flag = false;    //set to true if a bc.json file exists
  bool ic_flag = false;    //set to true if an ic.json file exists


  /*****************multiphysics_in data************/
  bool multiphysics_file_exists = false;
  int number_of_physics = 1;
  std::vector<Physics> physics_list;

  /*********multiscale macro data (.in.co_props)****/
  bool co_props_flag = false;  //set to true if a in_co_props.json file exists
  int number_of_cohesive_potentials = 1;
  std::vector<Cohesive_Potential> cohesive_potentials;

  /**********multiscale micro data (normal_in)******/
  bool normal_in_flag = false;
  double microcell_volume = 0;
  double microcell_thickness = 0;
  double microcell_normal_vector[3] = {0};

  /*******************periodic data*****************/
  bool periodic_flag = false;
  std::vector<std::vector<TypeID>> typeID_sets;

  /*****************numerical_param data************/
  double Newton_Raphson_tolerance = 1e-5;
  int Newton_Raphson_max_num_of_iterations = 5;
  int order_of_elements = 0;
  int number_of_spatial_dimensions = 3;       //always set to 3; gives a warning if it's not 3
  int solver_max_num_of_iterations = 1000;
  int solver_num_of_Krylov_subspaces = 300;
  double solver_tolerance = 1e-5;
  double absolute_tolerance = 1e-15;
  int pressure_num = 0; 

  int max_number_of_staggering = 5;
  int solution_scheme = 4;
  
  //nonlinear_solution_method parameters:
  double tolerance = 1e-5;
  int max_num_of_iterations = 5;


  /*****************material data***************/
  int number_of_materials = 1;

  int number_of_volume_fraction = 0;
  std::vector<double> volume_fraction;
  std::vector<double> density;              //global material density

  int number_of_regions_to_set_material = 0;
  //these have number_of_regions_to_set_material elements
  std::vector<int> geom_type;
  std::vector<int> geom_id;
  std::vector<int> mp1;
  std::vector<int> mp2;

  int number_of_bases = 0;
  std::vector<Basis_Vector> basis_vectors;

  std::vector<HR_Material> material_list;
  
  //chemical data
  uint nReactions = 0;
  int rho_mixture = 0;
  int diffusivityCoefficient = 0;
  int diffActivationEnergyByR = 0;
  int bcHeatTime = 0;
  std::vector<Reaction> reaction_list;

  /*****************timestep data***************/
  int number_of_time_steps = 0;
  std::vector<double> time_steps;
  int number_of_steps_to_print = 0;
  std::vector<int> list_of_steps_to_print;


  /*******************output settings data*****************/
  bool output_settings_flag = false;
  std::string reaction_filename = "Name_not_set";
  int exascale_settings = 0;
  std::vector<Prob> probs;
};

struct Basis_Vector{
  int e1[3] = {0, 0, 0};
  int e2[3] = {0, 0, 0};
  int e3[3] = {0, 0, 0};
};


//contains physics-specific data
struct Physics{

  /*****************numerical_param data************/
  //nonlinear_solution_method parameters:
  int max_num_of_iterations = 0;
  bool compute_initial_residuals = false;
  double value_for_initial_residuals = 0;


  /*****************multiphysics_in data*************/
  int equation_id = -1;    //0: Momentum, 1: Energy, 2: Chemistry
  std::string physics_name = "Name_not_set";
  int degree_of_freedom = 0;
  int num_of_coupled_physics = 0;
  std::vector<int> coupled_physics_ids;
  int num_vars_to_print = -1;
  std::vector<int> vars_to_print;


  /*****************bc data*********************/
  /*
  int n_pre_disp;  //size of the next 5 arrays
  std::vector<long> geometry_type;
  std::vector<long> geometry_id;
  std::vector<long> xcomp;
  std::vector<long> ycomp;
  std::vector<long> zcomp;
  */
  std::vector<std::vector<int>> bc_data;
  std::vector<double> bc_replacements;

  /*****************load data*******************/
  int num_of_loading_steps = 0;
  std::vector<int> loading_time_steps;
  std::vector<std::vector<double>> loading_increments;  //number of vectors == bc_replacements.size()

  /*****************ic data*********************/
  //int restart_number
  double time_integration_rule = .5;
  std::vector<double> inertial_density;     //num_of_materials amount of elements
  double reference_value = 0;       //meaning changes based on physics used (temp for energy, displacement for momentum)
  //reference_value defaults to 0 for momentum and 300 for energy

  std::vector<std::vector<int>> g_id;
  std::vector<double> ic_replacements;

  /*
  std::vector<int> node_number_N;  
  std::vector<int> initial_displacement_x;
  std::vector<int> initial_displacement_y;
  std::vector<int> initial_displacement_z;
  std::vector<int> initial_velocity_x;
  std::vector<int> initial_velocity_y;
  std::vector<int> initial_velocity_z;
  */
};

//contains material-specific data in human-readable format
//(con3d++ already has a class named Material)
struct HR_Material{
  std::string name = "-1";       //defaults to ID
  int ID = -1;

  //Mechanical
  double youngs_modulus[3] = {0, 0, 0};
  double shear_modulus[3] = {0, 0, 0}; 
  double poissons_ratio[3] = {0, 0, 0};
  double coefficient_of_thermal_expansion[3] = {0, 0, 0};
  double mooney_rivlin[2] = {0, 0};  
  double sig = 0;   //not used for now, but needed for con3d
  double strain_energy_function_vol = 2;
  double strain_energy_function_dev = 1;

  //Thermal
  double heat_capacity = 0;
  double thermal_conductivity[9] = {0};
  double FHS_MW = 0.8; // fraction of heat source from mechanical work
  
  //Chemical
  double molecularWeights = 0;
  double initialMassFractions = 0;
  double initialVolumeFractions = 0;
};

struct Reaction{
  double reactionCoefficients = 0;
  double oneByEquiliReactionCoefficients = 0;
  double temperatureExponents = 0;
  double heatReactions = 0;
  double activationEnergyByR = 0;

  //material_list.size():
  std::vector<double> stoichiometricConstants;
  std::vector<double> speciesExponents;
};

struct Cohesive_Potential{
  int type_of_potential = -1;
  double phenomenological_potentials[4] = {0};
};

struct TypeID{
  int type = 0;
  int ID = 0;
};

struct NodeDomID{
  size_t dom = 0;
  size_t ID = 0;
};

struct Prob{
  std::vector<TypeID> geomTypeID;
  std::vector<NodeDomID> MatchedNodeIDs;
  int quantity_of_interest;
  bool is_cell_data;
  bool integrate;
  
  std::vector<int> outputTimeStep;
  std::string out_filename;
};
