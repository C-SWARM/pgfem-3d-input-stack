/*
 ************************************************************************
 *			  VECTOR of floats
 * 		Description of data and operations
 */

#ifndef V$elem_type

#define V$elem_type float

typedef struct {		/* 	Vector structure		*/
	short int valid;		/* Validation code		*/
	short int ident;		/* Identification		*/
	int upb;			/* Upper bound value		*/
	int lwb;			/* Lower bound value		*/
	V$elem_type * ptr; 		/* Ptr to the vector body	*/
		}
*VECTOR, _VECTOR;

#define _V$validation_code	23567	/* VECTOR->valid contents	*/

				/* Vector identification codes		*/
#define _V$id_dynamic	1		/* Storage was obtained via malloc*/

/*
 *-----------------------------------------------------------------------
 *     			Allocation routines
 */
				/* Defining a not-dynamical vector	*/
 /* This seemed not to be working.
        The original:
#define	Vector(name,lwb,upb) V$elem_type _$_name [(upb)-(lwb)+1]; \
		VECTOR name = &{_V$validation_code, 0, (upb), (lwb), _$_name}
        The fix (Petr Krysl, 15.2.1994):
#define	Vector(name,lwb,upb) V$elem_type _V_##name [(upb)-(lwb)+1]; \
		VECTOR name = &{_V$validation_code, 0, (upb), (lwb), _V_##name}
*/
#define	Vector(name,lwb,upb) V$elem_type _V_##name [(upb)-(lwb)+1]; \
		VECTOR name = &{_V$validation_code, 0, (upb), (lwb), _V_##name}
VECTOR V_new(			/* Create a new dynamic vector		*/
	const int lwb,			/* Lower bound 			*/
	const int upb			/* Upper bound 			*/
	    );

VECTOR V_build(			/* Build a vector and assign init val	*/
	const int lwb,			/* Lower bound 			*/
	const int upb,			/* Upper bound 			*/
	...				/* DOUBLE numbers - values of 	*/
					/* vector components		*/
					/* List must contain an extra   */
					/* argument - string "END"	*/
	    );


void   V_free(			/* Delete a vector and free its storage	*/
	VECTOR v			/* Dynamic vector to be freed	*/
	     );

				/* Assure that two vectors are compatible*/
#define	V_are_compatible(v1,v2) {if( v1->lwb != v2->lwb || v1->upb != v2->upb )\
	_error("Vectors [%d:%d] and [%d:%d] are incompatible.\nFile %s, line \
	%d\n",v1->lwb,v1->upb, v2->lwb,v2->upb, __FILE__, __LINE__);}


/*
 *-----------------------------------------------------------------------
 *     		       	 Access to VECTOR fields
 */
				/* Assure the vector having been initialized*/
#define V_verify(v) if( v->valid != _V$validation_code ) \
	_error("Illegal vector.\nFile %s, line %d\n",__FILE__,__LINE__)

#define V_lwb(v) v->lwb			/* Lower vector bound		*/
#define V_upb(v) v->upb			/* Upper vector bound		*/
#define V_no_elems(v) (v->upb-v->lwb+1) /* No. of vector elements	*/

                 		/* Get ptr to the vector body		*/
V$elem_type * V_body(
	const VECTOR v 			/* Vector under operation	*/
		    );
#define V_pointer(ptr,v) V$elem_type *ptr = V_body(v)

				/* Get ptr to a vector element		*/
V$elem_type * V_elem(
	const VECTOR v,			/* Vector under operation	*/
	const int i    			/* Index			*/
		    );

/*
 *-----------------------------------------------------------------------
 *     		       	 	Assignments
 */

void V_assign(			/* Assign vector to vector		*/
	VECTOR dest,			/* Destination vector		*/
	const VECTOR source       	/* Source vector		*/
	     );

void V_clear(			/* Clear the entire vector (fill with 0)*/
	VECTOR v			/* Vector being operated	*/
	    );

void V_setto(			/* Set all vector elements to a given val*/
	VECTOR v,			/* Vector being operated	*/
	const double val        	/* Value to be set		*/
	    );

/*
 *-----------------------------------------------------------------------
 *     		  	  Compute vector norms
 */

double V_1_norm(		/* 1. vector norm = SUM[ |vector[i]| ]	*/
	const VECTOR v    	   	/* Vector under operation	*/
	       );	

double V_2_norm(		/* 2. vector norm = MAX[ |vector[i]| ]	*/
	const VECTOR v		       	/* Vector under operation	*/
	       );	

double V_e2_norm(		/* Square of the Euclid norm =         	*/
	const VECTOR v		    	/* SUM[ vector[i]^2 ]    	*/
	       );	

double V_diff_e2_norm(		/* Square of the Euclid norm for       	*/
	const VECTOR v1,  	  	/* the difference of 2 vectors 	*/
	const VECTOR v2	
	       );	



/*
 *-----------------------------------------------------------------------
 *     		       	   Vector arithmetics
 */

double V_scalar_prod( 		/* Scalar product of two vectors      	*/
	const VECTOR v1, 
	const VECTOR v2
	       );	


void V_op2(			/* Vector arithmetics: 	binary operations*/
	VECTOR dest,			/* Destination			*/
	char   sign
	  );

/*
 *-----------------------------------------------------------------------
 *     		       	  Service functions
 */

void V_compare(			/* Compare two vectors			*/
	const VECTOR v1,   		/* Vectors to be compared      	*/
	const VECTOR v2,   
	const char * message		/* Explanation message		*/
	      );	

void V_print(  			/* Print a vector     			*/
	const VECTOR v,    		/* Vector to be printed       	*/
	const char * title  		/* Explanation message		*/
	      );	

VECTOR V_read(   		/* Read a vector from stdin		*/
	const char * title		/* File is assumed to contain	*/
	      );	               	/* title, counter, and elements */

#endif
