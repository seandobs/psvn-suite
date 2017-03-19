// Generates a random PSVN state

#include <vector>
#include <iostream>
#include <string>

#include "../include/state.hpp"
#include "../include/domain.hpp"
#include "../include/ezOptionParser.hpp"

using namespace std;

using namespace ez;

void Usage(ezOptionParser& opt) {
	std::string usage;
	opt.getUsage(usage);
	std::cout << usage;
};

int main( int argc, const char **argv )
{
  ezOptionParser opt;

  opt.overview = "Generates a state via random backward walk from the goal.";
  opt.syntax = "randomstate --domain <file> --f-max <num> [--rngseed <num>]";
  opt.example = "randomstate --domain pancake15.so --f-max 10000\n\n";
  opt.footer = "";

  

  opt.add("", // Default.
	  0, // Required?
	  0, // Number of args expected.
	  0, // Delimiter if expecting multiple args.
	  "Display usage instructions.", // Help description.
	  "-h",     // Flag token. 
	  "--help" // Flag token.
	  );
  opt.add("",1,1,0,
	  "PSVN Domain file","-d","--domain");
  opt.add("",1,1,0,
	  "Number of steps to walk backward from goal","-f","--f-max");
  opt.add("0",0,1,0,
	  "RNG seed","-r","--rng-seed");

  opt.parse(argc, argv);

  if (opt.isSet("-h")) {
    Usage(opt);
    return 1;
  }

  std::vector<std::string> badOptions;
  int i;
  if(!opt.gotRequired(badOptions)) {
    for(i=0; i < badOptions.size(); ++i)
      std::cerr << "ERROR: Missing required option "
		<< badOptions[i] << ".\n";
    Usage(opt);
    return 1;
  }

  if(!opt.gotExpected(badOptions)) {
    for(i=0; i < badOptions.size(); ++i)
      std::cerr << "ERROR: Got unexpected number of arguments for option "
		<< badOptions[i] << ".\n\n";
			
    Usage(opt);
    return 1;
  }

  string domain_file_name;
  opt.get("-d")->getString(domain_file_name);
  

  int f_max;
  opt.get("-f")->getInt(f_max);

  int rngseed;
  opt.get("-r")->getInt(rngseed);
  
  PSVNDomain domain(domain_file_name);
  mt19937 rng(rngseed);
  State s(&domain);
  s.from_random_walk(f_max, rng);

  cout << s.to_string();
  
  return 0;
}
