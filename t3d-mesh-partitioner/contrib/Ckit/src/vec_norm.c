/*
 ************************************************************************
 *			    VECTOR of floats
 * 		       Compute various vector norms
 *			  and the scalar product
 */

#include "vector.h"

#include "math.h"


double V_1_norm(v)		/* SUM[ |vi| ]				*/
const VECTOR v;
{
  register int i;
  register V$elem_type *p = V_body(v);
  register double vi;
  double sum = 0;

  for(i=V_no_elems(v); --i >=0;)
  {
     vi = *p++;
     sum += ( vi<0 ? -vi : vi );
  }

  return sum;
}


double V_2_norm(v)		/* MAX[ |vi| ]				*/
const VECTOR v;
{
  register int i;
  register V$elem_type *p = V_body(v);
  register double vi;
  double max = -HUGE_VAL;

  for(i=V_no_elems(v); --i >= 0;)
  {
     vi = *p++;
     if( vi < 0 )
       vi = -vi;
     if( vi > max )
       max = vi;
  }

  return max;
}


double V_e2_norm(v)		/* SUM[ vi^2 ]				*/
const VECTOR v;
{
  register int i;
  register V$elem_type *p = V_body(v);
  register double vi;
  double sum = 0;

  for(i=V_no_elems(v); --i >= 0;)
  {
     vi = *p++;
     sum += vi*vi;
  }

  return sum;
}


double V_diff_e2_norm(v1,v2)	/* SUM[ (v1i-v2i)^2 ]	   		*/
const VECTOR v1,v2;
{
  register int i;
  register V$elem_type *p = V_body(v1);
  register V$elem_type *q = V_body(v2);
  register double vi;
  double sum = 0;

  V_are_compatible(v1,v2);

  for(i=V_no_elems(v1); --i >= 0;)
  {
     vi = (*p++) - (*q++);
     sum += vi*vi;
  }

  return sum;
}


double V_scalar_prod(v1,v2)	/* SUM[ v1i*v2i ]	   		*/
const VECTOR v1,v2;
{
  register int i;
  register V$elem_type *p = V_body(v1);
  register V$elem_type *q = V_body(v2);
  double sum = 0;

  V_are_compatible(v1,v2);

  for(i=V_no_elems(v1); --i >= 0;)
     sum += (*p++) * (*q++);

  return sum;
}
