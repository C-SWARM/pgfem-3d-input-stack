/* -*- mode: c++; -*- */
/* HEADER */
/**
 * AUTHORS:
 * Matt Mosby
 */
#pragma once
#ifndef CON3D_UTILS_H
#define CON3D_UTILS_H

#include <cstdlib>
#include <iostream>
#include <vector>
#include <cmath>
#include <utility>
#include <iterator>  // std::ostream_iterator
#include <algorithm> // std::copy

template<class T>
inline void file_check_error(T &str,
			     const char *filename)
{
  if(!str.good()){
    std::cerr << "ERROR: Error with file (" << filename 
	      << ") at " << __func__ << ':' << __FILE__ << ':' << __LINE__
	      << std::endl;
    abort();
  }
}

// Print an STL container using std::copy
template<class STL_Container>
inline void print_stl_container(std::ostream &out,
                                const STL_Container &container,
                                const char *delim)
{
  std::ostream_iterator< typename STL_Container::value_type > it(out, delim);
  std::copy(container.begin(), container.end(), it);
}

// Output operator for STL vector/array/list containers
template<template <typename, typename> class STL_Container, typename T>
std::ostream& operator<<(std::ostream &lhs,
			 const STL_Container< T, std::allocator< T > > &rhs)
{
  print_stl_container(lhs, rhs, " ");
  return lhs;
}

template<class T>
std::istream& operator>>(std::istream &lhs,
			 std::vector<T> &rhs)
{
  typename std::vector<T>::iterator it;
  for(it = rhs.begin(); it != rhs.end(); ++it){
    lhs >> *it;
  }
  return lhs;
}

/**
 * Floating point equality with maximum relative error.
 */
template <class float_type>
bool is_equal(const float_type a,
	      const float_type b)
{
  static const double max_rel_err = 1.0e-14;
  // still need epsilon value for difference around very small numbers
  static const double epsilon = 1.0e-10;
  static const double small = 1.0e-5;
  if(a == b) return true;
  float_type diff = std::abs(a-b);
  float_type lrg = (std::abs(b) > std::abs(a)) ? std::abs(b) : std::abs(a);
  if(std::abs(b) <= small && std::abs(a) <= small && diff <= epsilon) return true;
  else if(diff <= lrg*max_rel_err) return true;
  return false;
}

template<class A,class B>
bool compare_pair_first(const std::pair<A,B> &a,
			const std::pair<A,B> &b)
{
  return (a.first < b.first);
}

template<class A,class B>
bool compare_pair_second(const std::pair<A,B> &a,
			 const std::pair<A,B> &b)
{
  return (a.second < b.second);
}

#endif
