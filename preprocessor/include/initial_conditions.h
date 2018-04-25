/* -*- mode: c++; -*- */
/* HEADER */
/**
 * AUTHORS:
 * Aaron Howell
 * Matt Mosby
 * Ivan Viti
 */

#pragma once
#ifndef CON3D_IC_H
#define CON3D_IC_H

#include "t3d_model.h"
#include <cstdlib>
#include <vector>
#include <iostream>
#include "structures.h"

/**
 * \brief Base IC object.
 *
 * Contains the boundary condition information.
 */
template<typename T>
class BaseIC
{
public:
  BaseIC():_ic(){};

  void set_ic(const std::vector<T> &ic);
  void set_ic_0(T x0);
  void set_ic_1(T x1);
  void set_ic_2(T x2);
  void set_ic_3(T x3);
  void set_ic_4(T x4);
  void set_ic_5(T x5);

  const std::vector<T>& ic() const;
  T ic_0() const;
  T ic_1() const;
  T ic_2() const;
  T ic_3() const;
  T ic_4() const;
  T ic_5() const;

  std::vector<T> _ic;
};



/**
 * \brief IC object for model entities.
 */
template<typename T>
class ModelIC : public BaseIC<T>
{
public:
  ModelIC():_model(-1,-1){}
  void set_model_type(const size_t t);
  void set_model_id(const size_t i);
  size_t model_type() const;
  size_t model_id() const;

  T3dModel model() const;

  void read(std::istream &in);
  void write(std::ostream &out) const;


  static bool compare_model(const ModelIC<T> &a, const ModelIC<T> &b);


//Human readable Stuff
  std::vector <double> grabT3d(std::vector<double> claw);
  void putIC(Input_Data inputs, int i, int physics);


private:
  T3dModel _model;
};
     


// I/O operators
template<typename T>
inline std::istream& operator>>(std::istream &lhs,
				ModelIC<T> &rhs){rhs.read(lhs); return lhs;}
template<typename T>
inline std::ostream& operator<<(std::ostream &lhs,
				const ModelIC<T> &rhs){rhs.write(lhs); return lhs;}


/**
 * \brief IC object for nodes.
 */
template<typename T>
class NodeIC : public BaseIC<T>
{
public:
  NodeIC():_node_id(-1) {}
  void set_node_id(size_t i);
  size_t node_id() const;

  void write(std::ostream &out) const;
  static bool compare_node_id(const NodeIC<T> &a, const NodeIC<T> &b);

private:
  size_t _node_id;
};
// I/O operators
template<typename T>
inline std::ostream& operator<<(std::ostream &lhs,
				const NodeIC<T> &rhs){rhs.write(lhs); return lhs;}

/**
 * \brief Container for all model entity boundary conditions.
 */
template<typename T>
class ModelICList : public std::vector< ModelIC<T> >
{
public:
  void read(std::istream &in);
  void write(std::ostream &out) const;
  void sort_model();
};


// I/O operators
template<typename T>
inline std::istream& operator>>(std::istream &lhs,
				ModelICList<T> &rhs){rhs.read(lhs); return lhs;}
template<typename T>
inline std::ostream& operator<<(std::ostream &lhs,
				const ModelICList<T> &rhs){rhs.write(lhs); return lhs;}

/**
 * \brief Container for all nodal boundary conditions.
 */
template<typename T>
class NodeICList : public std::vector< NodeIC<T> >
{
public:
  void write(std::ostream &out) const;
  void sort_node_id();

  void write_replacements(Input_Data inputs, int physics, std::ofstream &out);
};
// I/O operator
template<typename T>
std::ostream& operator<<(std::ostream &lhs,
			 const NodeICList<T> &rhs){rhs.write(lhs); return lhs;}

#endif
