/* -*- mode: c++; -*- */
/* HEADER */
/**
 * Some helper functions to compute communication hints for a
 * previously processed data-stack.
 *
 * AUTHORS:
 * Matt Mosby
 */

#pragma once
#ifndef CON3D_COMM_HINTS_UTIL
#define CON3D_COMM_HINTS_UTIL

#include <iostream>

class Options;
class Domain;
class Domains;

namespace ch_util {
  void read_nodes_dom(std::istream &in,
                      Domain &dom);

  void read_input_files(const Options &opt,
                        Domains &domains);
}

#endif
