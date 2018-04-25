/* -*- mode: c++; -*- */
/* HEADER */
/**
 * AUTHORS:
 * Matt Mosby
 */

#pragma once
#ifndef CON3D_OPTIONS_H
#define CON3D_OPTIONS_H

#include <cstdlib>
#include <vector>

/**
 * \brief Class for storing command line options.
 *
 * The class is constructed from command line arguments and is not
 * modifiable after that.
 */
class Options{
 public:
  Options(int argc, char **argv);
  ~Options(){};

  // Access functions
  size_t nproc() const {return _np;}
  size_t nthreads() const {return _nt;}
  const char* base_fname() const {return (const char*) _p_fname;}
  const char* base_dname() const {return (const char*) _p_dname;} 
  bool verbose() const {return _verb;}
  bool help() const;

  bool cohesive() const {return _coh;}

  bool periodic() const {return _per;}
  const std::vector<double>& per_bounds() const {return _per_bounds;}

  bool thin_film() const {return _film;}
  double film_eps() const {return _film_eps;}

  bool comm_hints() const {return _ch;}
  bool hints_only() const {return _ch_only;}
	bool human_readable() const {return _human_readable;}
  bool mat_only() const {return _mat_only;}
 private:
  Options(){};
  size_t _np;
  size_t _nt;
  char *_p_fname;
  char *_p_dname;
  bool _per;
  bool _ch;
  bool _ch_only;
  std::vector<double> _per_bounds;
  bool _coh;
  bool _verb;
  bool _film;
  double _film_eps;
  bool _usage;
	bool _human_readable;
  bool _mat_only;
};

#endif
