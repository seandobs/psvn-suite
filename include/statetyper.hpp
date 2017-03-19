#ifndef _STATETYPE_HPP
#define _STATETYPE_HPP

#include <vector>
#include <string>
#include <sstream>

#include "state.hpp"

using namespace std;

class StateTyper
{
public:
  typedef vector<int> statetype_t;
  
  StateTyper();
  statetype_t operator()(const State& s);
};

#endif
