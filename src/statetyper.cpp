// Maps states to types

#include "../include/statetyper.hpp"

StateTyper::StateTyper()
{}

int twelvetype(int b) {
  int t = -1;
  if(b == 0
     || b == 3
     || b == 8
     || b == 11)
    t = 0;
  else if(b == 1
	  || b == 2
	  || b == 9
	  || b == 10)
    t = 1;
  else if(b == 4
	  || b == 7)
    t = 2;
  else if(b == 5
	  || b == 6)
    t = 3;
  return t;
}

int eighttype(int b) {
  int t = -1;
  if(b == 0
     || b == 2
     || b == 6
     || b == 8)
    t = 0;
  else if(b == 1
	  || b == 3
	  || b == 5
	  || b == 7)
    t = 1;
  else if(b == 4)
    t = 2;
  return t;
}

StateTyper::statetype_t StateTyper::operator()(const State& s)
{
  vector<int8_t> vars = s.get_vars();
  
  int s_t;
  for(s_t = 0; vars[s_t] != 0; s_t++);
  
    
  int p_t = -1;
  if(s.get_parent() != nullptr) {
    vector<int8_t> p_vars = s.get_parent()->get_vars();
    for(p_t = 0; p_vars[p_t] != 0; p_t++);
  }
  
  return (statetype_t){
    twelvetype(s_t), twelvetype(p_t)
  };
}
