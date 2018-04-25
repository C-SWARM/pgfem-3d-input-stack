/* -*- mode: c++; -*- */
/* HEADER */
/**
 * AUTHORS:
 * Matt Mosby
 * Ivan Viti
 * Aaron Howell
 */

#include "write_pgfem_input.h"
#include "utils.h"
#include "material.h"
#include "boundary_conditions.h"
#include "basis.h"
#include "omp_param.h"
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <sys/stat.h>  //for creating directories


static void write_dot_in_file(const char *filename,
			      const Header &header,
			      const Domain dom)
{
  std::ofstream out(filename);
  out << dom.nodes.size() << " " << header.n_dim() << " " << dom.elements.size()      //nn,ndofn,ne 
      << std::endl << std::endl
      << header.max_lin_iter() << " " << header.lin_tol() << " " << header.epsilon()  //lin_maxit,lin_err,lim_zero
      << std::endl << std::endl
      << header.n_materials() << " " << header.n_concentrations() << " "              //nmat,n_concentrations
      << header.n_orientations() << std::endl << std::endl;                           //n_orient

  // element sizes .geo
  for(ElementList::const_iterator it = dom.elements.begin();                          //pom[].toe
      it != dom.elements.end(); ++it){
    out << it->nnodes() << " ";
  }
  out << std::endl << std::endl;

  // nodes .geo
  out << dom.nodes << std::endl;                                                      //gnn, Dom, id

  // prescribed displacements
  out << dom.pre_disp.size() << std::endl                                             //sup->nsn (notice it is dom.predisp, not header.predisp)
      << dom.pre_disp << std::endl;                                                   //sup->supp and node[].id[]

  // (BC) displacements 
  // only outputs the displacements for the 1st physics
  out << header.displacements().size() << std::endl                                   //sup->npd
      << header.displacements() << std::endl << std::endl;                            //sup->defl_d[]

  // elements .geo 
  out << dom.elements << std::endl;                                                   //nod,ftype,fid,pr,bnd_id,bnd_type

  // material assignment
  for(ElementList::const_iterator it = dom.elements.begin();
      it != dom.elements.end(); ++it){
    it->write_material(out);                                                          //material
  }
  out << std::endl << std::endl;

  // material properties
  out << *(header.materials()) << std::endl;                                          //material

  // concentrations
  out << header.concentrations() << std::endl;                                        //n_concentrations

  // orientations
  out << *(header.orientations()) << std::endl;                                       //n_orient

  // ???
  out << "0 0.5 0.5" << std::endl << std::endl;                                       //matgeom

  // nodal forces
  out << dom.pre_force.size() << std::endl                                            //nln, nel_s, nel_v
      << dom.pre_force << std::endl;

  // volume and surface loads (not supported)
  out << "0\n\n0\n";

  out.close();
}




static void write_dot_mat_file(const char *filename,
                              const Header &header,
                              const Domain dom)
{
  std::ofstream out(filename);
  out << header.n_dim()                                                                     //ndofn 
      << std::endl << std::endl
      << header.max_lin_iter() << " " << header.lin_tol() << " " << header.epsilon()        //lin_maxit,lin_err,lim_zero
      << std::endl << std::endl
      << header.n_materials() << " " << header.n_concentrations() << " "                    //nmat,n_concentrations
      << header.n_orientations() << std::endl << std::endl;                                 //n_orient
/*
  // material assignment
  for(ElementList::const_iterator it = dom.elements.begin();
      it != dom.elements.end(); ++it){
    it->write_material(out);
  }
  out << std::endl << std::endl;
*/
  // material properties
  out << *(header.materials()) << std::endl;

  // concentrations
  out << header.concentrations() << std::endl;

  // orientations
  out << *(header.orientations()) << std::endl;

  // ???
  out << "0 0.5 0.5" << std::endl << std::endl;

  // nodal forces
  out << dom.pre_force.size() << std::endl
      << dom.pre_force << std::endl;

  // volume and surface loads (not supported)
  out << "0\n\n0\n";


  out.close();
}

/*
  This function writes the IC file for the given physics.
  The IC data is first processed by replacing the negative values in g_id
  with their corresponding values in ic_replacements.
*/
static void write_dot_ic_file(const char *filename,
                              Domain dom,
                              const Input_Data inputs,
                              int physics)
{
  std::ofstream out(filename);

  switch (inputs.physics_list[physics].equation_id) {
  case 0:  //momentum equataion data
    out << inputs.physics_list[physics].time_integration_rule << std::endl;
    for (int mat = 0; mat < inputs.number_of_materials; ++mat) {
      out << inputs.physics_list[physics].inertial_density[mat] << "\n";
    }
    break;
    
  case 1:  //energy equation data
    out << inputs.physics_list[physics].reference_value << "\n";
    break;
  }


  if (dom.initial_conditions[physics].size() == 0){
    out.close();
    return;  //return if there's no g_id IC data from IC files
  }


  out << dom.initial_conditions[physics].size() << std::endl;

  //replaces values in g_id with their corresponding values in ic_replacements and writes ICs
  dom.initial_conditions[physics].write_replacements(inputs, physics, out);

  out.close();
}

static void write_dot_bc_file(const char *filename,
                              const Domain dom,
                              int physics)
{
  std::ofstream out(filename);

  //outputs the domain's processed BCs
  out << dom.boundary_conditions[physics].size() << std::endl                                             //sup->nsn
      << dom.boundary_conditions[physics]; 

  out.close();
}

static void write_dot_bcv_file(const char *filename,
                              const Input_Data inputs,
                              int physics)
{
   std::ofstream out(filename);
   out << inputs.physics_list[physics].bc_replacements.size() << std::endl;
   
   for (int i = 0; i < inputs.physics_list[physics].bc_replacements.size(); ++i){
     out << inputs.physics_list[physics].bc_replacements[i] << std::endl;
   }
  
   out.close();
}




