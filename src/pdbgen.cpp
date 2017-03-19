// Generates a PDB file from a domain

#include <vector>
#include <queue>
#include <iostream>

#include "../include/state.hpp"
#include "../include/domain.hpp"
#include "../include/state_map.hpp"

using namespace std;

StateMap PDBgen(PSVNDomain* domain)
{
  class StateComparator {
  public:
    bool operator()(const shared_ptr<State>& a,
		    const shared_ptr<State>& b)
    {
      return a->get_g() > b->get_g();
    }
  };
  
  // Open list as a priority queue
  priority_queue<shared_ptr<State>,
		 vector< shared_ptr<State> >,
		 StateComparator> open;

  // Closed list as a state map
  StateMap closed = StateMap(domain);
  
  for(State g : domain->goals()) {
    open.push(shared_ptr<State>(new State(g)));
  }
  
  while(!open.empty()) {
    
    shared_ptr<State> s(open.top());
    open.pop();
    
    int* check_dup = closed.get(*s);
    if(check_dup == NULL || *check_dup > s->get_g()) {
      closed.add(*s, s->get_g());

      for(State c : s->expand_bwd()) {
	shared_ptr<State> cptr(new State(c));
	open.push(cptr);
      }
    }
  }

  return closed;
}


int main( int argc, char **argv )
{
  PSVNDomain domain(argv[1]);
  {
    StateMap pdb = PDBgen(&domain);
    pdb.to_file(argv[2]);
  }
  return 0;
}
