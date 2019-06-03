#include "utils.h"
#include "boundaryConditions.h"

#include <cstdlib>

//#define MOD_ENT_ID 1
#ifndef MOD_ENT_ID
#define MOD_ENT_ID 0
#endif

using namespace std;

void readBC(std::istream &in,
	    BoundCond &bc)
{
  check_stream(&in);

  int feat, id;
  std::vector<int> bc_t(3);
  while (!in.eof()) {
    in >> feat >> id >> bc_t[0] >> bc_t[1] >> bc_t[2]; 
    bc.feat.push_back(feat);
    bc.id.push_back(id);
    bc.bc.insert(bc.bc.end(),bc_t.begin(),bc_t.end());
  }
}

void printBC(ostream *file, int ndom, int ndom_layer, FeatureList *feats)
{
  check_stream(file);

  int nvert = feats->nvert();
  int ncurve = feats->ncurve();
  int npatch = feats->npatch();

  // determine the domains which live on the top layer.
  int top_start = ndom - ndom_layer;

  // Print number of BCs
  *file << 2*ndom_layer*9 << endl;

  // Fixed side
  for(int i=0;i<ndom_layer;i++){
    //Print verticies
    *file << 1 << " " << 1 + i*nvert << " 1 1 1" << endl
	  << 1 << " " << 2 + i*nvert << " 1 1 1" << endl
	  << 1 << " " << 3 + i*nvert << " 1 1 1" << endl
	  << 1 << " " << 4 + i*nvert << " 1 1 1" << endl;

    //Print curves
    *file << 2 << " " << 1 + ncurve*i << " 1 1 1" << endl
	  << 2 << " " << 2 + ncurve*i << " 1 1 1" << endl
	  << 2 << " " << 3 + ncurve*i << " 1 1 1" << endl
	  << 2 << " " << 4 + ncurve*i << " 1 1 1" << endl;

    //Print surfaces (Not used)

    //Print patches
    *file << 5 << " " << 5 + npatch*i << " 1 1 1" << endl;
  }

  // Loaded side
  for(int i=top_start;i<ndom;i++){

    //Print verticies
    *file << 1 << " " << 5 + i*nvert << " 1 1 -1" << endl
	  << 1 << " " << 6 + i*nvert << " 1 1 -1" << endl
	  << 1 << " " << 7 + i*nvert << " 1 1 -1" << endl
	  << 1 << " " << 8 + i*nvert << " 1 1 -1" << endl;

    //Print curves
    *file << 2 << " " << 5 + ncurve*i << " 1 1 -1" << endl
	  << 2 << " " << 6 + ncurve*i << " 1 1 -1" << endl
	  << 2 << " " << 7 + ncurve*i << " 1 1 -1" << endl
	  << 2 << " " << 8 + ncurve*i << " 1 1 -1" << endl;

    //Print surfaces (Not used)

    //Print patches
    *file << 5 << " " << 6 + npatch*i << " 1 1 -1" << endl;
  }

} // printBC

void printRegions(std::ostream *file, int ndom, int nreg)
{
  //ndom => number of domains
  //nreg => number of regions in each domain.

  // Note that the material type must be set by hand if not simply material 0

  check_stream(file);

  *file << ndom*nreg << endl;
  int count = 1;
  for(int i=0;i<ndom;i++){
    for(int j=0;j<nreg;j++){
      *file << "4 " << count << " 0 0" <<endl;
      count++;
    }
  }


} // printRegions

void printSpecialBC(std::ostream &out,
		    BoundCond &bc,
		    FeatureList &feats,
		    int ndom)
{
  check_stream(&out);

  int mod_ndom = 1;
  if(MOD_ENT_ID){
    mod_ndom = ndom;
  }

  out << bc.feat.size()*mod_ndom << std::endl;

  for (int i=0; i< bc.feat.size(); i++){
    switch (bc.feat[i]) {
    case 1: // vertex
      for (int j=0; j<mod_ndom; j++){
	out << "1 " << bc.id[i] + j*feats.nvert() << " "
	    << bc.bc[i*3] << " "
	    << bc.bc[i*3+1] << " "
	    << bc.bc[i*3+2] << std::endl;
      }
      break;

    case 2: // curve
      for (int j=0; j<mod_ndom; j++){
	out << "2 " << bc.id[i] + j*feats.ncurve() << " "
	    << bc.bc[i*3] << " "
	    << bc.bc[i*3+1] << " "
	    << bc.bc[i*3+2] << std::endl;
      }
      break;

    case 5: // patch
      for (int j=0; j<mod_ndom; j++){
	out << "5 " << bc.id[i] + j*feats.npatch() << " "
	    << bc.bc[i*3] << " "
	    << bc.bc[i*3+1] << " "
	    << bc.bc[i*3+2] << std::endl;
      }
      break;

    default:
      std::cerr << "Unsupported feature type in " << __func__  
		<< bc.feat[i] << std::endl;
      abort();
    }
  }

}
