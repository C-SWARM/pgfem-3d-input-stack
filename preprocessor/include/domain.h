/* -*- mode: c++; -*- */
/* HEADER */
/**
 * AUTHORS:
 * Matt Mosby
 */

#pragma once
#ifndef CON3D_DOMAIN_H
#define  CON3D_DOMAIN_H

#include "options.h"
#include "node.h"
#include "element.h"
#include "boundary_conditions.h"
#include "initial_conditions.h"
#include <cstdlib>
#include <iostream>
#include <vector>

/**
 * \brief Object for single processor domain.
 *
 * Contains all information pertaining to the domain.
 */
class Domain
{
 public:
  Domain():_parallel(true){};
  void set_serial();
  void set_parallel();
  bool parallel() const;

  /**
   * \brief Sort data by model.
   */
  void sort_model();

  /**
   * \brief Sort data by id number (ascending).
   */
  void sort_id();

  /**
   * \brief Determine the maximum global node id on the domain.
   *
   * The domain nodes are sorted by gid on return.
   */
  size_t get_max_gid();

  /**
   * \brief move the cohesive elements from elements to coh_elements and renumber id's.
   *
   * On return, coh_elements is sorted in original id order with new id #'s
   */
  void filter_cohesive_elements();

  void read(std::istream &in);
  void write(std::ostream &out) const;

  // Public data
  NodeList nodes;
  ElementList elements;
  ElementList coh_elements;
  NodeBCList<int> pre_disp;
  NodeBCList<int> pre_therm;
  std::vector <NodeBCList<int>> boundary_conditions;
  std::vector <NodeICList<int>> initial_conditions;
  NodeBCList<double> pre_force;

private:
  bool _parallel;
};

/**
 * \brief Container class for Domain
 */
class Domains : public std::vector<Domain>
{
public:
  Domains(size_t n) : std::vector<Domain>(n), num_global_nodes(0){}
  size_t num_global_nodes;
  void sort_id(const Options &opt);
  void sort_nodes_model(const Options &opt);
  void sort_nodes_own(const Options &opt);
private:
};

#endif
