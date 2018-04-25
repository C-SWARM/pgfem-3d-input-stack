/* -*- mode: c++; -*- */
/* HEADER */
/**
 * AUTHORS:
 * Matt Mosby
 */

#pragma once
#ifndef CON3D_APPLY_HEADER_H
#define CON3D_APPLY_HEADER_H

#include "header.h"
#include "options.h"
#include "domain.h"

/**
 * \brief Apply derichlet boudary conditions to each domain.
 *
 * Sets the degrees of freedom for ALL nodes.
 */
void calculate_bc(const Header &header, Domains &domains, int physics_num);
void calculate_ic(const Header &header, Domains &domains, int physics_num);

void apply_header_derichlet_bc(const Header &header,
			       const Options &opt,
			       Domains &domains);

//apply thermal_bc to each domain
void apply_thermal_derichlet_bc(const Header &header,
                               const Options &opt,
                               Domains &domains);

/**
 * \brief Apply Neumann boudary conditions to each domain.
 *
 * Loads are applied only on the owning domain so they aren't
 * double-counted.
 */
void apply_header_neumann_bc(const Header &header,
             const Options &opt,
			       Domains &domains);

/**
 * \brief Set the material flags for all elements.
 */
void apply_header_mat_assignment(const Header &header,
         const Options &opt,
				 Domains &domains);

#endif
