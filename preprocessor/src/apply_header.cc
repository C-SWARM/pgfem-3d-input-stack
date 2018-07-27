/* -*- mode: c++; -*- */
/* HEADER */
/**
 * AUTHORS:
 * Matt Mosby
 * Ivan Viti
 * Aaron Howell
 */

#include "apply_header.h"
#include "boundary_conditions.h"
#include "initial_conditions.h"
#include "material.h"
#include "omp_param.h"

//modified version of apply_header_derichlet_bc to support multiple physics for hr files
void calculate_bc(const Header &header, Domains &domains, int physics_num = 1)
{
  for (int physics = 0; physics < physics_num; ++physics){
    const ModelBCList<int> *bc = header.disp_assignment(physics); //returns boundary_condtions[physics_num]
    ModelBCList<int>::const_iterator bc_it;
    Domains::iterator dom;
    NodeList::iterator nodes_it;
    NodeList::Range node_range;
    NodeBC<int> tmp;

    // for each domain
    for(size_t dom_id = 0; dom_id < domains.size(); dom_id++){                          //parallel loop over domains
    NodeBCList<int> node_entry;
      dom = domains.begin()+dom_id;
      // for each boundary condition, find range of matching nodes
      for(bc_it = bc->begin(); bc_it != bc->end(); ++bc_it){                            //loop over bc's
	      node_range = dom->nodes.find_model_range(bc_it->model());                       //find which nodes belong to that range
	      // for each node in range, set dofs according to boundary condition 
	      for(nodes_it = node_range.first; nodes_it != node_range.second; ++nodes_it){    //apply bc to those nodes
	        tmp.set_node_id(nodes_it->id());
	        tmp.set_bc(bc_it->bc());
	        node_entry.push_back(tmp);
	      }
      }
      node_entry.sort_node_id();      // sort bc by node id

      dom->boundary_conditions.push_back(node_entry);
    }  //end domain loop
  }  //end physics loop
}

void calculate_ic(const Header &header, Domains &domains, int physics_num)
{
  for (int physics = 0; physics < physics_num; ++physics){
    const ModelICList<int> *ic = header.get_initial_conditions(physics); //returns initial_condtions[physics_num]
    ModelICList<int>::const_iterator ic_it;
    Domains::iterator dom;
    NodeList::iterator nodes_it;
    NodeList::Range node_range;
    NodeIC<int> tmp;

    // for each domain
    for(size_t dom_id = 0; dom_id < domains.size(); dom_id++){                          //parallel loop over domains
    NodeICList<int> node_entry;
      dom = domains.begin()+dom_id;
      // for each boundary condition, find range of matching nodes
      for(ic_it = ic->begin(); ic_it != ic->end(); ++ic_it){                            //loop over ic's
	      node_range = dom->nodes.find_model_range(ic_it->model());                       //find which nodes belong to that range
	      // for each node in range, set dofs according to boundary condition 
	      for(nodes_it = node_range.first; nodes_it != node_range.second; ++nodes_it){    //apply ic to those nodes
	        tmp.set_node_id(nodes_it->id());
	        tmp.set_ic(ic_it->ic());
	        node_entry.push_back(tmp);
	      }
      }
      node_entry.sort_node_id();      // sort ic by node id

      dom->initial_conditions.push_back(node_entry);
    }  //end domain loop
  }  //end physics loop
}

//called when non-human readable files are used
void apply_header_derichlet_bc(const Header &header,
             const Options &opt,
			       Domains &domains)
{
  #pragma omp parallel num_threads(opt.nthreads()) shared(header,domains)
  {
    const ModelBCList<int> *bc = header.disp_assignment();
    ModelBCList<int>::const_iterator bc_it;
    Domains::iterator dom;
    NodeList::iterator nodes_it;
    NodeList::Range node_range;
    NodeBC<int> tmp;
    // for each domain
    #pragma omp for schedule(static,CHUNKSIZE)
    for(size_t dom_id = 0; dom_id < domains.size(); dom_id++){
      dom = domains.begin()+dom_id;
      // for each boundary condition, find range of matching nodes
      for(bc_it = bc->begin(); bc_it != bc->end(); ++bc_it){
        node_range = dom->nodes.find_model_range(bc_it->model());
        // for each node in range, set dofs according to boundary condition 
        for(nodes_it = node_range.first; nodes_it != node_range.second; ++nodes_it){
	        tmp.set_node_id(nodes_it->id());
	        tmp.set_bc(bc_it->bc());
	        dom->pre_disp.push_back(tmp);
        }
      }
      // sort bc by node id
      dom->pre_disp.sort_node_id();
    }
  }
}

