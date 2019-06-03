#include <cmath>
#include <cstdlib>

#include "utils.h"
#include "connectivity.h"

using namespace std;

//////////////////////////////
//    PUBLIC FUNCTIONS
//////////////////////////////

Connectivity::Connectivity(int ndom)
{
  numdom = ndom;
  this->Build(ndom);
}

void Connectivity::Build(int ndom)
{
  numdom = ndom;

  // Allocate corner connectivity
  corner = new int** [ndom];
  for(int i=0;i<ndom;i++){
    corner[i] = new int* [8];
    for(int j=0;j<8;j++)
      corner[i][j] = new int[7];
  }

  // Allocate edgecurve connectivity
  edgecurve = new int** [ndom];
  for(int i=0;i<ndom;i++){
    edgecurve[i] = new int* [12];
    for(int j=0;j<12;j++)
      edgecurve[i][j] = new int[3];
  }

  // Allocate patch connectivity
  patch = new int* [ndom];
  for(int i=0;i<ndom;i++)
    patch[i] = new int[6];

  // determine the domain type
  int ival = floor(log(ndom)/log(2));// ndom should already be a power
				     // of two as this was checked
				     // earlier with an exit result if
				     // it isn't
  int type;
  if(ival%3 == 0) type = 1; // cube
  else if( ((int)floor(log(ndom/2)/log(2)))%3 == 0 || ival == 1) type = 2; // Row
  else type = 3; // layer of four cubes

  switch(type){

  case 1: // cube
    cout << "Domain type: Cube\n\n";
    ival = floor(pow(ndom,(1./3.)));  //this is now the primary dimension
    if(ival == 0){//ndom = 1 => no connectivity

      nlayer = 1;
      nlaydom = 1;
      nrow = ncol = 1;
      // Corner connectivity
      for(int i=0;i<8;i++){
	for(int j=0;j<7;j++)
	  corner[0][i][j] = 0;
      }// i<8

    // Edgecurve connectivity
      for(int i=0;i<12;i++){
	for(int j=0;j<3;j++)
	  edgecurve[0][i][j] = 0;
      }// i<12

      // Patch connectivity
      for(int i=0;i<6;i++)
	patch[0][i] = 0;

      break; 
    }//ndom = 1

    if(pow((double)ival,3) != ndom) ival++;
    nlayer = ival;
    nlaydom = ival*ival;
    nrow = ncol = ival;
    for(int i=0;i<ival;i++)
      this->layer(ival,ival,i,ival);

    break;

  case 2: // two cubes
    cout << "Domain type: Row\n\n";
    ival = floor(pow(ndom/2,1./3.)); // This is now the width, height and 1/2 length
    if(ival == 0) ival = 1; //ndom = 2

    if(pow((double)ival,3) != ndom/2) ival++;
    nlayer = ival;
    nlaydom = ival*2*ival;
    nrow = ival;
    ncol = 2*ival;
    for(int i=0;i<ival;i++)
      this->layer(ival,2*ival,i,ival);

    break;

  case 3: // four cubes
    cout << "Domain type: Layer\n\n";
    ival = floor(pow(ndom/4,1./3.)); // This is now the height and 1/2 sidelength
    if(ival == 0) ival = 1; //ndom = 4;

    if(pow((double)ival,3) != ndom/4) ival++;
    nlayer = ival;
    nlaydom = 2*ival*2*ival;
    nrow = ncol = 2*ival;
    for(int i=0;i<ival;i++)
      this->layer(2*ival,2*ival,i,ival);

    break;

  default:
    cout << "Error in " << __func__ << ". Unknown type put through switch.  Exiting\n";
    exit(-10);
    break;
  } // switch(type)

  this->sanity();

} // Build

void Connectivity::Print(ostream *file)
{
  check_stream(file);

  // For each domain print the connectivity for each feature on a line
  for(int i=0;i<numdom;i++){
    *file << "Domain " << i+1 <<" connectivity:\n";

    *file << "Corners:\n";
    for(int j=0;j<8;j++){
      *file << j+1 << " :: ";
      for(int k=0;k<7;k++)
	*file << corner[i][j][k] << " ";
      *file << endl;
    } // corner connectivity

    *file << "Edgecurves:\n";
    for(int j=0;j<12;j++){
      *file << j+1 << " :: ";
      for(int k=0;k<3;k++)
	*file << edgecurve[i][j][k] << " ";
      *file << endl;
    } // edgecurve connectivity

    *file << "Patches:\n";
    for(int j=0;j<6;j++)
      *file << j+1 << " :: " << patch[i][j] << endl;
    
    *file << endl;
  } // Domain
} //Print

