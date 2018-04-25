/* -*- mode: c++; -*- */
/* HEADER */
/**
 * AUTHORS:
 * Matt Mosby
 * Ivan Viti
 * Aaron Howell
 */

#pragma once
#ifndef CON3D_HEADER_H
#define CON3D_HEADER_H

#include <cstdlib>
#include <vector>
#include <iostream>
#include "structures.h"

// pre-declare classes that we will hold (reduces file dependencies)
template<typename T> class ModelBCList; 
template<typename T> class ModelICList; 
template<typename T> class ModelThermList;
class MaterialList;
class HeatList;
class MatAssignList;
class BasisList;

/**
 * \brief Object to hold information contained in the *.out.header file.
 *
 * The information contained in this object pertains to ALL
 * domains. The data is not directly modifiable through this class
 * except by the read function.
 */
class Header
{
public:
  Header();
  ~Header();

  /**
   * \brief Primary function to read the header file.
   *
   * Performs necessary resizing of underlying datastructures and
   * fills data.
   */
  void read(std::istream &in);

  /**
   * \brief Output function primarily used for debugging.
   */
  void write(std::ostream &out) const;

  // Access
  size_t quadradic() const;
  size_t n_dim() const;
  size_t n_materials() const;
  size_t n_concentrations() const;
  size_t n_orientations() const;
  size_t max_lin_iter() const;
  double lin_tol() const;
  double epsilon() const;

  const HeatList* heat() const;
  const MaterialList* materials() const;
  const std::vector<double>& concentrations() const;
  const BasisList* orientations() const;
  const MatAssignList* mat_assignment() const;
  const ModelBCList<int>* disp_assignment() const;
  const ModelBCList<int>* disp_assignment(int i) const;
  const ModelICList<int>* get_initial_conditions(int i) const;
  const std::vector<double>& displacements() const;
  const std::vector<double>& therm_types() const;
  const ModelBCList<double>* force_assignment() const;
  const ModelThermList<int>* therm_list() const;
  // Operations
  void sort_and_unique_assignments() const;
  void sort_and_unique_mat_assignment() const;
  void sort_and_unique_disp_assignment() const;
  void sort_and_unique_force_assignment() const;


  //pass-convert
	void passQuadratic(int input);	
	void passNDim(int input);
	void passNiter(int input);
	void passLintol(double input);
	void passEpsilon(double input);
	void passCon(int input);
	
  //Human Readable Functions	
  double readMaterials(int i,int j);
  double readHeat(int i, int j);
  void fillMaterialList(Header &header, Input_Data inputs, int n_materials);
  void fillHeatList(Header &header,Input_Data inputs, int n_materials);
  void fillCon(Header &header, Input_Data inputs);
  void fillBasisList(Header &header, Input_Data inputs, int n_bases);
  std::vector<double> printT3d();
  void fillT3d(Input_Data inputs); 	
  void fillBC(Input_Data inputs, int physics);
  void fillIC(Input_Data inputs, int physics);
  void fillPrescribed(std::vector<double> old_disp, int n_disp);
  void fillPrescribed(std::vector<double> old_disp, int n_disp, int physics);
  void fillPrescribedThermal(double *old_thermal, int n_therm);
  void fillThermal(struct data_header header_struct);
  void allocateBCIC(int physics_num);
private:
  size_t _quadradic;
  size_t _ndim;
  size_t _niter;
  double _lin_tol;
  double _epsilon;
  
  MaterialList *_mat_list;
  HeatList *_heat_list;
  BasisList *_basis_list;
  std::vector<double> _con;
  MatAssignList *_mat_assign;
  ModelBCList<int> *_pre_disp;

  std::vector<ModelBCList<int> *> boundary_conditions;  //one per physics
  std::vector<ModelICList<int> *> initial_conditions;

  ModelThermList<int> *_thermal;
  std::vector<double> _disp;

  std::vector<std::vector<double>> bc_replacements;  //one per physics
  /*
    ICs also have replacements similar to BCs, but the replacements are
    performed before writing to the output file using data from the 
    input libary's data class
  */

  std::vector<double> _therm_types;
  ModelBCList<double> *_pre_force;
};




#endif
