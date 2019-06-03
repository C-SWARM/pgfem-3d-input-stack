#ifndef OPTIONS_H
#define OPTIONS_H

#include <string>

class Options
{
 public:
  Options();
  Options(int argc, char **argv);
  void print();
  void useage();

  // Access to private members
  int np(){return num_proc;};
  const char* in(){return input.c_str();};
  const char* out(){return output.c_str();};
  std::string filebase(){return Filebase;};

  double x(){return dx;};
  double y(){return dy;};
  double z(){return dz;};
  //  double* trans(){return double temp = {dx,dy,dz};};

  inline bool bc(){return BC;};

 private:
  int num_proc;
  std::string input,
    output,
    Filebase;
  double dx,
    dy,
    dz;

  bool BC;
};

#endif // #ifndef OPTIONS_H
