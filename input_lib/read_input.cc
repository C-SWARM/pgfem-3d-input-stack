/* AUTHORS:
 * Aaron Howell
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "structures.h"
#include "read_input.h"
#include "json.hpp"
#include "input_parser.h"
#include <string>
#include <cmath>
#include <vector>
#include <typeinfo>
#include <cassert>

#include <iostream>
#include <fstream>

using json = nlohmann::json;
using namespace std;

/*
 *These overloaded functions handle the optional looping syntax read in from certain json file parameters
 *The parameters supported by the load and timestep files are either a list of values, or a string in the looping syntax
 *A string (eg: "1:100:2") will be passed to the parse_input function to be parsed, while a 
 *list of values (eg: [1,3,5,7,...,99]) will be moved directly into the destination vector
 */

//This function converts an input string into a character array before calling parse_input to parse it
template <class T>
void parse_json_string(string input_string, vector<T> &destination_vector, int destination_size)
{
  char *char_buffer = new char[input_string.length() + 1];
  strcpy(char_buffer, input_string.c_str());
  parse_input(char_buffer, destination_vector, destination_size, input_string.length());

  delete[] char_buffer;
}

//These functions move the int/double values from a source vector from a JSON datatype to a destination vector
//Since json types are determined at runtime, templates don't seem to work (at least for the first parameter)
void parse_json_list(vector<double> input_list, vector<double> &destination_vector, int destination_size)
{
  for (int i = 0; i < destination_size; ++i) {
    destination_vector.push_back(input_list[i]);
  }
}
void parse_json_list(vector<int> input_list, vector<int> &destination_vector, int destination_size)
{
  for (int i = 0; i < destination_size; ++i) {
    destination_vector.push_back(input_list[i]);
  }
}


//calls all of the read functions and reads them into inputs by reference
void read_input (Input_Data &inputs)
{
  read_multiphysics(inputs);
  read_timesteps(inputs);
  read_numerical_param(inputs);
  read_material_properties(inputs);
  read_boundary_conditions(inputs);
  read_initial_conditions(inputs);
  read_loads(inputs);
  read_co_props(inputs);
  read_normal_in(inputs);
  read_periodic(inputs);
  read_output_settings(inputs);
}

//reads multiphysics data into data structure
void read_multiphysics(Input_Data &inputs)
{
  ifstream fmultiphysics("multiphysics.json");

  if (!fmultiphysics.good()) {   //file doesn't exist
    cout << "multiphysics.json not found\n";

    //assumes 1 momentum physics if file doesn't exist
    Physics physics;
    physics.equation_id = 0;
    physics.physics_name = "Momentum_default";
    inputs.physics_list.push_back(physics);

    return;
  }
  cout << "reading multiphysics file\n";

  inputs.multiphysics_file_exists = true;
  json json_in;

  fmultiphysics >> json_in;

  if (!(json_in["number_of_physics"].is_null())) {  //use default if missing
    inputs.number_of_physics = json_in["number_of_physics"];
  }

  //loop through and read each physics data
  for (int i = 0; i < inputs.number_of_physics; ++i) {
    Physics physics;

    physics.equation_id = json_in["physics_list"][i]["equation_id"];
      
    if (!(json_in["physics_list"][i]["physics_name"].is_null())) {
      physics.physics_name = json_in["physics_list"][i]["physics_name"];
    }
    else{                             //use default name of "Physics_#" if missing
      physics.physics_name = "Physics_";
      physics.physics_name += to_string(i);
    }

    if (!(json_in["physics_list"][i]["degree_of_freedom"].is_null())) {
      physics.degree_of_freedom = json_in["physics_list"][i]["degree_of_freedom"];
    }
    else{                       //set defaults if degree_of_freedom wasn't provided
      if (physics.equation_id == 0) {        //momentum default = 3
	physics.degree_of_freedom = 3;
      }
      else if (physics.equation_id == 1) {   //energy default = 1
	physics.degree_of_freedom = 1;
      }
    }

    //read the list of coupled physics
    if (!(json_in["physics_list"][i]["number_of_coupled_physics"].is_null())) {   //use default if tag wasn't found
      //if size is given, make sure that there are that many elements in the file
      assert (json_in["physics_list"][i]["number_of_coupled_physics"] == json_in["physics_list"][i]["coupled_physics_ids"].size() 
	      && "multiphysics file: number_of_coupled_physics doesn't match number of ids");
      physics.num_of_coupled_physics = json_in["physics_list"][i]["number_of_coupled_physics"];
    }
    else {       //defualt to number of elements
      physics.num_of_coupled_physics = json_in["physics_list"][i]["coupled_physics_ids"].size();
    }

    for (int j = 0; j < physics.num_of_coupled_physics; ++j) {
      physics.coupled_physics_ids.push_back( json_in["physics_list"][i]["coupled_physics_ids"][j] );
    }

    //read the list of variables to print
    if (!(json_in["physics_list"][i]["number_of_vars_to_print"].is_null())) {
      //if size is given, make sure that there are that many elements in the file
      assert (json_in["physics_list"][i]["number_of_vars_to_print"] == json_in["physics_list"][i]["vars_to_print"].size() 
	      && "multiphysics file: number_of_vars_to_print doesn't match number of vars_to_print");
      physics.num_vars_to_print = json_in["physics_list"][i]["number_of_vars_to_print"];
    }
    else {      //defualt to number of elements
      physics.num_vars_to_print = json_in["physics_list"][i]["vars_to_print"].size();  
    }

    for (int j = 0; j < physics.num_vars_to_print; ++j) {
      physics.vars_to_print.push_back( json_in["physics_list"][i]["vars_to_print"][j] );
    }

    inputs.physics_list.push_back(physics);
  } //end of loop for each physics

  fmultiphysics.close();
}