void apply_thermal_derichlet_bc(const Header &header,
                               const Options &opt,
                               Domains &domains)
{
  #pragma omp parallel num_threads(opt.nthreads()) shared(header,domains)
  {
    const ModelThermList<int> *th = header.therm_list();
    ModelThermList<int>::const_iterator th_it;
    Domains::iterator dom;
    NodeList::iterator nodes_it;
    NodeList::Range node_range;
    NodeBC<int> tmp;
    #pragma omp for schedule(static,CHUNKSIZE)
    for(size_t dom_id = 0; dom_id < domains.size(); dom_id++) {
      dom = domains.begin()+dom_id;
      for(th_it = th->begin(); th_it != th->end(); ++th_it) {
        node_range = dom->nodes.find_model_range(th_it->model());
        for(nodes_it = node_range.first; nodes_it != node_range.second; ++nodes_it){
          tmp.set_node_id(nodes_it->id());
          tmp.set_bc(th_it->th());
          dom->pre_therm.push_back(tmp);
        }
      } 
    
      dom->pre_therm.sort_node_id(); 
    }
  }
}



void apply_header_neumann_bc(const Header &header,
           const Options &opt,
			     Domains &domains)
{
  #pragma omp parallel num_threads(opt.nthreads()) shared(header,domains)
  {
    const ModelBCList<double> *bc = header.force_assignment();
    ModelBCList<double>::const_iterator bc_it;
    Domains::iterator dom;
    NodeList::iterator nodes_it;
    NodeList::Range node_range;
    NodeBC<double> tmp;
    //size_t dom_id = 0;
    // for each domain
    #pragma omp for schedule(static,CHUNKSIZE)
    for(size_t dom_id = 0; dom_id < domains.size(); dom_id++){                                //parallel loop over domains
      dom = domains.begin()+dom_id;
      // for each boundary condition, find range of matching nodes
      for(bc_it = bc->begin(); bc_it != bc->end(); ++bc_it){                                  //loop over bcs
        node_range = dom->nodes.find_model_range(bc_it->model());                             //find which nodes belong 
        //for each node in range, add to list of pre_force bc
        for(nodes_it = node_range.first; nodes_it != node_range.second; ++nodes_it){          //apply bcs to those nodes
	        tmp.set_node_id(nodes_it->id());
	        tmp.set_bc(bc_it->bc());
	        // only apply forces to nodes on the owning domain
	        if(nodes_it->own() == dom_id){                                                      //if I share but do not own node
	          dom->pre_force.push_back(tmp);                                                    //then ignore
	        }
        }
      }
      // sort BC by node id
      dom->pre_force.sort_node_id();
    }
  }
}

void apply_header_mat_assignment(const Header &header,
         const Options &opt,
				 Domains &domains)
{
  #pragma omp parallel num_threads(opt.nthreads()) shared(header,domains)
  {
    const MatAssignList *mat = header.mat_assignment();
    MatAssignList::const_iterator mat_it;
    Domains::iterator dom;
    ElementList::iterator elem;
    ElementList::Range elem_range;
    // for each domain
    #pragma omp for schedule(static,CHUNKSIZE)
    for(size_t dom_id = 0; dom_id < domains.size(); dom_id++){                                //parallel loop over domains
      dom = domains.begin()+dom_id;
      // for each assignment, find range of matching elements
      for(mat_it = mat->begin(); mat_it != mat->end(); ++mat_it){                             //loop over material assignment
        elem_range = dom->elements.find_model_range(mat_it->model());
        // for each element in matching range
        for(elem = elem_range.first; elem != elem_range.second; ++elem){                      //loop over elements, giving them
	        elem->set_material(mat_it->material_id(),mat_it->fiber_id());                       //appropriate properties
        }
      }
    }
  }
}
