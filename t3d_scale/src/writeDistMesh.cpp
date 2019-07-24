#include <fstream>
#include <cstdio>

#include "writeDistMesh.h"

using namespace std;

void writeDistMesh(Options *options, T3d_Header *MeshInfo,
		   T3d_Node **Domain, std::vector<std::string> footer)
{
  for(int i=0;i<options->np();i++){
    char filename[250];
    sprintf(filename,"%s.out.%d",options->filebase().c_str(),i);
    ofstream file(filename);
    MeshInfo->print(&file,i+1);

    for(int j=0;j<MeshInfo->nnodes();j++){
      Domain[i][j].write(&file);
    } // for each node

    for(int j=0;j<footer.size();j++){
      file << footer[j] << endl;
    }

    file.close();
  } // for each domain
}


void writeDistMesh(Options *options, T3d_Header *MeshInfo,
		   T3d_Node **Domain, FeatureList *Features, T3dFooter *footer)
{
  for(int i=0;i<options->np();i++){
    char filename[250];
    sprintf(filename,"%s.out.%d",options->filebase().c_str(),i);
    ofstream file(filename);
    MeshInfo->print(&file,i+1);

    for(int j=0;j<MeshInfo->nnodes();j++){
      Domain[i][j].write(&file);
    } // for each node

    file << endl << endl;

    footer->write(&file,Features,i);

    file.close();
  } // for each domain
}