//reads timestep data into data structure
void read_timesteps(Input_Data &inputs)
{
  ifstream ftimesteps("timesteps.json");

  if (!ftimesteps.good()) {   //file doesn't exist
    cout << "timesteps.json not found" << endl;
    return;
  }
  cout << "reading timesteps file\n";
  json json_in;

  ftimesteps >> json_in;

  inputs.number_of_time_steps = json_in["number_of_time_steps"];

  //read each time step
  if (json_in["time_steps"].is_string()) {  //check whether to call the string parser
    parse_json_string(json_in["time_steps"], inputs.time_steps, inputs.number_of_time_steps + 1);  //timestep list also includes 0, the list always has one more than number_of_time_steps
  }
  else{
    parse_json_list(json_in["time_steps"], inputs.time_steps, inputs.number_of_time_steps + 1);
  }
  
  inputs.number_of_steps_to_print = json_in["number_of_steps_to_print"];

  //set the size in advance
  inputs.list_of_steps_to_print.reserve(inputs.number_of_steps_to_print);

  if (json_in["list_of_steps_to_print"].is_string()) {  //check whether to call the string parser
    parse_json_string(json_in["list_of_steps_to_print"], inputs.list_of_steps_to_print, inputs.number_of_steps_to_print);
  }
  else{
    parse_json_list(json_in["list_of_steps_to_print"], inputs.list_of_steps_to_print, inputs.number_of_steps_to_print);
  }

  ftimesteps.close();
}


