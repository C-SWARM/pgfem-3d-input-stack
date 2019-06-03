#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>

#include "t3d_node.h"

/// Check general i/o stream readiness.
void check_stream(std::ios *file);
void check_stream(std::ios *file,std::string msg);

/// Check if the number of domains is a power of 2.
void  check_pow2(int ndom);

bool compare_eid(T3d_Node *i, T3d_Node *j);

bool compare_coord(T3d_Node *i, T3d_Node *j);

/// Find the minimum non-zero (unless array of zeros) value in an
/// un-ordered array and return the index
int minimum(int nelem, int *array);

#endif // #ifndef UTILS_H
