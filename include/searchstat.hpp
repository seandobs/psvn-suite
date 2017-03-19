#ifndef _SEARCHSTAT_HPP
#define _SEARCHSTAT_HPP

#include <vector>
#include <map>
#include <functional>
#include <numeric>
#include <string>
#include <sstream>

#include "statetyper.hpp"

using namespace std;

class SearchStat
{
private:
  typedef map<double, map<StateTyper::statetype_t, double> > stat_t;
  stat_t expanded;
  stat_t duplicated;
  stat_t generated;
  
  double& at(double f, StateTyper::statetype_t t, stat_t& v);
  map<StateTyper::statetype_t, double>& types_at(double f, stat_t& v);
  
  double total_at(double f, const stat_t& v) const;
  double total_up_to(double f, const stat_t& v) const;
  
public:
  SearchStat();
  
  double& expanded_at(double f, StateTyper::statetype_t t);
  double& duplicated_at(double f, StateTyper::statetype_t t);
  double& generated_at(double f, StateTyper::statetype_t t);
  map<StateTyper::statetype_t, double>& expanded_types_at(double f);
  map<StateTyper::statetype_t, double>& duplicated_types_at(double f);
  map<StateTyper::statetype_t, double>& generated_types_at(double f);
  double total_expanded_at(double f) const;
  double total_duplicated_at(double f) const;
  double total_generated_at(double f) const;
  double total_expanded_up_to(double f) const;
  double total_duplicated_up_to(double f) const;
  double total_generated_up_to(double f) const;
  double get_f_max() const;
  string to_string(string prefix, int f_max); 
};


#endif
