#include <vector>
#include <algorithm>
#include <fstream>

#include "utils.h"
#include "globalNodes.h"
#include "copy_nodes.h"

//#define MOD_ENT_ID 1
#ifndef MOD_ENT_ID
#define MOD_ENT_ID 0
#endif

using namespace std;

void createDomain(Options *options,T3d_Header *MeshInfo, FeatureList *Features,
		  Connectivity *conn, T3d_Node *Nodes, T3d_Node ***Domain)
{

  int ndom = options->np();
  int nnodes = MeshInfo->nnodes();

  // Allocate the Domain and global nodes
  *Domain = new T3d_Node* [ndom];

  // Populate nodal information for each domain
  for(int i=0;i<ndom;i++){
    (*Domain)[i] = new T3d_Node[nnodes];
    for(int j=0;j<nnodes;j++){
      // Copy information
      (*Domain)[i][j] = Nodes[j];

      // Set the domain
      (*Domain)[i][j].set_dom(i+1);

      // Index the t3d feature ID
      int mod = 0;
      if(MOD_ENT_ID){
	mod = i;
      }

      switch ((*Domain)[i][j].etype()){
      case 1://Vertex
	(*Domain)[i][j].set_edid(Nodes[j].eid()+mod*Features->nvert());
	break;

      case 2://Curve
	(*Domain)[i][j].set_edid(Nodes[j].eid()+mod*Features->ncurve());
	break;

      case 3://Surface
	(*Domain)[i][j].set_edid(Nodes[j].eid()+mod*Features->nsurf());
	break;

      case 4://Region
	(*Domain)[i][j].set_edid(Nodes[j].eid()+mod*Features->nregion());
	break;

      case 5://Patch
	(*Domain)[i][j].set_edid(Nodes[j].eid()+mod*Features->npatch());
	break;

      default:
	cout << "WARNING:: Unknown feature type " << (*Domain)[i][j].etype() 
	     << " encountered in " << __func__ <<endl;
	break;
      } // Switch

    } // j<nnodes
  } // i<ndom

  // Translate domains to proper location
  int cur_dom = 0;

  cout << "layers:\t" << conn->nlayers() << endl
       << "rows:  \t" << conn->nrows() << endl
       << "cols:  \t" << conn->ncols() << endl;

  for(int i=0;i<conn->nlayers();i++){
    for(int j=0;j<conn->nrows();j++){
      for(int k=0;k<conn->ncols();k++){
	for(int l=0;l<nnodes;l++){
	  (*Domain)[cur_dom][l].translate(j*options->x(),k*options->y(),i*options->z());
	  (*Domain)[cur_dom][l].set_GID(0);
	} // l<nnodes
	cur_dom++;
      } // k<ncols
    }// j<nrows
  }// i<nlayers

  // Sanity check
  if(cur_dom!=ndom){
    cout << "WARNING:: when translating domains, not all domains may have been reached!\n";
    cout << "Domains translated: " << cur_dom << endl;
    }

  // Create pointers to global nodes on each domain
  GNodePointer *GNodes;
  GNodes = new GNodePointer [ndom];
  for(int i=0;i<ndom;i++){
    GNodes[i].setGlobalNodes(nnodes,(*Domain)[i],(*Features));
    //GNodes[i].print(&cout);
  }

  ////////////////////////////////////////////////////////////
  // Set Global Node numbers and owning domain
  ////////////////////////////////////////////////////////////

  // Loop through global features on each domain and number
  // consecutively.  If a connecting domain with lower rank already
  // has a global number, then all higher ranks get that
  // number. First connecting domain owns the node.

  // Number nodes 1st by vertex, then by curve, then by surface, then
  // by patch.
  int vdom[7],cdom[3];
  int gnnum = 1;
  int domain = 0;
  int min;
  for(int i=0;i<ndom;i++){

    /*////////////////////////////////////////
                  BEGIN Verticies
    ////////////////////////////////////////*/

    for(int j=0;j<GNodes[i].nvnode();j++){

      int conn_idx[7] = {-1,-1,-1,-1,-1,-1,-1};

      //cout << "Connecting domains for node " << GNodes[i].vertex(j)->eid() 
      //   << " on domain " << i+1 << ":\n";
      //Connecting domains
      for(int k=0;k<7;k++){
	vdom[k] = conn->corner[i][j][k];
	//cout << vdom[k] << "\t";
      }
      //cout << endl;

      if(vdom[0] <= 0 && vdom[1] <= 0 &&
	 vdom[2] <= 0 && vdom[3] <= 0 &&
	 vdom[4] <= 0 && vdom[5] <= 0 &&
	 vdom[6] <= 0){//no connections
	GNodes[i].vdom(j,i+1);
	continue;
      }

      if(GNodes[i].vgid(j) <= 0){
	if(vdom[0] > 0){
	  domain = vdom[0];
	} else {domain = i+1;}

	// Determine lowest rank domain in the connection
	min = minimum(7,vdom);

	//Find and store matching node indicies
	for(int k=0;k<7;k++){ //connecting domains
	  if(vdom[k] <= 0) continue;
	  for(int l=0;l<GNodes[vdom[k]-1].nvnode();l++){
	    if( compare_coord(GNodes[i].vertex(j),GNodes[vdom[k]-1].vertex(l)) ){
	      conn_idx[k]=l;
	      break;
	    }
	    if(l == GNodes[vdom[k]-1].nvnode()-1)
	      cout << "ERROR in " << __func__ << ": no matching node found for node " 
		   << GNodes[i].vertex(j)->LID() << " in domain "
		   << i << "!!\n";
	  }
	} //Connecting domains

	if(min >= 0 && vdom[min] > 0 && vdom[min]-1 < i ){
	  GNodes[i].vgid(j,GNodes[vdom[min]-1].vgid(conn_idx[min]));
	} else {
	  GNodes[i].vgid(j,gnnum);	  
	  for(int k=0;k<7;k++){ //connecting domains
	    if(vdom[k] <= 0) continue;
	    GNodes[vdom[k]-1].vgid(conn_idx[k],gnnum);
	  }
	  gnnum++;

	} // connecting domain less than current.

	//cout << "Global id of this node is: " << GNodes[i].vgid(j) << endl;

	// Set domain
	GNodes[i].vdom(j,domain);
	for(int k=0;k<7;k++){ //connecting domains
	  if(vdom[k] <= 0) continue;
	  GNodes[vdom[k]-1].vdom(conn_idx[k],domain);
	}

      } // if GID <= 0
    } // j<nvnode

    /*////////////////////////////////////////
           END Verticies :: BEGIN Curves
    ////////////////////////////////////////*/

    for(int j=0;j<GNodes[i].ncnode();j++){
      if(GNodes[i].cgid(j) <= 0){

	string curve_warning = "WARNING::Found matching node on unexpected feature!\n";
	int conn_idx[3]={-1,-1,-1};
	int curve_id = GNodes[i].curve(j)->eid()-1;

	cdom[0] = conn->edgecurve[i][curve_id][0];
	cdom[1] = conn->edgecurve[i][curve_id][1];
	cdom[2] = conn->edgecurve[i][curve_id][2];

	if(cdom[0] <= 0 && cdom[1] <= 0 &&
	   cdom[2] <= 0){//No connections
	  GNodes[i].cdom(j,i+1);
	  continue;
	}

	if(cdom[0] > 0){
	  domain = cdom[0];
	} else {domain = i+1;}

	min = minimum(3,cdom);

	//Find and store matching node indicies
	for(int k=0;k<3;k++){ //connecting domains
	  if(cdom[k] <= 0) continue;
	  for(int l=0;l<GNodes[cdom[k]-1].ncnode();l++){
	    if( compare_coord(GNodes[i].curve(j),GNodes[cdom[k]-1].curve(l)) ){
	      conn_idx[k]=l;
	      break;
	    }
	    if(l == GNodes[cdom[k]-1].ncnode()-1)
	      cout << "ERROR in " << __func__ << ": no matching node found for node " 
		   << GNodes[i].curve(j)->LID() << " on curve " 
		   << GNodes[i].curve(j)->eid() << " in domain "
		   << i << "!!\n";
	  }
	} //Connecting domains

	if(min > 0 && cdom[min] > 0 && cdom[min]-1 < i ){
	  GNodes[i].cgid(j,GNodes[cdom[min]-1].cgid(conn_idx[min]));
	} else {
	  GNodes[i].cgid(j,gnnum);	  
	  for(int k=0;k<3;k++){ //connecting domains
	    if(cdom[k] <= 0) continue;
	    GNodes[cdom[k]-1].cgid(conn_idx[k],gnnum);
	  }
	  gnnum++;

	} // connecting domain less than current.

	// Set domain
	GNodes[i].cdom(j,domain);
	for(int k=0;k<3;k++){ //connecting domains
	  if(cdom[k] <= 0) continue;
	  GNodes[cdom[k]-1].cdom(conn_idx[k],domain);
	}

      } // if GID <= 0
    } // j<ncnode

    /*////////////////////////////////////////
           END Curves :: BEGIN Patches
    ////////////////////////////////////////*/

    for(int j=0;j<GNodes[i].npnode();j++){
      if(GNodes[i].pgid(j) <= 0){

	int patch_id = GNodes[i].patch(j)->eid()-1;
	int pdom = conn->patch[i][patch_id];
	int match = -1;

	if(pdom <= 0){ // no connections
	  GNodes[i].pdom(j,i+1);
	  continue;
	} else { domain = pdom; }

	// Search for matching node
	for(int k=0;k<GNodes[pdom-1].npnode();k++){
	  if( compare_coord(GNodes[i].patch(j),GNodes[pdom-1].patch(k)) ){
	    match = k;
	    break;
	  }

	  if(k == GNodes[pdom-1].npnode()-1){
	    cout << "ERROR in " << __func__ << ": no match found for node " 
		 << GNodes[i].patch(j)->LID() << " on patch " 
		 << GNodes[i].patch(j)->eid() << " in domain " << i+1 << "!!\n";
	  } //ERROR
	} // node search

	// Set GID
	if(pdom < i+1){
	  GNodes[i].pgid(j,GNodes[pdom-1].pgid(match));
	} else {
	  GNodes[i].pgid(j,gnnum);
	  GNodes[pdom-1].pgid(match,gnnum);
	  gnnum++;
	}

	// Set domain
	GNodes[i].pdom(j,domain);
	GNodes[pdom-1].pdom(match,domain);


      } // if GID <= 0
    } // j<npnode

    /*////////////////////////////////////////
                   END Patches
    ////////////////////////////////////////*/

  } // i<ndom


#ifdef SC_DEBUG

  // Testing node numbering of verticies

  ofstream temp_out("test.conn.out");
  ostream *outp;

  outp = &temp_out;
  //  outp = &cout;

  for(int i=0;i<ndom;i++){
    GNodes[i].write(outp,"all");
    *outp << endl;
  }

//   //testing
//   *outp << endl << "*** Nodal Information for Domains ***" << endl;
//   for(int i=0;i<ndom;i++){
//     *outp << "Domain " << i+1 << ":\n";
//     for(int j=0;j<nnodes;j++){
//       (*Domain)[i][j].print(outp);
//     }
//     *outp << endl;
//   }
//   *outp << endl;

  temp_out.close();

#endif // #ifdef SC_DEBUG


} // createDomain
