/*
Subj:	EE.C  Expression Evaluator Source

Date: Wed, 11 Dec 1991 14:17:59 PST
From: MARK MORLEY <morley@camins.Camosun.BC.CA>
To: brennan@coco.cchs.su.oz.au
Message-ID: <00952F4C.E2B7BD00.28293@camins.Camosun.BC.CA>
Subject: EE.C  Expression Evaluator Source
*/
/*************************************************************************
**                                                                       **
** EE.C         Expression Evaluator                                     **
**                                                                       **
** AUTHOR:      Mark Morley                                              **
** DATE:        December 1991                                            **
** HISTORY:     It's brand spankin' new!                                 **
**                                                                       **
** You are free to use and distribute this code in any way you can       **
** possibly imagine, as long as you give credit where credit is due...   **
**                                                                       **
** I don't promise that this code does what you think it does...         **
**                                                                       **
** Please mail any bug reports/fixes/enhancments to me at:               **
**      morley@camins.camosun.bc.ca                                      **
** or                                                                    **
**      Mark Morley                                                      **
**      3889 Mildred Street                                              **
**      Victoria, BC  Canada                                             **
**      V8Z 7G1                                                          **
**      (604) 479-7861                                                   **
**                                                                       **


       Modified for Ckit by Petr Krysl, Tue Jan 16 15:04:51 CST 1996


 *************************************************************************/

#include <stdlib.h>
#include <math.h>
#include <setjmp.h>
#define EXPEVAL_PRIVATE_HEADER
#include "expevalP.h"


/* Some of you may choose to define TYPE as a "float" instead... */
#define VARLEN          32              /* Max length of variable names */
#define MAXVARS         50              /* Max variables */
#define TOKLEN          32              /* Max token length */

#define VAR             1
#define DEL             2
#define NUM             3

typedef struct
{
   char   name[VARLEN + 1];               /* Variable name */
   double value;                          /* Variable value */
} VARIABLE;

typedef struct
{
   char   *name;                          /* Function name */
   int     args;                          /* Number of arguments to expect */
   double  (*func)();                     /* Pointer to function */
} FUNCTION;

#define iswhite(c)  (c == ' ' || c == '\t')
#define isnumer(c)  ((c >= '0' && c <= '9') || c == '.')
#define isalpha(c)  ((c >= 'a' && c <= 'z')     \
                     || (c >= '0' && c <= '9')  \
                     || c == '_')
#define isdelim(c)  (   c == '+'                \
                     || c == '-'                \
                     || c == '*'                \
                     || c == '/'                \
                     || c == '%'                \
                     || c == '^'                \
                     || c == '('                \
                     || c == ')'                \
                     || c == ','                \
                     || c == '='                \
                     )


#define ERR(n)                                                          \
  {                                                                     \
    int cntr, stop;                                                     \
                                                                        \
    ERRTAG = n;                                                         \
    for (cntr = 0, stop = strlen(expression); cntr < stop; cntr++) {    \
      ERRPOS = cntr;                                                    \
      if (expression+cntr == ERANC) {                                   \
        break;                                                          \
      }                                                                 \
    }                                                                   \
    strcpy(ERRTOK, token);                                              \
    longjmp(jb,1);                                                      \
  }

#ifndef M_PI
#define M_PI    3.14159265358979323846
#endif
#ifndef M_E
#define M_E     2.71828182845904523536
#endif


/*************************************************************************
**                                                                       **
** PROTOTYPES FOR CUSTOM MATH FUNCTIONS                                  **
**                                                                       **
 *************************************************************************/

static double
deg(double x);
static double
rad(double x);


/*************************************************************************
**                                                                       **
** VARIABLE DECLARATIONS                                                 **
**                                                                       **
 *************************************************************************/

static int   ERRTAG;          /* The error number */
static char  ERRTOK[TOKLEN+1]; /* The token that generated the error */
static int   ERRPOS;          /* The offset from the start of the expression */
static char *ERANC;           /* Used to calculate ERRPOS */

