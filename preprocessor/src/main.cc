/* HEADER */
/**
 * AUTHORS:
 * Matt Mosby
 * Ivan Viti
 * Aaron Howell
 */

#include "material.h"

#include "options.h"
#include "domain.h"
#include "header.h"
#include "periodic.h"
#include "process_input_files.h"
#include "apply_periodic_bc.h"
#include "apply_header.h"
#include "write_pgfem_input.h"
#include "renumber_global_nodes.h"
#include "comm_hints.h"
#include "comm_hints_util.h"
#include "omp_param.h"
#include <cstdlib>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <time.h>

#include "read_input.h"
#include "write_input.h"
#include "input_parser.h"
using namespace std;

static double start;
static void tick() { start = omp_get_wtime();}
static void tock(const char *msg){
 std::cout << msg << omp_get_wtime() - start << std::endl;
 start = 0;
}

const int MAX_CHAR = 255;

int main(int argc, char **argv)
{
  printf("======================================= we are now in con3d++ ==================================\n");

  double prog_start = omp_get_wtime();
  Options opt(argc,argv);
  Domains domains(opt.nproc());
  if (opt.hints_only()) {
    tick();
    ch_util::read_input_files(opt,domains);
    tock("process_mesh_files (seconds)          = ");
 
    tick();
    CommHintsList hints(domains, opt);
    hints.print(opt);
    tock("Compute comminication hints (seconds) = ");
  } else {
    Header header;
    Periodic periodic;

    //read files:
    Input_Data inputs;

    // process header file
    if(opt.human_readable()){

      read_input(inputs);   //read inputs

      process_inputs(header, inputs);         //move read data into con3d header data type

      printf("finished reading JSON files\n");

      write_periodic(inputs, opt.base_fname());

    } else {
      process_header_file(opt,header);
      printf("finished reading classic file\n");
    }


    // process mesh files -- sorts nodes by model type
    tick();
    process_mesh_files(opt,domains);
    tock("process_mesh_files (seconds)          = ");

    // Periodic operations
    if(opt.periodic()){
      process_periodic_file(opt,periodic);

      tick();
      apply_periodic_bc(opt,domains,periodic);
      tock("apply_periodic_bc (seconds)           = ");

      tick();
      renumber_global_nodes(opt,domains);
      tock("renumber_global_nodes (seconds)       = ");

      // re-sort nodes by model for other BC assignment
      domains.sort_nodes_model(opt);
    }

    // apply header file
    if(opt.human_readable()){
      tick();
      calculate_bc(header, domains, inputs.number_of_physics);
      tock("calculate_bc (seconds)   = ");
      
      tick();
      calculate_ic(header, domains, inputs.number_of_physics);
      tock("calculate_ic (seconds)   = ");
    }
    else{
      tick();
      apply_header_derichlet_bc(header,opt,domains);
      tock("apply_header_derichlet_bc (seconds)   = ");
    }

    tick();
    apply_thermal_derichlet_bc(header,opt,domains);
    tock("apply thermal derichlet bc (seconds) = ");


    tick();
    apply_header_neumann_bc(header,opt,domains);
    tock("apply_header_neumann_bc (seconds)     = ");

    tick();
    apply_header_mat_assignment(header,opt,domains);
    tock("apply_header_mat_assignment (seconds) = ");

    // filter cohesive elements
    if(opt.cohesive()){
      tick();
#pragma omp parallel for num_threads(opt.nthreads()) schedule(static,CHUNKSIZE) shared(domains)
      for(size_t dom_id = 0; dom_id < domains.size(); dom_id++){
        Domains::iterator it=domains.begin()+dom_id;
        it->filter_cohesive_elements();
      }
      tock("filter cohesive elements (seconds)    = ");
    }


    if (opt.comm_hints()) {
      // sort nodes by domain ownership
      domains.sort_nodes_own(opt);

      // compute communication hints
      tick();
      CommHintsList hints(domains, opt);
      hints.print(opt);
      tock("Compute comminication hints (seconds) = ");
    }

    // sort everything in id-order
    domains.sort_id(opt);

    // write PGFem3D input files
    tick();
    write_PGFem3D_input_files(opt, header, domains, inputs);

    if(opt.human_readable()){
      write_multiphysics_in(inputs, opt.base_dname());
      write_material_properties(inputs, opt.base_dname());
      
      std::stringstream filename;
      filename << opt.base_fname() << "_0";
      write_in_st(inputs, filename.str().c_str());
      write_co_props(inputs, filename.str().c_str());
      
      write_normal_in(inputs, opt.base_dname());
      
      write_loads(inputs, opt.base_dname());
    }
    tock("write_PGFem3D_input_files (seconds)   = ");
  }

  start = prog_start;
  tock("Total time (seconds)                  = ");

  

  return 0;
}
