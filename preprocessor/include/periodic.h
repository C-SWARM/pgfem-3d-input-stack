/* -*- mode: c++; -*- */
/* HEADER */
/**
 * AUTHORS:
 * Matt Mosby
 */

#pragma once
#ifndef CON3D_PERIODIC_H
#define CON3D_PERIODIC_H

#include "t3d_model.h"

#include <vector>
#include <utility>
#include <iostream>

/**
 * \brief Object stores and manipulates the 
 *
 */
template<size_t N>
class PeriodicSet
{
 public:
  PeriodicSet();
  /**
   * \brief Reads a PeriodicSet from a stream.
   *
   * Also sorts the set in order of decreasing model_type then
   * increasing model_id.
   */
  void read(std::istream &in);

  /**
   * \brief Writes a PeriodicSet to a stream.
   *
   * Typically used for debugging purposes.
   */
  void write(std::ostream &out) const;

  /**
   * \brief Sorts the periodic sets in order of decreasing entity type
   * and then increasing entity id.
   */
  void sort();

  /// Get the model type of the i-th periodic entity
  size_t model_type(size_t i) const;

  /// Get the model type of the i-th periodic entity
  size_t model_id(size_t i) const;

  /// Get the i-th periodic entity
  T3dModel model(size_t i) const;

  /// Get the number of periodic entities
  size_t size() const;

 private:
  std::vector< T3dModel > _data;
};
// I/O
template<size_t N>
inline std::istream& operator>>(std::istream &lhs,
				PeriodicSet<N> &rhs){rhs.read(lhs); return lhs;}
template<size_t N>
inline std::ostream& operator<<(std::ostream &lhs,
				const PeriodicSet<N> &rhs){rhs.write(lhs); return lhs;}

// typedefs for readability
typedef PeriodicSet<4> EdgePeriodicSet;
typedef PeriodicSet<2> FacePeriodicSet;

/**
 * \brief Container for information given in the *.out.periodic file.
 */
class Periodic
{
public:
  Periodic();

  /**
   * \brief Read contents for *.out.periodic file
   */
  void read(std::istream &in);

  /**
   * \brief Write function predominantly used for debugging.
   */
  void write(std::ostream &out) const;

  void set_bounds(const std::vector<double> &b);
  const std::vector<double>& bounds() const;
  double x_length() const;
  double y_length() const;
  double z_length() const;

  // edges are in the specified direction
  std::vector< EdgePeriodicSet > x_edges;
  std::vector< EdgePeriodicSet > y_edges;
  std::vector< EdgePeriodicSet > z_edges;

  // direction specifies normal to the face
  std::vector< FacePeriodicSet > x_faces;
  std::vector< FacePeriodicSet > y_faces;
  std::vector< FacePeriodicSet > z_faces;

private:
  std::vector<double> _bounds;
  static const size_t _edge = 4;
  static const size_t _face = 2;
};

#endif