//reads material data into data structure
void read_material_properties(Input_Data &inputs)
{
  ifstream fmaterial_mat("material_mat.json");

  if (!fmaterial_mat.good()) {   //file doesn't exist
    cout << "material_mat.json not found" << endl;

    //assumes 1 material if file doesn't exist
    HR_Material material;
    inputs.material_list.push_back(material);

    return;
  }
  cout << "reading material file\n";

  json json_in;

  fmaterial_mat >> json_in;

  if (json_in["number_of_materials"].is_null())           //optional parameter
    inputs.number_of_materials = json_in["materials"].size();
  else{
    //if size is given, make sure that there are that many elements in the file
    assert (json_in["number_of_materials"] == json_in["materials"].size()
      && "material file: number_of_materials doesn't equal elements in materials list");
    inputs.number_of_materials = json_in["number_of_materials"];
  }

  //read volume fraction
  if (json_in["number_of_volume_fraction"].is_null())     //optional parameter
    inputs.number_of_volume_fraction = json_in["volume_fraction"].size();
  else{
    //if size is given, make sure that there are that many elements in the file
    assert (json_in["number_of_volume_fraction"] == json_in["volume_fraction"].size() 
	    && "material file: number_of_volume_fraction doesn't match number of volume_fraction elements");
    inputs.number_of_volume_fraction = json_in["number_of_volume_fraction"];
  }

  for (int i = 0; i < inputs.number_of_volume_fraction; ++i)
    inputs.volume_fraction.push_back(json_in["volume_fraction"][i]);

  //set inertial density for each material
  for (int mat = 0; mat < inputs.number_of_materials; ++mat) {  //loop over # of materials
    if (json_in["density"][mat].is_null()) //set to 0 if not provided for that material
      inputs.density.push_back(0);
    else
      inputs.density.push_back(json_in["density"][mat]);
  }

  if (!(json_in["number_of_regions_to_set_material"].is_null())) {
    //if size is given, make sure that there are that many elements in the file
    assert (json_in["number_of_regions_to_set_material"] == json_in["material_regions"].size()
	    && "material file: number_of_regions_to_set_material doesn't match number of material_regions elements");
    inputs.number_of_regions_to_set_material = json_in["number_of_regions_to_set_material"];
  }
  else
    inputs.number_of_regions_to_set_material = json_in["material_regions"].size();

  for (int i = 0; i < inputs.number_of_regions_to_set_material; ++i) {

    //optional string input for geometry type
    if (json_in["material_regions"][i][0].is_string()) {
      if (json_in["material_regions"][i][0] == "vertex")
      	inputs.geom_type.push_back(1);
      else if (json_in["material_regions"][i][0] == "curve")
      	inputs.geom_type.push_back(2);
      else if (json_in["material_regions"][i][0] == "surface")
      	inputs.geom_type.push_back(3);
      else if (json_in["material_regions"][i][0] == "region")
      	inputs.geom_type.push_back(4);
      else if (json_in["material_regions"][i][0] == "patch")
      	inputs.geom_type.push_back(5);
      else if (json_in["material_regions"][i][0] == "interface")
      	inputs.geom_type.push_back(7);
      else{
      	cerr << "Invalid material_regions geometry type\n";
      	cerr << "First element in material regions must be:\n";
      	cerr << "\"vertex\", \"curve\", \"surface\", \"region\", \"patch\", or \"interface\" or the integer equivalent\n";
      	exit(1);
      }
    }
    //original number input
    else
      inputs.geom_type.push_back(json_in["material_regions"][i][0]);

    inputs.geom_id.push_back(json_in["material_regions"][i][1]);
    inputs.mp1.push_back(json_in["material_regions"][i][2]);
    inputs.mp2.push_back(json_in["material_regions"][i][3]);
  }

  
  if (!(json_in["number_of_bases"].is_null())) {
    //if size is given, make sure that there are that many elements in the file
    assert (json_in["number_of_bases"] == json_in["basis_vectors"].size()
	    && "material file: number_of_bases doesn't match number of basis_vectors elements");
    inputs.number_of_bases = json_in["number_of_bases"];
  }
  else
    inputs.number_of_bases = json_in["basis_vectors"].size();  //default to number of elements

  for (int i = 0; i < inputs.number_of_bases; ++i) {
    Basis_Vector basis_vector;
    for (int j = 0; j < 3; ++j) {
      basis_vector.e1[j] = json_in["basis_vectors"][i]["e1"][j];
      basis_vector.e2[j] = json_in["basis_vectors"][i]["e2"][j];
      basis_vector.e3[j] = json_in["basis_vectors"][i]["e3"][j];
    }
    inputs.basis_vectors.push_back(basis_vector);
  }

  //read each material
  for (int mat = 0; mat < inputs.number_of_materials; ++mat) {
    HR_Material material;

    //check if ID was provided. Otherwise keep it as its default value
    if (!json_in["materials"][mat]["ID"].is_null())
      material.ID = json_in["materials"][mat]["ID"];

    //check if name was provided. Otherwise set it equal to ID
    if (!json_in["materials"][mat]["name"].is_null())
      material.name = json_in["materials"][mat]["name"];
    else{
      //found at: http://www.cplusplus.com/articles/D9j2Nwbp/
      ostringstream convert_int_to_string;  
      convert_int_to_string << material.ID;
      material.name = convert_int_to_string.str();
    }

    //Momentum equation data
    if (!json_in["materials"][mat]["youngs_modulus"].is_null()) {
      material.youngs_modulus[0] = json_in["materials"][mat]["youngs_modulus"][0];
      material.youngs_modulus[1] = json_in["materials"][mat]["youngs_modulus"][1];
      material.youngs_modulus[2] = json_in["materials"][mat]["youngs_modulus"][2];
    }
    if (!json_in["materials"][mat]["shear_modulus"].is_null()) {
      material.shear_modulus[0] = json_in["materials"][mat]["shear_modulus"][0];
      material.shear_modulus[1] = json_in["materials"][mat]["shear_modulus"][1];
      material.shear_modulus[2] = json_in["materials"][mat]["shear_modulus"][2];
    }
    if (!json_in["materials"][mat]["poissons_ratio"].is_null()) {
      material.poissons_ratio[0] = json_in["materials"][mat]["poissons_ratio"][0];
      material.poissons_ratio[1] = json_in["materials"][mat]["poissons_ratio"][1];
      material.poissons_ratio[2] = json_in["materials"][mat]["poissons_ratio"][2];
    }
    if (!json_in["materials"][mat]["coefficient_of_thermal_expansion"].is_null()) {
      material.coefficient_of_thermal_expansion[0] = json_in["materials"][mat]["coefficient_of_thermal_expansion"][0];
      material.coefficient_of_thermal_expansion[1] = json_in["materials"][mat]["coefficient_of_thermal_expansion"][1];
      material.coefficient_of_thermal_expansion[2] = json_in["materials"][mat]["coefficient_of_thermal_expansion"][2];
    }
    if (!json_in["materials"][mat]["mooney_rivlin"].is_null()) {
      material.mooney_rivlin[0] = json_in["materials"][mat]["mooney_rivlin"][0];
      material.mooney_rivlin[1] = json_in["materials"][mat]["mooney_rivlin"][1];
    }
    if (!json_in["materials"][mat]["sig"].is_null())
      material.sig = json_in["materials"][mat]["sig"];
    if (!json_in["materials"][mat]["strain_energy_function_vol"].is_null())
      material.strain_energy_function_vol = json_in["materials"][mat]["strain_energy_function_vol"];
    if (!json_in["materials"][mat]["strain_energy_function_dev"].is_null())
      material.strain_energy_function_dev = json_in["materials"][mat]["strain_energy_function_dev"];


    //Energy equation data
    if (!json_in["materials"][mat]["heat_capacity"].is_null())
      material.heat_capacity = json_in["materials"][mat]["heat_capacity"];

    if (!json_in["materials"][mat]["thermal_conductivity"].is_null())
      for (int i = 0; i < 9; ++i)
	      material.thermal_conductivity[i] = json_in["materials"][mat]["thermal_conductivity"][i];
	
    if (!json_in["materials"][mat]["fraction_of_heat_from_mechanical"].is_null())
      material.FHS_MW = json_in["materials"][mat]["fraction_of_heat_from_mechanical"];


    //Chemestry material data
    if (!json_in["materials"][mat]["molecularWeights"].is_null())
      material.molecularWeights = json_in["materials"][mat]["molecularWeights"];
    if (!json_in["materials"][mat]["initialMassFractions"].is_null())
      material.initialMassFractions = json_in["materials"][mat]["initialMassFractions"];
    if (!json_in["materials"][mat]["initialVolumeFractions"].is_null())
      material.initialVolumeFractions = json_in["materials"][mat]["initialVolumeFractions"];

    inputs.material_list.push_back(material);    //add each material to the material list
  }
  
  //Chemestry data
  if (json_in["nReactions"].is_null())
    inputs.nReactions = json_in["reactions"].size();  //default to number of elements
  else{
    //if size is given, make sure that there are that many elements in the file
    assert (json_in["nReactions"] == json_in["reactions"].size()
	    && "material file: number of reactions doesn't match amount of reactions elements");
    inputs.nReactions = json_in["nReactions"];
  }
  
  if (!json_in["rho_mixture"].is_null())
    inputs.rho_mixture = json_in["rho_mixture"];
  if (!json_in["diffusivityCoefficient"].is_null())
    inputs.diffusivityCoefficient = json_in["diffusivityCoefficient"];
  if (!json_in["diffActivationEnergyByR"].is_null())
    inputs.diffActivationEnergyByR = json_in["diffActivationEnergyByR"];
  if (!json_in["bcHeatTime"].is_null())
    inputs.bcHeatTime = json_in["bcHeatTime"];

  for (uint i = 0; i < inputs.nReactions; ++i){
    Reaction reaction;
    
    if (!json_in["reactions"][i]["reactionCoefficients"].is_null())
      reaction.reactionCoefficients = json_in["reactions"][i]["reactionCoefficients"];
    if (!json_in["reactions"][i]["oneByEquiliReactionCoefficients"].is_null())
      reaction.oneByEquiliReactionCoefficients = json_in["reactions"][i]["oneByEquiliReactionCoefficients"];
    if (!json_in["reactions"][i]["temperatureExponents"].is_null())
      reaction.temperatureExponents = json_in["reactions"][i]["temperatureExponents"];
    if (!json_in["reactions"][i]["heatReactions"].is_null())
      reaction.heatReactions = json_in["reactions"][i]["heatReactions"];
    if (!json_in["reactions"][i]["activationEnergyByR"].is_null())
      reaction.activationEnergyByR = json_in["reactions"][i]["activationEnergyByR"];
    
    assert((json_in["reactions"][i]["stoichiometricConstants"].size() == (uint)inputs.number_of_materials) && 
           "material file: stoichiometricConstantsdoesn't match number of materials");
    assert((json_in["reactions"][i]["speciesExponents"].size() == (uint)inputs.number_of_materials) && 
           "material file: speciesExponents doesn't match number of materials");
    
    for (int mat = 0; mat < inputs.number_of_materials; ++mat){
      reaction.stoichiometricConstants.push_back( json_in["reactions"][i]["stoichiometricConstants"][mat] );
      reaction.speciesExponents.push_back( json_in["reactions"][i]["speciesExponents"][mat] );
    }

    inputs.reaction_list.push_back(reaction);
  }
  //end Chemestry data

  fmaterial_mat.close();
}

