/// T3d Element datatype.  Contains connectivity and boundary info.
/// ONLY TETRAS ARE SUPPORTED!!!

#ifndef T3D_ELEMT_H
#define T3D_ELEMT_H

#include <iostream>

class T3dElement
{
 public:
  T3dElement();
  void read(std::istream *file);

  // Access to private members
  int id(){return el_id;};
  int node_id(int idx){return nodes[idx];}; // idx starts @ 0
  int e_type(){return ent_type;};
  int e_id(int adder){return ent_id+adder;};
  int e_prop(){return ent_prop;};
  int be_type(int idx){return bnd_ent_type[idx];}; // idx starts @ 0
  int be_id(int idx,int adder){return bnd_ent_id[idx]+adder;}; // idx starts @ 0
  int be_prop(int idx){return bnd_ent_prop[idx];}; // idx starts @ 0

 private:
  int el_id, nodes[4],
    ent_type, ent_id,
    ent_prop, bnd_ent_type[4],
    bnd_ent_id[4], bnd_ent_prop[4];

};

#endif // #ifndef T3D_ELEMT_H
