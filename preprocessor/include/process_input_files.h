/* -*- mode: c++; -*- */
/* HEADER */
/**
 * AUTHORS:
 * Matt Mosby
 */

#pragma once
#ifndef CON3D_PROCESS_INPUT_H
#define CON3D_PROCESS_INPUT_H

#include "options.h"
#include "domain.h"
#include "periodic.h"
#include "header.h"
#include <vector>


typedef enum {MULTIPHYSICS_MECHANICAL,               
MULTIPHYSICS_THERMAL,               
MULTIPHYSICS_CHEMICAL,               
MULTIPHYSICS_NO} MULTIPHYSICS_ANALYSIS; 

typedef enum{MECHANICAL_Var_Displacement,
              MECHANICAL_Var_MacroDisplacement,
              MECHANICAL_Var_NodalPressure,
              MECHANICAL_Var_CauchyStress,
              MECHANICAL_Var_EulerStrain,
              MECHANICAL_Var_EffectiveStrain,
              MECHANICAL_Var_EffectiveStress,
              MECHANICAL_Var_CellProperty,
              MECHANICAL_Var_Damage,
              MECHANICAL_Var_Chi,
              MECHANICAL_Var_F,
              MECHANICAL_Var_P,
              MECHANICAL_Var_W,
              MECHANICAL_Var_ElementPressure,
              MECHANICAL_Var_ElementVolume, 
              MECHANICAL_Var_NO} MECHANICAL_Var;

typedef enum{THERMAL_Var_Temperature,
              THERMAL_Var_HeatFlux,
              THERMAL_Var_HeatGenerations,
                  Thermal_Var_NO} THERMAL_Var;

typedef enum{CHEMICAL_VAR_SPECIES,
              CHEMICAL_Var_NO} CHEMICAL_Var; 


typedef struct {
  int physicsno;      /// number of physics 
  char **physicsname; /// physics names
  int *physics_ids;   /// physics ids
  int *ndim;          /// degree of feedom of the physics 
  int *write_no;      /// number of variables to be written as results
  int total_write_no; /// total number of variables to be written as results
  int **write_ids;    /// index of physical varialbes to be written
  int **coupled_ids;  /// coupled physics id
} MULTIPHYSICS;

int construct_multiphysics(MULTIPHYSICS *mp, int physicsno) ;
int read_multiphysics_settings(MULTIPHYSICS *mp, const Options &opt);


/**
 * \brief Reads in the header file and prepares the data for
 * processing.
 */
void process_header_file(const Options &opt,
			 Header &header);

//void process_header_struct(int argc, char **argv);
void process_inputs(Header &header, Input_Data inputs);

/**
 * \brief Reads in the periodic file and prepares the data for
 * processing.
 */
void process_periodic_file(const Options &opt,
			   Periodic &periodic);

void process_mesh_files(const Options &opt,
			Domains &domains);

void printT3d(Header &header);

#endif
