#ifndef _ASTAR_HPP
#define _ASTAR_HPP

#include <vector>
#include <unordered_set>
#include <queue>
#include <functional>
#include <iostream>

#include "../include/state.hpp"
#include "../include/heuristic.hpp"
#include "../include/searchstat.hpp"

using namespace std;

class Astar
{
private:
  State start;
  Abstraction a;
  Heuristic& h;
  function<vector<int>(const State&)> T;
  bool is_check_dup;

  class OpenCompare {
  public:
    bool operator() (const State& left, const State& right) {
      return left.get_f() > right.get_f();
    }
  };

  priority_queue<State, vector<State>, OpenCompare> open;
  unordered_set<State> closed;
  vector<State> sln;
  
  SearchStat stats;
  SearchStat fstats;
  size_t size;
  bool goal_found = false;

public:
  Astar(State start,
	Abstraction a,
	Heuristic& h,
	function<vector<int>(const State&)> T = [](const State& s)
	  {return vector<int>();},
	bool is_check_dup = true,
	bool is_ignore_goal = false);

  vector<State> get_solution() const;
  SearchStat get_stats() const;
  SearchStat get_fstats() const;
  bool is_goal_found() const;
};

#endif
