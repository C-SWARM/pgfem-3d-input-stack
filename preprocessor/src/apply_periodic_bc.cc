/* HEADER */
/**
 * AUTHORS:
 * Matt Mosby
 * Aaron Howell
 */

#include "apply_periodic_bc.h"
#include "node.h"
#include "utils.h"
#include "omp_param.h"
#include <list>
#include <vector>
#include <algorithm>


/**
 * Helper class for managing nodes in a set.
 */
class NodesInSet : public std::list<NodeList::iterator>
{
public:
  /**
   * Seach list for sub-list of NodeList::iterators that match 'val'
   * given 'equal' where equal is a function of the type 
   * bool equal(const Node &a, const Node &) { return a==b;}
   */
  template<class Node_equal>
  NodesInSet find_match(Node &val, Node_equal equal) const
  {
    NodesInSet temp;
    NodesInSet::const_iterator it;
    for(it = this->begin(); it != this->end(); ++it){
      if(equal(*(*it),val)) temp.push_back(*it);
    }
    return temp;
  }
};

/**
 * \brief Determine what nodes in a given domain match periodic
 * entities specified in a given set.
 *
 * dom is not modified in this function, but non-const iterators to
 * nodes are stored in per_nodes. Assumes that nodes are sorted by
 * model_type in decending order and then model_id in ascending order.
 */
template<class PerSet>
static void search_domain_for_nodes_in_set(size_t dom_id,
             Domain &dom, const PerSet &set,
					   NodesInSet pper_nodes[])
{
  NodeList::Range matching_range;
  NodeList::iterator it;
  for(size_t i=0; i<set.size(); i++){
    matching_range = dom.nodes.find_model_range(set.model_type(i),
						set.model_id(i));
    for(it = matching_range.first; it != matching_range.second; ++it){
      pper_nodes[dom_id].push_back(it);
    }
  }
}

/**
 * \brief Assign new global node id and node ownership for periodic
 * nodes in the set.
 *
 * If there is a node with a positive global node id, all nodes are
 * assigned that number and ownership.  Otherwise, the nodes are owned
 * by the lowest process in the set and are asssigned the id
 * new_gid. In this case, new_gid is incremented before returning.
 */
static void assign_periodic_numbers_on_set(const NodesInSet &set,
					   size_t &new_gid)
{
  // error check the set and print message/abort
  if(set.size() < 2){
    std::cerr << "ERROR: No matching periodic nodes found for:\n"
	      << *(*(set.begin())) << std::endl;
    abort();
  }

  // get the list of global node id's and ownership pairs
  std::vector< std::pair<long,size_t> > gnn_own(set.size());
  std::vector< std::pair<long,size_t> >::iterator gnn_it = gnn_own.begin();
  NodesInSet::const_iterator set_it = set.begin();
  for(; set_it != set.end() && gnn_it != gnn_own.end(); ++set_it, ++gnn_it){
    gnn_it->first = (*set_it)->gid();
    gnn_it->second = (*set_it)->own();
  }

  // sort the list by gnn
  std::sort(gnn_own.begin(),gnn_own.end(),compare_pair_first<long,size_t>);

  // Search for positive global node id
  std::pair<long,size_t> global_node(0,0);
  gnn_it = std::lower_bound(gnn_own.begin(),gnn_own.end(),global_node,
			    compare_pair_first<long,size_t>);
  if(gnn_it != gnn_own.end()){
    // found positive defined global node number
    global_node = *gnn_it;
  } else {
    // no defined global node number, sort the list by ownership
    std::sort(gnn_own.begin(),gnn_own.end(),compare_pair_second<long,size_t>);
    global_node.first = new_gid++;
    global_node.second = gnn_own.begin()->second;
  }

  for(set_it = set.begin(); set_it != set.end(); ++set_it){
    (*set_it)->set_gid(global_node.first);
    (*set_it)->set_own(global_node.second);
  }
}

/**
 * \brief Search the list per_nodes for elements that match according
 * to equal_1 && equal_2, then assign global node numbers and
 * ownership.
 *
 */