/*
   Add any "constants" here...  These are "read-only" values that are
   provided as a convienence to the user.  Their values can not be
   permanently changed.  The first field is the variable name, the second
   is its value.
*/
VARIABLE Consts[] =
{
   /* name, value */
   { "pi",      M_PI },
   {  "e",       M_E },
   {  "*",         0 }
};

/*
   Add any math functions that you wish to recognise here...  The first
   field is the name of the function as it would appear in an expression.
   The second field tells how many arguments to expect.  The third is
   a pointer to the actual function to use.
*/
FUNCTION Funcs[] =
{
   /* name, funtion to call */
   { "sin",     1,    sin },
   { "cos",     1,    cos },
   { "tan",     1,    tan },
   { "asin",    1,    asin },
   { "acos",    1,    acos },
   { "atan",    1,    atan },
   { "sinh",    1,    sinh },
   { "cosh",    1,    cosh },
   { "tanh",    1,    tanh },
   { "exp",     1,    exp },
   { "log",     1,    log },
   { "log10",   1,    log10 },
   { "sqrt",    1,    sqrt },
   { "floor",   1,    floor },
   { "ceil",    1,    ceil },
   { "abs",     1,    fabs },
   { "hypot",   2,    hypot },
   { "deg",     1,    deg },
   { "rad",     1,    rad },
   { 0,         0,    0   }
};

static VARIABLE        Vars[MAXVARS];    /* Array for user-defined variables */
static char           *expression;       /* Pointer to the user's expression */
static char            token[TOKLEN + 1];/* Holds the current token */
static int             type;             /* Type of the current token */
static jmp_buf         jb;               /* jmp_buf for errors */


/*************************************************************************
**                                                                       **
** Some custom math functions...   Note that they must be prototyped     **
** above (if your compiler requires it)                                  **
**                                                                       **
** deg(x)             Converts x radians to degrees.                   **
** rad(x)             Converts x degrees to radians.                   **
**                                                                       **
 *************************************************************************/

static double
deg(double x)
{
   return(x * 180.0 / M_PI);
}

static double
rad(double x)
{
   return(x * M_PI / 180.0);
}


/*************************************************************************
**                                                                       **
** strlwr(char *s)   Internal use only                                 **
**                                                                       **
** This routine converts a string to lowercase                           **
**                                                                       **
 *************************************************************************/

static int
strlwr(char *s)
{
   while(*s)
   {
      if(*s >= 'A' && *s <= 'Z')
         *s += 32;
      s++;
   }
   return 1;
}


/*************************************************************************
**                                                                       **
** expeval_clear_all_vars()                                               **
**                                                                       **
** Erases all user-defined variables from memory. Note that constants    **
** can not be erased or modified in any way by the user.                 **
**                                                                       **
** Returns nothing.                                                      **
**                                                                       **
 *************************************************************************/

int
expeval_clear_all_vars(void)
{
   int i;

   for(i = 0; i < MAXVARS; i++)
   {
      *Vars[i].name = 0;
      Vars[i].value = 0;
   }
   return 1;
}


static int iterate_curr = 0;

/*  Function can be used together with expeval_next_var() to iterate */
/*  through currently defined variables. */
/*  If there is at least one variable defined, the function returns */
/*  non-zero, and the variables /name/ and /value/ are set to the */
/*  variable name and value respectively.   If there is no next variable */
/*  defined, the function returns zero, and /name/ and /value/ */
/*  are undefined. */

int
expeval_first_var(char **name, double *value)
{
  iterate_curr = 0;
  *value = Vars[iterate_curr].value;
  *name  = Vars[iterate_curr].name;
  return (*Vars[iterate_curr].name);
}
 
