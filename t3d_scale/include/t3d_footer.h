/// Class serves to store the connectivity information and boundary
/// information for the T3d Mesh.

#ifndef T3D_FOOTER_H
#define T3D_FOOTER_H

#include <iostream>
#include "t3d_element.h"
#include "Features.h"

class T3dFooter
{
 public:
  T3dFooter(){init_bit = false;};
  T3dFooter(std::istream *file,int n_el);
  void init(int n_el);
  void read(std::istream *file);
  void write(std::ostream *file, FeatureList *Features, int dom);


 private:
  T3dElement *Elements;
  int nelem;
  bool init_bit;


};


#endif // #ifndef T3D_FOOTER_H
