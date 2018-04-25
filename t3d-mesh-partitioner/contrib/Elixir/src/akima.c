#include "Etypes.h"


typedef FPNum Vector[3];

#define EPS 1.e-9

static void
akima_tangents(Vector tangent[],
               int n,
               Vector ctrl_point[]
               );

#define ALLOC_ARRAY(NELT, TYPE) \
                      ((TYPE *) calloc((NELT), (size_t) sizeof(TYPE)))
#define FREE(ptr) free(ptr)

#define X 0
#define Y 1
#define Z 2

#define Copy_vec(dest, src)                     \
  {                                             \
    (dest)[X] = (src)[X];                       \
    (dest)[Y] = (src)[Y];                       \
    (dest)[Z] = (src)[Z];                       \
  }
#define Subt_vec(ab, a, b)                      \
  {                                             \
    (ab)[X] = (b)[X] - (a)[X];                  \
    (ab)[Y] = (b)[Y] - (a)[Y];                  \
    (ab)[Z] = (b)[Z] - (a)[Z];                  \
  }
#define Add_vec(ab, a, b)                       \
  {                                             \
    (ab)[X] = (b)[X] + (a)[X];                  \
    (ab)[Y] = (b)[Y] + (a)[Y];                  \
    (ab)[Z] = (b)[Z] + (a)[Z];                  \
  }


void
akima_spline_curve(int   *m,            /* INOUT number of vertices */
                   Vector vertices[],   /* OUT points on the curve */
                   int    n,            /* IN number of control points */
                   Vector ctrl_point[], /* IN control points */
                   int    span_size[]   /* IN number of points on each span */
                   );


void
akima_spline_curve(int   *m,            /* INOUT number of vertices */
                   Vector vertices[],   /* OUT points on the curve */
                   int    n,            /* IN number of control points */
                   Vector ctrl_point[], /* IN control points */
                   int    span_size[]   /* IN number of points on each span */
                   )
{
  Vector coeff[4];
  register FPNum *a, *b, *c, *d;
  Vector *tangent;              /* tangent vectors in the control points */
  Vector *tg, *tg1;
  Vector *v = vertices;
  Vector *hi_v;
  Vector *q = ctrl_point;
  Vector *hi_q = q + n -1;
  Vector *q1;
  int j;
  double t, delta_t, h;

  tangent = ALLOC_ARRAY(n, Vector);
  akima_tangents(tangent, n, ctrl_point);

  tg = tangent;
  for (; q < hi_q; q++, tg++, span_size++) {
    q1 = q + 1; tg1 = tg + 1;
    a = &coeff[0][X]; b = a + 3; c = b + 3; d = c + 3;
    for (j = 0; j < 3; j++) {
      h = (*q1)[j] - (*q)[j];
      *a++ = (*tg)[j] + (*tg1)[j] - 2 * h;
      *b++ = 3 * h - 2 * (*tg)[j] - (*tg1)[j];
      *c++ = (*tg)[j];
      *d++ = (*q)[j];
    }
    Copy_vec(*v, *q); /* trivial first point */
    t = delta_t  = 1.0;           /* span size */
    hi_v = v++  + *span_size;
    for (; v < hi_v; v++, t += delta_t) {
      a = &coeff[0][X]; b = a + 3; c = b + 3; d = c + 3;
      for (j = 0; j < 3; j++, a++, b++, c++, d++)
        (*v)[j] = ((*a * t + *b) * t + *c) * t + *d;
    }
  }
  Copy_vec(*v, *q); /* trivial last point */
  *m = (v - vertices) + 1;
  FREE(tangent);
}


static void
akima_tangents(Vector tangent[],
               int n,
               Vector ctrl_point[]
               )
{
  int i, imax = 3 * n;
  Vector *diff, *d, *v;
  double len1, len2, *result;
  register double *d0, *d1, *d2, *d3;

  diff = ALLOC_ARRAY(n+3, Vector);
  d = diff + 2;
  v = ctrl_point;
  for (i = 2; i <= n; i++, d++, v++)
    Subt_vec(*d, *v, *(v+1));
#define Invent_diff(diff_vec, a, b)  \
     { d = (Vector *)diff_vec; Add_vec(*d, a, a); Subt_vec(*d, b, *d); }
  Invent_diff(diff[1], diff[2], diff[3]);
  Invent_diff(diff[0], diff[1], diff[2]);
  Invent_diff(diff[n + 1], diff[n], diff[n - 1]);
  Invent_diff(diff[n + 2], diff[n + 1], diff[n]);
#undef Invent_diff
  d0 = &diff[0][X]; d1 = &diff[1][X]; d2 = &diff[2][X]; d3 = &diff[3][X];
  result = &tangent[0][X];
  for (i = 0; i < imax; i++, d0++, d1++, d2++, d3++) {
    len1 = fabs(*d3 - *d2);
    len2 = fabs(*d1 - *d0);
    if (len1 + len2 < EPS)
      len1 = len2 = 1;
    *result++ = (len1 * (*d1) + len2 * (*d2)) / (len1 + len2);
  }
  FREE(diff);
}
