/* -*- mode: c++; -*- */
/* HEADER */
/**
 * AUTHORS:
 * Matt Mosby
 * Ivan Viti
 */

#pragma once
#ifndef CON3D_BC_H
#define CON3D_BC_H

#include "t3d_model.h"
#include <cstdlib>
#include <vector>
#include <iostream>
#include "structures.h"

/**
 * \brief Base BC object.
 *
 * Contains the boundary condition information.
 */
template<typename T>
class BaseBC
{
public:
  BaseBC():_bc(){};
  void set_bc(const std::vector<T> &bc);
  void set_bc_x(T x);
  void set_bc_y(T y);
  void set_bc_z(T z);
  const std::vector<T>& bc() const;
  T bc_x() const;
  T bc_y() const;
  T bc_z() const;

  std::vector<T> _bc;
};


template <typename T>
class BaseTherm
{
public:
  BaseTherm():_th(1,0){};
  void set_th(const std::vector<T> &th);
  void set_th_1(T temp);

  const std::vector<T>& th() const;
  T th_1() const;
protected:
  std::vector<T> _th;  
};

/**
 * \brief BC object for model entities.
 */
template<typename T>
class ModelBC : public BaseBC<T>
{
public:
  ModelBC():_model(-1,-1){}
  void set_model_type(const size_t t);
  void set_model_id(const size_t i);
  size_t model_type() const;
  size_t model_id() const;

  T3dModel model() const;

  void read(std::istream &in);
  void write(std::ostream &out) const;

  static bool compare_model(const ModelBC<T> &a, const ModelBC<T> &b);


//Human readable Stuff
  std::vector <double> grabT3d(std::vector<double> claw);
  void putT3d(Input_Data inputs, int i, int physics);


private:
  T3dModel _model;
};

//thermal_bc
template<typename T>
class ModelThermal : public BaseTherm<T>
{
public:
  ModelThermal():_model(-1,-1){}
  void set_model_type(const size_t t);
  void set_model_id(const size_t i);
  size_t model_type() const;
  size_t model_id() const;
  T3dModel model() const;
  static bool compare_model(const ModelThermal<T> &a, const ModelThermal<T> &b);
//Human readable Stuff
//  std::vector <double> grabT3d(std::vector<double> claw);
  void putTherm(Input_Data inputs, int i);
      
private:
  T3dModel _model;
};
                  


// I/O operators
template<typename T>
inline std::istream& operator>>(std::istream &lhs,
				ModelBC<T> &rhs){rhs.read(lhs); return lhs;}
template<typename T>
inline std::ostream& operator<<(std::ostream &lhs,
				const ModelBC<T> &rhs){rhs.write(lhs); return lhs;}


/**
 * \brief BC object for nodes.
 */
template<typename T>
class NodeBC : public BaseBC<T>
{
public:
  NodeBC():_node_id(-1) {}
  void set_node_id(size_t i);
  size_t node_id() const;

  void write(std::ostream &out) const;
  static bool compare_node_id(const NodeBC<T> &a, const NodeBC<T> &b);

private:
  size_t _node_id;
};
// I/O operators
template<typename T>
inline std::ostream& operator<<(std::ostream &lhs,
				const NodeBC<T> &rhs){rhs.write(lhs); return lhs;}

/**
 * \brief Container for all model entity boundary conditions.
 */
template<typename T>
class ModelBCList : public std::vector< ModelBC<T> >
{
public:
  void read(std::istream &in);
  void write(std::ostream &out) const;
  void sort_model();
};

template<typename T>
class ModelThermList : public std::vector< ModelThermal<T> >
{
public:
  void sort_model();
};

// I/O operators
template<typename T>
inline std::istream& operator>>(std::istream &lhs,
				ModelBCList<T> &rhs){rhs.read(lhs); return lhs;}
template<typename T>
inline std::ostream& operator<<(std::ostream &lhs,
				const ModelBCList<T> &rhs){rhs.write(lhs); return lhs;}

/**
 * \brief Container for all nodal boundary conditions.
 */
template<typename T>
class NodeBCList : public std::vector< NodeBC<T> >
{
public:
  void write(std::ostream &out) const;
  void sort_node_id();
};
// I/O operator
template<typename T>
std::ostream& operator<<(std::ostream &lhs,
			 const NodeBCList<T> &rhs){rhs.write(lhs); return lhs;}

#endif
