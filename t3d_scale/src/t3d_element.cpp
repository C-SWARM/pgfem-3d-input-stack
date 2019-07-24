#include "t3d_element.h"
#include "utils.h"

#include <sstream>

using namespace std;

T3dElement::T3dElement()
{
  el_id = ent_type = ent_id = 0;
  for(int i=0;i<4;i++){
    nodes[i] = bnd_ent_type[i] = bnd_ent_id[i] = bnd_ent_prop[i] = 0;
  }
}

void T3dElement::read(istream *file)
{
  // Note, T3d must be run with option -p 8
  check_stream(file);

//   el_id = 0;

//   stringstream ss;
//   string line;


//   getline(*file,line);

//   cout << line << endl;
//   ss << line;

  *file >> el_id;
  for(int i=0;i<4;i++)
    *file >> nodes[i];
  *file >> ent_type >> ent_id >> ent_prop;
  for(int i=0;i<4;i++)
    *file >> bnd_ent_id[i];
  for(int i=0;i<4;i++)
    *file >> bnd_ent_type[i];
  for(int i=0;i<4;i++)
    *file >> bnd_ent_prop[i];

} // read
