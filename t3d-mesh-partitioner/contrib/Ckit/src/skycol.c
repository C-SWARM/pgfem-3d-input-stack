
#include "skysol.h"



skysol_col_t *
skysol_col_new(int number, int size, skysol_matrix_t *sky);
double  
skysol_col_dot(skysol_col_t *This, skysol_col_t *b, int start, int stop);
static skysol_col_t *
skysol_col_resize(skysol_col_t *col, int new_size);
static void
destroy_col(skysol_col_t *col);


static double  
dotprod (double *P1, double *P2, register int i);


double  
skysol_col_dot(skysol_col_t *This, skysol_col_t *b, int start, int stop)
   /* Returns the scalar product of the receiver by /b/, from index /start/
      (included) to index /stop/ (included). */
{
   double *P1,*P2;
   int    i;

   P1 = This->values + This->number - stop; /* P1 points to a(stop) */
   P2 = b->values + b->number - stop;       /* P2 points to b(stop) */
   i  = stop - start + 1;

   return dotprod(P1, P2, i);
}


static double  
dotprod (double *P1, double *P2, register int i)
   /* Returns the dot product of the first 'i' coefficients */
   /* of the two arrays P1 and P2. */
{
   double res;

   res = 0;
   while (i--)
      res += *P1++ * *P2++;
   return res;
}


static skysol_col_t *
skysol_col_resize(skysol_col_t *col, int new_size)
{
  register int  i, n;

  n = max(col->size, new_size); /* new size adjusted for /new_size/ < /size/ */
  col->values = skysol_realloc_array(col->values, n, double);
  if (col->values == NULL)
    return NULL;

  for (i = col->size; i < n; i++)
    col->values[i] = 0;

  col->size   = n;
  
  return col;
}


skysol_col_t *
skysol_col_new(int number, int size, skysol_matrix_t *sky)
{
  skysol_col_t *c;
  int i;

  c = skysol_new(skysol_col_t);
  if (c == NULL)
    return NULL;

  c->size   = size;
  c->number = number;
  c->values = skysol_new_array(c->size, double);
  for (i = 0; i < c->size; i++) c->values[i] = 0;

  return c;
}


static void
destroy_col(skysol_col_t *col)
{
  skysol_free(col->values);
  skysol_free(col);
}


skysol_col_t *
grow_col_size(skysol_col_t *col, int highest_row)
{
  int  s = col->number - highest_row  + 1;
  
  return skysol_col_resize(col, s);
}


void
skysol_print_col(skysol_col_t *column, FILE *fp)
{
  int row;
  double v;
  
  fprintf(fp, "Row\n");
  for (row = COL_HIGHEST_ROW(column); row <= column->number; row++) {
    v = COL_AT(column, row);
    if (v != 0)
      fprintf(fp, "%d    %g\n", row, v);
  }
}

