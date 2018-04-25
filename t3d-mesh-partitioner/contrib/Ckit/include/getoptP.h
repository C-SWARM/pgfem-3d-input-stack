
#ifndef GETOPTP_H
#   define GETOPTP_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "tokensP.h"

/* NOTE:  THERE ARE TWO PACKAGES HERE, CkitGetOpt() (for simple one-letter */
/* options), and ckit_opt_xxx() functions which recognize more complicated */
/* options. */


/* ======================================================================== */
/* CkitGetOpt() recognizes options in the form */
/* -i file1 -D 1.13 -b -H -? -E 0*/
/* ie. each option is preceded by a dash, and it can have an argument */
/* separated by a blank.  The arguments to CkitGetOpt() are: */
/* argc = IN number of command-line arguments */
/* argv = IN array of strings of command-line arguments */
/* flags= IN array of chars in the form "i:D:bH?E:".  The options */
/*        i,D,E take an argument as denoted by the ':' (see the */
/*        example above). */


extern int ckit_optind, ckit_opterr;
extern char *ckit_optarg;
#define optarg ckit_optarg
#define optind ckit_optind
#define opterr ckit_opterr
#define get_cl_options CkitGetOpt
#define getopt CkitGetOpt
int 
CkitGetOpt(int argc, char * const argv[], const char *flags);




/* ======================================================================== */
/* This package is smarter.  It can test for options of any length. */

/* Given the standard main() arguments, this function tests to see whether */
/* the specified option has been given on the command line.  It must be */
/* called before any of the following functions can be used. */
int 
ckit_opt_is_set(int argc, char * const argv[], char *opt);

/* Does the option /opt/ (given as the third argument in the previous */
/* routine) have arguments? */
int
ckit_opt_has_arg(void);

/* If it had an argument, get its "float" value. */
int
ckit_opt_double_val(double *val);

/* If it had an argument, get its "double" value. */
int
ckit_opt_float_val(float *val);

/* If it had an argument, get its "int" value. */
int
ckit_opt_int_val(int *val);

/* If it had an argument, get the *COPY* of the "char *" value. */
int
ckit_opt_char_val(char **s);

/* Macro short-cuts:   */

#ifndef TRUE
#   define TRUE  1
#   define FALSE 0
#endif

/* See whether the option has been specified on */
/* the command line.  This (a) checks for the presence, and (b) executes */
/* the expression todo_when_set, if the option was present. */
#define GET_OPT_BOOL(argc, argv, opt, todo_when_set)                        \
  {                                                                         \
    if (ckit_opt_is_set(argc, argv, opt)) {                                 \
      todo_when_set;                                                        \
    }                                                                       \
  }

/* See whether the option /opt/ has been set on the command line. */
/* If this is the case, assign the variable the argument value of type */
/* specified, and executes the expression todo_when_set. */
#define GET_OPT_ARG(argc, argv, opt, type, var, todo_when_set, the_usage)   \
  {                                                                         \
    if (ckit_opt_is_set(argc, argv, opt)) {                                 \
      if (ckit_opt_has_arg()) {                                             \
        if (!ckit_opt_##type##_val(&var))                                   \
          goto usage##var;                                                  \
      } else {                                                              \
        goto usage##var;                                                    \
      }                                                                     \
      todo_when_set;                                                        \
      goto ok##var;                                                         \
    usage##var:                                                             \
      fprintf(stderr,                                                       \
              "Bad argument for option " #opt ".  Usage: " the_usage "\n"); \
      exit(EXIT_FAILURE);                                                   \
    ok##var:                                                                \
      ;                                                                     \
    }                                                                       \
  }


#endif
