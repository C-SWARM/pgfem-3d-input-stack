#ifndef CONNECTIVITY_H
#define CONNECTIVITY_H

#include <iostream>

class Connectivity
{
 public:
  Connectivity(){};
  Connectivity(int ndom); //Constructor which sets numdom and calls build
  void Build(int ndom);
  void Print(std::ostream *file);
  ~Connectivity();
  int ndom_layer(){return nlaydom;};
  int nlayers(){return nlayer;};
  int nrows(){return nrow;};
  int ncols(){return ncol;};

  int ***corner; //numdom x 8 x 7
  int ***edgecurve; //numdom x 12 x 3
  int **patch; //numdom x 6

 private:
  int numdom,nlaydom,nlayer,nrow,ncol;
  void layer(int x, int y, int clayer, int tlayer);
  void sanity();


};

#endif // #ifndef CONNECTIVITY_H

/// Connectivity by feature (number of domains includes own)
// Patches connect to two domains
// Edgecurves connect to as many as 4 domains
// Verticies connect to as many as 8 domains
