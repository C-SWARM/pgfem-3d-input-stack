/* -*- mode: c++; -*- */
/* HEADER */
/**
 * AUTHORS:
 * Matt Mosby
 */

#include "renumber_global_nodes.h"
#include "omp_param.h"
#include <list>
#include <algorithm>
#include <utility>
#include <vector>

class NodeIterList : public std::vector<NodeList::iterator>
{
public:
  typedef std::pair<NodeIterList::iterator,NodeIterList::iterator> Range;

  static bool compare_gid(const NodeList::iterator &a,
			  const NodeList::iterator &b)
  {
    return (a->gid() < b->gid());
  }
};


void renumber_global_nodes(const Options &opt,
                           Domains &domains)
{
  NodeIterList it_nodes;
  NodeIterList *pit_nodes = new NodeIterList[domains.size()];

  // get list of global nodes on all domains
  {
    Domains::iterator dom0 = domains.begin();
    Node global;
    global.set_gid(0);
    // for each domain
    #pragma omp parallel num_threads(opt.nthreads()) shared(domains,dom0,global,pit_nodes) 
    {
      Domains::iterator dom;
      size_t dom_id;
      #pragma omp for schedule(static,CHUNKSIZE)
      for(dom_id = 0; dom_id < domains.size(); dom_id++){

        dom = dom0 + dom_id;
        dom->nodes.sort_gid();

        // Get lower bound iterator of global nodes. Since the nodes are
        // sorted by gid, all global nodes are in the range
        // [it , dom->nodes.end())
        NodeList::iterator it = std::lower_bound(dom->nodes.begin(),
				  	       dom->nodes.end(),
					         global,Node::compare_gid);

        // for each in range add *iterator* to list
        for(; it != dom->nodes.end(); ++it){
        	pit_nodes[dom_id].push_back(it);
        }
      }
    } //ends pragma
  }

  for(size_t dom_id = 0; dom_id < domains.size(); dom_id++){
    it_nodes.insert(it_nodes.end(),pit_nodes[dom_id].begin(),pit_nodes[dom_id].end());
  }
  delete[] pit_nodes;



  if(it_nodes.empty()){
    std::cerr << "ERROR: no global nodes!?!" << std::endl;
  }
  // sort iterators by gid
  std::sort(it_nodes.begin(),it_nodes.end(),NodeIterList::compare_gid);

  // search for nodes with equal gid and renumber
  NodeIterList::iterator it = it_nodes.begin();
  NodeIterList::Range range;
  size_t gid = 0;
  while(it != it_nodes.end()){
    range = std::equal_range(it,it_nodes.end(),*it,
			     NodeIterList::compare_gid);

    // renumber nodes with same gid
    if(range.first == range.second){
      std::cerr << "WARNING: no nodes matching nodes!?!" << std::endl;
    } else {
      for(it = range.first; it != range.second; ++it){
      	(*it)->set_gid(gid);
      }

      // it = range.second (next global node || it_nodes.end() )
      gid++;
    }
  }

  std::cout << "Number of global nodes: " << gid << std::endl;

}
