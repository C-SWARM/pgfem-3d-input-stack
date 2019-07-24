#ifndef FEATURES_H
#define FEATURES_H

#include <iostream>
#include <vector>

class FeatureList
{
 public:
  FeatureList();
  FeatureList(std::istream*file);
  void read(std::istream *file);
  void write(std::ostream *file);

  std::vector<int> Gvert(){return gvert;};
  std::vector<int> Gcurve(){return gcurve;};
  std::vector<int> Gsurf(){return gsurf;};
  std::vector<int> Gpatch(){return gpatch;};
  std::vector<int> Gregion(){return gregion;};

  int adder(int ent_type);

  // Number of features
  int nvert(){return nv;};
  int ncurve(){return nc;};
  int nsurf(){return ns;};
  int npatch(){return np;};
  int nregion(){return nr;};

  // Number of global features
  int ngvert(){return gvert.size();};
  int ngcurve(){return gcurve.size();};
  int ngsurf(){return gsurf.size();};
  int ngpatch(){return gpatch.size();};
  int ngregion(){return gregion.size();};


 private:
  int nv,nc,ns,np,nr;
  std::vector<int> gvert,
    gcurve,
    gsurf,
    gpatch,
    gregion;
  
};

#endif // #ifndef FEATURES_H