template<class Node_equal>
static void search_set_for_periodic_nodes(size_t &new_gid,
					  NodesInSet &per_nodes,
					  Node_equal equal_1,
					  Node_equal equal_2)
{
  NodesInSet temp;
  NodesInSet::iterator pn_it;
  NodesInSet::iterator tmp_it;

   // exit early
  if(per_nodes.empty()) return;

  // get list of NodeList::iterators that point to nodes with equal 1-coordinate
  while ((pn_it = per_nodes.begin()) != per_nodes.end()){
    temp = per_nodes.find_match(*(*pn_it),equal_1);

    // reduce list to NodeList::iterators that point to nodes with
    // equal 1- && 2-coordinates
    if(!temp.empty()){
      if(equal_1 != equal_2){
	int count = 0;     //used to update tmp_it when temp is changed
	for(tmp_it = temp.begin(); tmp_it != temp.end(); ++tmp_it){
	  temp = temp.find_match(*(*tmp_it),equal_2);   //temp is changed

	  tmp_it = temp.begin();                        //set iterator to new temp
	  for (int i = 0; i < count && tmp_it != temp.end(); ++i)
	    ++tmp_it;                                   //update iterator position
	  ++count;
	}
      }
    }

    // remove matching nodes from the per_nodes list
    if(!temp.empty()){
      for(tmp_it = temp.begin(); tmp_it != temp.end(); ++tmp_it){
	per_nodes.remove(*tmp_it);
      }
    }

    // Assign periodic gid and ownership
    assign_periodic_numbers_on_set(temp,new_gid);

  }
}

template<class Container, class Node_equal_1, class Node_equal_2>
static void apply_periodic_bc_on_group(const Container &group,
				       const Periodic &periodic,
               const Options &opt, 
				       Domains &domains,
				       Node_equal_1 equal_1,
				       Node_equal_2 equal_2)
{
  NodesInSet per_nodes;
  Domains::iterator dom_it0 = domains.begin();
  typename Container::const_iterator grp_it;

  for(grp_it = group.begin(); grp_it != group.end(); ++grp_it){

    NodesInSet *pper_nodes = new NodesInSet[domains.size()];

    #pragma omp parallel for num_threads(opt.nthreads()) schedule(static,CHUNKSIZE) shared(domains,dom_it0,grp_it,pper_nodes)
    for(size_t dom_id = 0; dom_id < domains.size(); dom_id++){
      Domains::iterator dom_it = dom_it0 + dom_id;  
      search_domain_for_nodes_in_set(dom_id,*dom_it,*grp_it,pper_nodes);
    }
   
    for(size_t dom_id = 0; dom_id < domains.size(); dom_id++){
      per_nodes.insert(per_nodes.end(),pper_nodes[dom_id].begin(),pper_nodes[dom_id].end());
    }
    delete[] pper_nodes;

    search_set_for_periodic_nodes(domains.num_global_nodes,
				  per_nodes,equal_1,equal_2);
  }
}
				       

void apply_periodic_bc(const Options &opt,
           Domains &domains,
		       const Periodic &periodic)
{
  // edges
  apply_periodic_bc_on_group(periodic.x_edges,
			     periodic,opt,domains,
			     Node::equal_x,
			     Node::equal_x);

  apply_periodic_bc_on_group(periodic.y_edges,
  			     periodic,opt,domains,
  			     Node::equal_y,
  			     Node::equal_y);

  apply_periodic_bc_on_group(periodic.z_edges,
  			     periodic,opt,domains,
  			     Node::equal_z,
  			     Node::equal_z);

  // faces
  apply_periodic_bc_on_group(periodic.x_faces,
  			     periodic,opt,domains,
  			     Node::equal_y,
  			     Node::equal_z);

  apply_periodic_bc_on_group(periodic.y_faces,
  			     periodic,opt,domains,
  			     Node::equal_x,
  			     Node::equal_z);

  apply_periodic_bc_on_group(periodic.z_faces,
  			     periodic,opt,domains,
  			     Node::equal_x,
  			     Node::equal_y);

}
