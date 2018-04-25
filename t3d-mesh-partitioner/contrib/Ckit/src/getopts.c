
#include "getoptP.h"

#ifndef YES
#   define YES 1
#   define NO  0
#endif

/* Global variables */
int ckit_optind = 0, ckit_opterr = 1;
char *ckit_optarg = NULL;


int 
CkitGetOpt(int argc, char * const argv[], const char *flags)
{
  static int started = NO;
  static const char *current_flag;
  int i, retval;

  if (!started) {
    current_flag  = flags;
    started       = YES;
  }
  if (*current_flag == '\0') {
    started = NO;
    return EOF;
  }
  
  for (i = 1; i < argc; i++) {
    if ((int)argv[i][0] == (int)'-') {
      if (   ((int)argv[i][1] == (int)(*current_flag))
          && ((int)argv[i][1] != '\0' && (int)argv[i][2] == '\0')) {
        retval = (int)(*current_flag);
        if ((int)(*(current_flag+1)) == (int)':') {
          ckit_optarg = (char *)argv[i+1];
          current_flag++;
        }
        current_flag++;
        return retval;
      }
    }
  }

  if (*(++current_flag) == '\0') {
    started = NO;
    return EOF;
  } else 
    return 0;
}



int 
ckit_opt_is_set(int argc, char * const argv[], char *opt)
{
  int  i;

  if (opt == NULL)
    return NO;

  ckit_optarg = NULL;
  
  for (i = 1; i < argc; i++) {
    if (argv[i] != NULL)
      if (strcmp(opt, argv[i]) == 0) {
        if (i+1 < argc)
          ckit_optarg = argv[i+1];
        return YES;
      }
  }
  return NO; 
}


int
ckit_opt_has_arg(void)
{
  return (ckit_optarg != NULL);
}


static int
arg_number(void);

static double num_value = 0;


static int
arg_number(void)
{
  if (!tokens_parse_line(ckit_optarg))
    return NO;
  if (tokens_type_of_token(1) != TOKENS_NUMBER)
    return NO;
  num_value = tokens_token_as_double(1);
  return YES;
}



int
ckit_opt_double_val(double *val)
{
  if (ckit_optarg == NULL)
    return NO;

  if (arg_number()) {
    *val = num_value;
    return YES;
  }
  return NO;
}


int
ckit_opt_float_val(float *val)
{
  if (ckit_optarg == NULL)
    return NO;

  if (arg_number()) {
    *val = (float)num_value;
    return YES;
  }
  return NO;
}


int
ckit_opt_int_val(int *val)
{
  if (ckit_optarg == NULL)
    return NO;

  if (arg_number()) {
    *val = (int)num_value;
    return YES;
  }
  return NO;
}



#define DUPL_STRNG_(dest, src, if_fail)                                 \
  {                                                                     \
    if (((dest) = (char *)malloc((size_t)(strlen(src)+1))) == NULL) {   \
      if_fail;                                                          \
    } else {                                                            \
      strcpy((dest), (src));                                            \
    }                                                                   \
  }


int
ckit_opt_char_val(char **s)
{
  DUPL_STRNG_(*s, ckit_optarg, *s=NULL);
  return (*s != NULL);
}

