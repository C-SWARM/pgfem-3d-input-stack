/*
 ************************************************************************
 *			    VECTOR of floats
 * 		   	 Some service functions
 */

#include "vector.h"
#include "math.h"
#include <stdio.h>


/*
 *	Compare two vectors
 */

void V_compare(v1,v2,msg)
const VECTOR v1,v2;			/* Vectors to be compared	*/
const char * msg;			/* Explanation message		*/
{
  double max_disc = -1;			/* Maximal discrepancy		*/
  int index_max_disc;			/*    index for the corr. element*/
  register int i;

  printf("\n\nCompare two [%d:%d] vectors - %s\n",
	  V_lwb(v1), V_upb(v1), msg);
  V_are_compatible(v1,v2);

  for(i=V_lwb(v1); i<=V_upb(v1); i++)
  {
    double delta = fabs( *V_elem(v1,i) - *V_elem(v2,i) );
    if( delta > max_disc )
    {
	max_disc = delta;
	index_max_disc = i;
    }
  }

  printf("\nMaximal discrepancy    \t\t%g",max_disc);
  printf("\n   reached at point no.\t\t%d",index_max_disc);
  {
    double v1i = *V_elem(v1,index_max_disc);
    double v2i = *V_elem(v2,index_max_disc);
    printf("\n Vector 1 at this point    \t\t%g",v1i);
    printf("\n Vector 2 at this point    \t\t%g",v2i);
    printf("\n Absolute error v2[i]-v1[i]\t\t%g",v2i-v1i);
    printf("\n Relative error\t\t\t\t%g\n",
		(v2i-v1i)/fmax(fabs(v2i+v1i)/2,1e-7) );
  }
  {
    double v1n = sqrt( V_e2_norm(v1) );
    double v2n = sqrt( V_e2_norm(v2) );
    double vdn = sqrt( V_diff_e2_norm(v1,v2) );
    printf("\n||v1||   \t\t\t%g",v1n);
    printf("\n||v2||   \t\t\t%g",v2n);
    printf("\n||v1-v2||\t\t\t\t%g",vdn);
    printf("\n||v1-v2||/sqrt(||v1|| ||v2||)\t\t%g",
           vdn/fmax( sqrt(v1n*v2n), 1e-7 )         );
  }

}


 
/*
 *	Print a vector
 */

void V_print(v,msg)
const VECTOR v;				/* Vector to be printed		*/
const char * msg;			/* Explanation message		*/
{
  const int elems_per_line = 10;	/* No. of elems per line	*/
  extern const char _Minuses[];
  register int i,j,k;

  printf("\n\nvector [%d:%d] - %s\n\n",V_lwb(v),V_upb(v),msg);
  for(i=V_lwb(v); i<=V_upb(v); i+=elems_per_line)
  {
    k = imin(i+elems_per_line-1,V_upb(v)); /* Index for the last elem at line*/
    for(j=i; j<=k; j++)			/* Print indices		*/
       printf("%6d\t",j);
    printf("\n%s\n",_Minuses);
    
    for(j=i; j<=k; j++)			/* Print values 		*/
       printf("%6g\t",*V_elem(v,j));
    printf("\n\n");
  }
}
  
