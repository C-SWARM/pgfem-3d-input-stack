#include <algorithm>
#include <string.h>

#include "utils.h"
#include "globalNodes.h"

using namespace std;


void GNodePointer::setGlobalNodes(int nnodes, T3d_Node *Nodes, FeatureList interfaces)
{
  // We search the interfaces to see if the node lies on or in an
  // interface feature. We determine the parent entity type and then
  // search the interface list for a matching index.

  nvert = interfaces.ngvert();
  ncurve = interfaces.ngcurve();
  nsurface = interfaces.ngsurf();
  npatch = interfaces.ngpatch();

  for(int i=0;i<nnodes;i++){
    switch(Nodes[i].etype()){

    case 1: // Vertex
      for(int j=0;j<nvert;j++){
	if(Nodes[i].eid() == (interfaces.Gvert())[j]){
	  Nodes[i].set_Global(true);
	  All.push_back( &(Nodes[i]) );
	  Vertex.push_back( &(Nodes[i]) );
	  break;
	}
      } // for j<nvert
      break;

    case 2: //Curve
      for(int j=0;j<ncurve;j++){
	if(Nodes[i].eid() == (interfaces.Gcurve())[j]){
	  Nodes[i].set_Global(true);
	  All.push_back( &(Nodes[i]) );
	  Curve.push_back( &(Nodes[i]) );
	  break;
	}
      } // for j<ncurve
      break;

    case 3: // Surface
      for(int j=0;j<nsurface;j++){
	if(Nodes[i].eid() == (interfaces.Gsurf())[j]){
	  Nodes[i].set_Global(true);
	  All.push_back( &(Nodes[i]) );
	  Surface.push_back( &(Nodes[i]) );
	  break;
	}
      } // for j<ncurve
      break;

    case 5: // Patch
      for(int j=0;j<npatch;j++){
	if(Nodes[i].eid() == (interfaces.Gpatch())[j]){
	  Nodes[i].set_Global(true);
	  All.push_back( &(Nodes[i]) );
	  Patch.push_back( &(Nodes[i]) );
	  break;
	}
      } // for j<ncurve
      break;

    default:
      break;

    } //End switch
  } // for i<nnodes

  sort_features();

} // setGlobalNodes

void GNodePointer::sort_features()
{
  //Note that this sorts the POINTERS, not the actual nodes.

  stable_sort(Vertex.begin(),Vertex.end(),compare_eid);
  stable_sort(Curve.begin(),Curve.end(),compare_eid);
  stable_sort(Surface.begin(),Surface.end(),compare_eid);
  stable_sort(Patch.begin(),Patch.end(),compare_eid);

}// sort_features

void GNodePointer::update_id(int current_dom)
{
  int adder = current_dom*nnode(); // Note that when looping through
				  // the domains, should use C
				  // idexing, i.e. begin at 0.

  // Loop through all the global nodes and assign gid numbers starting from 1.
  for(int i=0;i<nnode();i++){
    All[i]->set_GID(i+1+adder);
  }

  // Loop through global features and re-assign entity id number
  // Verticies
  adder = current_dom*nvert;
  for(int i=0;i<nvnode();i++){
    Vertex[i]->set_edid(Vertex[i]->eid()+adder);
  }

  // Curves
  adder = current_dom*ncurve;
  for(int i=0;i<ncnode();i++){
    Curve[i]->set_edid(Curve[i]->eid()+adder);
  }

//   // Surfaces
//   adder = current_dom*nsurface;
//   for(int i=0;i<nsnode();i++){
//     Surface[i]->set_edid(Surface[i]->eid()+adder);
//   }

  // Patches
  adder = current_dom*npatch;
  for(int i=0;i<npnode();i++){
    Patch[i]->set_edid(Patch[i]->eid()+adder);
  }

}

