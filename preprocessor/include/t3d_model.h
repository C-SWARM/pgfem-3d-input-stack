/* -*- mode: c++; -*- */
/* HEADER */
/**
 * AUTHORS:
 * Matt Mosby
 */
#pragma once
#ifndef CON3D_T3D_MODEL_H
#define CON3D_T3D_MODEL_H

#include <iostream>
#include <utility>

class T3dModel : public std::pair<size_t,size_t>
{
public:
  T3dModel() : std::pair<size_t,size_t>(){}
  T3dModel(const size_t &t, const size_t &id) : std::pair<size_t,size_t>(t,id){}
  void set_type(size_t t) {first = t;}
  void set_id(size_t i) {second = i;}
  size_t type() const {return first;}
  size_t id() const {return second;}

  static T3dModel make_model(const size_t &t, const size_t &id){return T3dModel(t,id);}
  static bool compare(const T3dModel &a, const T3dModel &b)
  {
    if(a.type() > b.type()) return true;
    else if(a.type() >= b.type() && a.id() < b.id()) return true;
    else return false;
  }
private:
};
inline std::istream& operator>>(std::istream &lhs,T3dModel &rhs)
{ lhs >> rhs.first >> rhs.second; return lhs;}
inline std::ostream& operator<<(std::ostream &lhs,const T3dModel &rhs)
{ lhs << rhs.first << " " << rhs.second; return lhs;}
#endif


