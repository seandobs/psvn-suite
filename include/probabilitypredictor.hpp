#ifndef _PROBABILITYPREDICTOR_HPP
#define _PROBABILITYPREDICTOR_HPP

#include <vector>
#include <random>
#include <map>

#include "statetyper.hpp"
#include "astar.hpp"
#include "searchstat.hpp"
#include "state.hpp"
#include "abstraction.hpp"
#include "heuristic.hpp"

using namespace std;

class ProbabilityPredictor
{
  vector< SearchStat > stats;
  SearchStat aggregate_stat;
  double f_max;
  double avg_abst_f_max;
  int level;
  
public:
  ProbabilityPredictor(const State& s,
		       Heuristic& H,
		       const StateTyper& T,
		       int l,
		       int n,
		       int f_max,
		       mt19937& rng,
		       bool is_ignore_goal = false);
  
  double operator()(int f, StateTyper::statetype_t t);

  double total_expanded() const;

  vector< SearchStat > get_abstract_stats();
};

#endif