//reads numerical.param data into data structure
void read_numerical_param(Input_Data &inputs)
{
  ifstream fnumerical_param("numerical_param.json");

  if (!fnumerical_param.good()) {    //file doesn't exist
    cout << "numerical_param.json not found" << endl;
    return;
  }
  cout << "reading numerical param file\n";

  json json_in;

  fnumerical_param >> json_in;

  if (!(json_in["Newton_Raphson_tolerance"].is_null())) {  //use default if missing
    inputs.Newton_Raphson_tolerance = json_in["Newton_Raphson_tolerance"];
  }
  if (!(json_in["Newton_Raphson_max_num_of_iterations"].is_null())) {  //use default if missing
    inputs.Newton_Raphson_max_num_of_iterations = json_in["Newton_Raphson_max_num_of_iterations"];
  }
  if (!(json_in["order_of_elements"].is_null())) {  //use default if missing
    inputs.order_of_elements = json_in["order_of_elements"];
  }
  if (!(json_in["number_of_spatial_dimensions"].is_null())) {  //use default if missing
    if (json_in["number_of_spatial_dimensions"] != 3) { //should be 3; gives a warning if it's not 3
      cout << "WARNING: number_of_spatial_dimensions should be set to 3\n";
    }
    inputs.number_of_spatial_dimensions = json_in["number_of_spatial_dimensions"];
  }
  if (!(json_in["solver_max_num_of_iterations"].is_null())) {  //use default if missing
    inputs.solver_max_num_of_iterations = json_in["solver_max_num_of_iterations"];
  }
  if (!json_in["solver_num_of_Krylov_subspaces"].is_null()) {  //optional parameter: default = 300
    inputs.solver_num_of_Krylov_subspaces = json_in["solver_num_of_Krylov_subspaces"];
  }
  if (!(json_in["solver_tolerance"].is_null())) {  //use default if missing
    inputs.solver_tolerance = json_in["solver_tolerance"];
  }
  if (!(json_in["absolute_tolerance"].is_null())) {  //use default if missing
    inputs.absolute_tolerance = json_in["absolute_tolerance"];
  }
  if (!(json_in["pressure_num"].is_null())) {  //use default if missing
    inputs.pressure_num = json_in["pressure_num"];
  }
  if (!(json_in["multiphysics_staggering_method"]["maximum_num_of_staggering"].is_null())) {  //optional parameter: default = 5
    inputs.max_number_of_staggering = json_in["multiphysics_staggering_method"]["maximum_num_of_staggering"];
  }
  if (!(json_in["multiphysics_staggering_method"]["solution_scheme"].is_null())) {
    inputs.solution_scheme = json_in["multiphysics_staggering_method"]["solution_scheme"];
  }
  if (!(json_in["nonlinear_solution_method"]["tolerance"].is_null())) {
    inputs.tolerance = json_in["nonlinear_solution_method"]["tolerance"];
  }
  if (!(json_in["nonlinear_solution_method"]["max_num_of_iterations"].is_null())) {
    inputs.max_num_of_iterations = json_in["nonlinear_solution_method"]["max_num_of_iterations"];
  }

  //loop through each physics listed in the numerical_param json file
  for (uint i = 0; i < json_in["nonlinear_solution_method"]["physics"].size(); ++i) {

    //These physics-specific data can be unordered in the json file. 
    //As a result, physics_ID is their position in the physics_list
    size_t current_physics = json_in["nonlinear_solution_method"]["physics"][i]["physics_ID"];
    assert(size_t(inputs.number_of_physics) >= current_physics + 1
                    && "numerical param file: out of bounds physics_ID in nonlinear_solution_method");

    if(!(json_in["nonlinear_solution_method"]["physics"][i]["max_num_of_iterations"].is_null())) {
      inputs.physics_list[current_physics].max_num_of_iterations = json_in["nonlinear_solution_method"]["physics"][i]["max_num_of_iterations"];
    }

    if (!json_in["nonlinear_solution_method"]["physics"][i]["compute_initial_residuals"].is_null()) {  //optional parameter: default = false
      inputs.physics_list[current_physics].compute_initial_residuals = json_in["nonlinear_solution_method"]["physics"][i]["compute_initial_residuals"];
    }

    //value_for_initial_residuals isn't needed if compute_initial_residuals is false
    if (inputs.physics_list[current_physics].compute_initial_residuals) {
      //give a warning if compute_initial_residuals is true, but no value was provided
      if (json_in["nonlinear_solution_method"]["physics"][i]["value_for_initial_residuals"].is_null()) {
	cout << "WARNING: compute_initial_residuals was set to true, but no value was provided.\n";
	cout << "setting value_for_initial_residuals to 0\n";
	inputs.physics_list[current_physics].value_for_initial_residuals = 0;
      }
      else{
	inputs.physics_list[current_physics].value_for_initial_residuals = json_in["nonlinear_solution_method"]["physics"][i]["value_for_initial_residuals"];
      }
    }
  }

  fnumerical_param.close();
}


