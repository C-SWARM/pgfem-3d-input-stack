#include "utils.h"
#include "t3d_header.h"

#include <cstdlib>

using namespace std;

/************************************************************/
/*                     MEMBER FUNCTIONS                     */
/************************************************************/

void T3d_Header::null()
{
  mesh_type = 0;
  elem_degree = 0;
  renum_type = 0;
  output_type = 0;
  nodes = 0;
  edges = 0;
  trias = 0;
  tetras = 0;
  quads = 0;
  hexas = 0;
  pyrams = 0;
  wedges = 0;
}

T3d_Header::T3d_Header()
{
  this->null();
}

T3d_Header::T3d_Header(istream *file)
{
  this->null();
  this->populate(file);
}

void T3d_Header::populate(istream *file)
{
  check_stream(file);

  *file >> mesh_type >> elem_degree >> renum_type >> output_type;

  switch(mesh_type){
  case 3: // tria/tetra mesh
    *file >> nodes >> edges >> trias >> tetras;
    break;

  case 4: // quad/hex mexh
    *file >> nodes >> edges >> quads >> hexas;
    break;

  case 7: // mixed mesh
    *file >> nodes >> edges >> trias >> quads
	 >> tetras >> pyrams >> wedges >> hexas;
    break;

  default:
    cout << "Unknown mesh type. Exiting...\n";
    exit(-2);
    break;
  } // switch(type)
}

void T3d_Header::print(ostream *file)
{
  check_stream(file);

  *file << "\t" << mesh_type << "\t" << elem_degree 
	<< "\t" << renum_type << "\t" << output_type << endl;

  switch(mesh_type){

  case 3: // tria/tetra mesh
    *file  << "\t" << nodes  << "\t" << edges  << "\t"
	   << trias  << "\t" << tetras << endl << endl;
    break;

  case 4: // quad/hex mexh
    *file  << "\t" << nodes  << "\t" << edges  << "\t" 
	   << quads  << "\t" << hexas << endl << endl;
    break;

  case 7: // mixed mesh
    *file  << "\t" << nodes  << "\t" << edges  << "\t" 
	   << trias  << "\t" << quads << "\t" << tetras
	   << "\t" << pyrams  << "\t" << wedges  << "\t" 
	   << hexas << endl <<endl;
    break;

  default:
    cout << "Unknown mesh type. Exiting...\n";
    exit(-2);
    break;
  } // switch(type)
}

void T3d_Header::print(ostream *file,int dom)
{
  check_stream(file);

  *file << "\t" << mesh_type << "\t" << elem_degree 
	<< "\t" << renum_type << "\t" << output_type 
	<< "\t" << dom << endl;

  switch(mesh_type){

  case 3: // tria/tetra mesh
    *file  << "\t" << nodes  << "\t" << edges  << "\t"
	   << trias  << "\t" << tetras << endl << endl;
    break;

  case 4: // quad/hex mexh
    *file  << "\t" << nodes  << "\t" << edges  << "\t" 
	   << quads  << "\t" << hexas << endl << endl;
    break;

  case 7: // mixed mesh
    *file  << "\t" << nodes  << "\t" << edges  << "\t" 
	   << trias  << "\t" << quads << "\t" << tetras
	   << "\t" << pyrams  << "\t" << wedges  << "\t" 
	   << hexas << endl <<endl;
    break;

  default:
    cout << "Unknown mesh type. Exiting...\n";
    exit(-2);
    break;
  } // switch(type)
}
