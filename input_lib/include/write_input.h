#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
//#include "structures.h"

extern const int MAX_CHAR;

//writes data to .in.st file using the data structure
void write_in_st(const Input_Data inputs, const char *st_namebase);

//writes material.mat data to file using the data structure
void write_material_properties(const Input_Data inputs, const char *filebase);

//writes multiphysics.in data to file using the data structure
void write_multiphysics_in(const Input_Data inputs, const char *filebase);

//writes boundary conditions to files using the data structure
void write_boundary_conditions (const Input_Data inputs, const char *filebase);

//writes load data to files using the data structure
void write_loads(const Input_Data inputs, const char *filebase);

//writes .in.co_props data to file using the data structure
void write_co_props(const Input_Data inputs, const char *co_namebase);

//writes normal_in data to files using the data structure
void write_normal_in(const Input_Data inputs, const char *filebase);

//writes periodic data to file using the data structure
void write_periodic(const Input_Data inputs, const char *periodic_namebase);

//writes output settings to file using the data structure
void write_output_settings(const Input_Data inputs, const char *filebase);
