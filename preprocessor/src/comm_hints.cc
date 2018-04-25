/* -*- mode: c++; -*- */
/* HEADER */
/**
 * AUTHORS:
 * Matt Mosby
 */

#include "comm_hints.h"
#include "domain.h"
#include "node.h"
#include "options.h"
#include "omp_param.h"
#include "utils.h"

#include <algorithm> // std::binary_search, std::sort, std::unique, std::distance
#include <sstream>   // std::stringstream
#include <string>    // std::string
#include <fstream>   // std::ofstream

void CommHints::operator()(const Domains &domains,
                           const size_t this_dom)
{
  size_t n_dom = domains.size();
  Node comp_node;
  const size_t size_heuristic = 27;
  _receive_from.reserve(size_heuristic);
  _send_to.reserve(size_heuristic);

  for (size_t i = 0; i < n_dom; i++) {
    Domains::const_iterator dom = domains.begin() + i;
    NodeList::const_iterator first = dom->nodes.begin(), last = dom->nodes.end();

    // if this_dom, search for domains to send info to during assembly
    if (i == this_dom) {
      for (size_t j = 0; j < n_dom; j++){
        comp_node.set_own(j);
        if (j == this_dom) continue;
        if ( std::binary_search(first, last, comp_node, Node::compare_own) )
          _send_to.push_back(j);
      }
    } 
    // otherwise, determine if this_dom receives information from dom
    else {
      comp_node.set_own(this_dom);
      if ( std::binary_search(first, last, comp_node, Node::compare_own) )
        _receive_from.push_back(i);
    }
  }

  // sort and unique hints (required?)
  std::vector<size_t>::iterator it;
  std::sort(_send_to.begin(), _send_to.end());
  it = std::unique(_send_to.begin(), _send_to.end());
  _send_to.resize(std::distance(_send_to.begin(), it));

  std::sort(_receive_from.begin(), _receive_from.end());
  it = std::unique(_receive_from.begin(), _receive_from.end());
  _receive_from.resize(std::distance(_receive_from.begin(), it));
}

void CommHints::print(std::ostream &out) const
{
  out << _send_to.size() << "\t" << _send_to << std::endl;
  out << _receive_from.size() << "\t" << _receive_from << std::endl;
}

CommHintsList::CommHintsList(const Domains &domains,
                             const Options &opt)
{
  resize(domains.size());
  #pragma omp parallel for num_threads(opt.nthreads()) schedule(static,CHUNKSIZE)
  for (size_t i = 0; i < domains.size(); i++) {
    CommHintsList::iterator hint = begin() + i;
    (*hint)(domains, i);
  }
}

static std::string ch_get_filename(const char* base,
                                   const size_t dom)
{
  std::stringstream ss;
  ss << base << "_comm_hints_" << dom << ".in";
  return ss.str();
}

void CommHintsList::print(const Options &opt) const
{
  #pragma omp parallel for num_threads(opt.nthreads()) schedule(static, CHUNKSIZE)
  for (size_t i = 0; i < this->size(); i++) {
    std::string filename = ch_get_filename(opt.base_fname(), i);
    std::ofstream out(filename.c_str());
    file_check_error(out, filename.c_str());
    CommHintsList::const_iterator hint = begin() + i;
    hint->print(out);
    out.close();
  }
}
