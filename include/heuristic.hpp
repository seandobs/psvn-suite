#ifndef _HEURISTIC_HPP
#define _HEURISTIC_HPP

#include "state.hpp"
#include "abstraction.hpp"
#include "state_map.hpp"
#include "domain.hpp"

class Heuristic
{
  
public:
  virtual PSVNDomain::cost_t operator()(const State& s) const =0;
};

class PDBHeuristic : public Heuristic
{
  StateMap& pdb;
  Abstraction& abstraction;
  PSVNDomain::cost_t cheapest;

public:
  
  PDBHeuristic(StateMap& pdb,
	       Abstraction& abstraction);

  PSVNDomain::cost_t operator()(const State& s) const;
};

#endif
