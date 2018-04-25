/*		Test for VECTOR managing routines		*/

#include "vector.h"


test_alloc()			/* Vector allocating			*/
{
 /* Vector(v1,1,20); */
  VECTOR v1, v2, v3, v4;

  printf("\n\n\tTest vector allocating\n");

  v1 = V_new(1,20);
  v2 = V_new(1,20);
  v3 = V_new(0,19);
  printf("\nv1 is declared as automatic [1:20] vector");
  printf("\nv2[1:20] and v3[0:19] have been constructed dynamically");
  V_are_compatible(v1,v2);
  printf("\nv1 and v2 are compatible");
  V_free(v2);
  printf("\nv2 has been freed");

  printf("\nv2 is builded with values (1,2,3,4,5)\n");
  v2 = V_build(1,5,1.,2.,3.,4.,5.,"END");
  V_print(v2,"vector created");

  printf("\nInvalid call to V_build: V_build(1,5,1,2.,3.,4.,5.,\"END\")\n");
  v2 = V_build(1,5,1,2.,3.,4.,5.,"END");

/*
  printf("\nAn attempt to compare v1 and v3");
  V_are_compatible(v1,v3);
  
  printf("\nAn attempt to free automatic vector v1");
  V_free(v1);
 */
}


test_base()			/* Test base vector operations		*/
{
  VECTOR v, v1, v2;
  double norm;

  printf("\n\nTest base VECTOR operations\n");

  v = V_new(1,10);
/*  v1 = V_new(2,10); */
  v1 = V_new(1,10);
  printf("\nCreated vectors v[%d:%d], v1[%d:%d]", 
	V_lwb(v),V_upb(v),V_lwb(v1),V_upb(v1));

  V_clear(v);
  V_print(v,"v after clear");

  V_setto(v1,4.2);
  V_print(v1,"v1 after all elements are set to 4.2");

  V_assign(v,v1);
  V_print(v,"v after assignments v=v1");
  V_print(v1,"v1 after assignments v=v1");

  V_setto(v,-2);
  *V_elem(v,2) = 4;
  V_print(v,"v after been set to -2.0 and v[2]=4");
  printf("\n1. norm of v	= %g",V_1_norm(v));
  printf("\n2. norm of v	= %g",V_2_norm(v));
  printf("\nE. norm of v ^2	= %g",V_e2_norm(v));
  printf("\ncomputed via v*v	= %g",V_scalar_prod(v,v));
  printf("\n||v-v1||^2		= %g",V_diff_e2_norm(v,v1));

  printf("\n Scalar prod. v*v1	= %g",V_scalar_prod(v,v1));

  V_compare(v,v1,"Compare v and v1");

  printf("\nAn attempt to access 13. element of v1");
  V_elem(v1,13);
}


main()
{
  test_alloc();
  test_base();
}
