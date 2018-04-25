/* -*- mode: c++; -*- */
/* HEADER */
/**
 * AUTHORS:
 * Matt Mosby
 */

#pragma once
#ifndef CON3D_NODE_H
#define CON3D_NODE_H

#include "t3d_model.h"
#include <cstdlib>
#include <iostream>
#include <vector>
#include <utility>

/**
 * \brief Class to contain node information.
 */
class Node{
public:
  Node();

  // read/write functions
  void read(std::istream &in);
  void read_serial(std::istream &in);
  void read_PGFem3D(std::istream &in);
  void write(std::ostream &out) const;

  // access functions
  void set_id(const size_t i);
  size_t id() const;

  void set_own(const size_t i);
  size_t own() const;

  void set_gid(long i);
  long gid() const;

  void set_coord(const std::vector<double> &c);
  void set_dofs(const std::vector<int> &d);
  const std::vector<double>& coord() const;
  const std::vector<int>& dofs() const;

  double x() const;
  double y() const;
  double z() const;

  void set_model_type(size_t t);
  void set_model_id(size_t i);
  T3dModel model() const;
  size_t model_type() const;
  size_t model_id() const;
  size_t model_prop() const;

 // static functions
  static bool compare_id(const Node &first,
			 const Node &second);
  static bool compare_gid(const Node &first,
			  const Node &second);
  static bool compare_own(const Node &first,
			  const Node &second);
  static bool compare_model(const Node &first,
			    const Node &second);
  static bool compare_x(const Node &first,
			const Node &second);
  static bool compare_y(const Node &first,
			const Node &second);
  static bool compare_z(const Node &first,
			const Node &second);
  static bool equal_x(const Node &first,
		      const Node &second);
  static bool equal_y(const Node &first,
		      const Node &second);
  static bool equal_z(const Node &first,
		      const Node &second);

private:
  size_t _id;
  size_t _own;
  long _gid;
  std::vector<double> _coord;
  T3dModel _model;
  size_t _t3d_prop;
  std::vector<int> _dofs;
};
// iostream operators
inline std::ostream& operator<<(std::ostream &lhs, const Node &rhs){ rhs.write(lhs); return lhs;}

/**
 * \brief Class for set of nodes. Inherits from std::list
 *
 * Provides routines to reorder list based on various node properties.
 */
class NodeList : public std::vector<Node>
{
 public:
  NodeList();
  typedef std::pair<NodeList::iterator, NodeList::iterator> Range;
  // i/o functions
  void read(std::istream &in);
  void read_PGFem3D(std::istream &in);
  void write(std::ostream &out) const;

  // sorting functions
  void sort_id();
  void sort_gid();
  void sort_own();
  void sort_model();

  /**
   * \brief Find range of nodes with matching model type and id.
   *
   * \return iterators to [first,last) matching nodes.
   */
  NodeList::Range find_model_range(const T3dModel &m);
  NodeList::Range find_model_range(const size_t type,
				   const size_t id);

  void set_parallel(bool p);
  bool parallel() const;
  private:
  bool _parallel;
};
// I/O operators
inline std::istream& operator>>(std::istream &lhs, NodeList &rhs){rhs.read(lhs); return lhs;}
inline std::ostream& operator<<(std::ostream &lhs, const NodeList &rhs){rhs.write(lhs); return lhs;}

#endif
