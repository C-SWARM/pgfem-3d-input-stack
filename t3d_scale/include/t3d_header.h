#ifndef T3D_HEADER_H
#define T3D_HEADER_H

#include <iostream>

class T3d_Header
{
 public:
  T3d_Header(); /**< Default constructor. */
  T3d_Header(std::istream *file); /**< Constructor which calls read. */
  void null(); /**< Initializes all private members. */
  void populate(std::istream *file); /**< Reads in header information from file. */
  void print(std::ostream *file); /**< Prints header info to file. */
  void print(std::ostream *file, int dom); /**< Prints header info to file. */

  // Access to private members
  int mesh(){return mesh_type;};
  int elem(){return elem_degree;};
  int renum(){return renum_type;};
  int output(){return output_type;};
  int nnodes(){return nodes;};
  int ntrias(){return trias;};
  int ntetras(){return tetras;};
  int nquads(){return quads;};
  int nhexas(){return hexas;};
  int npyrams(){return pyrams;};
  int nwedges(){return wedges;};

 private:
  int mesh_type,
    elem_degree,
    renum_type,
    output_type,
    nodes,
    edges,
    trias,
    tetras,
    quads,
    hexas,
    pyrams,
    wedges;
};

#endif // #ifndef T3D_HEADER_H
