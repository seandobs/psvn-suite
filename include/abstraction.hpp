#ifndef _ABSTRACTION_HPP
#define _ABSTRACTION_HPP

#include <vector>
#include <random>

#include "state.hpp"
#include "domain.hpp"

using namespace std;

class Abstraction
{
  typedef shared_ptr<compiled_game_so_t> psvn_t;
  typedef shared_ptr<void> abstraction_t;
  
  PSVNDomain* domain;
  abstraction_t abstraction;

public:
  Abstraction(PSVNDomain* domain);

  void from_file(const string& filename);

  void map_domain_value(const int domain_index, const int old_value, const int new_value);

  vector< vector<int> > get_value_map() const;
  
  State operator()(const State& s) const;

  const void* get_abst() const;
  
};

#endif
