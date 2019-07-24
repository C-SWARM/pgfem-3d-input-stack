#ifndef T3D_NODE_H
#define T3D_NODE_H

#include <iostream>

class T3d_Node
{
 public:
  T3d_Node(); /**< Default constructor. */
  void read(std::istream *file); /**< Populates node info from file. */
  void print(std::ostream *file); /**< Prints node info to file. */
  void write(std::ostream *file); /**< Writes split node. */
  void translate(double ddx, double ddy, double ddz);
  void translate(double delta[3]);
  void set_dom(int dom){domain = dom;};
  void set_GID(int id){g_id = id;};
  void set_Global(bool val){global = val;};
  void set_edid(int val){ent_did = val;};
  T3d_Node &operator=(const T3d_Node &node);

  // Access to private members
  int LID(){return l_id;};
  int GID(){return g_id;};

  int etype(){return ent_type;};
  int eid(){return ent_id;};
  int deid(){return ent_did;};
  int eprop(){return ent_prop;};
  int DOM(){return domain;};
  double X(){return x;};
  double Y(){return y;};
  double Z(){return z;};
  bool Global(){return global;};

 private:

 int l_id,
    g_id,
    ent_type,
    ent_id,
    ent_did, /**< This value changes depending on the domain */
    ent_prop,
    domain;

  double x,
    y,
    z;

  bool global;
};

#endif // #ifndef T3D_NODE_H
