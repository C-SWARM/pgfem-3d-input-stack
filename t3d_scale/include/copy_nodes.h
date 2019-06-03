#ifndef COPY_NODES_H
#define COPY_NODES_H

#include "t3d_node.h"
#include "Features.h"
#include "t3d_header.h"
#include "options.h"
#include "connectivity.h"

void createDomain(Options *options,T3d_Header *Meshinfo, FeatureList *Features,
		  Connectivity *connectivity, T3d_Node *Nodes, T3d_Node ***Domain);

#endif // #ifndef COPY_NODES_H
