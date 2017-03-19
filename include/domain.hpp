#ifndef _DOMAIN_HPP
#define _DOMAIN_HPP

#include <vector>
#include <memory>
#include <utility>
#include <string>

extern "C" {
#include "psvn_game_so.h"
#include "so_util.h"
}

using namespace std;

class State;
class Abstraction;
class PSVNDomain
{
  typedef compiled_game_so_t* psvn_t;
  
public:
  typedef int cost_t;
  
  PSVNDomain(const string& filename);
  psvn_t get_psvn();
  cost_t cost_of_cheapest_fwd_rule() const;
  cost_t cost_of_cheapest_bwd_rule() const;
  vector<State> goals();
  vector<State> goals_abst(const Abstraction& a);

private:
  psvn_t psvn;
};

#include "../include/abstraction.hpp"
#include "../include/state.hpp"

#endif
