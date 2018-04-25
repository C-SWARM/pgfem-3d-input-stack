/* -*- mode: c++; -*- */
/* HEADER */
/**
 * Some helper functions to compute communication hints for a
 * previously processed data-stack.
 *
 * AUTHORS:
 * Matt Mosby
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "comm_hints.h"
#include "domain.h"
#include "utils.h"
#include "omp_param.h"

namespace ch_util {
  void read_nodes_dom(std::istream &in,
                      Domain &dom)
  {
    int nnodes = 0;
    int nelem = 0;
    int ijunk = 0;
    double djunk = 0;
    std::vector<int> vjunk;

    in >> nnodes >> ijunk >> nelem
       >> ijunk >> djunk >> djunk
       >> ijunk >> ijunk >> ijunk;
    vjunk.resize(nelem);
    in >> vjunk;
    dom.nodes.resize(nnodes);
    dom.nodes.set_parallel(true);
    dom.nodes.read_PGFem3D(in);
  }

  void read_input_files(const Options &opt,
                        Domains &domains)
  {
    if(opt.nproc() <= 1) domains.begin()->set_serial();
#pragma omp parallel num_threads(opt.nthreads()) shared(opt,domains)
    {
      Domains::iterator it;
      size_t dom_id, tmp;
#pragma omp for schedule(static,CHUNKSIZE)
      for(dom_id = 0; dom_id < domains.size(); dom_id++){
        it = domains.begin()+dom_id;
        std::stringstream mesh;
        mesh << opt.base_fname() << "_" << dom_id << ".in";
        std::ifstream in(mesh.str().c_str());
        file_check_error(in,mesh.str().c_str());
        read_nodes_dom(in, *it);
        in.close();
      }
    }  //end pragma
    domains.sort_nodes_own(opt);
  }
}
