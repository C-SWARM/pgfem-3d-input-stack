/* -*- mode: c++; -*- */
/* HEADER */
/**
 * AUTHORS:
 * Matt Mosby
 */

#pragma once
#ifndef CON3D_WRITE_PGFEM_H
#define CON3D_WRITE_PGFEM_H

#include "options.h"
#include "header.h"
#include "domain.h"
#include "process_input_files.h"

/**
 * \brief Write input files for PGFem3D
 *
 * Writes the prperly formatted input files and conditionally writes
 * the cohesive files.
 */
void write_PGFem3D_input_files(const Options &opt,
			       const Header &header,
			       const Domains &domains,
			       const Input_Data inputs);

#endif
