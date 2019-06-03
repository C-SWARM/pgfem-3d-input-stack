/// Main program for creating scaling problems

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "utils.h"
#include "options.h"
#include "t3d_io.h"
#include "globalNodes.h"
#include "connectivity.h"
#include "Features.h"
#include "boundaryConditions.h"
#include "copy_nodes.h"
#include "writeDistMesh.h"
#include "t3d_footer.h"

using namespace std;

int main(int argc, char **argv)
{
  Options options(argc,argv);
  T3d_Header MeshInfo;
  T3d_Node *Nodes,**Domain;
  vector<string> footer;
  T3dFooter Footer;
  BoundCond bnd;
  //GNodePointer GlobalNodes;

  if(argc <= 1){
    options.useage();
    return 0;
  } else {
    options.print();
  }

  check_pow2(options.np());

  ifstream t3d((options.filebase()+".t3d").c_str());
  FeatureList feats(&t3d);
  //  feats.write(&cout);
  t3d.close();

  if (options.bc()){
    t3d.open((options.filebase()+".bc.in").c_str());
    readBC(t3d,bnd);
    t3d.close();
  }

  //  read_t3d(options.filebase()+".out",&MeshInfo,&Nodes,&footer);
  read_t3d(options.filebase()+".out",&MeshInfo,&Nodes,&Footer);

  Connectivity connections(options.np());
  createDomain(&options,&MeshInfo,&feats,&connections,Nodes,&Domain);
  delete Nodes;

  //  writeDistMesh(&options,&MeshInfo,Domain,footer);

  writeDistMesh(&options,&MeshInfo,Domain,&feats,&Footer);

//   ofstream REG((options.filebase()+".reg").c_str());
//   printRegions(&REG,options.np(),feats.nregion());
//   REG.close();

  ofstream BC((options.filebase()+".bc").c_str());
  printRegions(&BC,options.np(),feats.nregion());
  BC << endl << endl;

  if (options.bc()){
    printSpecialBC(BC,bnd,feats,options.np());
  } else {
    printBC(&BC,options.np(),connections.ndom_layer(),&feats);
  }
  BC.close();

  return 0;
}


