/*
 ************************************************************************
 *			  VECTOR of floats
 * 			  Base operations
 */

#include "vector.h"

#include "assert.h"
#include <stdlib.h>
#include <malloc.h>

/*
 *			Create a dynamic vector
 *
 */

VECTOR V_new(lwb,upb)
const int lwb, upb;
{
  register int no_elements = upb - lwb + 1;
  register VECTOR v;

  assure(no_elements>0,"V_new: lwb > upb");
  assure((v = (VECTOR)malloc(sizeof(_VECTOR))) != (VECTOR)0, 
				"V_new: no memory");
  
  v->valid = _V$validation_code;
  v->ident = _V$id_dynamic;
  v->lwb = lwb;
  v->upb = upb;
  assure((v->ptr = (V$elem_type *)malloc(no_elements*sizeof(V$elem_type))
         ) != NULL, 
	 "V_new: no memory");
  return v;
}


#include <stdarg.h>

			/* Build a vector and assign init values	*/
			/* The arg list must contain an extra argument- */
			/* string "END"					*/
VECTOR V_build(
	const int lwb,			/* Lower bound 			*/
	const int upb,			/* Upper bound 			*/
	...
)
{
  va_list args;
  VECTOR v = V_new(lwb,upb);
  register int i;

  va_start(args,upb);			/* Init 'args' to the beginning of */
					/* the variable length list of args*/
  for(i=lwb; i<=upb; i++)
     *V_elem(v,i) = (double)va_arg(args,double);
  assure( strcmp((char *)va_arg(args,char *),"END") == 0, 
		"V_build: argument list should be terminated by \"END\" ");
  return v;
}


/*
 *			Free a dynamic vector
 *
 */

void V_free(v)
VECTOR v;
{
  assure( v->valid == _V$validation_code && 
	  (v->ident & _V$id_dynamic == _V$id_dynamic),
	  "V_free: invalid dynamic vector to free" );
  free( v->ptr );
  v->valid = 0; v->lwb = 0; v->upb = -1;
  free(v);
}


/*
 *		        Get ptr to the VECTOR body
 *
 */

V$elem_type * V_body(v)
const VECTOR v;
{
  assure( v->valid == _V$validation_code, "V_body: invalid VECTOR ptr");
  assure( v->upb >= v->lwb, "V_body: VECTOR must have been corrupted");
  return v->ptr;
}


/*
 *		    Get ptr to the specified VECTOR element
 *
 */

V$elem_type * V_elem(v,i)
const VECTOR v;   				/* Vector under operation */
const int i;                                    /* Index of the req. elem */
{
  assure( v->valid == _V$validation_code, "V_elem: invalid VECTOR ptr");
  if( i>V_upb(v) || i<V_lwb(v) )
    _error("V_elem: index %d is out of range [%d:%d]",i,V_lwb(v),V_upb(v));
  return v->ptr + (i-V_lwb(v));
}


/*
 *=======================================================================
 *			   VECTOR assignments
 */


/*
 *		      Clear all the vector elements
 *
 */

void V_clear(v)
VECTOR v;   	      
{
  register int i;
  register V$elem_type *p = V_body(v);

  for(i=V_no_elems(v); --i >=0;)
     *p++ = 0;
}


/*
 *		   Set all the vector elements to a given value
 *
 */

void V_setto(v,val)
VECTOR v;   			   
const double val;                          
{
  register int i;
  register V$elem_type *p = V_body(v);

  for(i=V_no_elems(v); --i >= 0;)
     *p++ = val;
}

/*
 *		       Assign one vector to another
 *
 */

void V_assign(dest,src)
VECTOR dest;   					/* Destination		*/
const VECTOR src;                               /* Source		*/
{
  register int i;
  register V$elem_type *p = V_body(dest);
  register V$elem_type *q = V_body(src);

  V_are_compatible(dest,src);

  for(i=V_no_elems(src); --i >= 0;)
     (*p++) = (*q++);

}
