#pragma once

#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "structures.h"




//reads in all of the input files into input data structure
void read_input (Input_Data &inputs);

//reads timestep data into input data structure
void read_timesteps(Input_Data &inputs);

//reads material.mat data into input data structure
void read_material_properties(Input_Data &inputs);

//reads numerical.param data into input data structure
void read_numerical_param(Input_Data &inputs);

//reads multiphysics.in data into input data structure
void read_multiphysics(Input_Data &inputs);

//reads boundary conditions data to input data structure
void read_boundary_conditions (Input_Data &inputs);

//reads initial conditions to input data structure
void read_initial_conditions (Input_Data &inputs);

//reads Neumann boundary conditions to input data structure
void read_neumann_boundary_conditions(Input_Data &inputs);

//reads load data into input data structure
void read_loads (Input_Data &inputs);

//reads cohesive properties data into input data structure
void read_co_props (Input_Data &inputs);

//reads normal_in data into input data structure
void read_normal_in (Input_Data &inputs);

//reads periodic data into input data structure
void read_periodic (Input_Data &inputs);

//read output settings file into inputs data structure
void read_output_settings (Input_Data &inputs);