/*  Function can be used together with expeval_first_var() to iterate */
/*  through currently defined variables. */
/*  If there is another variable defined, the function returns non-zero, */
/*  and the variables /name/ and /value/ are set to the variable name */
/*  and value respectively.   If there is no next variable defined, the */
/*  function returns zero, and /name/ and /value/ are undefined. */

int
expeval_next_var(char **name, double *value)
{
  iterate_curr++;
  if (iterate_curr >= MAXVARS)
    return 0;
  *value = Vars[iterate_curr].value;
  *name  = Vars[iterate_curr].name;
  return (*Vars[iterate_curr].name);
}


/*************************************************************************
**                                                                       **
** ClearVar(char *name)                                                **
**                                                                       **
** Erases the user-defined variable that is called NAME from memory.     **
** Note that constants are not affected.                                 **
**                                                                       **
** Returns 1 if the variable was found and erased, or 0 if it didn't     **
** exist.                                                                **
**                                                                       **
 *************************************************************************/

int
expeval_clear_var(char *name)
{
   int i;

   for(i = 0; i < MAXVARS; i++)
      if(*Vars[i].name && ! strcmp(name, Vars[i].name))
      {
         *Vars[i].name = 0;
         Vars[i].value = 0;
         return(1);
      }
   return(0);
}


/*************************************************************************
**                                                                       **
** expeval_get_value(char *name, double* value)                          **
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
expeval_get_value(char *name, double* value)
{
   int i;

   for(i = 0; i < MAXVARS; i++)
      if(*Vars[i].name && ! strcmp(name, Vars[i].name))
      {
         *value = Vars[i].value;
         return(1);
      }
   for(i = 0; Consts[i].name[0] != '*'; i++)
      if(*Consts[i].name && ! strcmp(name, Consts[i].name))
      {
         *value = Consts[i].value;
         return(1);
      }
   return(0);
}


/*************************************************************************
**                                                                       **
** expeval_set_value(char *name, double* value)                          **
**                                                                       **
** First, it erases any user-defined variable that is called NAME.  Then **
** it creates a new variable called NAME and gives it the value VALUE.   **
**                                                                       **
** Returns 1 if the value was added, or 0 if there was no more room.     **
**                                                                       **
 *************************************************************************/

int
expeval_set_value(char *name, double value)
{
   int i;

   expeval_clear_var(name);
   for(i = 0; i < MAXVARS; i++)
      if(! *Vars[i].name)
      {
         strcpy(Vars[i].name, name);
         Vars[i].name[VARLEN] = 0;
         Vars[i].value = value;
         return(1);
      }
   return(0);
}


/*************************************************************************
**                                                                       **
** Parse()   Internal use only                                           **
**                                                                       **
** This function is used to grab the next token from the expression that **
** is being evaluated.                                                   **
**                                                                       **
 *************************************************************************/
static void
Parse(void);

static void
Parse(void)
{
   char *t;

   type = 0;
   t = token;
   while(iswhite(*expression))
      expression++;
   if(isdelim(*expression))
   {
      type = DEL;
      *t++ = *expression++;
   }
   else if(isnumer(*expression))
   {
      type = NUM;
      while(isnumer(*expression))
         *t++ = *expression++;
   }
   else if(isalpha(*expression))
   {
      type = VAR;
      while(isalpha(*expression))
        *t++ = *expression++;
      token[VARLEN] = 0;
   }
   else if(*expression)
   {
      *t++ = *expression++;
      *t = 0;
      ERR(EXPEVAL_E_SYNTAX);
   }
   *t = 0;
   while(iswhite(*expression))
      expression++;
}



static void
Level1(double* r);
static void
Level2(double* r);
static void
Level3(double* r);
static void
Level4(double* r);
static void
Level5(double* r);
static void
Level6(double* r);

/*************************************************************************
**                                                                       **
** Level1(double* r)   Internal use only                                 **
**                                                                       **
** This function handles any variable assignment operations.             **
**                                                                       **
 *************************************************************************/

