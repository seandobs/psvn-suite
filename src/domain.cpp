// Class wrapper for the PSVN domain struct

#include "../include/domain.hpp"


PSVNDomain::PSVNDomain(const string& filename)
  : psvn(load_psvn_so_object(filename.c_str()))
{
  //psvn->init_dyn_abstractions();
}

PSVNDomain::psvn_t PSVNDomain::get_psvn()
{
  return psvn;
}

PSVNDomain::cost_t PSVNDomain::cost_of_cheapest_fwd_rule() const
{
  return psvn->cost_of_cheapest_fwd_rule;
}

PSVNDomain::cost_t PSVNDomain::cost_of_cheapest_bwd_rule() const
{
  return psvn->cost_of_cheapest_bwd_rule;
}

vector<State> PSVNDomain::goals()
{
  vector<State> G;
    
  State g(this);
  int gid = 0;
  first_so_goal_state(g.get_state(),
		      &gid,
		      psvn);
  do {
    G.push_back(g);
    State g = State(this);
  } while(next_so_goal_state(g.get_state(),
			     &gid,
			     psvn) > 0);

  return G;
}

vector<State> PSVNDomain::goals_abst(const Abstraction& a)
{
  vector<State> G;
    
  State g(this);
  int gid = 0;
  psvn->init_dyn_goal_state(g.get_state(),
  			    &gid,
  			    a.get_abst());
  do {
    G.push_back(g);
    State g = State(this);
  } while(psvn->next_dyn_goal_state(g.get_state(),
  				    &gid,
  				    a.get_abst()) > 0);

  return G;
}