void GNodePointer::print(ostream *file)
{
  check_stream(file);

  *file << "Total number of global nodes:\t\t" << nnode() <<endl
	<< "Number of global verticies:  \t\t" << nvert << endl
	<< "Number of global curves:     \t\t" << ncurve << endl
	<< "Number of global surfaces:   \t\t" << nsurface << endl
	<< "Number of global patches:    \t\t" << npatch << endl << endl;

  *file << "Total number of global nodes on global verticies: " << nvnode() << endl;
  for(int i=0;i<nvnode();i++){
    *file << "Vertex ID: " << Vertex[i]->eid() 
	  << "\tLocal node ID: " << Vertex[i]->LID() << endl;
  }
  *file << endl;

  *file << "Total number of global nodes on global curves: " << ncnode() << endl;
  for(int i=0;i<ncnode();i++){
    *file << "Curve ID: " << Curve[i]->eid() 
	  << "\tLocal node ID: " << Curve[i]->LID() << endl;
  }
  *file << endl;

  *file << "Total number of global nodes on global surfaces: " << nsnode() << endl;
  for(int i=0;i<nsnode();i++){
    *file << "Surface ID: " << Surface[i]->eid() 
	  << "\tLocal node ID: " << Surface[i]->LID() << endl;
  }
  *file << endl;

  *file << "Total number of global nodes on global patches: " << npnode() << endl;
  for(int i=0;i<npnode();i++){
    *file << "Patch ID: " << Patch[i]->eid() 
	  << "\tLocal node ID: " << Patch[i]->LID() << endl;
  }
  *file << endl;
}

void GNodePointer::write(std::ostream *file, char *type)
{
  int print;


  //Determine type
  if(strcmp(type,"all") == 0||strcmp(type,"All") == 0||strcmp(type,"ALL") == 0)
    print = 0;
  else if(strcmp(type,"vertex") == 0||strcmp(type,"Vertex") == 0||strcmp(type,"VERTEX")==0)
    print = 1;
  else if(strcmp(type,"curve") == 0||strcmp(type,"Curve") == 0||strcmp(type,"CURVE")==0)
    print = 2;
  else if(strcmp(type,"surface") == 0||strcmp(type,"Surface") == 0||strcmp(type,"SURFACE")==0)
    print = 3;
  else if(strcmp(type,"patch") == 0||strcmp(type,"Patch") == 0||strcmp(type,"PATCH")==0)
    print = 5;
  else
    print = -1;

  switch (print){
  case 0: //All
    for(int i=0;i<nnode();i++)
      All[i]->write(file);
    break;

  case 1: //Vertex
    for(int i=0;i<nvnode();i++)
      Vertex[i]->write(file);
    break;

  case 2: //Curve
    for(int i=0;i<ncnode();i++)
      Curve[i]->write(file);
    break;

  case 3: //Surface
    for(int i=0;i<nsnode();i++)
      Surface[i]->write(file);
    break;

  case 5: //Patch
    for(int i=0;i<npnode();i++)
      Patch[i]->write(file);
    break;

  default:
    cout << "Unknown feature type for printing global node information.\n";
    break;
  }//switch print type
}// write

int GNodePointer::vgid(int idx)
{
  return Vertex[idx]->GID();
}
 
int GNodePointer::cgid(int idx)
{
  return Curve[idx]->GID();
}

int GNodePointer::sgid(int idx)
{
  return Surface[idx]->GID();
}

int GNodePointer::pgid(int idx)
{
  return Patch[idx]->GID();
}

void GNodePointer::vgid(int idx,int val)
{
  Vertex[idx]->set_GID(val);
}

void GNodePointer::cgid(int idx,int val)
{
  Curve[idx]->set_GID(val);
}

void GNodePointer::sgid(int idx,int val)
{
  Surface[idx]->set_GID(val);
}

void GNodePointer::pgid(int idx,int val)
{
  Patch[idx]->set_GID(val);
}

void GNodePointer::vdom(int idx,int val)
{
  Vertex[idx]->set_dom(val);
}

void GNodePointer::cdom(int idx,int val)
{
  Curve[idx]->set_dom(val);
}

void GNodePointer::sdom(int idx,int val)
{
  Surface[idx]->set_dom(val);
}

void GNodePointer::pdom(int idx,int val)
{
  Patch[idx]->set_dom(val);
}
