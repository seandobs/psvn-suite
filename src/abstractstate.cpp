// Applies an abstraction to a state

#include <vector>
#include <iostream>
#include <string>
#include <thread>

#include "../include/domain.hpp"
#include "../include/state.hpp"
#include "../include/abstraction.hpp"
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

  opt.overview = "Abstracts the given state.";
  opt.syntax = "abstractstate --domain <file> --state <file> --abstraction <file>";
  opt.example = "abstractstate --domain pancake15.so --state s1.state --abstraction a1.abst";
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
	  "PSVN Domain file for the unabstracted state","-d","--domain");
  opt.add("",1,1,0,
	  "PSVN State file to be abstracted","-s","--state");
  opt.add("",1,1,0,
	  "PSVN Abstraction file to be applied","-a","--abstraction");

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

  string file;
  
  opt.get("-d")->getString(file);
  PSVNDomain domain(file);

  State s(&domain);
  Abstraction abs(&domain);

  opt.get("-s")->getString(file);
  s.from_file(file);

  opt.get("-a")->getString(file);
  abs.from_file(file);

  State s_abs = abs(s);

  cout << s_abs.to_string();
  
  return 0;
}
