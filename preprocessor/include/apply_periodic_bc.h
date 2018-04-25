/* -*- mode: c++; -*- */
/* HEADER */
/**
 * AUTHORS:
 * Matt Mosby
 */

#pragma once
#ifndef CON3D_APPLY_PER_BC_H
#define CON3D_APPLY_PER_BC_H

#include "options.h"
#include "domain.h"
#include "periodic.h"
#include <vector>

/**
 * \brief Apply periodic boundary conditions to nodes on all domains.
 *
 * Find nodes on each domain associated with periodic model
 * entities. Give periodic nodes the same global node number and
 * owning domain (keep lowest dom then lowest Gnn). Renumber ALL
 * global node numbers starting from 0 on domain 0.
 */
void apply_periodic_bc(const Options &opt,
          Domains &domains,
		      const Periodic &periodic);

#endif
