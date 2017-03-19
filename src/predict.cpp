// Performs SSDP to predict the number of problems
// that an A* search will expand.

#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <thread>
#include <random>
#include <functional>

#include "../include/probabilitypredictor.hpp"
#include "../include/ss.hpp"
#include "../include/state.hpp"
#include "../include/abstraction.hpp"
#include "../include/heuristic.hpp"
#include "../include/domain.hpp"
#include "../include/ezOptionParser.hpp"
#include "../include/statetyper.hpp"
#include "../include/searchstat.hpp"
#include "../include/astar.hpp"

using namespace ez;

void Usage(ezOptionParser& opt) {
	std::string usage;
	opt.getUsage(usage);
	std::cout << usage;
};

int main( int argc, const char **argv )
{
  ezOptionParser opt;

  opt.overview =
    "Performs Stratified Sampling with Duplicate Detection on the given inputs.";
  opt.syntax = "sd --domain  --start <file>,<file>,... "
    "[--pdb <file> --abstraction <file> --probes <num> --walks <num> --f-max <num> --num-abstractions <num> --level-abstractions <num> --rngseed <num>]";
  opt.example = "sd --domain pancake15.so --pdb a1.pdb --abstraction a1.abst "
    "--probes 1 --walks 100 --f-max 10  --start s1,s2,s3\n\n";
  opt.footer = "";

  

  opt.add("", // Default.
	  0, // Required?
	  0, // Number of args expected.
	  0, // Delimiter if expecting multiple args.
	  "Display usage instructions.", // Help description.
	  "-h",     // Flag token. 
	  "--help" // Flag token.
	  );
  opt.add("",1,1,0,"PSVN Domain file","-d","--domain");
  opt.add("",0,1,0,"PSVN PDB file","-p","--pdb");
  opt.add("",0,1,0,"PSVN Abstraction file","-a","--abstraction");
  opt.add("1",0,1,0,"Number of SS probes.","-m","--probes");
  opt.add("0",0,1,0,"Number of SDD walks.","-k","--walks");
  opt.add("0",0,1,0,"Number of Abstractions to use when predicting duplicate probabilities.","-n","--num-abstractions");
  opt.add("0",0,1,0,"Level of Abstraction to use when predicting duplicate probabilities.","-l","--level-abstractions");
  opt.add("",1,1,0,"Maximum f-level for SS.","-f","--f-max");
  opt.add("",1,-1,';',"PSVN Start files","-s","--start");
  opt.add("",0,1,0,"Output SS statistics","-ps","--pred-stats");
  opt.add("",0,1,0,"Output SS statistics","-as","--abstract-stats");
  opt.add("0",0,1,0,"RNG Seed","-r","--rng-seed");
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
      std::cerr << "ERROR: Missing required option " << badOptions[i] << ".\n";
    Usage(opt);
    return 1;
  }

  if(!opt.gotExpected(badOptions)) {
    for(i=0; i < badOptions.size(); ++i)
      std::cerr << "ERROR: Got unexpected number of arguments for option " << badOptions[i] << ".\n\n";
			
    Usage(opt);
    return 1;
  }

 
  cout.precision(10);

  string domain_file_name;
  opt.get("-d")->getString(domain_file_name);

  int seed;
  opt.get("-r")->getInt(seed);
  
  int m;
  opt.get("-m")->getInt(m);

  int k;
  opt.get("-k")->getInt(k);
  
  int f_max;
  opt.get("-f")->getInt(f_max);

  int n;
  opt.get("-n")->getInt(n);

  int l;
  opt.get("-l")->getInt(l);

  vector<vector<string>> starts;
  opt.get("-s")->getMultiStrings(starts);

  bool is_ignore_goal = opt.isSet("-i");

  ofstream pred_stats_file;
  if(opt.isSet("-ps")){
    string pred_stats_file_name;
    opt.get("-ps")->getString(pred_stats_file_name);
    pred_stats_file = ofstream(pred_stats_file_name);
    pred_stats_file << "start,g,t,generated,duplicated,expanded" << endl;
  }

  ofstream abstract_stats_file;
  if(opt.isSet("-as")){
    string abstract_stats_file_name;
    opt.get("-as")->getString(abstract_stats_file_name);
    abstract_stats_file = ofstream(abstract_stats_file_name);
    abstract_stats_file << "start,g,t,generated,duplicated,expanded" << endl;
  }

  mt19937 rng(seed);
  PSVNDomain domain(domain_file_name);
  Abstraction abs(&domain);
  StateMap pdb(&domain);

  if(opt.isSet("-p")) {
    string pdb_file_name;
    opt.get("-p")->getString(pdb_file_name);
    pdb.from_file(pdb_file_name);
  }
  if(opt.isSet("-a")){
    string abs_file_name;
    opt.get("-a")->getString(abs_file_name);
    abs.from_file(abs_file_name);
  }
  
  PDBHeuristic h(pdb, abs);
  StateTyper T;

  cout << "start,pred_generated,pred_duplicated,pred_expanded,ssdp_expanded" << endl;
  
  for(int j = 0; j < starts.size(); j++) {
    State start(&domain);
    start.from_file(starts[j][0]);
    start.set_h(h(start));

    ProbabilityPredictor P_pred(start, h, T, l, n, f_max, rng, is_ignore_goal);
    std::vector<SearchStat> abstract_stats = P_pred.get_abstract_stats();
    if(abstract_stats_file) {
      ostringstream os;
      os << i;
      for(auto& abstract_stat : abstract_stats) {
	abstract_stats_file << abstract_stat.to_string(os.str(), abstract_stat.get_f_max() - 1);
      }
    }

    double pred_generated = 0;
    double pred_expanded = 0;
    double pred_duplicated = 0;
    double ss_expanded = 0;
    for(int i = 0; i < m; i++) {
      
      StratifiedSampler ssdd(start,
			     k,
			     f_max,
			     T,
			     h,
			     P_pred,
			     rng);

      SearchStat stats = ssdd.get_stats();
      SearchStat pred_stats = ssdd.get_pred_stats();

      pred_generated += pred_stats.total_generated_up_to(pred_stats.get_f_max() -1);
      pred_expanded += pred_stats.total_expanded_up_to(pred_stats.get_f_max() -1);
      pred_duplicated += pred_stats.total_duplicated_up_to(pred_stats.get_f_max() -1);
      ss_expanded += stats.total_expanded_up_to(stats.get_f_max());
      
      if(pred_stats_file) {
	ostringstream os;
	os << i;
	pred_stats_file << pred_stats.to_string(os.str(), pred_stats.get_f_max() - 1);
      }

     
    }
    
    pred_generated /= m;
    pred_expanded /= m;
    pred_duplicated /= m;

    cout << j << ","
	 << pred_generated << ","
	 << pred_duplicated << ","
	 << pred_expanded << ","
	 << ss_expanded + P_pred.total_expanded() << endl;

  }
  
  return 0;
}
