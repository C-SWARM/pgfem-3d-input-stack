#ifndef SKYSOL_H
#   define SKYSOL_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* Symbolic return codes  */
#define SKYSOL_OK              0
#define SKYSOL_E_MEM          -1
#define SKYSOL_E_DIV_BY_ZERO  -2
#define SKYSOL_E_BAD_MATRIX   -3
#define SKYSOL_E_BAD_INDEX    -4

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min_of_3(a,b,c) (((a) < (b))? \
                             (((c) < (a))? \
                                 (c): \
                                 (a)): \
                             (((c) < (b))? \
                                 (c): \
                                 (b)) \
                        )
#endif

#define skysol_new(TYPE) ((TYPE *) malloc((size_t) sizeof(TYPE)))

#define skysol_new_array(NELT, TYPE) \
                      ((TYPE *) calloc((NELT), (size_t) sizeof(TYPE)))

#define skysol_realloc_array(PTR, NEW_SIZE, TYPE) \
   ((TYPE *) realloc(PTR, (size_t)(NEW_SIZE * sizeof(TYPE))))

#define skysol_free(PTR) free(PTR)

typedef enum  { SKYSOL_NOT_DIAGONAL = 0,
                SKYSOL_DIAGONAL     = 1
              } skysol_type_t;

/*
   This class implements a symmetric matrix stored in a compact (skyline)
   form. 
 DESCRIPTION :
   Attribute /size/ is number of columns of the skyline. Attribute /columns/
   is a list of /size/ columns, each of them of any size. Attribute /isFac-
   torized/ is True if the skyline is already in U(T).D.U factorized form,
   else it is False.
*/

struct skysol_col_t;


typedef struct skysol_matrix_t {
  int                      size;
  struct skysol_col_t    **columns;
  int                      isFactorized;
}              skysol_matrix_t;


/*
   This class implements a column in a matrix stored in segmented form
   (symmetric skyline). 
 DESCRIPTION :
   A column n stores in /values/ its /size/ coefficients, upwards:
   .values[0]      = diagonal coefficient A(n,n)
   .values[1]      = off-diagonal coefficient A(n-1,n)
   .values[size-1] = highest non-0 coefficient of the n-th column.
*/

typedef struct skysol_col_t {
  int               number;
  int               size;
  skysol_matrix_t  *matrix;
  double *values;
}              skysol_col_t;

#define COL_AT(col, i) col->values[col->number - i]
#define COL_HIGHEST_ROW(col) (col->number - col->size + 1)
#define COL_SIZE(col) (col->size)

#define SKY_AT(sky, i, j) COL_AT(skysol_col(sky, j), i)
#define SKY_NUM_OF_COLS(sky) (sky->size)

#define SKY_COL(sky, j) \
   (sky->columns[j-1]) /* 0-based array addressed by 1-based index */

#define VEC_AT(v, i) v[i-1]

/* Construct new skyline matrix */
skysol_matrix_t * 
skysol_new_matrix(void);

/* Assemble column vector into the global skyline matrix.  Returns SKYSOL_OK */
/* if all went smoothly;  otherwise an error occurred. */
int
skysol_assemble_col_mat(skysol_matrix_t *sky,
                        int glcol,
                        double *mat,
                        int *loc,
                        int dim);

/* Backsubstitute;  Returns SKYSOL_OK if all went smoothly;  otherwise */
/* an error occurred.*/
int
skysol_backsubstitute(skysol_matrix_t *sky,
                      double *rhs /* IN rhs, OUT solution of U * x = rhs */
                      );

/* Factorize skyline matrix;  Returns SKYSOL_OK if all went smoothly; */
/* otherwise an error occurred.*/
/* Generates the  U(transp).D.U  Crout factorization form of the skyline */
/* matrix. */
int
skysol_factorize(skysol_matrix_t *sky);


/* Computes the solution of the system  /sky/(transp) * /y/ = /b/ , */
/* where /sky/  is assumed to be in a factorized form.  */
/* Note: /y/ overwrites /b/ */
int
skysol_forward_reduce(skysol_matrix_t *sky, double *b);

/* Bundled solver.  It does the following for a given matrix and */
/* a given right-hand side: */
/* (i)   factorize, */
/* (ii)  forward reduce, */
/* (iii) diagonal scaling, */
/* (iv)  backsubstitute. */
/* */
int
skysol_solve(skysol_matrix_t *sky, double *rhs);

/* Print skyline matrix to the specified stream. */
void
skysol_print_matrix(skysol_matrix_t *sky, FILE *fp);

/* Access a value within the skyline matrix; returns SKYSOL_OK, if the */
/* access was correct (in terms of indices);  otherwise error is returned. */
/* On successful exit, the parameter /v/ holds the value of the term */
/* sky[/row/][/col/]. */
int
skysol_val_at(skysol_matrix_t *sky, int row, int col, double *v);

#endif
