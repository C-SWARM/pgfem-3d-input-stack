/* -*- mode: c++; -*- */
/* HEADER */
/**
 * AUTHORS:
 * Matt Mosby
 * Ivan Viti
 */

#pragma once
#ifndef CON3D_MATERIAL_H
#define CON3D_MATERIAL_H

#include "t3d_model.h"
#include <vector>
#include <iostream>
#include <utility>
#include "structures.h"
/**
 * \brief Object for storing the material properties.
 */
;

class Heat
{
public:
  Heat();
  void read(std::istream &in);
  void write(std::ostream &out) const;
  double readHeat(int j);
  Heat *fillHeat(std::vector<double> tempVec);
private:
  std::vector<double> _props;
};
// I/O operators
inline std::istream& operator>>(std::istream& lhs,
         Heat &rhs){rhs.read(lhs); return lhs;}
inline std::ostream& operator<<(std::ostream& lhs,
         const Heat &rhs){rhs.write(lhs); return lhs;}
                 
class HeatList : public std::vector<Heat>
{
public:
  void read(std::istream &in);
  void write(std::ostream &out) const;
};
// I/O operators
 inline std::istream& operator>>(std::istream& lhs,
         HeatList &rhs){rhs.read(lhs); return lhs;}
 inline std::ostream& operator<<(std::ostream& lhs,
         const HeatList &rhs){rhs.write(lhs); return lhs;}


                 
class Material
{
public:
  Material();
  void read(std::istream &in);
  void write(std::ostream &out) const;
	double readMat(int j);
	double readFlag(int j);
	Material *fillMaterial(std::vector<double> tempVec,std::vector<double> tempFlags);

private:
  std::vector<double> _props;
  std::vector<size_t> _flags;
};
// I/O operators
inline std::istream& operator>>(std::istream& lhs,
				Material &rhs){rhs.read(lhs); return lhs;}
inline std::ostream& operator<<(std::ostream& lhs,
				const Material &rhs){rhs.write(lhs); return lhs;}

/**
 * \brief Container for material property objects.
 */
class MaterialList : public std::vector<Material>
{
public:
  void read(std::istream &in);
  void write(std::ostream &out) const;
};
// I/O operators
inline std::istream& operator>>(std::istream& lhs,
				MaterialList &rhs){rhs.read(lhs); return lhs;}
inline std::ostream& operator<<(std::ostream& lhs,
				const MaterialList &rhs){rhs.write(lhs); return lhs;}


/**
 * \brief Material assignment object.
 */
class MaterialAssignment
{
public:
  MaterialAssignment();

  void read(std::istream &in);
  void write(std::ostream &out) const;

  size_t model_type() const;
  size_t model_id() const;
  size_t material_id() const;
  size_t fiber_id() const;

  T3dModel model() const;
  static bool compare_model(const MaterialAssignment &a,
			    const MaterialAssignment &b);
	
	std::vector <double> grabT3d(std::vector<double> claw);
	void putT3d(Input_Data inputs, int i);
private:
  /// enumeration for extracting data
  T3dModel _model;
  std::pair<size_t,size_t> _mat_fib;
};
// I/O operators
inline std::istream& operator>>(std::istream& lhs,
				MaterialAssignment &rhs){rhs.read(lhs); return lhs;}
inline std::ostream& operator<<(std::ostream& lhs,
				const MaterialAssignment &rhs){rhs.write(lhs); return lhs;}

/**
 * \brief Container for model material assignment.
 */
class MatAssignList : public std::vector<MaterialAssignment>
{
public:
  void read(std::istream &in);
  void write(std::ostream &out) const;
  void sort_model();
};
// I/O operators
inline std::istream& operator>>(std::istream& lhs,
				MatAssignList &rhs){rhs.read(lhs); return lhs;}
inline std::ostream& operator<<(std::ostream& lhs,
				const MatAssignList &rhs){rhs.write(lhs); return lhs;}

#endif
