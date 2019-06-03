#include <cmath>
#include <cstring>
#include <cstdlib>
#include <climits>

#include "utils.h"

using namespace std;

void check_stream(ios *file)
{
  // Check if the file is open
  if(!file->good()){
    cout << "Error, stream is not good for i/o.\n"
	 << "Check file names and try again. Exiting.\n";
      exit(-5);
    }
}

void check_stream(std::ios *file,std::string msg)
{
  // Check if the file is open
  if(!file->good()){
    cout << "Error, stream is not good for i/o.\n"
	 << msg << endl;
      exit(-5);
    }
}

void  check_pow2(int ndom)
{
  double result;
  result = log(ndom)/log(2);

  if(result != floor(result)){
    cout << "ERROR: number of domains must be a power of 2. Exiting program.\n";
    exit(-2);
  }
}

bool compare_eid(T3d_Node* i, T3d_Node* j)
{
  return (i->eid() < j->eid());
}

bool compare_coord(T3d_Node* i, T3d_Node* j)
{
  //  double error=0.000000000005;

  //   return (fabs(i->X() - j->X()) <= error &&
  // 	  fabs(i->Y() - j->Z()) <= error &&
  // 	  fabs(i->Z() - j->Z()) <= error );

  return ( i->X() == j->X() &&
	  i->Y() == j->Y() &&
	  i->Z() == j->Z() );
}

int minimum(int nelem, int *array)
{
  int idx = 0;
  int value = INT_MAX;
  for(int i=0;i<nelem;i++){
    if(array[i] < value && array[i] != 0){
      value = array[i];
      idx = i;
    }
  }
  if(idx == 0 && array[0] == 0)
    idx = -1;
  return idx;
}
