/* -*- mode: c++; -*- */
/* HEADER */
/**
 * AUTHORS:
 * Matt Mosby
 */

#pragma once
#ifndef CON3D_RENUMBER_NODES_H
#define CON3D_RENUMBER_NODES_H

#include "options.h"
#include "domain.h"

/**
 * \brief Renumber the global node numbers on all domains preserving
 * their relative order, but making them sequential.
 *
 * Side effects: all nodes ordered by gid on return
 */
void renumber_global_nodes(const Options &opt,
                           Domains &domains);

#endif
