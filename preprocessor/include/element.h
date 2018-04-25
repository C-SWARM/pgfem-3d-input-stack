/* -*- mode: c++; -*- */
/* HEADER */
/**
 * AUTHORS:
 * Matt Mosby
 */

#pragma once
#ifndef CON3D_ELEMENT_H
#define CON3D_ELEMENT_H

#include "t3d_model.h"
#include <vector>
#include <iostream>
#include <utility>

class Element
{
 public:
  Element();
  Element(const size_t type);
  Element(const Element &e);
  ~Element(){};

  enum{EDGE=0,TRIA,QUAD,TETRA,PYRAM,WEDGE,HEX,
       Q_EDGE,Q_TRIA,Q_QUAD,Q_TETRA,Q_PYRAM,
       Q_WEDGE,Q_HEXA};

  void read(std::istream &in);
  void write(std::ostream &out) const;
  void write_coh(std::ostream &out) const;
  void write_material(std::ostream &out) const;

  // access functions
  size_t nnodes() const;
  void set_id(size_t i);
  size_t id() const;
  void set_cohesive(const bool c);
  bool cohesive() const;

  void set_type(const size_t t,
		const bool parallel = true);
  size_t type() const;
  void set_model_type(size_t t);
  void set_model_id(size_t i);
  size_t model_type() const;
  size_t model_id() const;
  size_t model_prop() const;
  T3dModel model() const;

  void set_material(const size_t mat, const size_t fib);
  size_t material_id() const;
  size_t fiber_id() const;
  const std::vector<size_t>& connectivity() const;

  static bool compare_model_type(const Element &a,
				 const Element &b);
  static bool compare_model_id(const Element &a,
			       const Element &b);
  static bool compare_model(const Element &a,
			    const Element &b);
  static bool compare_id(const Element &a,
			 const Element &b);

 private:
  void init();
  bool _cohesive;
  size_t _id;
  size_t _type;
  T3dModel _model;
  size_t _t3d_prop;
  size_t _mat_id;
  size_t _fib_id;
  std::vector<size_t> _conn;
  std::vector<int> _elem_data;
};
// I/O operators
inline std::istream& operator>>(std::istream &lhs, Element &rhs){ rhs.read(lhs); return lhs;}
inline std::ostream& operator<<(std::ostream &lhs, const Element &rhs)
{
  if(rhs.cohesive()) rhs.write_coh(lhs);
  else rhs.write(lhs);
  return lhs;
}

/**
 * \brief Container class for Element.
 */
class ElementList : public std::vector<Element>
{
 public:
  typedef std::pair<ElementList::iterator,ElementList::iterator> Range;
  void read(std::istream &in);
  void write(std::ostream &out) const;

  void sort_model();
  void sort_id();

  /**
   * \brief Find range of nodes with matching model type and id.
   *
   * \return iterators to [first,last) matching nodes.
   */
  ElementList::Range find_model_range(const T3dModel &m);
 private:
};
// I/O operators
inline std::istream& operator>>(std::istream &lhs,
				ElementList &rhs){rhs.read(lhs); return lhs;}
inline std::ostream& operator<<(std::ostream &lhs,
				const ElementList &rhs){rhs.write(lhs); return lhs;}

#endif
