/* HEADER */
/**
 * AUTHORS:
 * Matt Mosby
 */

#include "element.h"
#include "utils.h"
#include <cstdlib>
#include <iomanip>
#include <algorithm>

void Element::init()
{
  _cohesive = (false);
  _id = (0);
  _type = (0);
  _model = T3dModel::make_model(0,0);
  _t3d_prop = (0);
  _mat_id = -1;
  _fib_id = -1;
}

Element::Element() 
{init();}

Element::Element(const size_t type)
{
  this->init();
  this->set_type(type);
}

Element::Element(const Element &e)
{
  this->_cohesive = e._cohesive;
  this->_id = e._id;
  this->_type = e._type;
  this->_model = e._model;
  this->_t3d_prop = e._t3d_prop;
  this->_mat_id = e._mat_id;
  this->_fib_id = e._fib_id;
  this->_conn = e._conn;
  this->_elem_data = e._elem_data;
}

void Element::read(std::istream &in)
{
  in >> _id >> _conn >> _model
     >> _t3d_prop >> _elem_data;

  // decrement id and connectivity to start from 0
  _id -= 1;
  for(size_t i=0; i<_conn.size(); i++){
    _conn[i] -= 1;
  }
}

void Element::write(std::ostream &out) const
{
  int w = 6;
  out << std::setw(w) << id() << '\t';

  w = 5;
  out << std::setw(w) <<  _conn;

  w = 4;
  out << '\t' << model_type() << ' '
      << std::setw(w) << model_id() << ' '
      << model_prop() << '\t'
      << _elem_data;
}

void  Element::write_coh(std::ostream &out) const
{
  int w = 6;
  out << std::setw(w) << this->id() << '\t';

  w = 5;
  for(std::vector<size_t>::const_iterator it = _conn.begin(), e = _conn.end();
      it != e; ++it){
    out << std::setw(w) << *it << ' ';
  }

  out << '\t' << "0 " 
      << this->material_id() << ' '
      << this->model_prop();
}

void  Element::write_material(std::ostream &out) const
{
  out << std::setw(6) << this->id() << " " << this->material_id() << " "
      << this->fiber_id() << " 0 0\n";
}

size_t Element::nnodes() const
{
  return _conn.size();
}

void Element::set_id(size_t i){
  _id = i;
}

size_t  Element::id() const 
{
  return _id;
}

void  Element::set_cohesive(const bool c) 
{
  _cohesive = c;
}

bool  Element::cohesive() const 
{
  return _cohesive;
}

void Element::set_type(const size_t t,
		       const bool parallel)
{
  _type = t;
  switch(t){
  case TRIA:
    _conn.resize(3);
    _elem_data.resize(parallel?3:6);
    break;
  case QUAD:
    _conn.resize(4);
    _elem_data.resize(parallel?4:8);
    break;
  case TETRA:
    _conn.resize(4);
    _elem_data.resize(parallel?8:12);
  break;
  case Q_TETRA:
    _conn.resize(10);
    _elem_data.resize(parallel?8:12);
    break;
  case WEDGE:
    _conn.resize(6);
    _elem_data.resize(parallel?10:15);
    break;
  case HEX:
    _conn.resize(8);
    _elem_data.resize(parallel?12:18);
    break;
  default:
    std::cerr << "ERROR: Element type not supported! (" << _type << ")\n";
    abort();
    break;
  }
}

size_t  Element::type() const
{
  return _type;
}

void Element::set_model_type(size_t t)
{
  _model.set_type(t);
}

void Element::set_model_id(size_t i)
{
  _model.set_id(i);
}

size_t Element::model_type() const 
{
  return _model.type();
}

size_t Element::model_id() const 
{
  return _model.id();
}

size_t Element::model_prop() const 
{
  return _t3d_prop;
}

T3dModel Element::model() const
{
  return _model;
}

const std::vector<size_t>& Element::connectivity() const 
{
  return _conn;
}

void Element::set_material(const size_t mat, const size_t fib)
{
  _mat_id = mat;
  _fib_id = fib;
}

size_t Element::material_id() const
{
  return _mat_id;
}

size_t Element::fiber_id() const
{
  return _fib_id;
}

bool Element::compare_model_type(const Element &a,
				 const Element &b)
{
  return (a.model_type() > b.model_type());
}

bool Element::compare_model_id(const Element &a,
			       const Element &b)
{
  return (a.model_id() < b.model_id());
}

bool Element::compare_model(const Element &a,
			    const Element &b)
{
  return T3dModel::compare(a.model(),b.model());
}

bool Element::compare_id(const Element &a,
			 const Element &b)
{
  return (a.id() < b.id());
}

/*====== ElementList ======*/

void ElementList::read(std::istream &in)
{
  ElementList::iterator it;
  for(it = this->begin(); it != this->end(); ++it)
    in >> *it;
}

void ElementList::write(std::ostream &out) const
{
  ElementList::const_iterator it;
  for(it = this->begin(); it != this->end(); ++it)
    out << *it << std::endl;
}

void ElementList::sort_model()
{
  std::sort(begin(), end(), Element::compare_model);
}

void ElementList::sort_id()
{
  std::sort(begin(), end(), Element::compare_id);
}

ElementList::Range ElementList::find_model_range(const T3dModel &m)
{
  Element tmp;
  tmp.set_model_type(m.type());
  tmp.set_model_id(m.id());
  return std::equal_range(this->begin(),this->end(),tmp,
			  Element::compare_model);
}
