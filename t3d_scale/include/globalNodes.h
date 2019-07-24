#ifndef GLOBAL_NODES_H
#define GLOBAL_NODES_H

#include <vector>
#include <iostream>

#include "t3d_node.h"
#include "Features.h"


class GNodePointer
{

 public:
  GNodePointer(){};
  void setGlobalNodes(int nnodes, T3d_Node *Nodes, FeatureList interfaces);
  std::vector< T3d_Node* > all(){return All;};
  std::vector< T3d_Node* > vertex(){return Vertex;};
  T3d_Node* vertex(int i){return Vertex[i];};
  std::vector< T3d_Node* > curve(){return Curve;};
  T3d_Node* curve(int i){return Curve[i];};
  std::vector< T3d_Node* > patch(){return Patch;};
  T3d_Node* patch(int i){return Patch[i];};
  void sort_features(); /**< Sorts the feature GNodePointers by eid */
  void update_id(int current_dom);
  int get_gid(int dom);
  void print(std::ostream *file);
  void write(std::ostream *file, char *type); /**< writes node info
						 according to type in
						 split format */

  // Get Global ID
  int vgid(int idx);
  int cgid(int idx);
  int sgid(int idx);
  int pgid(int idx);

  // Set Global ID
  void vgid(int idx,int val);
  void cgid(int idx,int val);
  void sgid(int idx,int val);
  void pgid(int idx,int val);

  // Set domain
  void vdom(int idx,int val);
  void cdom(int idx,int val);
  void sdom(int idx,int val);
  void pdom(int idx,int val);

  int nnode(){return All.size();};
  int nvnode(){return Vertex.size();};
  int ncnode(){return Curve.size();};
  int nsnode(){return Surface.size();};
  int npnode(){return Patch.size();};

 private:
  int nvert,ncurve,nsurface,npatch;
  std::vector< T3d_Node* > All;
  std::vector< T3d_Node* > Vertex;
  std::vector< T3d_Node* > Curve;
  std::vector< T3d_Node* > Surface;
  std::vector< T3d_Node* > Patch;

};
#endif //#ifndef GLOBAL_NODES_H