//read each physics' boundary conditions file
void read_boundary_conditions (Input_Data &inputs)
{
  cout << "reading bc files\n";

  //loop through each physics
  for (int phys = 0; phys < inputs.number_of_physics; ++phys) {
    string BC_file = inputs.physics_list[phys].physics_name + "_bc.json";
    ifstream fBC(BC_file);

    if (!fBC.good()) {    //file doesn't exist
      cout << BC_file << " not found" << endl;
      continue;
    }
    inputs.bc_flag = true;   //at least 1 bc.json file exists

    json json_in;
    fBC >> json_in;
   
    //store a vector of vectors for bc_data
    for (uint i = 0; i < json_in["bc_data"].size(); ++i) {  //loop for each bc_data entry
      std::vector<int> bc_data_entry;
       
      //read 1st BC parameter:
      //optional string input for geometry type
      if (json_in["bc_data"][i][0].is_string()) {
	if (json_in["bc_data"][i][0] == "vertex")
	  bc_data_entry.push_back(1);
	else if (json_in["bc_data"][i][0] == "curve")
	  bc_data_entry.push_back(2);
	else if (json_in["bc_data"][i][0] == "surface")
      	bc_data_entry.push_back(3);
	else if (json_in["bc_data"][i][0] == "region")
	  bc_data_entry.push_back(4);
	else if (json_in["bc_data"][i][0] == "patch")
	  bc_data_entry.push_back(5);
	else if (json_in["bc_data"][i][0] == "interface")
	  bc_data_entry.push_back(7);
	else{
	  cerr << "Invalid bc_data geometry type\n";
	  cerr << "First element in material regions must be:\n";
	  cerr << "\"vertex\", \"curve\", \"surface\", \"region\", \"patch\", or \"interface\" or the integer equivalent\n";
      	exit(1);
	}
      }
      //original number input
      else
	bc_data_entry.push_back(json_in["bc_data"][i][0]);

      //read additional BC parameters:
      for (uint j = 1; j < json_in["bc_data"][i].size(); ++j) {   //loop for each additional value in each entry
	bc_data_entry.push_back(json_in["bc_data"][i][j]);


      }
	
      inputs.physics_list[phys].bc_data.push_back(bc_data_entry);
    }
      
    for (uint i = 0; i < json_in["replacements"].size(); ++i) {
      inputs.physics_list[phys].bc_replacements.push_back(json_in["replacements"][i]);
    }
      
    fBC.close();
  }
}

