#ifndef EXPEVALP_H
#   define EXPEVALP_H

/*
Date: Wed, 11 Dec 1991 14:18:23 PST
From: MARK MORLEY <morley@camins.Camosun.BC.CA>
To: brennan@coco.cchs.su.oz.au
Message-ID: <00952F4C.F14468A0.28295@camins.Camosun.BC.CA>
Subject: EE.H  Expression Evaluator Source
*/


#define EXPEVAL_E_OK           0   /* Successful evaluation */
#define EXPEVAL_E_SYNTAX       1   /* Syntax error */
#define EXPEVAL_E_UNBALAN      2   /* Unbalanced parenthesis */
#define EXPEVAL_E_DIVZERO      3   /* Attempted division by zero */
#define EXPEVAL_E_UNKNOWN      4   /* Reference to unknown variable */
#define EXPEVAL_E_MAXVARS      5   /* Maximum variables exceeded */
#define EXPEVAL_E_BADFUNC      6   /* Unrecognised function */
#define EXPEVAL_E_NUMARGS      7   /* Wrong number of arguments to funtion */
#define EXPEVAL_E_NOARG        8   /* Missing an argument to a funtion */
#define EXPEVAL_E_EMPTY        9   /* Empty expression */
#define EXPEVAL_E_SENTINEL    10
 

#ifdef EXPEVAL_PRIVATE_HEADER
char *expeval_err_msgs[EXPEVAL_E_SENTINEL] =
{
  "OK",
  "Syntax error",
  "Unbalanced parenthesis",
  "Division by zero",
  "Unknown variable",
  "Maximum variables exceeded",
  "Unrecognised funtion",
  "Wrong number of arguments to funtion",
  "Missing an argument",
  "Empty expression"
};
#else
extern char *expeval_err_msgs[EXPEVAL_E_SENTINEL];
#endif

/*************************************************************************
**                                                                       **
** expeval_eval(char* e, double* result)                                     **
**                                                                       **
** This function is called to evaluate the expression E and return the   **
** answer in RESULT.  Multiple expression per string are supported;      **
** They can be simply separated by the '&' character.                    **
**   Example: b=12 & a=sin(b)                                            **
**                                                                       **
** Returns E_OK if the expression is valid, or an error code.            **
**                                                                       **
 *************************************************************************/

int
expeval_eval(char* e, double* result);


/*************************************************************************
**                                                                       **
** expeval_clear_all_vars()                                              **
**                                                                       **
** Erases all user-defined variables from memory. Note that constants    **
** can not be erased or modified in any way by the user.                 **
**                                                                       **
** Returns nothing.                                                      **
**                                                                       **
 *************************************************************************/

int
expeval_clear_all_vars(void);

/*************************************************************************
**                                                                       **
** expeval_clear_var(char* name)                                         **
**                                                                       **
** Erases the user-defined variable that is called NAME from memory.     **
** Note that constants are not affected.                                 **
**                                                                       **
** Returns 1 if the variable was found and erased, or 0 if it didn't     **
** exist.                                                                **
**                                                                       **
 *************************************************************************/

int
expeval_clear_var(char* name);


/*************************************************************************
**                                                                       **
** expeval_get_value(char* name, double* value)                          **
**                                                                       **
** Looks up the specified variable (or constant) known as NAME and       **
** returns its contents in VALUE.                                        **
**                                                                       **
** First the user-defined variables are searched, then the constants are **
** searched.                                                             **
**                                                                       **
** Returns 1 if the value was found, or 0 if it wasn't.                  **
**                                                                       **
 *************************************************************************/

int
expeval_get_value(char* name, double* value);


/*************************************************************************
**                                                                       **
** expeval_set_value(char* name, double* value)                          **
**                                                                       **
** First, it erases any user-defined variable that is called NAME.  Then **
** it creates a new variable called NAME and gives it the value VALUE.   **
**                                                                       **
** Returns 1 if the value was added, or 0 if there was no more room.     **
**                                                                       **
 *************************************************************************/

int
expeval_set_value(char* name, double value);



/*  Function can be used together with expeval_next_var() to iterate */
/*  through currently defined variables. */
/*  If there is at least one variable defined, the function returns */
/*  non-zero, and the variables /name/ and /value/ are set to the */
/*  variable name and value respectively.   If there is no next variable */
/*  defined, the function returns zero, and /name/ and /value/ */
/*  are undefined. */

int
expeval_first_var(char **name, double *value);

/*  Function can be used together with expeval_first_var() to iterate */
/*  through currently defined variables. */
/*  If there is another variable defined, the function returns non-zero, */
/*  and the variables /name/ and /value/ are set to the variable name */
/*  and value respectively.   If there is no next variable defined, the */
/*  function returns zero, and /name/ and /value/ are undefined. */

int
expeval_next_var(char **name, double *value);


#endif /* EXPEVALP_H */
