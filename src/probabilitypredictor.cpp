// Predicts the proportion of states
// generated at each g-level that will be duplicates

#include "../include/probabilitypredictor.hpp"

ProbabilityPredictor::ProbabilityPredictor(const State& s,
					   Heuristic& H,
					   const StateTyper& T,
					   int l,
					   int n,
					   int f_max,
					   mt19937& rng,
					   bool is_ignore_goal)
  : f_max(f_max),
    level(l)
{
  int numvals = Abstraction(s.get_domain()).get_value_map()[0].size();
  avg_abst_f_max = 0;
  uniform_real_distribution<double> U( 1,  numvals);
  for(int i = 0; i < n; i++) {
    Abstraction a(s.get_domain());
    vector<bool> used = vector<bool>(numvals, false);
	
    int mapval = -1;//U(rng);
    //used[mapval] = true;
	
    for(int j = 0; j < l; j++) {

      int x;
      for(x = U(rng); used[x]; x = U(rng));
	  
      used[x] = true;
	  
      a.map_domain_value(0, x, mapval);
    }

    SearchStat abst_stat = Astar(s,
				 a,
				 H,
				 T,
				 true,
				 is_ignore_goal).get_stats();
    stats.push_back(abst_stat);
    double abst_f_max = abst_stat.get_f_max();
    avg_abst_f_max += abst_f_max;
    double decompression =  (f_max) / (abst_f_max);
    for(int f = 0; f < abst_f_max; f++) {
      auto& agg_generated = aggregate_stat.generated_types_at(f * decompression);
      auto& abst_generated = abst_stat.generated_types_at(f);
      for(auto it = abst_generated.begin(); it != abst_generated.end(); it++) {
	agg_generated[it->first] += it->second;
      }

      auto& agg_duplicated = aggregate_stat.duplicated_types_at(f * decompression);
      auto& abst_duplicated = abst_stat.duplicated_types_at(f);
      for(auto it = abst_duplicated.begin(); it != abst_duplicated.end(); it++) {
	agg_duplicated[it->first] += it->second;
      }
    }
  }

  avg_abst_f_max /= n;
}
  
double ProbabilityPredictor::operator()(int f, StateTyper::statetype_t t)
{
  if(stats.size() == 0) return 0;
  
  
  double avg = 0;
  for(auto& abst_stat : stats) {
    double abst_f_max = abst_stat.get_f_max();
    double base_compression = ((double)abst_f_max-1) / ((double)f_max-1);
    double f_percentage = ((double) f) / ((double) f_max-1);
    double compression = pow(base_compression, 1);
    double stretch_f = f * compression;
    if(stretch_f > abst_f_max-1) stretch_f = abst_f_max-1;

    // The stretched f may not be a round integer,
    // so we can't directly find the duplicate probability at that f-level.
    // We need to search for two f-levels to interpolate between.
    
    // Round stretch_f down to get our lower f-level
    int floor_f = (int) stretch_f;
    
    // Take floor_f + 1 as our upper f-level
    int ceil_f = floor_f + 1;

    // We take the duplicate probability for a given
    // f-level and type as the proportion of states
    // of that type that were generated at that level that were also duplicates.
    // Some types may not have been generated at the desired level,
    // in this case, we use the duplicate probability for that level regardless of type.
    double floor_P = (abst_stat.generated_at(floor_f, t) == 0) ?
      abst_stat.total_duplicated_at(floor_f) /
      abst_stat.total_generated_at(floor_f):
      abst_stat.duplicated_at(floor_f, t)
      / abst_stat.generated_at(floor_f, t);

    double ceil_P = (abst_stat.generated_at(ceil_f, t) == 0) ?
      abst_stat.total_duplicated_at(ceil_f) /
      abst_stat.total_generated_at(ceil_f) :
      abst_stat.duplicated_at(ceil_f, t)
      / abst_stat.generated_at(ceil_f, t);

    // Calculate what percentage of our value should come
    // from the upper P.
    double interp =
      (stretch_f - floor_f)
      / (double) (ceil_f - floor_f);

    // Total the interpolated probabilities
    avg += (1 - interp) * floor_P + interp * ceil_P;
  }

  //cout << avg / n << endl;
  return avg / stats.size();
}
double ProbabilityPredictor::total_expanded() const
{
  double total = 0;
  for(auto& stat : stats) {
    total += stat.total_expanded_up_to(stat.get_f_max()-1);
  }
  return total;
}

vector<SearchStat> ProbabilityPredictor::get_abstract_stats() {
  return stats;
}
