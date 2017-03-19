// Implements Stratified Sampling

#include "../include/ss.hpp"

StratifiedSampler::StratifiedSampler(State start,
				     int k,
				     int f_max,
				     function<vector<int>(const State&)> T,
				     Heuristic& H,
				     function<double(int, vector<int>)> P,
				     mt19937& rng)
  : start(start),
    T(T),
    H(H),
    P(P),
    rng(rng),
    size(0)
{
  // strata[g][t] refers to the representative-weight pair at level g with type t
  strata.resize( f_max+1 );

  start.set_h( H(start) );
  
  strata[ start.get_g() ].insert({
      T(start), {start, 1}
    });

  stats.generated_at(start.get_g(), T(start))
    += 1;

  uniform_real_distribution<double> U( 0.0, 1.0 );

  for( int g = 0; g <= f_max; g++ ) {
    
    for(auto& t : strata[g]) {
      auto& rep = t.second;
      State& rep_state = rep.first;
      auto& rep_weight = rep.second;

      //Account for reversal duplicates
      /*
      if(k == 0 && rep_state.get_parent() != nullptr) {
	vector<int> p_type = (vector<int>){t.first[0], t.first[2], t.first[1]};
	rep_weight -= pred_stats.expanded_at(rep_state.get_parent()->get_g(), p_type);
      }
      */

      
      pred_stats.generated_at(rep_state.get_g(), t.first)
	+= rep_weight;

      double pr_dup = P(rep_state.get_g(), t.first);
      
      pred_stats.duplicated_at(rep_state.get_g(), t.first)
	+= rep_weight * pr_dup; 
      
      rep_weight *= (1 - pr_dup);

      auto check_dup = closed.find(rep_state);
      if(!SampleDupDetect( rep_state, k ) ) {

	stats.expanded_at(rep_state.get_g(), t.first)
	  += 1;

	pred_stats.expanded_at(rep_state.get_g(), t.first) += rep_weight;

	// Add s to the closed list (or update it's g-value if we are reopening it)
	State& sr = const_cast<State&>
	  ((check_dup != closed.end()) ?
	   *check_dup :
	   *(closed.emplace(rep_state).first));
      
	   if( sr.get_g() > rep_state.get_g() )
	     sr.set_g(rep_state.get_g());

	for( State child_state : rep_state.expand_fwd() ) {
	  child_state.set_h( H(child_state) );

	  auto c_t = T(child_state);

	  stats.generated_at(child_state.get_g(), c_t)
	    += 1;

	  if(child_state.get_f() <= f_max &&
	     (child_state.get_parent() == nullptr ||
	      child_state.get_parent()->get_parent() == nullptr ||
	      !(child_state == *(child_state.get_parent()->get_parent())))) {
	  
	    auto child_t = strata[ child_state.get_g() ].find( c_t );

	    if (child_t != strata[ child_state.get_g() ].end() ) {
	      auto& child_rep = child_t->second;
	      State& child_rep_state = child_rep.first;
	      auto& child_rep_weight = child_rep.second;

	      child_rep_weight += rep_weight;
	    
	      double P = rep_weight / child_rep_weight;

	      if(U(rng) <= P) {
		child_rep_state = child_state;
	      }
	    
	    } else {
	      strata[ child_state.get_g() ].insert({
		  c_t, {child_state, rep_weight}
		});
	    }
	  }
	}
      } else {
	stats.duplicated_at(rep_state.get_g(), t.first)
	  += 1;
	pred_stats.duplicated_at(rep_state.get_g(), t.first)
	  += rep_weight;
      }
    }
  }
}

bool StratifiedSampler::SampleDupDetect(State& n, int k)
{

  // The length of our random-walks is limited at the stored cost of
  // start -> n
  int g_max = n.get_g();

  // Perform k random walks
  for(int i = 0; i < k; i++) {
    // Start from s = n, with the cost of s -> n as 0
    State s = n;
    s.set_g(0);

    // Walk backward until s -> n is more expensive than start -> n
    while(s.get_g() < g_max) {
      // Check if s is in the closed list,
      // meaning that we know the cost of start -> s
      auto check_dup = closed.find(s);
      //const State* check_dup;
      //for(check_dup = &n;
      //	  check_dup != nullptr && State::Hash(*check_dup) != State::Hash(s);
      //	  check_dup = check_dup->get_parent());
      if(check_dup != closed.end()) {

	// Find the cost of start -> s
	int g = check_dup->get_g();
	
	// Check if the cost of start -> s -> n
	// is cheaper than the stored cost of start -> n
	if(g + s.get_g() < g_max) {
	  return true; // Cheaper path found, n is a duplicate
	}
      }

      // Expand s backward and update s as a randomly chosen backward-child
      auto C = s.expand_bwd();
      uniform_int_distribution<int> U(0, C.size()-1);
      s = C[U(rng)];
    }
  }

  return false; // No shorter path found
}

SearchStat StratifiedSampler::get_stats() const
{
  return stats;
}

SearchStat StratifiedSampler::get_pred_stats() const
{
  return pred_stats;
}
