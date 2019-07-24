#ifndef BOUNDARYCONDITIONS_H
#define BOUNDARYCONDITIONS_H

#include <iostream>
#include <vector>
#include "Features.h"

struct BOUNDARY_CONDITIONS{
  std::vector<int> feat;
  std::vector<int> id;
  std::vector<int> bc;
};

typedef struct BOUNDARY_CONDITIONS BoundCond;

void readBC(std::istream &in,
	    BoundCond &bc);

void printBC(std::ostream *file,
	     int ndom,
	     int ndom_layer,
	     FeatureList *feats);

void printRegions(std::ostream *file, int ndom, int nreg);

/// Print boundary conditions read from file
void printSpecialBC(std::ostream &out,
		    BoundCond &bc,
		    FeatureList &feats,
		    int ndom);
		    

#endif // #ifndef BOUNDARYCONDITIONS_H
