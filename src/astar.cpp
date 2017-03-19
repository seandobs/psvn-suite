// Class implementing the A* search algorithm

#include "../include/astar.hpp"

Astar::Astar(State start,
	     Abstraction a,
	     Heuristic& h,
	     function<vector<int>(const State&)> T,
	     bool is_check_dup,
	     bool is_ignore_goal)
	     
  : start(start),
    a(a),
    h(h),
    T(T),
    is_check_dup(is_check_dup),
    size(0)
{
  start = a(start);
  vector<State> goals = start.get_domain()->goals();
  for(auto g = goals.begin(); g != goals.end(); g++) {
    *g = a(*g);
  }
  
  // Set the start-state's h-value and add it to the open list
  start.set_h(h(start));
  open.push(start);
  stats.generated_at(start.get_g(), T(start))++;
  fstats.generated_at(start.get_f(), T(start))++;
  size += start.size();

  // Search until we have explored the entire state space
  while(!open.empty()){
    // Retrieve the open state with the lowest f-value
    State s = open.top();
    open.pop();

    // Only expand s if it isn't a duplicate, or we have found a cheaper path to s
    auto check_dup = closed.find(s);
    if (!is_check_dup
	|| check_dup == closed.end()
	|| check_dup->get_g() > s.get_g()) {
      
      // Add s to the closed list (or update it's g-value if we are reopening it)
      State& sr = const_cast<State&>
	((check_dup != closed.end()) ?
	 *check_dup :
	 *(closed.emplace(s).first));
      
      sr.set_g(s.get_g());
      
      // Count s
      stats.expanded_at(s.get_g(), T(s))++;
      fstats.expanded_at(s.get_f(), T(s))++;

      for(auto g = goals.begin(); g != goals.end(); g++) {
	if(s == *g && ! is_ignore_goal) { // Goal found?
	  goal_found = true;

	  // Recover and return the solution path
	  for(const State* sptr = &s; sptr != nullptr; sptr = sptr->get_parent()) {
	    sln.push_back(*sptr);
	  }
	  sln = vector<State>(sln.rbegin(), sln.rend());
	  return;
	}
      }
      

      // Expand s to get its children
      for(State& c : sr.expand_fwd()) {
	if(!(sr.get_parent() != nullptr &&
	     c == *(sr.get_parent()))) {
	  // Set c's h-value and push it to the open list
	  c.set_h(h(c));
	  open.push(c);

	  // Count c
	  stats.generated_at(c.get_g(), T(c))++;
	  fstats.generated_at(c.get_f(), T(c))++;
	  size += c.size();
	  }
      }
    } else {
      stats.duplicated_at(s.get_g(), T(s))++;
      fstats.duplicated_at(s.get_f(), T(s))++;
    }
  }
}

vector<State> Astar::get_solution() const
{
  return sln;
}

SearchStat Astar::get_stats() const
{
  return stats;
}

SearchStat Astar::get_fstats() const
{
  return fstats;
}

bool Astar::is_goal_found() const
{
  return goal_found;
}
