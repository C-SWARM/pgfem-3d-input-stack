/* HEADER */
/**
 * AUTHORS:
 * Matt Mosby
 * Ivan Viti
 */

#include "material.h"
#include "utils.h"
#include <iomanip>
#include <algorithm>
#include <fstream>


/*conductivity properties*/
Heat::Heat() : _props(10,0.0) {}

void Heat::read(std::istream &in) 
{
  in >> _props;
}

void Heat::write(std::ostream &out) const
{
  int w = 16;
  out.precision(8);
  out << std::scientific;
  for(std::vector<double>::const_iterator it = _props.begin(), e = _props.end();
    it != e; ++it){
    out << std::setw(w) << *it << ' ';
  }

}

double Heat::readHeat(int j) {
  return _props.at(j);
}

Heat *Heat::fillHeat(std::vector<double> tempVec) {
  int j ;
  for (j = 0; j < 10; j++) {
    this->_props.at(j) = tempVec.at(j);
  }
 
return this;
}

/*====== Material ======*/
Material::Material() : _props(13,0.0),_flags(2,-1) {}

void Material::read(std::istream &in)
{
  in >> _props >> _flags;
}

void Material::write(std::ostream &out) const
{
 // get original formatting
  std::ios::fmtflags f(out.flags());

  // set formatting for doubles
  int w = 16;
  out.precision(8);
  out << std::scientific;
  for(std::vector<double>::const_iterator it = _props.begin(), e = _props.end();
      it != e; ++it){
    out << std::setw(w) << *it << ' ';
  }

  // reset formatting and output flags
  out.flags(f);
  out << '\t' << _flags;
}



double Material::readMat(int j) {
	return _props.at(j);
}

double Material::readFlag(int j) {
        return _flags.at(j);
}

Material *Material::fillMaterial(std::vector<double> tempVec,std::vector<double> tempFlags) {
	int j ;
	for (j = 0; j < 13; j++) {
		this->_props.at(j) = tempVec.at(j);
	}
 
	this->_flags.at(0) = tempFlags.at(0);
	this->_flags.at(1) = tempFlags.at(1);
return this;
}

/*==== conductivty list ====*/
void HeatList::read(std::istream &in)
{
  HeatList::iterator it;
  for(it = this->begin(); it != this->end(); ++it) in >> *it;
}

void HeatList::write(std::ostream &out) const
{
  HeatList::const_iterator it;
  for(it = this->begin(); it != this->end(); ++it)
    out << *it << std::endl;
}




/*====== MaterialList ======*/
void MaterialList::read(std::istream &in)
{
  MaterialList::iterator it;
  for(it = this->begin(); it != this->end(); ++it) in >> *it;
}

void MaterialList::write(std::ostream &out) const
{
  MaterialList::const_iterator it;
  for(it = this->begin(); it != this->end(); ++it)
    out << *it << std::endl;
}

/*======  MaterialAssignment ======*/
MaterialAssignment::MaterialAssignment():_model(-1,-1), _mat_fib(-1,-1){} //in _model, this stores geom type and geom id, while mat_fib store mp1 and mp2. -IV
void MaterialAssignment::read(std::istream &in)
{
  in >> _model >> _mat_fib.first >> _mat_fib.second;
}

void MaterialAssignment::write(std::ostream &out) const
{
  out << _model << " " << _mat_fib.first << "" << _mat_fib.second;
}

std::vector<double> MaterialAssignment::grabT3d(std::vector <double> claw) {
	
	claw.push_back(this->_model.type());
	claw.push_back(this->_model.id());

return claw;
}


void MaterialAssignment::putT3d(Input_Data inputs, int i) {


	this->_model.first  = inputs.geom_type[i];
	this->_model.second = inputs.geom_id[i];
	this->_mat_fib.first  = inputs.mp1[i];
	this->_mat_fib.second = inputs.mp2[i];
}


size_t MaterialAssignment::model_type() const
{
  return _model.type(); //geom type	-IV
}

size_t MaterialAssignment::model_id() const
{
  return _model.id(); // geom id	-IV
}

size_t MaterialAssignment::material_id() const
{
  return _mat_fib.first;
}

size_t MaterialAssignment::fiber_id() const
{
  return _mat_fib.second;
}

T3dModel MaterialAssignment::model() const
{
  return _model;
}

bool MaterialAssignment::compare_model(const MaterialAssignment &a,
				       const MaterialAssignment &b)
{
  return T3dModel::compare(a.model(),b.model());
}

/*======  MatAssignList ======*/
void MatAssignList::read(std::istream &in)
{
  size_t n_assignments = -1;
  in >> n_assignments;
  this->resize(n_assignments);
  MatAssignList::iterator it;
  for(it = this->begin(); it != this->end(); ++it) in >> *it;
}

void MatAssignList::write(std::ostream &out) const
{
  MatAssignList::const_iterator it;
  for(it = this->begin(); it != this->end(); ++it) out << *it << std::endl;
}

void MatAssignList::sort_model()
{
  std::sort(begin(), end(), MaterialAssignment::compare_model);
}
