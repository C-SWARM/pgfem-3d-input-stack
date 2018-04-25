/* -*- mode: c++; -*- */
/* HEADER */
/**
 * AUTHORS:
 * Matt Mosby
 */

#ifndef CON3D_BASIS_H
#define CON3D_BASIS_H

#include <vector>
#include <list>
#include <iostream>

/**
 * \brief Orientation basis object.
 */
class Basis
{
 public:
  Basis();
  void read(std::istream &in);
  void write(std::ostream &out) const;
  std::vector<double> e1() const;
  std::vector<double> e2() const;
  std::vector<double> e3() const;
	Basis *fillBasis(std::vector<double> tempDat);

 private:
  std::vector<double> _data;
};
// I/O
inline std::istream& operator>>(std::istream &lhs,
				Basis &rhs){rhs.read(lhs); return lhs;}
inline std::ostream& operator<<(std::ostream &lhs,
				const Basis &rhs){rhs.write(lhs); return lhs;}

class BasisList : public std::list<Basis>
{
public:
  void read(std::istream &in);
  void write(std::ostream &out) const;
private:
};
// I/O
inline std::istream& operator>>(std::istream &lhs,
				BasisList &rhs){rhs.read(lhs); return lhs;}
inline std::ostream& operator<<(std::ostream &lhs,
				const BasisList &rhs){rhs.write(lhs); return lhs;}

#endif