static void write_dot_geo_file(const char *filename,
                              const Header &header,
                              const Domain dom)
{
  std::ofstream out(filename);

  out << dom.nodes.size() << std::endl;                                 //nn

  out << dom.elements.size() << std::endl;                              //ne

  out << header.n_dim() <<std::endl;                                      //ndofm (also needed here)


  for(ElementList::const_iterator it = dom.elements.begin();
      it != dom.elements.end(); ++it){
    out << it->nnodes() << " ";                                         //pom[].toe
  }
  out << std::endl << std::endl;

  out << dom.nodes << std::endl;                                        //gnn,Dom,id

   out << std::endl << std::endl;

  out << dom.elements << std::endl;                                     //nod,ftype,fid,pr,bnd_id,bnd_type

  // material assignment
     for(ElementList::const_iterator it = dom.elements.begin();
     it != dom.elements.end(); ++it){
       it->write_material(out);                                                          //material
     }
                 


}

static void write_mech_dot_mat(const char *filename,                     //typically Mechanical.mat
                               const Header &header,
                               const Domain dom)
{
  std::ofstream out(filename);
  out << *(header.materials()) << std::endl;
  out.close();

}

//Cii & heat_capacity
static void write_heat_dot_mat(const char *filename,                       //typically Thermal.mat
                               const Header &header,
                               const Domain dom)
{
  std::ofstream out(filename);
  out << *(header.heat()) << std::endl;
  out.close();

}



static void write_cohesive_file(const char *filename,
				const Header &header,
				const Domain dom)
{
  std::ofstream out(filename);
  // cohesive materials (deprecated, now in co_props)
  out << 0 << std::endl << std::endl;

  // number of elements and sizes
  out << dom.coh_elements.size() << std::endl;
  for(ElementList::const_iterator it = dom.coh_elements.begin();
      it != dom.coh_elements.end(); ++it){
    out << it->nnodes() << " ";
  }
  out << std::endl << std::endl;

  // connectivity and material assignment
  out << dom.coh_elements << std::endl;
  out.close();
}


void write_PGFem3D_input_files(const Options &opt,
			       const Header &header,
			       const Domains &domains,
			       const Input_Data inputs)
 {

  Domains::const_iterator it0 = domains.begin();
  int i;
  #pragma omp parallel num_threads(opt.nthreads()) shared(opt,header,domains,it0)
  {
    Domains::const_iterator it;
    size_t dom_id;

    #pragma omp for schedule(static,CHUNKSIZE) 
    for(dom_id = 0; dom_id < domains.size(); dom_id++){

      
      it = it0 + dom_id; 

      std::stringstream filename;
       if(opt.human_readable()){
       	  std::stringstream filename1;                      //.ic
          std::stringstream filename2;                      //.bc
          std::stringstream filename3;                      //.bcv

          filename << opt.base_fname() << "_" << dom_id << ".in";
          write_dot_in_file(filename.str().c_str(),header,*it);


	  /*******************write ICs**********************/
	 
          //loop over number of physics (mech and thermal)
	  for(int physics = 0; physics < inputs.number_of_physics; ++physics) {  
	    std::stringstream filename1;   //.ic
	    
	    filename1 << opt.base_dname() << "/IC/"<< inputs.physics_list[physics].physics_name << "_" << dom_id << ".initial";
	    
	    if (physics == 0){     //create directory at filebase/IC during the first iteration
	      std::string fileDir = opt.base_dname();
	      fileDir += "/IC";
	      mode_t mode = 0755;
	      mkdir(fileDir.c_str(), mode);
	    }
	    
	    //write IC data
	    write_dot_ic_file(filename1.str().c_str(),*it,inputs,physics);
	  }
       

	  /*******************write BCs**********************/
	  if (!inputs.bc_flag){   //don't write any bc files if no bc.json files where provided
	    continue;
	  }
          //loop over number of physics (mech and thermal)
          for(int physics = 0; physics < inputs.number_of_physics; ++physics) {  
	    std::stringstream filename2;     //.bc
	    std::stringstream filename3;     //.bcv

	    filename2 << opt.base_dname() << "/BC/"<< inputs.physics_list[physics].physics_name << "_" << dom_id << ".bc";
	    if (physics == 0){     //create directory at filebase/BC during the first iteration
	    std::string fileDir = opt.base_dname();
	    fileDir += "/BC";
	      mode_t mode = 0755;
	      mkdir(fileDir.c_str(), mode);
	    }

	    //write BC data
            write_dot_bc_file(filename2.str().c_str(),*it,physics);
  
	    if (dom_id == 0){  //bcv file is written only once
	      filename3 << opt.base_dname() <<"/BC/"<< inputs.physics_list[physics].physics_name << ".bcv";
	      write_dot_bcv_file(filename3.str().c_str(),inputs,physics);
	    }
	  } //ends physics loop
       }
       else {
     		  filename << opt.base_fname() << "_" << dom_id << ".in";
       		write_dot_in_file(filename.str().c_str(),header,*it);
       }
        if(opt.cohesive()){
         filename << ".co";
         write_cohesive_file(filename.str().c_str(),header,*it);
       }
    } //ends domain.size() loop

    if (inputs.bc_flag)
      std::cout << "wrote to " << domains.size() << " BC files" << std::endl;

    if (inputs.ic_flag)
      std::cout << "wrote to " << domains.size() << " IC files" << std::endl;
    else
      std::cout << "Warning: No initial.json file provided. Using default values" << std::endl;
   } //ends pragma
 }
