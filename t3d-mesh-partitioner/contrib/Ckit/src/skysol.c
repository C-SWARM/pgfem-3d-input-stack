#include "skysol.h"

skysol_col_t *
skysol_col_new(int number, int size, skysol_matrix_t *sky);
double  
skysol_col_dot(skysol_col_t *This, skysol_col_t *b, int start, int stop);
static int                      /* OUT SKYSOL_OK or non-zero for error */
grow_sky_size(skysol_matrix_t *sky, int m);
skysol_col_t *
grow_col_size(skysol_col_t *col, int new_size);


skysol_matrix_t * 
skysol_new_matrix(void)
{
  skysol_matrix_t *S;

  S               = skysol_new(skysol_matrix_t);
  S->size         = 0;
  S->columns      = NULL;
  S->isFactorized = 0;

  return S;
}



int
skysol_assemble_col_mat(skysol_matrix_t *sky,
                        int glcol,
                        double *mat,
                        int *loc,
                        int dim)
{
  int     row, glrow, hglrow;
  skysol_col_t *column;

  if (glcol > sky->size)
    grow_sky_size(sky, glcol);
    
  column = SKY_COL(sky, glcol);
  
  hglrow = sky->size;
  for (row = 0; row < dim; row++)
    hglrow = min(hglrow, loc[row]);
  if (hglrow < COL_HIGHEST_ROW(column)) {
    SKY_COL(sky, glcol) = grow_col_size(column, hglrow);
    if (SKY_COL(sky, glcol) == NULL)
      return SKYSOL_E_MEM;
  }

  for (row = 1; row <= dim; row++) {
    glrow = VEC_AT(loc, row);
    if (glrow > 0 && glrow <= glcol) {
      COL_AT(column, glrow) += VEC_AT(mat, row);
    }
  }
  return SKYSOL_OK;
}


int
skysol_val_at(skysol_matrix_t *sky, int row, int col, double *v)
{
  skysol_col_t *column;
  int tmp;

  if (row > col) { tmp = row; row = col; col = tmp; } /* swap */
  if (col < 1 || col > sky->size)
    return SKYSOL_E_BAD_INDEX;
  if (row < 1 || row > sky->size)
    return SKYSOL_E_BAD_INDEX;
  
  column = SKY_COL(sky, col);

  if (row >= COL_HIGHEST_ROW(column))
    *v = COL_AT(column, row);
  else
    *v = 0;

  return SKYSOL_OK;
}

int
skysol_backsubstitute(skysol_matrix_t *sky,
                      double *rhs /* IN rhs, OUT solution of U * x = rhs */
                      )
{
  int     i, j, start;
  double  y;
  skysol_col_t *columnJ;
  
  for (j = sky->size; j > 0; j--) {
    columnJ = SKY_COL(sky, j);
    y       = VEC_AT(rhs, j);
    start   = COL_HIGHEST_ROW(columnJ);
    for (i = start; i < j; i++)
      VEC_AT(rhs, i) -= COL_AT(columnJ, i) * y;
  }
  return SKYSOL_OK;
}


static int  
create_column(skysol_matrix_t *sky,
              int j, int s);

static int 
create_column(skysol_matrix_t *sky,
              int j, int s)
   /* Construct a new j-th column, of size /s/. */
{
  skysol_col_t *column;

  column = skysol_col_new(j, s, sky);
  if (column == NULL)
    return SKYSOL_E_MEM;
  SKY_COL(sky, j) = column;
  return SKYSOL_OK;
}


int
skysol_diagonal_scaling(skysol_matrix_t *sky, double *b)
   /* Scales /b/ by the diagonal of the skyline matrix. */
{
  int j;
  skysol_col_t *column;
  
  for (j = 1; j <= sky->size; j++) {
    column = SKY_COL(sky, j);
    VEC_AT(b, j) /= COL_AT(column, j);
  }
  return SKYSOL_OK;
}


int
skysol_factorize(skysol_matrix_t *sky)
   /* Generates the  U(transp).D.U  Crout factorization form. */
{
  int    i, j, hr, start;
  double sum, coljv, coliv;
  skysol_col_t *columnI, *columnJ;
  
  if (sky->isFactorized)
    return SKYSOL_OK;
  
  /* for every column */
  for (j=2; j <= sky->size; j++) {	    /* first, the off-diagonal terms */
    columnJ   = SKY_COL(sky, j);
    hr = COL_HIGHEST_ROW(columnJ);
    for (i = hr+1; i < j; i++) {
      columnI            = SKY_COL(sky, i);
      start              = max(hr, COL_HIGHEST_ROW(columnI));
      COL_AT(columnJ, i) -= skysol_col_dot(columnI, columnJ, start, i-1);
    }
    /* next, the diagonal term */
    sum = 0.;
    for (i = hr; i < j; i++) {
      columnI = SKY_COL(sky, i);
      coljv   = COL_AT(columnJ, i);
      coliv   = COL_AT(columnI, i);
      if (coliv == 0)
        return SKYSOL_E_DIV_BY_ZERO;
      COL_AT(columnJ, i) = coljv / coliv;
      sum                += coljv * COL_AT(columnJ, i);
    }
    COL_AT(columnJ, j) -= sum;
  }

  sky->isFactorized = 1;

  return SKYSOL_OK;
}



