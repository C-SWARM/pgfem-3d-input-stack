/* HEADER */
/**
 * AUTHORS:
 * Matt Mosby
 */

#include "basis.h"
#include "utils.h"

Basis::Basis():_data(9,0.0) {}

void Basis::read(std::istream &in)
{
  in >> _data;
}

void Basis::write(std::ostream &out) const
{
  out << this->e1() << '\t'
      << this->e2() << '\t'
      << this->e3() << std::endl;
}

std::vector<double> Basis::e1() const
{
  return (std::vector<double>(_data.begin(),_data.begin()+3));
}

std::vector<double> Basis::e2() const
{
  return (std::vector<double>(_data.begin()+3,_data.begin()+6));
}

std::vector<double> Basis::e3() const
{
  return (std::vector<double>(_data.begin()+6,_data.end()));
}

Basis *Basis::fillBasis(std::vector<double> tempDat) {
	int j;
	for (j = 0; j < 9; j++) {
		this->_data.at(j) = tempDat.at(j);
	}	
return this;
}


/*====== BasisList ======*/
void BasisList::read(std::istream &in)
{
  BasisList::iterator it;
  for(it = this->begin(); it != this->end(); ++it) in >> *it;
}

void BasisList::write(std::ostream &out) const
{
  BasisList::const_iterator it;
  for(it = this->begin(); it != this->end(); ++it) out << *it;
}
