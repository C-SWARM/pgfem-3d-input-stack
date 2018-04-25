/* -*- mode: c++; -*- */
/* HEADER */
/**
 * AUTHORS:
 * Matt Mosby
 *
 * This file defines the interface for functions to compute and output
 * communication hints to accelerate the initialization of PGFem3D and
 * potentially remove some issues related to communication among large
 * numbers of prcesses (i.e., >10k) on certain machines.
 */

#pragma once
#ifndef CON3D_COMM_HINTS_H
#define CON3D_COMM_HINTS_H

#include <vector>
#include <iostream>

class Domains;
class Options;

/*
 * Provide hints for which *other* domains/processes a given domain
 * communicates with.
 *
 * Given a set of domians with the nodes sorted by ownership, search
 * the specified domain to mark which other domains to send
 * information to. Then search the other domains to determine which
 * ones to receive information from. The 'receive_from' list
 * corresponds to information received during the assembly process and
 * send during the scatter/localization process. The 'send_to' list
 * corresponds to information sent during the assembly process and
 * received during scatter process.
 */
class CommHints
{
 public:
  void operator()(const Domains &domains,
                  const size_t this_dom);
  void print(std::ostream &out) const;

 private:
  std::vector<size_t> _receive_from;
  std::vector<size_t> _send_to;
};

class CommHintsList : std::vector<CommHints>
{
public:
  CommHintsList(const Domains &domains,
                const Options &opt);
  void print(const Options &opt) const;

private:
  CommHintsList(); /* not implemented */
};

#endif