Connectivity::~Connectivity()
{
  for(int i=0;i<numdom;i++){
    //Delete corner connectivity
    for(int j=0;j<8;j++)
      delete corner[i][j];
    delete corner[i];

    //Delete edgecurve connectivity
    for(int j=0;j<12;j++)
      delete edgecurve[i][j];
    delete edgecurve[i];

    //Delete patch connectivity
    delete patch[i];
  }

  delete patch;
  delete corner;
  delete edgecurve;

}//Destructor

//////////////////////////////
//    PRIVATE FUNCTIONS
//////////////////////////////

void Connectivity::layer(int x, int y, int clayer, int tlayer)
{
  // clayer = current layer
  // tlayer = total layers
  int dom_adder = clayer*x*y;
  int ndom = x*y;

  int **Layer;
  int dom = 1;
  Layer = new int* [x]; // Rows
  for(int i=0;i<x;i++){
    Layer[i] = new int[y]; // Columns
    for(int j=0;j<y;j++){
      Layer[i][j] = dom+dom_adder;
      dom++;
    }
  }

  // Loop through each domain in layer and resolve
  // connections. i.e. all faces and diagonals and if requires a
  // negative index, then set value to 0.  There are 26 connections
  // going ccx from -layer 1st corner. Connections are ordered in the
  // same fashion (i.e. number ccw from bottom left connection)

  int connection_idx[26][3];// For each connection, x_idx, y_idx, +- layer
  int connection_val[26];
  int current_dom;

  for(int l=0;l<x;l++){//row
    for(int m=0;m<y;m++){//coluumn
      current_dom = Layer[l][m];

      //connections
      int idx = 0;
      for(int i=-1;i<2;i++){ //layer
	for(int j=-1;j<2;j++){ //row
	  for(int k=-1;k<2;k++){ //column
	    if(i == 0 && j == 0 && k == 0) continue;
	    connection_idx[idx][0] = l+j; //domain row +-
	    connection_idx[idx][1] = m+k; //domain column +-
	    connection_idx[idx][2] = i; //domain layer +-
	    idx++;
	  }
	}
      }//connections

      for(int h=0;h<26;h++){
	if(connection_idx[h][0] < 0 ||
	   connection_idx[h][0] >= x ||
	   connection_idx[h][1] < 0 ||
	   connection_idx[h][1] >= y ||
	   clayer + connection_idx[h][2] < 0 || 
	   clayer + connection_idx[h][2] >= tlayer){
	  connection_val[h] = 0;
	} else {
	  connection_val[h] = Layer[ connection_idx[h][0] ][ connection_idx[h][1] ] +
	    connection_idx[h][2]*ndom;
	}
      }

      // Corner connections

      // Vertex 1 on domain of interest
      corner[current_dom-1][0][0] = connection_val[0];  // Vertex 7 on connecting domain
      corner[current_dom-1][0][1] = connection_val[1];  // 8
      corner[current_dom-1][0][2] = connection_val[4];  // 5
      corner[current_dom-1][0][3] = connection_val[3];  // 6
      corner[current_dom-1][0][4] = connection_val[9];  // 3
      corner[current_dom-1][0][5] = connection_val[10]; // 4
      corner[current_dom-1][0][6] = connection_val[12]; // 2

      // Vertex 2 on domain of interest
      corner[current_dom-1][1][0] = connection_val[1];  // 7
      corner[current_dom-1][1][1] = connection_val[2];  // 8
      corner[current_dom-1][1][2] = connection_val[5];  // 5
      corner[current_dom-1][1][3] = connection_val[4];  // 6
      corner[current_dom-1][1][4] = connection_val[10]; // 3 
      corner[current_dom-1][1][5] = connection_val[11]; // 4
      corner[current_dom-1][1][6] = connection_val[13]; // 1

      // Vertex 3 on domain of interest
      corner[current_dom-1][2][0] = connection_val[4];  // 8
      corner[current_dom-1][2][1] = connection_val[5];  // 7
      corner[current_dom-1][2][2] = connection_val[8];  // 5
      corner[current_dom-1][2][3] = connection_val[7];  // 6
      corner[current_dom-1][2][4] = connection_val[13]; // 4
      corner[current_dom-1][2][5] = connection_val[16]; // 1 
      corner[current_dom-1][2][6] = connection_val[15]; // 2

      // Vertex 4 on domain of interest
      corner[current_dom-1][3][0] = connection_val[3];  // 7
      corner[current_dom-1][3][1] = connection_val[4];  // 8
      corner[current_dom-1][3][2] = connection_val[7];  // 5
      corner[current_dom-1][3][3] = connection_val[6];  // 6
      corner[current_dom-1][3][4] = connection_val[12]; // 3
      corner[current_dom-1][3][5] = connection_val[15]; // 1
      corner[current_dom-1][3][6] = connection_val[14]; // 2

      // Vertex 5 on domain of interest
      corner[current_dom-1][4][0] = connection_val[9];  // 7
      corner[current_dom-1][4][1] = connection_val[10]; // 8
      corner[current_dom-1][4][2] = connection_val[12]; // 6
      corner[current_dom-1][4][3] = connection_val[17]; // 3
      corner[current_dom-1][4][4] = connection_val[18]; // 4
      corner[current_dom-1][4][5] = connection_val[21]; // 1
      corner[current_dom-1][4][6] = connection_val[20]; // 2

      // Vertex 6 on domain of interest
      corner[current_dom-1][5][0] = connection_val[10]; // 7
      corner[current_dom-1][5][1] = connection_val[11]; // 8
      corner[current_dom-1][5][2] = connection_val[13]; // 5
      corner[current_dom-1][5][3] = connection_val[18]; // 3
      corner[current_dom-1][5][4] = connection_val[19]; // 4
      corner[current_dom-1][5][5] = connection_val[22]; // 1
      corner[current_dom-1][5][6] = connection_val[21]; // 2

      // Vertex 7 on domain of interest
      corner[current_dom-1][6][0] = connection_val[13]; // 8
      corner[current_dom-1][6][1] = connection_val[16]; // 5
      corner[current_dom-1][6][2] = connection_val[15]; // 6
      corner[current_dom-1][6][3] = connection_val[21]; // 3
      corner[current_dom-1][6][4] = connection_val[22]; // 4
      corner[current_dom-1][6][5] = connection_val[25]; // 1
      corner[current_dom-1][6][6] = connection_val[24]; // 2

      // Vertex 8 on domain of interest
      corner[current_dom-1][7][0] = connection_val[12]; // 7
      corner[current_dom-1][7][1] = connection_val[15]; // 5
      corner[current_dom-1][7][2] = connection_val[14]; // 6
      corner[current_dom-1][7][3] = connection_val[20]; // 3
      corner[current_dom-1][7][4] = connection_val[21]; // 4
      corner[current_dom-1][7][5] = connection_val[24]; // 1
      corner[current_dom-1][7][6] = connection_val[23]; // 2

      //Curves: These have 3 connections we start at bottom front
      //(x+,y+,z+) and number connections by right hand rule where the
      //fingers come up.

      // Curve 1 on domain of interest
      edgecurve[current_dom-1][0][0] = connection_val[1];  // 7
      edgecurve[current_dom-1][0][1] = connection_val[4];  // 5
      edgecurve[current_dom-1][0][2] = connection_val[10]; // 3

      // Curve 2 on domain of interest
      edgecurve[current_dom-1][1][0] = connection_val[5];  // 8
      edgecurve[current_dom-1][1][1] = connection_val[4];  // 6
      edgecurve[current_dom-1][1][2] = connection_val[13]; // 4

      // Curve 3 on domain of interest
      edgecurve[current_dom-1][2][0] = connection_val[4];  // 7
      edgecurve[current_dom-1][2][1] = connection_val[7];  // 5
      edgecurve[current_dom-1][2][2] = connection_val[15]; // 1

      // Curve 4 on domain of interest
      edgecurve[current_dom-1][3][0] = connection_val[3];  // 6
      edgecurve[current_dom-1][3][1] = connection_val[4];  // 8
      edgecurve[current_dom-1][3][2] = connection_val[12]; // 2

      // Curve 5 on domain of interest
      edgecurve[current_dom-1][4][0] = connection_val[10]; // 7
      edgecurve[current_dom-1][4][1] = connection_val[21]; // 1
      edgecurve[current_dom-1][4][2] = connection_val[18]; // 3

      // Curve 6 on domain of interest
      edgecurve[current_dom-1][5][0] = connection_val[13]; // 8
      edgecurve[current_dom-1][5][1] = connection_val[21]; // 2
      edgecurve[current_dom-1][5][2] = connection_val[22]; // 4

      // Curve 7 on domain of interest
      edgecurve[current_dom-1][6][0] = connection_val[15]; // 5
      edgecurve[current_dom-1][6][1] = connection_val[24]; // 1
      edgecurve[current_dom-1][6][2] = connection_val[21]; // 3

      // Curve 8 on domain of interest
      edgecurve[current_dom-1][7][0] = connection_val[12]; // 6
      edgecurve[current_dom-1][7][1] = connection_val[21]; // 4
      edgecurve[current_dom-1][7][2] = connection_val[20]; // 2

      // We number the connections on verticle curves starting from
      // the foremost (x+ & y+) domain which also shares a face with
      // the domain of interest.

      // Curve 9 on domain of interest
      edgecurve[current_dom-1][8][0] = connection_val[10]; // 12
      edgecurve[current_dom-1][8][1] = connection_val[12]; // 10
      edgecurve[current_dom-1][8][2] = connection_val[9];  // 11

      // Curve 10 on domain of interest
      edgecurve[current_dom-1][9][0] = connection_val[10]; // 11
      edgecurve[current_dom-1][9][1] = connection_val[11]; // 12
      edgecurve[current_dom-1][9][2] = connection_val[13]; // 9

      // Curve 11 on domain of interest
      edgecurve[current_dom-1][10][0] = connection_val[13]; // 12
      edgecurve[current_dom-1][10][1] = connection_val[16]; // 9
      edgecurve[current_dom-1][10][2] = connection_val[15]; // 10

      // Curve 12 on domain of interest
      edgecurve[current_dom-1][11][0] = connection_val[12]; // 11
      edgecurve[current_dom-1][11][1] = connection_val[15]; // 9
      edgecurve[current_dom-1][11][2] = connection_val[14]; // 10

      //Patches
      patch[current_dom-1][0] = connection_val[10]; //x+ => x-
      patch[current_dom-1][1] = connection_val[13]; //y+ => y-
      patch[current_dom-1][2] = connection_val[15]; //x- => x+
      patch[current_dom-1][3] = connection_val[12]; //y- => y+
      patch[current_dom-1][4] = connection_val[4];  //z- => z+
      patch[current_dom-1][5] = connection_val[21]; //z+ => z-
      
    }//Columns
  }//rows
} // layer

