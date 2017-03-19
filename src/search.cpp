// Runs the A* search algorithm on the given PSVN problem,
// with the given PDB heuristic (and its associated abstraction)

#include <vector>
#include <iostream>
#include <string>
#include <sstream>

#include "../include/state.hpp"
#include "../include/abstraction.hpp"
#include "../include/heuristic.hpp"
#include "../include/domain.hpp"
#include "../include/state_map.hpp"
#include "../include/astar.hpp"
#include "../include/ezOptionParser.hpp"
#include "../include/statetyper.hpp"
#include "../include/searchstat.hpp"

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

  opt.overview = "Performs A* Search on the given inputs.";
  opt.syntax = "search --domain <file> --start <file>;<file>;... "
    "[--pdb <file> --abstraction <file> --no-duplicate-check]";
  opt.example = "search --domain pancake15.so --start s1;s2;s3 "
    "--pdb a1.pdb --abstraction a1.abst\n\n";
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
  opt.add("",0,1,0,
	  "PSVN PDB file","-p","--pdb");
  opt.add("",0,1,0,
	  "PSVN Abstraction file","-a","--abstraction");
  opt.add("",1,-1,';',
	  "PSVN Start files","-s","--start");
  opt.add("",0,0,0,
	  "Disable Duplicate Checking","-n","--no-duplicate-check");
  opt.add("",0,1,0,
	  "Output search statistics","-o","--stats");
  opt.add("",0,0,0,
	  "Ignore Goal","-i","--ignore-goal");

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

  StateMap pdb(&domain);
  Abstraction abs(&domain);

  if(opt.isSet("-p")) {
    opt.get("-p")->getString(file);
    pdb.from_file(file);
  }
  
  if(opt.isSet("-a")){
    opt.get("-a")->getString(file);
    abs.from_file(file);
  }

  bool is_dup_check = ! opt.isSet("-n");
  bool is_ignore_goal = opt.isSet("-i");

  PDBHeuristic h(pdb, abs);

  ofstream outputfile;
  if(opt.isSet("-o")){
    opt.get("-o")->getString(file);
    outputfile = ofstream(file);
    outputfile << "start,g,t,generated,duplicated,expanded" << endl;
  }

  cout.precision(10);
  
  vector<vector<string>> starts;
  opt.get("-s")->getMultiStrings(starts);

  StateTyper T;

  cout << "start,bj_generated,bj_duplicated,bj_expanded,tot_generated,tot_duplicated,tot_expanded,f_max,goal_found" << endl;
  
  for(int j = 0; j < starts.size(); j++) {
    State start(&domain);
    start.from_file(starts[j][0]);
    start.set_h(h(start));

    Astar search(start,
		 Abstraction(&domain),
		 h,
		 T,
		 is_dup_check,
		 is_ignore_goal);
  
    
    SearchStat stats = search.get_fstats();
    cout << j << ","
	 << stats.total_generated_up_to(stats.get_f_max()-1) << ","
	 << stats.total_duplicated_up_to(stats.get_f_max()-1) << ","
	 << stats.total_expanded_up_to(stats.get_f_max()-1) << ","
	 << stats.total_generated_up_to(stats.get_f_max()) << ","
	 << stats.total_duplicated_up_to(stats.get_f_max()) << ","
	 << stats.total_expanded_up_to(stats.get_f_max()) << ","
	 << stats.get_f_max() << ","
	 << search.is_goal_found() << endl;
    
    // if(search.is_goal_found()) {
    //   vector<State> sln = search.get_solution();
    //   cout << "Solution:" << endl
    // 	   << "(rule) state" << endl;

    //   for(auto s = sln.begin(); s != sln.end(); s++) {
    // 	cout << "(" << s->get_rule() << ") " << s->to_string()
    // 	     << endl;
    //   }
    // }

    if(outputfile) {
      ostringstream os;
      os << j;
      outputfile << stats.to_string(os.str(), stats.get_f_max() - 1);
    }
  }
  
  return 0;
}
