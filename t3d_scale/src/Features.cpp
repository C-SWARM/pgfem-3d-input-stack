#include <sstream>
#include <string>
#include <string.h>

#include "utils.h"
#include "Features.h"

using namespace std;

FeatureList::FeatureList()
{
  nv = nc = ns = np = nr = 0;
} // constructor

FeatureList::FeatureList(istream *file)
{
  nv = nc = ns = np = nr = 0;
  read(file);
}

void FeatureList::read(istream *file)
{
  check_stream(file);

  char line[256];
  string sval;
  int ival;

  while(!(*file).eof()){
    (*file).getline(line,256);
    stringstream sstream;
    sstream << line;
    sstream >> sval;

    //Comment
    if(strcmp(&sval[0],"#") == 0)
      continue;

    //Vertex
    else if(strcmp(sval.c_str(),"vertex") == 0
	    || sval.compare("Bnd_vertex") == 0){
      nv++;
      sstream >> ival;
      while(sstream){
	sstream >> sval;
	if(strcmp(sval.c_str(),"global") == 0){
	  gvert.push_back(ival);
	  break;
	}
      }
    }

    //Curve
    else if(strcmp(sval.c_str(),"curve") == 0){
      nc++;
      sstream >> ival;
      while(sstream){
	sstream >> sval;
	if(strcmp(sval.c_str(),"global") == 0){
	  gcurve.push_back(ival);
	  break;
	}
      }
    }

    //Surface
    else if(strcmp(sval.c_str(),"surface") == 0
	    || sval.compare("Bnd_surface") == 0){
      ns++;
      sstream >> ival;
      while(sstream){
	sstream >> sval;
	if(strcmp(sval.c_str(),"global") == 0){
	  cout << "WARNING::Surfaces as global/interficial features is"
	       << " **NOT** currently implemented!\n";
	  gsurf.push_back(ival);
	  break;
	}
      }
    }

    //Patch
    else if(strcmp(sval.c_str(),"patch") == 0){
      np++;
      sstream >> ival;
      while(sstream){
	sstream >> sval;
	if(strcmp(sval.c_str(),"global") == 0){
	  gpatch.push_back(ival);
	  break;
	}
      }
    }

    // Region
    else if(strcmp(sval.c_str(),"region") == 0){
      nr++;
      sstream >> ival;
      while(sstream){
	sstream >> sval;
	if(strcmp(sval.c_str(),"global") == 0){
	  cout << "WARNING::Regions as global/interficial features is"
	       << " **NOT** currently implemented!\n";
	  gregion.push_back(ival);
	  break;
	}
      }
    }

  } // while not EOF

} // read

void FeatureList::write(ostream *file)
{
  check_stream(file);

  // Write a feature list to file (report style)

  *file << "*** Feature Overview ***\n"
	<< "Total number of features: \t\t" << nv+nc+ns+np+nr << endl
	<< "Total number of verticies:\t\t" << nv << endl
	<< "Total number of curves:   \t\t" << nc << endl
	<< "Total number of surfaces: \t\t" << ns << endl
	<< "Total number of patches:  \t\t" << np << endl
	<< "Total number of regions:  \t\t" << nr << endl << endl;

  *file << "*** Global Feature List ***\n"
	<< "Total number of global features:\t" 
	<< ngvert()+ngcurve()+ngsurf()+ngpatch()+ngregion() << endl << endl;

  *file << "Number of global verticies:\t" << ngvert() << endl
	<< "Global vertex ID's: ";
  for(int i=0;i<ngvert();i++)
    *file << gvert[i] << " ";
  *file << endl << endl;

  *file << "Number of global curves:\t" << ngcurve() << endl
	<< "Global curve ID's: ";
  for(int i=0;i<ngcurve();i++)
    *file << gcurve[i] << " ";
  *file << endl << endl;

  *file << "Number of global surfaces:\t" << ngsurf() << endl
	<< "Global surface ID's: ";
  for(int i=0;i<ngsurf();i++)
    *file << gsurf[i] << " ";
  *file << endl << endl;

  *file << "Number of global patches:\t" << ngpatch() << endl
	<< "Global patch ID's: ";
  for(int i=0;i<ngpatch();i++)
    *file << gpatch[i] << " ";
  *file << endl << endl;

  *file << "Number of global regions:\t" << ngregion() << endl
	<< "Global region ID's: ";
  for(int i=0;i<ngregion();i++)
    *file << gregion[i] << " ";
  *file << endl << endl;
}

int FeatureList::adder(int ent_type)
{
  switch(ent_type){
  case 1: // vertex
    return nv;

  case 2: //curve
    return nc;

  case 3: // surf
    return ns;

  case 4: // region
    return nr;

  case 5: //patch
    return np;

  default:
    return 0;
  }
}