static void
Level1(double* r)
{
   char t[VARLEN + 1];

   if(type == VAR)
      if(*expression == '=')
      {
         strcpy(t, token);
         Parse();
         Parse();
         if(!*token)
         {
            expeval_clear_var(t);
            return;
         }
         Level2(r);
         if(! expeval_set_value(t, *r))
            ERR(EXPEVAL_E_MAXVARS);
         return;
      }
   Level2(r);
}


/*************************************************************************
**                                                                       **
** Level2(double* r)   Internal use only                                 **
**                                                                       **
** This function handles any addition and subtraction operations.        **
**                                                                       **
 *************************************************************************/

static void
Level2(double* r)
{
   double t = 0;
   char o;

   Level3(r);
   while((o = *token) == '+' || o == '-')
   {
      Parse();
      Level3(&t);
      if(o == '+')
         *r = *r + t;
      else if(o == '-')
         *r = *r - t;
   }
}


/*************************************************************************
**                                                                       **
** Level3(double* r)   Internal use only                                 **
**                                                                       **
** This function handles any multiplication, division, or modulo.        **
**                                                                       **
 *************************************************************************/

static void
Level3(double* r)
{
   double t;
   char o;

   Level4(r);
   while((o = *token) == '*' || o == '/' || o == '%')
   {
      Parse();
      Level4(&t);
      if(o == '*')
         *r = *r * t;
      else if(o == '/')
      {
         if(t == 0)
            ERR(EXPEVAL_E_DIVZERO);
         *r = *r / t;
      }
      else if(o == '%')
      {
         if(t == 0)
            ERR(EXPEVAL_E_DIVZERO);
         *r = fmod(*r, t);
      }
   }
}


/*************************************************************************
**                                                                       **
** Level4(double* r)   Internal use only                                 **
**                                                                       **
** This function handles any "to the power of" operations.               **
**                                                                       **
 *************************************************************************/

static void
Level4(double* r)
{
   double t;

   Level5(r);
   if(*token == '^')
   {
      Parse();
      Level5(&t);
      *r = pow(*r, t);
   }
}


/*************************************************************************
**                                                                       **
** Level5(double* r)   Internal use only                                 **
**                                                                       **
** This function handles any unary + or - signs.                         **
**                                                                       **
 *************************************************************************/

static void
Level5(double* r)
{
   char o = 0;

   if(*token == '+' || *token == '-')
   {
      o = *token;
      Parse();
   }
   Level6(r);
   if(o == '-')
      *r = -*r;
}


/*************************************************************************
**                                                                       **
** Level6(double* r)   Internal use only                                 **
**                                                                       **
** This function handles any literal numbers, variables, or functions.   **
**                                                                       **
 *************************************************************************/

static void
Level6(double* r)
{
   int  i;
   int  n;
   double a[3];

   if(*token == '(')
   {
      Parse();
      if(*token == ')')
         ERR(EXPEVAL_E_NOARG);
      Level1(r);
      if(*token != ')')
         ERR(EXPEVAL_E_UNBALAN);
      Parse();
   }
   else
   {
      if(type == NUM)
      {
         *r = (double) atof(token);
         Parse();
      }
      else if(type == VAR)
      {
         if(*expression == '(')
         {
            for(i = 0; *Funcs[i].name; i++)
               if(! strcmp(token, Funcs[i].name))
               {
                  Parse();
                  n = 0;
                  do
                  {
                     Parse();
                     if(*token == ')' || *token == ',')
                        ERR(EXPEVAL_E_NOARG);
                     a[n] = 0;
                     Level1(&a[n]);
                     n++;
                  } while(n < 4 && *token == ',');
                  Parse();
                  if(n != Funcs[i].args)
                  {
                     strcpy(token, Funcs[i].name);
                     ERR(EXPEVAL_E_NUMARGS);
                  }
                  *r = Funcs[i].func(a[0], a[1], a[2]);
                  return;
               }
               if(! *Funcs[i].name)
                  ERR(EXPEVAL_E_BADFUNC);
            }
            else if(! expeval_get_value(token, r))
               ERR(EXPEVAL_E_UNKNOWN);
         Parse();
      }
      else
         ERR(EXPEVAL_E_SYNTAX);
   }
}


