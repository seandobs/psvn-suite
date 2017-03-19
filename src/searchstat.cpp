// Stores statistics from the search

#include "../include/searchstat.hpp"

SearchStat::SearchStat() {}

double& SearchStat::at(double f, StateTyper::statetype_t t, SearchStat::stat_t& v)
{  
  return v[f][t];
} 

map<StateTyper::statetype_t, double>& SearchStat::types_at(double f, SearchStat::stat_t& v)
{
  return v[f];
}

double SearchStat::total_at(double f, const SearchStat::stat_t& v) const
{
  auto found = v.find(f);
  if(found == v.end()) return 0;
  
  double total = 0;
  auto m = found->second;
  for(auto d = m.begin(); d != m.end(); d++) {
    total += d->second;
  }
  return total;
}

double SearchStat::total_up_to(double f, const SearchStat::stat_t& v) const
{
  double total = 0;
  for(auto i = v.begin(); i != v.end() && i->first < f; i++) {
    total += total_at(i->first, v);
  }
  return total;
}

double& SearchStat::expanded_at(double f, StateTyper::statetype_t t)
{
  return at(f, t, expanded);
}
double& SearchStat::duplicated_at(double f, StateTyper::statetype_t t)
{
  return at(f, t, duplicated);
}
double& SearchStat::generated_at(double f, StateTyper::statetype_t t)
{
  return at(f, t, generated);
}
map<StateTyper::statetype_t, double>&  SearchStat::expanded_types_at(double f)
{
  return types_at(f, expanded);
}
map<StateTyper::statetype_t, double>&  SearchStat::duplicated_types_at(double f)
{
  return types_at(f, duplicated);
}
map<StateTyper::statetype_t, double>&  SearchStat::generated_types_at(double f)
{
  return types_at(f, generated);
}
double SearchStat::total_expanded_at(double f) const
{
  return total_at(f, expanded);
}
double SearchStat::total_duplicated_at(double f) const
{
  return total_at(f, duplicated);
}
double SearchStat::total_generated_at(double f) const
{
  return total_at(f, generated);
}
double SearchStat::total_expanded_up_to(double f) const
{
  return total_up_to(f, expanded);
}
double SearchStat::total_duplicated_up_to(double f) const
{
  return total_up_to(f, duplicated);
}
double SearchStat::total_generated_up_to(double f) const
{
  return total_up_to(f, generated);
}

double SearchStat::get_f_max() const
{
  return expanded.rbegin()->first;
}

string SearchStat::to_string(string prefix, int f_max)
{
  ostringstream os;
  for(int f = 0; f <= f_max; f++) {
    auto m = generated.at(f);
    for(auto k : m) {
      auto t = k.first;
      os << prefix
	 << "," << f
	 << ",\"(";
      for(auto it = t.begin(); it != t.end(); it++) {
	os << *it;
	if(it+1 != t.end()) os << ",";
      }
      os << ")\"," << generated_at(f, t)
	 << "," << duplicated_at(f, t)
	 << "," << expanded_at(f, t)
	 << endl;
    }
  }
  return os.str();
}