//read each physics' initial conditions file
void read_initial_conditions (Input_Data &inputs)
{
  cout << "reading ic files\n";

  //loop through each physics
  for (int phys = 0; phys < inputs.number_of_physics; ++phys) {
    string IC_file = inputs.physics_list[phys].physics_name + "_initial.json";
    
    ifstream fIC(IC_file);

    if (!fIC.good()) {    //file doesn't exist
      cout << IC_file << " not found" << endl;

      switch (inputs.physics_list[phys].equation_id) {
      case 0: //momentum eqauation
        //set default inertial_density to 0 for each momentum material
        for (int mat = 0; mat < inputs.number_of_materials; ++mat)
          inputs.physics_list[phys].inertial_density.push_back(0);

        break;

      case 1: //thermal equation
        inputs.physics_list[phys].reference_value = 300;
        break;
      }
      continue;
    }

    inputs.ic_flag = true;   //at least 1 ic.json file exists
    
    json json_in;
 
    fIC >> json_in;

    switch (inputs.physics_list[phys].equation_id) {
    case 0:                                                        //momentum eqauation
      inputs.physics_list[phys].time_integration_rule = json_in["time_integration_rule"];

      //set inertial density for each material
      for (int mat = 0; mat < inputs.number_of_materials; ++mat) {  //loop over # of materials
        if (json_in["inertial_density"][mat].is_null()) {  //set to 0 if not provided for that material
           inputs.physics_list[phys].inertial_density.push_back(0);
        }
        else{
          inputs.physics_list[phys].inertial_density.push_back(json_in["inertial_density"][mat]);
        }
      }

      inputs.physics_list[phys].reference_value = 0;      //set default displacement ref value
      break;

    case 1:                                              //thermal equation
      if (json_in["reference value"].is_null())     //set to 0 if not provided for that material
        inputs.physics_list[phys].reference_value = 300;
      else
        inputs.physics_list[phys].reference_value = json_in["reference value"];

      break;
    }

   
    //store a vector of vectors for g_id
    for (uint i = 0; i < json_in["g_id"].size(); i++) {  //loop for each g_id entry
      std::vector<int> g_id_entry;
      
      //1st IC parameter:
      //optional string input for geometry type
      if (json_in["g_id"][i][0].is_string()) {
        if (json_in["g_id"][i][0] == "vertex")
          g_id_entry.push_back(1);
        else if (json_in["g_id"][i][0] == "curve")
          g_id_entry.push_back(2);
        else if (json_in["g_id"][i][0] == "surface")
          g_id_entry.push_back(3);
        else if (json_in["g_id"][i][0] == "region")
          g_id_entry.push_back(4);
        else if (json_in["g_id"][i][0] == "patch")
          g_id_entry.push_back(5);
        else if (json_in["g_id"][i][0] == "interface")
          g_id_entry.push_back(7);
        else{
          cerr << "Invalid g_id geometry type\n";
          cerr << "First element in material regions must be:\n";
          cerr << "\"vertex\", \"curve\", \"surface\", \"region\", \"patch\", or \"interface\" or the integer equivalent\n";
          exit(1);
        }
      }
      //original number input
      else
        g_id_entry.push_back(json_in["g_id"][i][0]);

      //other IC parameters:
      for (uint j = 1; j < json_in["g_id"][i].size(); ++j) {         //loop for each value in each entry
        g_id_entry.push_back(json_in["g_id"][i][j]);
      }
      
      inputs.physics_list[phys].g_id.push_back(g_id_entry);
    }
    for (uint i = 0; i < json_in["replacements"].size(); i++) {
      inputs.physics_list[phys].ic_replacements.push_back(json_in["replacements"][i]);
    }

    fIC.close();
  }
}




