#ifndef WRITE_DIST_MESH_H
#define WRITE_DIST_MESH_H

#include <iostream>
#include <vector>
#include <string>

#include "options.h"
#include "t3d_header.h"
#include "t3d_node.h"
#include "t3d_footer.h"
#include "Features.h"

void writeDistMesh(Options *options, T3d_Header *MeshInfo,
		   T3d_Node **Domain, std::vector<std::string> footer);

void writeDistMesh(Options *options, T3d_Header *MeshInfo,
		   T3d_Node **Domain, FeatureList *Features, T3dFooter *footer);

#endif //#ifndef WRITE_DIST_MESH_H