/*************************************************************************
**                                                                       **
** expeval_eval(char *e, double* result)                                 **
**                                                                       **
** This function is called to evaluate the expression E and return the   **
** answer in RESULT.                                                     **
**                                                                       **
** Returns EXPEVAL_E_OK if the expression is valid, or an error code.    **
**                                                                       **
 *************************************************************************/

int
expeval_eval(char *e, double* result)
{
  int i, n;
  char *next_expression = NULL;
  
  if(setjmp(jb))
    return(ERRTAG);
  expression = e;
  

 next:
  next_expression = NULL;
  n = strlen(expression);
  for (i = 0; i < n; i++) {
    if (expression[i] == '&') {
      expression[i] = '\0';
      next_expression = ((i+1 < n)? &expression[i+1]: NULL);
      break;
    }
  }
   
  ERANC = e;
  strlwr(expression);
  *result = 0;
  
  Parse();
  
  if(! *token)
    ERR(EXPEVAL_E_EMPTY);
  Level1(result);

  if (next_expression) {
    expression = next_expression;
    goto next;
  }
   
  return(EXPEVAL_E_OK);
}


/*************************************************************************
**                                                                       **
** What follows is a main() routine that evaluates the command line      **
** arguments one at a time, and displays the results of each expression. **
** Without arguments, it becomes an interactive calculator.              **
**                                                                       **
 *************************************************************************/

#include <stdio.h>


#if 0
void
main(int argc, char *argv[])
{
   double  result;
   int   i;
   int   ec;
   char  line[81];

   expeval_clear_all_vars();
   if(argc > 1)
   {
      for(i = 1; i < argc; i++)
         if((ec = expeval_eval(argv[1], &result)) == EXPEVAL_E_OK)
            printf("%g\n", result);
         else if(ec != EXPEVAL_E_EMPTY)
         {
            printf("ERROR: %s - %s", expeval_err_msgs[ERRTAG - 1], ERRTOK);
            printf("\n%s", ERANC);
            printf("\n%*s^\n", ERRPOS, "");
         }
      return;
   }
   printf("\nEE - Equation Evaluator");
   printf("\nBy Mark Morley  December 1991");
   printf("\nEnter EXIT to quit.\n");
   printf("\nEE> ");
   for(fgets(line, 80, stdin); !feof(stdin); fgets(line, 80, stdin))
   {
      for(i = 0; i < strlen(line); i++) { if (line[i] == '\n') line[i] = ' '; }
      strlwr(line);
      if(! strcmp(line, "exit"))
         return;
      else if(! strcmp(line, "vars"))
      {
         for(i = 0; i < MAXVARS; i++)
            if(*Vars[i].name)
               printf("%s = %g\n", Vars[i].name, Vars[i].value);
      }
      else if(! strcmp(line, "constants"))
      {
         for(i = 0; Consts[i].name[0] != '*'; i++)
            printf("%s = %g\n", Consts[i].name, Consts[i].value);
      }
      else if(! strcmp(line, "clr"))
         expeval_clear_all_vars();
      else
      {
         if((ec = expeval_eval(line, &result)) == EXPEVAL_E_OK)
            printf("%g\n", result);
         else if(ec != EXPEVAL_E_EMPTY) 
         {
            printf("ERROR: %s - %s", expeval_err_msgs[ERRTAG - 1], ERRTOK);
            printf("\n%s", ERANC);
            printf("\n%*s^\n", ERRPOS, "");
         }
      }
      printf("EE> ");
   }
}
#endif
