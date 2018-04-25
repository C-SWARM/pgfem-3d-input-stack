/* HEADER */
/**
 * AUTHORS:
 * Matt Mosby
 */

#include "periodic.h"
#include "utils.h"
#include <algorithm>
#include <fstream>

template<class PerList>
static void read_periodic_list(std::istream &in,
			       const size_t n,
			       PerList &list);
template<class PerList>
static void write_periodic_list(std::ostream &out,
				PerList &list);

typedef std::vector< T3dModel > PSetData;

template<size_t N>
PeriodicSet<N>::PeriodicSet() : _data(N){}

template<size_t N>
void PeriodicSet<N>::read(std::istream &in)
{
  PSetData::iterator it;
  for(it = _data.begin(); it != _data.end(); ++it){
    in >> *it;
  }
  this->sort();
}

template<size_t N>
void PeriodicSet<N>::write(std::ostream &out) const
{
  static const char t = '\t';
  PSetData::const_iterator it;
  for(it = _data.begin(); it != _data.end(); ++it){
    out << *it << t;
  }
}

template<size_t N>
void PeriodicSet<N>::sort()
{
  std::sort(_data.begin(),_data.end(),T3dModel::compare);
}

template<size_t N>
size_t PeriodicSet<N>::model_type(size_t i) const
{
  return _data.at(i).type();;
}

template<size_t N>
size_t PeriodicSet<N>::model_id(size_t i) const
{
  return _data.at(i).id();
}

template<size_t N>
T3dModel PeriodicSet<N>::model(size_t i) const
{
  return _data.at(i);
}

template<size_t N>
size_t PeriodicSet<N>::size() const
{
  return _data.size();
}

// instatniate template class
template class PeriodicSet<2>;
template class PeriodicSet<4>;

/*====== Periodic ======*/
Periodic::Periodic() : _bounds(3,0.0) {}

void Periodic::read(std::istream &in)
{
  read_periodic_list(in,_edge,y_edges);
  read_periodic_list(in,_edge,z_edges);
  read_periodic_list(in,_edge,x_edges);
  read_periodic_list(in,_face,z_faces);
  read_periodic_list(in,_face,x_faces);
  read_periodic_list(in,_face,y_faces);
}

void Periodic::write(std::ostream &out) const
{
  write_periodic_list(out,y_edges);
  write_periodic_list(out,z_edges);
  write_periodic_list(out,x_edges);
  write_periodic_list(out,z_faces);
  write_periodic_list(out,x_faces);
  write_periodic_list(out,y_faces);
}

void Periodic::set_bounds(const std::vector<double> &b)
{
  _bounds = b;
}

const std::vector<double>& Periodic::bounds() const
{
  return _bounds;
}

double Periodic::x_length() const
{
  return _bounds[0];
}

double Periodic::y_length() const
{
  return _bounds[1];
}

double Periodic::z_length() const
{
  return _bounds[2];
}

/*====== Non-Member Functions ======*/
template<class PerList>
static void read_periodic_list(std::istream &in,
			       const size_t n,
			       PerList &list)
{
  size_t size = -1;
  in >> size;
  if(size%n != 0){
    std::cerr << "ERROR: in *.out.periodic file,"
	      << " incorrect size of the periodic list!\n";
    abort();
  }
  size /= n;
  list.resize(size);
  in >> list;
}

template<class PerList>
static void write_periodic_list(std::ostream &out,
				PerList &list)
{
  out << list.size() << std::endl;
  typename PerList::const_iterator it;
  for(it = list.begin(); it != list.end(); ++it){
    out << *it << std::endl;
  }
  out << std::endl;
}
