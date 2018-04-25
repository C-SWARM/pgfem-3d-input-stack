/* HEADER */
/**
 * AUTHORS:
 * Matt Mosby
 */

#include "domain.h"
#include "omp_param.h"
#include <fstream>
#include <algorithm>
#include <numeric>
#include <iterator>

void Domain::set_serial()
{
  _parallel = false;
}

void Domain::set_parallel()
{
  _parallel = true;
}

bool Domain::parallel()const
{
  return _parallel;
}

void  Domain::sort_model()
{
  nodes.sort_model();
  elements.sort_model();
  coh_elements.sort_model();
}

void  Domain::sort_id()
{
  nodes.sort_id();
  elements.sort_id();
  coh_elements.sort_id();
  pre_disp.sort_node_id();
  pre_force.sort_node_id();
  pre_therm.sort_node_id();
}

size_t Domain::get_max_gid()
{
  nodes.sort_gid();
  return nodes.back().gid();
}

void Domain::read(std::istream &in)
{
  size_t junk;
  bool quadradic = false;
  size_t nnode;
  std::vector<size_t> n_elem(Element::HEX+1,0);

  // first line is all junk
  {
    in >> junk >> junk;
    if(junk == 2) quadradic = true;
    std::string line;
    std::getline(in,line);
  }

  // read number of mesh elements
  in >> nnode
     >> n_elem[Element::EDGE]
     >> n_elem[Element::TRIA]
     >> n_elem[Element::QUAD]
     >> n_elem[Element::TETRA]
     >> n_elem[Element::PYRAM]
     >> n_elem[Element::WEDGE]
     >> n_elem[Element::HEX];


  // create lists of entities
  nodes.resize(nnode);
  nodes.set_parallel(this->parallel());
  {
    Element tmp_elem;
    ElementList tmp_list;
    size_t add = 0;
    if(quadradic) add = Element::HEX+1;

    int total_n_elem = std::accumulate(n_elem.begin(), n_elem.end(), 0);
    elements.reserve(total_n_elem);
    // initialize list of each type of element and append to domain
    // list.
    for(size_t i=0; i<n_elem.size(); i++){
      if(n_elem[i] <= 0) continue;
      tmp_elem.set_type(i+add,this->parallel());
      tmp_list.resize(0);
      tmp_list.resize(n_elem[i],tmp_elem);
      elements.insert(elements.end(),
                      tmp_list.begin(),
                      tmp_list.end());
    }
  }

  // read nodes and elements
  in >> nodes >> elements;

  // reset the elements to parallel for output if !this->parallel()
  if(!this->parallel()){
    for(ElementList::iterator it=elements.begin(), e=elements.end();
	it != e; ++it){
      it->set_type(it->type(),true);
    }
  }
}

void Domain::write(std::ostream &out) const
{
  // This is temporary format
  out << nodes.size() << " " << elements.size() << std::endl;
  out << nodes << std::endl;
  out << elements << std::endl;
}

void Domain::filter_cohesive_elements()
{
  // get range of elements with interface model type
  Element tmp;
  tmp.set_model_type(7);
  ElementList::Range range = std::equal_range(elements.begin(),elements.end(),
					      tmp,Element::compare_model_type);

  // move range from elements to coh_elements
  size_t len = std::distance(range.first, range.second);
  coh_elements.reserve(len);
  coh_elements.insert(coh_elements.end(), range.first, range.second);
  elements.erase(range.first, range.second);

  // sort by original id #
  coh_elements.sort_id();

  // Assign new id #'s
  ElementList::iterator coel_it = coh_elements.begin();
  size_t id = 0;
  for(;coel_it != coh_elements.end(); ++coel_it,id++){
    coel_it->set_id(id);
    coel_it->set_cohesive(true);
  }
}

void Domains::sort_id(const Options &opt)
{
  #pragma omp parallel for num_threads(opt.nthreads()) schedule(static,CHUNKSIZE)
  for(size_t dom_id = 0; dom_id < this->size(); dom_id++){
    Domains::iterator it = this->begin() + dom_id;
    it->sort_id();
  }
}

void Domains::sort_nodes_model(const Options &opt)
{
  #pragma omp parallel for num_threads(opt.nthreads()) schedule(static,CHUNKSIZE)
  for(size_t dom_id = 0; dom_id < this->size(); dom_id++){
    Domains::iterator it = this->begin() + dom_id;
    it->nodes.sort_model();
  }
}

void Domains::sort_nodes_own(const Options &opt)
{
  #pragma omp parallel for num_threads(opt.nthreads()) schedule(static, CHUNKSIZE)
  for(size_t dom_id = 0; dom_id < this->size(); dom_id++){
    Domains::iterator it = this->begin() + dom_id;
    it->nodes.sort_own();
  }
}
