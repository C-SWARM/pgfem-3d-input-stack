#include "t3d_footer.h"
#include "utils.h"

#include <cstdlib>


using namespace std;

T3dFooter::T3dFooter(istream *file,int n_el)
{
  //initialize
  init(n_el);
  read(file);
}

void T3dFooter::init(int n_el)
{
  nelem = n_el;
  Elements = new T3dElement[nelem];
  init_bit = true;
}

void T3dFooter::read(istream *file)
{
  check_stream(file);

  // Check init_bit
  if(!init_bit){
    cout << "ERROR: Element container not initialized! Exiting...\n";
    exit(-10);
  }

  for(int i=0;i<nelem;i++){
    Elements[i].read(file);
  }
}

void T3dFooter::write(ostream *file, FeatureList *Features, int dom)
{
  check_stream(file);

  int adder;
  string tab="\t";

  T3dElement *c_el;

  // Write elements to file
  for(int i=0;i<nelem;i++){

    //set pointer to current element (just keeps me from having to
    //write Elements[i] a million times.
    c_el = &Elements[i];

    // Element id
    *file <<tab<< c_el->id();

    // Element connectivity
    for(int j=0;j<4;j++)
      *file << tab << c_el->node_id(j);

    // Element Ent type
    *file << tab << c_el->e_type();

    // Get Ent id for domain and write
    adder = dom*Features->adder(c_el->e_type());
    *file << tab << c_el->e_id(adder) << tab << c_el->e_prop();

    // Bounding element ent ids
    for(int j=0;j<4;j++){
      adder = dom*Features->adder(c_el->be_type(j));
      *file << tab << c_el->be_id(j,adder);
    }

    // Bounding ent types
    for(int j=0;j<4;j++)
      *file << tab << c_el->be_type(j);

    /* t3d2psifel does not output the bounding element property data
       and as we are mimicking the format from this program, we will
       not either

     // Bounding element props
     for(int j=0;j<4;j++)
        *file << tab << c_el->be_prop(j);
    */

    *file << endl;
  } // Write elements to file

}
