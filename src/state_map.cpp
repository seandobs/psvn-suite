// Class wrapper for a PSVN state map

#include "../include/state_map.hpp"

StateMap::StateMap(PSVNDomain* domain)
  : domain(domain),
    state_map(new_so_state_map(domain->get_psvn()),
	      [domain](void* m){
		destroy_so_state_map(m, domain->get_psvn());
	      })
{}

PSVNDomain::cost_t* StateMap::get(const State& s)
{
  return so_state_map_get(state_map.get(),
			  s.get_state(),
			  domain->get_psvn());
}

void StateMap::add(const State& s, const PSVNDomain::cost_t v)
{
  so_state_map_add(state_map.get(),
		   s.get_state(),
		   v,
		   domain->get_psvn());
}

void StateMap::to_file(const string& filename)
{
  FILE* file = fopen(filename.c_str(), "wb");
  write_so_state_map(file,
		     state_map.get(),
		     domain->get_psvn());
  fclose(file);
}

void StateMap::from_file(const string& filename)
{
  FILE* file = fopen(filename.c_str(), "rb");
  state_map.reset(read_so_state_map(file, domain->get_psvn()),
			    [&](void* m){
			      destroy_so_state_map(m, domain->get_psvn());
			    });
  fclose(file);
}