void Connectivity::sanity()
{
  //loop through connectivity tables and write an error message if
  //does not pass sanity check, i.e. there are negative domains and/or
  //there are domains outside the nmber of domains.

  bool cornerp, edgep, patchp;
  cornerp = edgep = patchp = true;

  for(int i=0;i<numdom;i++){
    //corners
    if(cornerp){
      for(int j=0;j<8;j++){
	for(int k=0;k<7;k++){
	  if(corner[i][j][k] < 0 || corner[i][j][k] > numdom){
	    cornerp = false;
	    break;
	  }
	}
	if(!cornerp) break;
      }
    }

    //Edges
    if(edgep){
      for(int j=0;j<8;j++){
	for(int k=0;k<3;k++){
	  if(edgecurve[i][j][k] < 0 || edgecurve[i][j][k] > numdom){
	    edgep = false;
	    break;
	  }
	}
	if(!edgep) break;
      }
    }


    //Patches
    if(patchp){
      for(int j=0;j<6;j++){
	if(patch[i][j] < 0 || patch[i][j] > numdom){
	  patchp = false;
	  break;
	}
      }
    }

    if(!cornerp && !edgep && !patchp) break;
  } // i < numdom

  if(!corner) cout << "Corner connectivity table did not pass the sanity test!\n";
  if(!edgep) cout << "Edge connectivity table did not pass the sanity test!\n";
  if(!patchp) cout << "Patch connectivity table did not pass the sanity test!\n";

}// sanity
