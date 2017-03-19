// Wraps the PSVN abstraction functions up in a class
// Can be used to produce abstracted states

#include "../include/abstraction.hpp"

Abstraction::Abstraction(PSVNDomain* domain)
  : domain(domain),
    abstraction(create_so_identity_abstraction(domain->get_psvn()),
		[domain](void* a){
		  destroy_so_abstraction(a, domain->get_psvn());
		})
{}

void Abstraction::from_file(const string& filename) {
  abstraction.reset(read_so_abstraction_from_file(filename.c_str(),
						  domain->get_psvn()),
		    [&](void* a){
		      destroy_so_abstraction(a, domain->get_psvn());
		    });
}

void Abstraction::map_domain_value(const int domain_index, const int old_value, const int new_value ) {
  so_abstraction_map_domain_value(abstraction.get(), domain_index, old_value, new_value, domain->get_psvn());
}

vector< vector<int> > Abstraction::get_value_map() const
{
  const int8_t* value_map = so_get_abstraction_value_map(abstraction.get(), domain->get_psvn());
  
  vector< vector<int> > v(so_get_numdomains(domain->get_psvn()));
  for(int i = 0; i < v.size(); i++) {
    v[i] = vector<int>(so_get_domain_size(i, domain->get_psvn()));

    for(int j = 0; j < v[i].size(); j++) {
      v[i][j] = value_map[i*v.size() + j];
    }
  }
  
  return v;
}


State Abstraction::operator()(const State& s) const {
  State a(domain);
  so_abstract_state(abstraction.get(),
		    s.get_state(),
		    a.get_state(),
		    domain->get_psvn());
  return a;
}

const void* Abstraction::get_abst() const
{
  return abstraction.get();
}
