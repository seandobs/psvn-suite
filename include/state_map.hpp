#ifndef _STATE_MAP_HPP
#define _STATE_MAP_HPP

#include "state.hpp"
#include "domain.hpp"

extern "C" {
#include "psvn_game_so.h"
}

class StateMap
{
  typedef shared_ptr<void> state_map_t;
  PSVNDomain* domain;
  state_map_t state_map;

public:
  StateMap(PSVNDomain* domain);

  PSVNDomain::cost_t* get(const State& s);

  void add(const State& s, const PSVNDomain::cost_t v);
  
  void to_file(const string& filename);

  void from_file(const string& filename);
  
};

#endif

