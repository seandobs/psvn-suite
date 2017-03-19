// Class wrapper for a PSVN state

#include "../include/state.hpp"

State::State(PSVNDomain* domain,
	     const PSVNDomain::cost_t g,
	     const PSVNDomain::cost_t h,
	     State::parent_t parent,
	     const int rid)
  : domain(domain),
    parent(parent),
    state(state_t(new_so_state(domain->get_psvn()),
		  [](void* s){destroy_so_state(s);})),
    g(g),
    h(h),
    rid(rid)
{}

State::State(const State& other)
  : domain(other.domain),
    parent(other.parent),
    state(other.state),
    //state(state_t(new_so_state(domain->get_psvn()),
    //[](void* s){destroy_so_state(s);})),
    
    g(other.g),
    h(other.h),
    rid(other.rid)
{
  //copy_so_state(state.get(),
  //		other.state.get(),
  //		domain->get_psvn());
}

bool State::operator==(const State& rhs) const
{
  return compare_so_states(state.get(),
			   rhs.state.get(),
			   domain->get_psvn()) == 0;
}

bool State::operator<(const State& rhs) const
{
  return compare_so_states(state.get(),
			   rhs.state.get(),
			   domain->get_psvn()) < 0;
}

bool State::operator>(const State& rhs) const
{
  return compare_so_states(state.get(),
			   rhs.state.get(),
			   domain->get_psvn()) > 0;
}

PSVNDomain::cost_t State::get_g() const
{
  return g;
}
void State::set_g(const PSVNDomain::cost_t gparam)
{
  g = gparam;
}
  
PSVNDomain::cost_t State::get_h() const
{
  return h;
}
void State::set_h(const PSVNDomain::cost_t hparam)
{
  h = hparam;
}
  
PSVNDomain::cost_t State::get_f() const
{
  return g+h;
}
  
void* State::get_state() const
{
  return state.get();
}

void State::from_string(const string& str)
{
  read_so_state(str.c_str(),
		state.get(),
		domain->get_psvn());
}

string State::to_string() const
{
  char str[1024];
  sprint_so_state(str, 1024,
		  state.get(),
		  domain->get_psvn());
  return string(str);
}

vector<int8_t> State::get_vars() const
{
  int n = domain->get_psvn()->get_numvars();
  int8_t* vars = (int8_t*) get_state();
  return vector<int8_t>(vars, vars + n);
}

void State::from_file(const string& filename)
{
  ifstream file(filename);
  string line;
  getline(file, line);
  file.close();
  from_string(line);
}

void State::to_file(const string& filename) const
{
  ofstream file(filename);
  file << to_string();
  file.close();
}

void State::from_random_walk(int f_max, mt19937& rng)
{
  
  auto G = domain->goals();
  uniform_int_distribution<int> U(0, G.size()-1);
  State s = G[U(rng)];

  for(int g = 0; g < f_max; g++) {
    auto C = s.expand_bwd();
    uniform_int_distribution<int> U(0, C.size()-1);
    s = C[U(rng)];
  }
  state = s.state;
}

size_t State::size() const
{
  return domain->get_psvn()->state_size;
}

const State* State::get_parent() const
{
  return parent;
}
void State::set_parent(State::parent_t p)
{
  parent = p;
}

int State::get_rule() const
{
  return rid;
}
  
vector<State> State::expand_fwd() const
{
  vector<State> C;
  
  int rid = 0;
  so_func_ptr iter = NULL;
  init_so_fwd_iter(iter, domain->get_psvn());
  while((rid = next_so_fwd_iter(iter, state.get())) >= 0) {  
    PSVNDomain::cost_t cost =
      g + so_fwd_rule_cost(rid, domain->get_psvn());
    
    State c(domain, cost, 0, this, rid);
    
    apply_so_fwd_rule(rid,
		      state.get(),
		      c.state.get(),
		      domain->get_psvn());
    
    C.push_back(c);
  }
    
  return C;
}

vector<State> State::expand_bwd() const
{
  
  vector<State> C;
  
  int rid = 0;
  so_func_ptr iter = NULL;
  init_so_bwd_iter(iter, domain->get_psvn());
  while((rid = next_so_bwd_iter(iter, state.get())) >= 0) {
    
    PSVNDomain::cost_t cost =
      g + so_bwd_rule_cost(rid, domain->get_psvn());
    
    State c(domain, cost, 0, this, rid);
    
    apply_so_bwd_rule(rid,
		      state.get(),
		      c.state.get(),
		      domain->get_psvn());
    
    
    C.push_back(c);
    
  }

  return C;
}

vector<State> State::expand_fwd_abst(const Abstraction& a) const
{
  vector<State> C;
  
  // int rid = 0;
  // dyn_iter_t* iter = NULL;
  // init_so_dyn_fwd_iter(iter, domain->get_psvn());
  // while((rid = next_so_dyn_fwd_iter(iter, state.get(), a.get_abst(), domain->get_psvn())) >= 0) {
    
  //   PSVNDomain::cost_t cost =
  //     g + so_fwd_rule_cost(rid, domain->get_psvn());
    
  //   State c(domain, cost, 0, this, rid);
    
  //   apply_so_dyn_fwd_rule(rid,
  // 			  state.get(),
  // 			  c.state.get(),
  // 			  a.get_abst(),
  // 			  domain->get_psvn());
    
    
  //   C.push_back(c);
    
  // }

  return C;
}

vector<State> State::expand_bwd_abst(const Abstraction& a) const
{
  
  vector<State> C;
  
  // int rid = 0;
  // dyn_iter_t* iter = NULL;
  // init_so_dyn_bwd_iter(iter, domain->get_psvn());
  // while((rid = next_so_dyn_bwd_iter(iter, state.get(), a.get_abst(), domain->get_psvn())) >= 0) {
    
  //   PSVNDomain::cost_t cost =
  //     g + so_bwd_rule_cost(rid, domain->get_psvn());
    
  //   State c(domain, cost, 0, this, rid);
    
  //   apply_so_dyn_bwd_rule(rid,
  // 			  state.get(),
  // 			  c.state.get(),
  // 			  a.get_abst(),
  // 			  domain->get_psvn());
    
    
  //   C.push_back(c);
    
  // }

  return C;
}

int State::num_fwd() const
{
  int num = 0;
  int rid = 0;
  so_func_ptr iter = NULL;
  init_so_fwd_iter(iter, domain->get_psvn());
  while((rid = next_so_fwd_iter(iter, state.get())) >= 0) num++;
  
  return num;
}
int State::num_bwd() const
{
  int num = 0;
  int rid = 0;
  so_func_ptr iter = NULL;
  init_so_bwd_iter(iter, domain->get_psvn());
  while((rid = next_so_bwd_iter(iter, state.get())) >= 0) num++;
  
  return num;
}

PSVNDomain::cost_t State::cheapest_fwd() const
{
  return domain->get_psvn()->cost_of_cheapest_applicable_fwd_rule(state.get());
}
PSVNDomain::cost_t State::cheapest_bwd() const
{
  return domain->get_psvn()->cost_of_cheapest_applicable_fwd_rule(state.get());
}

bool State::is_goal() const
{
  return is_so_goal(state.get(), domain->get_psvn());
}

bool State::is_goal_abst(const Abstraction& a) const
{
  return domain->get_psvn()->is_dyn_goal(state.get(), a.get_abst());
}

size_t State::get_hash() const
{
  return hash_so_state(state.get(), domain->get_psvn());
}


PSVNDomain* State::get_domain() const
{
  return domain;
}