//  reads the load file data into input data type
void read_loads (Input_Data &inputs)
{
  cout << "reading load files\n";

  //read each physics' load file
  for (int phys = 0; phys < inputs.number_of_physics; ++phys) {
    string load_file = inputs.physics_list[phys].physics_name + "_load.json";
    
    ifstream fload(load_file);

    if (!fload.good()) {    //file doesn't exist
      cout << load_file << " not found" << endl;
      continue;
    }

    json json_in;
 
    fload >> json_in;


    inputs.physics_list[phys].num_of_loading_steps = json_in["number_of_loading_steps"];

    //stop reading data if number_of_loading_steps is 0
    if (inputs.physics_list[phys].num_of_loading_steps <= 0) {
      break;
    }

    //set the size in advance
    inputs.physics_list[phys].loading_time_steps.reserve(inputs.physics_list[phys].num_of_loading_steps);

    if (json_in["loading_time_steps"].is_string()) {  //check whether to call the string parser
      parse_json_string(json_in["loading_time_steps"], inputs.physics_list[phys].loading_time_steps, inputs.physics_list[phys].num_of_loading_steps);
    }
    else{
      parse_json_list(json_in["loading_time_steps"], inputs.physics_list[phys].loading_time_steps, inputs.physics_list[phys].num_of_loading_steps);
    }

    //read loading_increments vectors
    for (uint i = 0; i < json_in["loading_increments"].size(); ++i) {
      std::vector<double> loads;
      if (json_in["loading_increments"][i].is_string()) {  //check whether to call the string parser
	parse_json_string(json_in["loading_increments"][i], loads, inputs.physics_list[phys].num_of_loading_steps);
      }
      else{
	parse_json_list(json_in["loading_increments"][i], loads, inputs.physics_list[phys].num_of_loading_steps);
      }

    inputs.physics_list[phys].loading_increments.push_back(loads);
    }

    fload.close();
    
  }
}


//reads cohesive properties data into input data structure
void read_co_props (Input_Data &inputs)
{
  cout << "reading co_props file\n";

  string coProps_file = "co_props.json";
  
  ifstream fcoProps(coProps_file);
  
  if (!fcoProps.good()) {    //file doesn't exist
    cout << coProps_file << " not found" << endl;
    return;
  }
  inputs.co_props_flag = true;

  json json_in;
 
  fcoProps >> json_in;

  if (!(json_in["number_of_cohesive_potentials"].is_null())) {
    //if size is given, make sure that there are that many elements in the file
    assert (json_in["number_of_cohesive_potentials"] == json_in["cohesive_potentials"].size()
	    && ".in.co_props file: number_of_cohesive_potentials doesn't match number of cohesive_potentials elements");
    inputs.number_of_cohesive_potentials = json_in["number_of_cohesive_potentials"];
  }
  else {
    inputs.number_of_cohesive_potentials = json_in["cohesive_potentials"].size();  //default to number of elements
  }

  for (int i = 0; i < inputs.number_of_cohesive_potentials; ++i) {
    Cohesive_Potential cohesive_potential;
    if (!(json_in["cohesive_potentials"][i]["type_of_potential"].is_null())) {
      cohesive_potential.type_of_potential = json_in["cohesive_potentials"][i]["type_of_potential"];
    }

    for (int j = 0; j < 4; ++j)
      if (!(json_in["cohesive_potentials"][i]["phenomenological_potentials"][j].is_null())) {
	cohesive_potential.phenomenological_potentials[j] = json_in["cohesive_potentials"][i]["phenomenological_potentials"][j];
      }

    inputs.cohesive_potentials.push_back(cohesive_potential);
  }


  fcoProps.close();
}


//reads normal_in data into input data structure
void read_normal_in (Input_Data &inputs)
{
  cout << "reading normal_in file\n";

  string normal_file = "normal_in.json";
  
  ifstream fnormal(normal_file);
  
  if (!fnormal.good()) {    //file doesn't exist
    cout << normal_file << " not found" << endl;
    return;
  }
  inputs.normal_in_flag = true;

  json json_in;
 
  fnormal >> json_in;


  if (!(json_in["microcell_volume"].is_null())) {   //use default if missing
    inputs.microcell_volume = json_in["microcell_volume"];
  }

  if (!(json_in["microcell_thickness"].is_null())) {
    inputs.microcell_thickness = json_in["microcell_thickness"];
  }

  for (int i = 0; i < 3; ++i)
    if (!(json_in["microcell_normal_vector"][i].is_null())) {
      inputs.microcell_normal_vector[i] = json_in["microcell_normal_vector"][i];
    }


  fnormal.close();
}



