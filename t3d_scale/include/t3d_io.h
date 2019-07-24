#ifndef T3D_IO_H
#define T3D_IO_H

#include <string>
#include <vector>
#include "t3d_header.h"
#include "t3d_node.h"
#include "t3d_footer.h"

/// Reads in the t3d output file.
void read_t3d(std::string filename, T3d_Header *MeshInfo,
	      T3d_Node **Nodes, std::vector<std::string> *footer);

void read_t3d(std::string filename, T3d_Header *MeshInfo,
	      T3d_Node **Nodes, T3dFooter *footer);


/// Writes the t3d output file as read (used mostly for debugging).
void write_t3d(std::string filename, T3d_Header *MeshInfo,
	       T3d_Node *Nodes, std::vector<std::string> *footer);

#endif // #ifndef T3D_IO_H
