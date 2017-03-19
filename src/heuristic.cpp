// Implements a PDB heuristic
// The inputted state is abstracted and then the abstract state is
// checked against the PDB for the relevant h-value

#include "../include/heuristic.hpp"

PDBHeuristic::PDBHeuristic(StateMap& pdb,
	     Abstraction& abstraction)
  : pdb(pdb),
    abstraction(abstraction)
{}

PSVNDomain::cost_t PDBHeuristic::operator()(const State& s) const {
  PSVNDomain::cost_t h;
    
  if (s.is_goal())
    h = 0;
  else {
    State abs = abstraction(s);
    PSVNDomain::cost_t* find = pdb.get(abs);
    if(find != NULL) {
      h = *find;
    } else {
      //h = s.cheapest_fwd();
      h = 0;
    }
  }

  return h;
}