//read periodic file into inputs data structure
void read_periodic (Input_Data &inputs)
{
  cout << "reading periodic file\n";

  string periodic_file = "periodic.json";
  ifstream fperiodic(periodic_file);

  if (!fperiodic.good()) {    //file doesn't exist
    cout << periodic_file << " not found" << endl;
    return;
  }
  inputs.periodic_flag = true;   //.out.periodic file exists

  json json_in;
  fperiodic >> json_in;
   
  //loop for each array of pairs (eg: each row if following example file format) in "pairs" array
  for (uint set = 0; set < json_in["pairs"].size(); ++set) { 
    std::vector<TypeID> typeID_set;
    
    //loop for each pair in that set
    for (uint pair = 0; pair < json_in["pairs"][set].size(); ++pair) {
      TypeID typeID;
      typeID.ID = json_in["pairs"][set][pair]["ID"];  //read ID

      //read type parameter:
      //optional string input for geometry type
      if (json_in["pairs"][set][pair]["type"].is_string()) {
	      if (json_in["pairs"][set][pair]["type"] == "vertex")
	        typeID.type = 1;
	      else if (json_in["pairs"][set][pair]["type"] == "curve")
	        typeID.type = 2;
	      else if (json_in["pairs"][set][pair]["type"] == "surface")
	        typeID.type = 3;
	      else if (json_in["pairs"][set][pair]["type"] == "region")
	        typeID.type = 4;
	      else if (json_in["pairs"][set][pair]["type"] == "patch")
	        typeID.type = 5;
	      else if (json_in["pairs"][set][pair]["type"] == "interface")
	        typeID.type = 7;
	      else{
	        cerr << "Invalid periodic geometry type\n";
	        cerr << "\"type\" in periodic pair must be:\n";
	        cerr << "\"vertex\", \"curve\", \"surface\", \"region\", \"patch\", or \"interface\" or the integer equivalent\n";
            	exit(1);
	      }
      }
      else                                     //read original integer input
        typeID.type = json_in["pairs"][set][pair]["type"];

      typeID_set.push_back(typeID);            //add this pair to the current set
    }

    inputs.typeID_sets.push_back(typeID_set);  //add this set to the input's set vector

      
  }
  fperiodic.close();
}


//read output settings file into inputs data structure
void read_output_settings(Input_Data &inputs)
{
  cout << "reading output settings file\n";

  string output_settings_file = "output.json";
  
  ifstream foutput(output_settings_file);
  
  if (!foutput.good()) {    //file doesn't exist
    cout << output_settings_file << " not found" << endl;
    return;
  }
  inputs.output_settings_flag = true;

  json json_in;
  foutput >> json_in;

  if (!(json_in["reaction_filename"].is_null())) //use default if missing
    inputs.reaction_filename = json_in["reaction_filename"];
    
  if (!(json_in["exascale"].is_null())) //use default if missing
    inputs.exascale_settings = json_in["exascale"];

  for (uint i = 0; i < json_in["probs"].size(); ++i) {
    Prob prob;
    for (uint j = 0; j < json_in["probs"][i]["pairs"].size(); ++j){
      TypeID typeID;
      
      //read type parameter:
      //optional string input for geometry type
      if (json_in["probs"][i]["pairs"][j]["type"].is_string()) {
	      if (json_in["probs"][i]["pairs"][j]["type"] == "vertex")
	        typeID.type = 1;
	      else if (json_in["probs"][i]["pairs"][j]["type"] == "curve")
	        typeID.type = 2;
	      else if (json_in["probs"][i]["pairs"][j]["type"] == "surface")
	        typeID.type = 3;
	      else if (json_in["probs"][i]["pairs"][j]["type"] == "region")
	        typeID.type = 4;
	      else if (json_in["probs"][i]["pairs"][j]["type"] == "patch")
	        typeID.type = 5;
	      else if (json_in["probs"][i]["pairs"][j]["type"] == "interface")
	        typeID.type = 7;
	      else{
	        cerr << "Invalid periodic geometry type\n";
	        cerr << "\"type\" in output settigns prob pair must be:\n";
	        cerr << "\"vertex\", \"curve\", \"surface\", \"region\", \"patch\", or \"interface\" or the integer equivalent\n";
          	exit(1);
	      }
      }
      else                                     //read original integer input
        typeID.type = json_in["probs"][i]["pairs"][j]["type"];
      
      typeID.ID = json_in["probs"][i]["pairs"][j]["ID"];
      prob.geomTypeID.push_back(typeID);
    }
    prob.quantity_of_interest = json_in["probs"][i]["quantity_of_interest"];
    prob.is_cell_data = json_in["probs"][i]["is_cell_data"];
    prob.integrate = json_in["probs"][i]["integrate"];
    
    for (uint j = 0; j < json_in["probs"][i]["output_time_step"].size(); ++j)
      prob.outputTimeStep.push_back( json_in["probs"][i]["output_time_step"][j] );
    prob.out_filename = json_in["probs"][i]["output_filename"];
    
    inputs.probs.push_back(prob);
  }
  foutput.close();
}
