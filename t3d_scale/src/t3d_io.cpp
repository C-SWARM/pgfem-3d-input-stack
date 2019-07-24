#include <fstream>
#include <sstream>

#include "utils.h"
#include "t3d_io.h"

using namespace std;

void read_t3d(std::string filename, T3d_Header *MeshInfo,
	      T3d_Node **Nodes, std::vector<std::string> *footer)
{
  ifstream file(filename.c_str());
  check_stream(&file);

  MeshInfo->populate(&file);
  (*Nodes) = new T3d_Node[MeshInfo->nnodes()];

  for(int i=0;i<MeshInfo->nnodes();i++)
    (*Nodes)[i].read(&file);

  string t_string,t_string2;
  while(getline(file,t_string)){
  //  for(int j=0;j<MeshInfo->ntetras();j++){
    getline(file,t_string);
    stringstream ss;
    ss << t_string;
    t_string = "";
    for(int i=0;i<16;i++){
      ss >> t_string2;
      if(t_string2=="0" && i==0) break;
      t_string += "\t";
      t_string += t_string2;
    }
    footer->push_back(t_string);
  }

  file.close();
}

void read_t3d(std::string filename, T3d_Header *MeshInfo,
	      T3d_Node **Nodes, T3dFooter *footer)
{
  ifstream file(filename.c_str());
  check_stream(&file);

  MeshInfo->populate(&file);
  (*Nodes) = new T3d_Node[MeshInfo->nnodes()];

  for(int i=0;i<MeshInfo->nnodes();i++)
    (*Nodes)[i].read(&file);

  footer->init(MeshInfo->ntetras());
  footer->read(&file);

  file.close();
}

void write_t3d(std::string filename, T3d_Header *MeshInfo,
	       T3d_Node *Nodes, std::vector<std::string> *footer)
{
  ofstream file(filename.c_str(),fstream::out);
  check_stream(&file);

  MeshInfo->print(&file);

  for(int i=0;i<MeshInfo->nnodes();i++)
    Nodes[i].print(&file);

  int nlines = footer->size();
  for(int i=0;i<nlines;i++)
    file << (*footer)[i] << endl;

  file.close();
}
