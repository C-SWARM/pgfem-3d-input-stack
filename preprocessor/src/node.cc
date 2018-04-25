/* HEADER */
/**
 * AUTHORS:
 * Matt Mosby
 */

#include "node.h"
#include "utils.h"
#include <iomanip>
#include <algorithm>

Node::Node():
  _id(0),
  _own(0),
  _gid(-1),
  _coord(3,0.0),
  _model(-1,-1),
  _t3d_prop(0)
{}

void Node::read(std::istream &in)
{
  // Function reads in from t3d2psifel format. Numbers are adjusted to
  // start from 0
  in >> _id >> _gid >> _own
     >> _coord[0] >> _coord[1] >> _coord[2]
     >> _model >> _t3d_prop;

  // decrement id numbers to start from 0
  _id -= 1;
  _gid -= 1;
  _own -= 1;
}

void Node::read_PGFem3D(std::istream &in)
{
  // Function reads in PGFem3D input format.
  in >> _gid >> _own >> _id
     >> _coord[0] >> _coord[1] >> _coord[2]
     >> _model >> _t3d_prop;
}

void Node::read_serial(std::istream &in)
{
  // Function reads in from t3d2psifel format. Numbers are adjusted to
  // start from 0
  in >> _id  >> _coord[0] >> _coord[1] >> _coord[2]
     >> _model >> _t3d_prop;

  // decrement id numbers to start from 0
  _id -= 1;
}


void Node::write(std::ostream &out) const
{
  // get original formatting
  std::ios::fmtflags f(out.flags());
  int w = 4;
  out << std::setw(w) << gid() << ' '
      << std::setw(w) << own() << ' '
      << std::setw(w) <<  id() << '\t';

  // set formatting for doubles
  out.precision(8);
  out << std::scientific;
  w = 16;
  for(std::vector<double>::const_iterator it = _coord.begin(),e = _coord.end();
      it != e; ++it){
    out << std::setw(w) << *it << ' ';
  }
  out << '\t';

  // reset formatting
  out.flags(f);
  w = 4;
  out << model_type() << ' ' 
      << std::setw(w) << model_id() << ' '
      << model_prop();
}

void Node::set_id(const size_t i)
{
  _id = i;
}

size_t Node::id() const
{
  return _id;
}

void Node::set_own(const size_t i)
{
  (_own = i);
}

size_t Node::own() const
{
  return _own;
}

void Node::set_gid(long i)
{
  (_gid = i);
}

long Node::gid() const
{
  return _gid;
}

void Node::set_coord(const std::vector<double> &c)
{
  _coord = c;
}

void Node::set_dofs(const std::vector<int> &d)
{
  _dofs = d;
}

const std::vector<double>& Node::coord() const
{
  return _coord;
}

const std::vector<int>& Node::dofs() const
{
  return _dofs;
}

double Node::x() const
{
  return _coord[0];
}

double Node::y() const
{
  return _coord[1];
}

double Node::z() const
{
  return _coord[2];
}

T3dModel Node::model() const
{
  return _model;
}

void Node::set_model_type(size_t t)
{
  _model.set_type(t);
}

void Node::set_model_id(size_t i)
{
  _model.set_id(i);
}

size_t Node::model_type() const
{
  return _model.type();
}

size_t Node::model_id() const
{
  return _model.id();
}

size_t Node::model_prop() const
{
  return _t3d_prop;
}

// static functions
bool Node::compare_id(const Node &first,
		      const Node &second)
{
  return (first.id() < second.id());
}

bool Node::compare_gid(const Node &first,
		       const Node &second)
{
  return (first.gid() < second.gid());
}

bool Node::compare_own(const Node &first,
		       const Node &second)
{
  return (first.own() < second.own());
}

bool Node::compare_model(const Node &first,
			 const Node &second)
{
  return T3dModel::compare(first.model(),second.model());
}

bool Node::compare_x(const Node &first,
		     const Node &second)
{
  return (first.x() < second.x());
}

bool Node::compare_y(const Node &first,
		     const Node &second)
{
  return (first.y() < second.y());
}

bool Node::compare_z(const Node &first,
		     const Node &second)
{
  return (first.z() < second.z());
}

bool Node::equal_x(const Node &first,
		   const Node &second)
{
  return is_equal(first.x(),second.x());
}

bool Node::equal_y(const Node &first,
		   const Node &second)
{
  return is_equal(first.y(),second.y());
}

bool Node::equal_z(const Node &first,
		   const Node &second)
{
  return is_equal(first.z(),second.z());
}

/*====== NodeList ======*/
NodeList::NodeList():_parallel(true){}

void NodeList::read(std::istream &in)
{
  NodeList::iterator it;
  if(parallel()){
    for(it = this->begin(); it != this->end(); ++it){
      it->read(in);
    }
  } else {
    for(it = this->begin(); it != this->end(); ++it){
      it->read_serial(in);
    }
  }
}

void NodeList::read_PGFem3D(std::istream &in)
{
  for(NodeList::iterator it = this->begin(); it != this->end(); ++it){
    it->read_PGFem3D(in);
  }
}

void NodeList::write(std::ostream &out) const
{
  NodeList::const_iterator it;
  for(it = this->begin(); it != this->end(); ++it){
    out << *it << std::endl;
  }
}

void NodeList::sort_id()
{
  std::sort(begin(), end(), Node::compare_id);
}

void NodeList::sort_gid()
{
  std::sort(begin(), end(), Node::compare_gid);
}

void NodeList::sort_own()
{
  std::sort(begin(), end(), Node::compare_own);
}

void NodeList::sort_model()
{
  std::sort(begin(), end(), Node::compare_model);
}

NodeList::Range NodeList::find_model_range(const T3dModel &m)
{
  return this->find_model_range(m.type(),m.id());
}

NodeList::Range NodeList::find_model_range(const size_t type,
					   const size_t id)
{
  Node tmp;
  tmp.set_model_type(type);
  tmp.set_model_id(id);
  return std::equal_range(this->begin(),this->end(),tmp,
			  Node::compare_model);
}

void NodeList::set_parallel(bool p)
{
  _parallel = p;
}

bool NodeList::parallel() const
{
  return _parallel;
}