int
skysol_forward_reduce(skysol_matrix_t *sky, double *b)
   /* Computes the solution of the system  U(transp).y = /b/ , where U is
      /sky/ (assumed to be in a factorized form).  
      Note: y overwrites b */
{
  int     i, j, start;
  double  sum;
  skysol_col_t *columnJ;
  
  if (sky->size == 0)
    return SKYSOL_E_BAD_MATRIX;
  
  for (j=1; j <= sky->size; j++) {
    columnJ = SKY_COL(sky, j);
    sum     = 0.;
    start   = COL_HIGHEST_ROW(columnJ);
    for (i = start; i < j; i++)
      sum += COL_AT(columnJ, i) * VEC_AT(b, i);
    VEC_AT(b, j) -= sum;
  }
  return SKYSOL_OK;
}


#if 0
FloatArray* Skyline :: Times (FloatArray *x)
// Computes y, the results  of the  y = U.x, where U is
// the receiver. Returns the result.
{
  int nn,nu,kk;
  double aa,cc;
  // 
  // first check sizes
  //
  if (this->giveNumberOfColumns() != (nn = x->giveSize())) {
    printf ("Class Skyline, function Times : size mismatch\n");
    exit(1);
  }
  FloatArray *answer = new FloatArray(nn);
  Column *columnI;
  int ii;

  for (int i=1; i<= nn; i++) {
    columnI = this-> giveColumn (i);
    nu = columnI -> giveHighestRow ();
    cc = x->at(i);
    ii = i;
    for (kk = i; kk >= nu; kk--) 
      answer->at(ii--) += columnI->at(kk)* cc;
  }
  if (nn == 1) return answer;
    
  for (i = 2; i<= nn; i++) {
     columnI = this-> giveColumn (i);
     nu = columnI->giveHighestRow();
     if (i== nu) continue;
     ii = i-1;
     aa = 0.;
     for (kk = i-1; kk >= nu; kk--,ii--)
       aa += columnI->at(kk) * x->at(ii);
     answer->at(i) += aa;
   }
  return answer;
}
#endif   


static int                      /* OUT SKYSOL_OK or non-zero for error */
grow_sky_size(skysol_matrix_t *sky, int m)
   /* Increases to /m/ the number of columns in skyline matrix. */
{
  int      j;
  skysol_col_t **new_cols;
  int error_code;
  
  if (m <= sky->size) {
    return SKYSOL_OK;           /* no need to make it smaller! */
  }
  
  new_cols = skysol_new_array(m, skysol_col_t *);
  for (j = 0; j < sky->size; j++)
    new_cols[j] = sky->columns[j];

  if (sky->columns)
    skysol_free(sky->columns);
  sky->columns = new_cols;
  
  for (j = sky->size; j < m; j++) /* create new columns */
    if ((error_code = create_column(sky, j+1, 1)) != SKYSOL_OK)
      return error_code;
  
  sky->size = m;

  return SKYSOL_OK;
}



void
skysol_print_col(skysol_col_t *column, FILE *fp);

void
skysol_print_matrix(skysol_matrix_t *sky, FILE *fp)
{
  int col;
  double v;
  skysol_col_t *column;

  fprintf(fp, "SkySol %d rows, %d columns\n", sky->size, sky->size);
  
  for (col = 1; col <= sky->size; col++) {
    fprintf(fp, "Col %d \n", col);
    column = SKY_COL(sky, col);
    skysol_print_col(column, fp);
  }
}



int
skysol_solve(skysol_matrix_t *sky, double *rhs)
{
  int error_code;

  error_code = skysol_factorize(sky);
  if (error_code != SKYSOL_OK)
    return error_code;
  
  error_code = skysol_forward_reduce(sky, rhs);
  if (error_code != SKYSOL_OK)
    return error_code;
  
  error_code = skysol_diagonal_scaling(sky, rhs);
  if (error_code != SKYSOL_OK)
    return error_code;
  
  error_code = skysol_backsubstitute(sky, rhs);
  if (error_code != SKYSOL_OK)
    return error_code;
  
  return SKYSOL_OK;
}
