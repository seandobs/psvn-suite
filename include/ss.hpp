#ifndef _SS_HPP
#define _SS_HPP

#include <vector>
#include <map>
#include <unordered_set>
#include <random>
#include <numeric>


#include "../include/state.hpp"
#include "../include/heuristic.hpp"
#include "../include/searchstat.hpp"

using namespace std;

class StratifiedSampler
{
private:
  State start;
  
  function<vector<int>(State&)> T;
  Heuristic& H;
  function<double(int, vector<int>)> P;
  mt19937& rng;
  
  vector< map<vector<int>, pair<State, double> > > strata;
  unordered_set<State> closed;
  
  SearchStat stats;
  SearchStat pred_stats;
  size_t size;

  bool is_sampling_dups;
  bool is_weighting_dups;
  
public:
  StratifiedSampler(State start,
		    int k,
		    int f_max,
		    function<vector<int>(const State&)> T,
		    Heuristic& H,
		    function<double(int, vector<int>)> P,
		    mt19937& rng);

  bool SampleDupDetect(State& n, int k);
  
  SearchStat get_stats() const;

  SearchStat get_pred_stats() const;
};

#endif
