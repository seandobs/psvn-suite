#ifndef _STATE_HPP
#define _STATE_HPP

#include <vector>
#include <fstream>
#include <string>
#include <functional>
#include <iostream>
#include <random>

#include "domain.hpp"
#include "abstraction.hpp"

extern "C" {
#include "psvn_game_so.h"
}

using namespace std;



class State
{
  typedef shared_ptr<void> state_t;
  typedef const State* parent_t;

public:
  PSVNDomain* domain;

private:
  
  parent_t parent;
  state_t state;
  PSVNDomain::cost_t g;
  PSVNDomain::cost_t h;
  int rid;
  
public:
 static size_t Hash(const State& s) {
   return s.get_hash();
 }
  
  
  
  State(PSVNDomain* domain,
	const PSVNDomain::cost_t g=0,
	const PSVNDomain::cost_t h=0,
        parent_t parent=nullptr,
	const int rid=-1);

  State(const State& other);

  bool operator==(const State& rhs) const;
  bool operator<(const State& rhs) const;
  bool operator>(const State& rhs) const;

  PSVNDomain::cost_t get_g() const;
  void set_g(const PSVNDomain::cost_t gparam);
  
  PSVNDomain::cost_t get_h() const;
  void set_h(const PSVNDomain::cost_t hparam);
  
  PSVNDomain::cost_t get_f() const;
  
  void* get_state() const;

  void from_string(const string& str);
  string to_string() const;

  vector<int8_t> get_vars() const;

  void from_file(const string& filename);
  void to_file(const string& filename) const;

  
  size_t get_hash() const;

  void from_random_walk(int f_max, mt19937& rng);

  size_t size() const;

  const State* get_parent() const;
  void set_parent(parent_t p);

  int get_rule() const;
  
  vector<State> expand_fwd() const;
  vector<State> expand_bwd() const;

  vector<State> expand_fwd_abst(const Abstraction& a) const;
  vector<State> expand_bwd_abst(const Abstraction& a) const;

  int num_fwd() const;
  int num_bwd() const;

  PSVNDomain::cost_t cheapest_fwd() const;
  PSVNDomain::cost_t cheapest_bwd() const;

  bool is_goal() const;

  bool is_goal_abst(const Abstraction& a) const;

  PSVNDomain* get_domain() const;

};

namespace std {
	template <>
	struct hash<State>
	{
	  size_t operator()(const State& s) const
	  {
	    return s.get_hash();
	  }
	};
}

#endif
