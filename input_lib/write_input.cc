/**
 * AUTHORS:
 * Aaron Howell
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "structures.h"
#include "write_input.h"
#include <sys/stat.h>  //for creating directories

#include <iostream>
#include <fstream>

using namespace std;



//writes multiphysics.in data to file using the data structure
void write_multiphysics_in(const Input_Data inputs, const char *filebase)
{
  //if multiphysics_in.json file wasn't provided
  if (!inputs.multiphysics_file_exists) {  
    return;
  }

  ofstream fmultiphysics_in;

  string fileToWrite = filebase;
  fileToWrite += "/multiphysics.in";

  fmultiphysics_in.open(fileToWrite);
  
  fmultiphysics_in << inputs.number_of_physics << "\n";

  //loop through and print each physics data
  for (const auto& physics : inputs.physics_list) {
    fmultiphysics_in << physics.equation_id << " "; 
    fmultiphysics_in << physics.physics_name << " ";
    fmultiphysics_in << physics.degree_of_freedom << "\n";

    fmultiphysics_in << physics.num_of_coupled_physics << " ";
    
    for (int j = 0; j < physics.num_of_coupled_physics; ++j) {
      fmultiphysics_in << physics.coupled_physics_ids[j] << " ";
    }
    fmultiphysics_in << endl;


    fmultiphysics_in << physics.num_vars_to_print << endl;
 
    for(int j = 0; j < physics.num_vars_to_print; ++j) {
      fmultiphysics_in << physics.vars_to_print[j] << " ";
    }
    fmultiphysics_in << "\n\n"; 
  }

  cout << "wrote to " << fileToWrite << endl;
  fmultiphysics_in.close();
}

//writes material.mat data to file using the data structure
void write_material_properties(const Input_Data inputs, const char *filebase)
{
  //*****write common material properties (material.mat)*****
  ofstream fmaterial;

  string fileToWrite = filebase;
  fileToWrite += "/Material";

  //create directory at filebase/Material
  mode_t mode = 0755;
  mkdir(fileToWrite.c_str(), mode);  //mkdir takes a character array
  

  fileToWrite += "/material.mat";

  fmaterial.open(fileToWrite);

  for (int mat = 0; mat < inputs.number_of_materials; ++mat) {
    fmaterial << inputs.density[mat] << "\n";
  }

  cout << "wrote to " << fileToWrite << endl;
  fmaterial.close();

  //*****write physics material properties (physics_name.mat)*****
  for (int phys = 0; phys < inputs.number_of_physics; ++phys) {

    fileToWrite = filebase;
    fileToWrite += "/Material";
    fileToWrite += "/" + inputs.physics_list[phys].physics_name + ".mat";
    fmaterial.open(fileToWrite);

    switch (inputs.physics_list[phys].equation_id) {
    case 0:  //momentum equataion data
      for (const auto& mat : inputs.material_list) {  //loop through each material

        fmaterial << mat.youngs_modulus[0] << " ";
        fmaterial << mat.youngs_modulus[1] << " ";
        fmaterial << mat.youngs_modulus[2] << endl;

        fmaterial << mat.shear_modulus[0] << " ";
        fmaterial << mat.shear_modulus[1] << " ";
        fmaterial << mat.shear_modulus[2] << endl;

        fmaterial << mat.poissons_ratio[0] << " ";
        fmaterial << mat.poissons_ratio[1] << " ";
        fmaterial << mat.poissons_ratio[2] << endl;

        fmaterial << mat.coefficient_of_thermal_expansion[0] << " ";
        fmaterial << mat.coefficient_of_thermal_expansion[1] << " ";
        fmaterial << mat.coefficient_of_thermal_expansion[2] << endl;

        fmaterial << mat.sig << endl;
        fmaterial << mat.strain_energy_function_dev << " ";
        fmaterial << mat.strain_energy_function_vol << endl << endl;
      }
      break;

    case 1:  //energy equation data
      for (const auto& mat : inputs.material_list) {  //loop through each material
        fmaterial << mat.heat_capacity << endl << endl;

        //print thermal conductivity
        fmaterial << mat.thermal_conductivity[0] << " ";
        fmaterial << mat.thermal_conductivity[1] << " ";
        fmaterial << mat.thermal_conductivity[2] << endl;
        fmaterial << mat.thermal_conductivity[3] << " ";
        fmaterial << mat.thermal_conductivity[4] << " ";
        fmaterial << mat.thermal_conductivity[5] << endl;
        fmaterial << mat.thermal_conductivity[6] << " ";
        fmaterial << mat.thermal_conductivity[7] << " ";
        fmaterial << mat.thermal_conductivity[8] << endl << endl;
        fmaterial << mat.FHS_MW << endl << endl;
      }
      fmaterial << inputs.heatSource << endl;
      break;
    case 2:  //chemistry data
      fmaterial << inputs.number_of_materials << endl;
      fmaterial << inputs.nReactions << endl;
      fmaterial << inputs.rho_mixture << endl;
      fmaterial << inputs.diffusivityCoefficient << endl;
      fmaterial << inputs.diffActivationEnergyByR << endl;
      fmaterial << inputs.bcHeatTime << endl;
    
      //write reaction data
      for (const auto& reaction : inputs.reaction_list){
        fmaterial << endl << reaction.reactionCoefficients << endl;
        fmaterial << endl << reaction.bwdReactionCoefficients << endl;
        fmaterial << reaction.oneByEquiliReactionCoefficients << endl;
        fmaterial << reaction.temperatureExponents << endl;
        fmaterial << reaction.heatReactions << endl;
        fmaterial << reaction.activationEnergyByR << endl;
        fmaterial << reaction.bwdActivationEnergyByR << endl;
      
        for (int mat = 0; mat < inputs.number_of_materials; ++mat){
          fmaterial << reaction.stoichiometricConstants[mat] << " ";
          fmaterial << reaction.speciesExponents[mat] << endl;
        }
      }
    
      //write material-specific data
      for (const auto& mat : inputs.material_list) {  //loop through each material
        fmaterial << endl << mat.molecularWeights << " ";
        fmaterial << mat.initialMassFractions <<  " ";
        fmaterial << mat.initialVolumeFractions << " ";
        fmaterial << mat.formationHeat;
      }
      break;
    }

    cout << "wrote to " << fileToWrite << endl;
    fmaterial.close();
  }
}


//writes .in.st data to file using the data structure
//This file's name depends on the base filename (filebase/base_filename.in.st)
void write_in_st(const Input_Data inputs, const char *st_namebase)
{
  ofstream fin_st;
  string fileToWrite = st_namebase;

  fileToWrite += ".in.st";

  fin_st.open(fileToWrite);


  fin_st << inputs.Newton_Raphson_tolerance << " " 
	 << inputs.Newton_Raphson_max_num_of_iterations << " " 
	 << inputs.pressure_num << " " 
	 << inputs.solution_scheme << " ";

  //print additional data for method 4
  if (inputs.solution_scheme == 4)
  {
    fin_st << inputs.number_of_physics << " "
           << inputs.max_number_of_staggering  << endl << endl;

    for (const auto& physics : inputs.physics_list) {
      fin_st << physics.max_num_of_iterations << " "
	     << (int) physics.compute_initial_residuals;
      //optional parameter if compute_initial_residuals is true
      if (physics.compute_initial_residuals)
	fin_st << " " << physics.value_for_initial_residuals;
      fin_st << endl;
    }
  }
  else
    fin_st << endl;

  fin_st << endl << inputs.number_of_time_steps << "\n";

  //print all timesteps
  for (int i = 0; i <= inputs.number_of_time_steps; ++i) {
    fin_st.precision(15);
    fin_st << inputs.time_steps[i] << " ";
  }

  fin_st << "\n\n" << inputs.number_of_steps_to_print << endl;
  for (int i = 0; i < inputs.number_of_steps_to_print; ++i) {
    fin_st << (inputs.list_of_steps_to_print)[i] << " ";
  }
  fin_st << endl;

  cout << "wrote to " << fileToWrite << endl;
  fin_st.close();
}


//writes solver mechanical load data to file using the data structure
void write_loads(const Input_Data inputs, const char *filebase)
{
  //loop through each physics
  for (int phys = 0; phys < inputs.number_of_physics; ++phys) {
    ofstream fload;
    string fileToWrite = filebase;
    fileToWrite += "/load";

    if (phys == 0) {
      //create directory at filebase/load during the first iteration
      mode_t mode = 0755;
      mkdir(fileToWrite.c_str(), mode);  //mkdir takes a character array
    }
 
    fileToWrite += "/" + inputs.physics_list[phys].physics_name + ".load";

    fload.open(fileToWrite);

    //write to each physics' .load file
    fload << inputs.physics_list[phys].num_of_loading_steps << endl;

    for (int i = 0; i < inputs.physics_list[phys].num_of_loading_steps; ++i) {
      fload << inputs.physics_list[phys].loading_time_steps[i] << " ";
    }
    fload << "\n\n";  

    //output the first of each displacement's load, then the second, etc...
    for (int i = 0; i < inputs.physics_list[phys].num_of_loading_steps; ++i) {
      for (uint j = 0; j < inputs.physics_list[phys].loading_increments.size(); ++j){
	fload.precision(15);
	fload << inputs.physics_list[phys].loading_increments[j][i] << " ";
      }
    }

    cout << "wrote to " << fileToWrite << endl;
    fload.close();
  }
}

//writes co_props data to file using the data structure
//This file's name depends on the base filename (filebase/base_filename.in.co)
void write_co_props(const Input_Data inputs, const char *co_namebase)
{
  if (!inputs.co_props_flag) return;  //no co_props.json file found

  ofstream fin_co;
  string fileToWrite = co_namebase;

  fileToWrite += ".in.co_props";

  fin_co.open(fileToWrite);


  fin_co << inputs.number_of_cohesive_potentials << "\n\n\n\n";

  for (const auto& cohesive_potential : inputs.cohesive_potentials){
    fin_co << cohesive_potential.type_of_potential << " ";
    fin_co << cohesive_potential.phenomenological_potentials[0] << " ";
    fin_co << cohesive_potential.phenomenological_potentials[1] << " ";
    fin_co << cohesive_potential.phenomenological_potentials[2] << " ";
    fin_co << cohesive_potential.phenomenological_potentials[3] << "\n";
  }


  cout << "wrote to " << fileToWrite << endl;
  fin_co.close();
}

//writes normal_in data to file using the data structure
void write_normal_in(const Input_Data inputs, const char *filebase)
{

  if (!inputs.normal_in_flag) return;  //no co_props.json file found

  ofstream fnormal;
  string fileToWrite = filebase;

  fileToWrite += "/normal.in";

  fnormal.open(fileToWrite);


  fnormal << inputs.microcell_volume<< " ";
  fnormal << inputs.microcell_thickness<< " ";

  fnormal << inputs.microcell_normal_vector[0] << " ";
  fnormal << inputs.microcell_normal_vector[1] << " ";
  fnormal << inputs.microcell_normal_vector[2] << " ";


  cout << "wrote to " << fileToWrite << endl;
  fnormal.close();
}

//writes periodic data to file using the data structure
void write_periodic(const Input_Data inputs, const char *periodic_namebase) {

  if (!inputs.periodic_flag) return;  //no periodic file found

  ofstream fperiodic;
  string fileToWrite = periodic_namebase;

  fileToWrite += ".out.periodic";

  fperiodic.open(fileToWrite);

  for (const std::vector<TypeID> typeID_set : inputs.typeID_sets) {
    fperiodic << typeID_set.size() << "\n";   //first output size of the set

    for (const TypeID pair : typeID_set)
      fperiodic << pair.type << " " << pair.ID << " ";

    fperiodic << "\n\n";
  }

  cout << "wrote to " << fileToWrite << endl;
  fperiodic.close();
}

//writes output settings to file using the data structure
void write_output_settings(const Input_Data inputs, const char *filebase)
{
  //if output.json file wasn't provided
  if (!inputs.output_settings_flag) {  
    return;
  }

  ofstream foutput_config_in;

  string fileToWrite = filebase;
  fileToWrite += "/output_config.in";

  foutput_config_in.open(fileToWrite);
  

  foutput_config_in << inputs.reaction_filename << "\n";
  foutput_config_in << inputs.exascale_settings << "\n\n";
  foutput_config_in << inputs.probs.size() << "\n\n";

  //loop through and print each physics data
  for (const auto& prob : inputs.probs) {
    foutput_config_in << prob.MatchedNodeIDs.size() << "\n";
    for (const auto& nodeDomID : prob.MatchedNodeIDs)
      foutput_config_in << nodeDomID.dom << " " << nodeDomID.ID << " ";
    
    foutput_config_in << "\n" << prob.quantity_of_interest << "\n";
    foutput_config_in << prob.is_cell_data << "\n";
    foutput_config_in << prob.integrate << "\n";
    
    foutput_config_in << prob.outputTimeStep.size() << "\n";
    for (const auto& timestep : prob.outputTimeStep)
      foutput_config_in << timestep << " ";
    foutput_config_in << "\n" << prob.out_filename ;
    
    foutput_config_in << "\n\n\n";
  }
  
  cout << "wrote to " << fileToWrite << endl;
  foutput_config_in.close();
}
