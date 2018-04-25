/**
 * AUTHORS:
 * Matt Mosby
 */

#include "options.h"
#include "utils.h"
#include <cstring>
#include <iostream>

static const char *prog_usage =
  "\ncon3d++ -- Create set of input files for PGFem3D.\n"
  "Usage: con3d++ -np [nproc] -nt [nthreads] -f [base filename] [options]\n"
  "\nOptions:\n"
  "-pr args(3):\tPeriodic unit cell. Args = x,y,z dims\n"
  "-coh:\t\tCohesive modeling with interface elements\n"
  "-ch:\t\tEmit communication hints as *_comm_hints_*.in (default=yes)\n"
  "-ch-only:\tSee \'-ch\', use on already processed .in files\n"
  "-v:\t\tPrint extra logging info to stderr\n"
  "-h,--help:\tPrint this help message\n"
	"-HumanReadable\n";

bool Options::help() const
{
  if(this->_usage){
    std::cout << prog_usage << std::endl;
  }
  return this->_usage;
}

Options::Options(int argc,
		 char **argv):
  _np(0),
  _nt(1),
  _p_fname(NULL),
  _p_dname(NULL),
  _per(false),
  _per_bounds(3,0.0),
  _coh(false),
  _ch(true),
  _ch_only(false),
  _verb(false),
  _film(false),
  _film_eps(0.0),
  _usage(false),
	_human_readable(false),
  _mat_only(false)
{
  if (argc <= 1) {
    _usage = true;
  }

  /* parse command line */
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i],"-np") == 0) { /* nproc */
      _np = std::atoi(argv[++i]);
    } else if (strcmp(argv[i],"-nt") == 0) { /* num_threads */
      _nt = std::atoi(argv[++i]);
    } else if (strcmp(argv[i],"-f") == 0) { /* base filename */
      _p_fname = argv[++i];
    } else if (strcmp(argv[i],"-d") == 0) { /* base pgfem input directory */
      _p_dname = argv[++i];
    } else if (strcmp(argv[i],"-pr") == 0) { /* periodic */
      _per = 1;
      _per_bounds[0] = std::atof(argv[++i]);
      _per_bounds[1] = std::atof(argv[++i]);
      _per_bounds[2] = std::atof(argv[++i]);
    } else if ((strcmp(argv[i],"-coh") == 0)
	       || (strcmp(argv[i],"-co1") == 0)) {
      _coh = true;
    } else if (strcmp(argv[i],"-ch") == 0) {
      _ch = true;
    } else if (strcmp(argv[i],"-ch-only") == 0) {
      _ch_only = true;
    } else if (strcmp(argv[i],"-fil") == 0) { /* This is for thin films */
      _film = true;
      i++;
      _film_eps = std::atof(argv[i]);
    } else if (strcmp(argv[i],"-v") == 0) { /* Extra output to stderr */
      _verb = true;
    } else if ((strcmp(argv[i],"-h") == 0) || (strcmp(argv[i],"--help") == 0)) {
      /* print useage and exit */
      _usage = 1;
      i = argc;
	} else if (strcmp(argv[i],"-HumanReadable") == 0) {
		_human_readable = true;
  } else if (strcmp(argv[i],"-MaterialOnly") == 0) {
    _mat_only = true;
  }
     else {
      std::cout << "Unrecognized option: " << argv[i] << "... Skipping.\n";
    }
  }

  if(this->help()) abort();
}
