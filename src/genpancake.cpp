/* 
 * Original file created by Meysam Bastani
 * Edited by: Shahab Jabbari
 * This file generates PSVN files for the N-pancake puzzle
*/

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <sstream>
#include <assert.h>

using namespace std;


/* GLOBAL VARIABLES */
int N; // N is the number of pancakes
int mincost, maxcost, range;
bool printcost;

#define MAX_N 30  // only applies to the --pcost model

string convertInt(int number)
{
   ostringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}


void pancake_rule_simple_cost(int r) {
    string tstr1="";
    string tstr2="";
    for (int i=0; i<=r; i++)  
    {
        tstr1=tstr1+"X"+convertInt(i)+" ";  // put variables X1...Xr in the first r positions of the LHS
        tstr2=tstr2+"X"+convertInt(r-i)+" ";// put variables Xr...X1 in the first r positions of the RHS
    }
    for (int i=r+1; i<N; i++)  // the remaining positions are "don't care" in both LHS and RHS
    {
        tstr1=tstr1+"-  ";
        tstr2=tstr2+"-  ";
    }

    cout << tstr1 << "=> " << tstr2 << "LABEL Rev" << convertInt(r+1);

    int cost=mincost;
    if (range > 1) { cost = mincost + (rand()%range); } // set the operator cost randomly
    if (printcost) { cout << " COST " << convertInt(cost); }
    cout << endl ;
}



void pancake_rules_complex_cost(int r, int costs[MAX_N][MAX_N]) {
/* In this cost model, the cost of an operator depends on which pancakes are at the end positions of the move.
   For example, if the state is <2,1,0,3> then reversing the first three positions would have a cost of
   costs[2][0].  Reversing the first three positions immediately afterwards would have a cost of costs[0][2],
   which might be different.
   Thus, if there are N pancakes there are N^2 copies of the rule, each with a different cost.
*/

    for (int p=0; p<N; p++) {
        for (int q=0; q<N; q++) {
             printf(" %d ", p);     // test the specific value in the first position
             for (int k = 0; k < r - 1; ++k)
                 printf("X%d ",k);  // need variables to capture the middle values
             printf("%d ", q);      // test the specific value in the last position involved in the reversal

             for (int k = r + 1; k < N; ++k)  // the rest are "don't cares"
                 printf("- ");

             printf("=> ");

             printf("%d ", q);     // similar for the RHS, but with p and q's roles reversed
             for (int k = r - 2; k >= 0; --k)
                 printf("X%d ",k);  // reverse the middle values
             printf("%d ", p);

             for (int k = r + 1; k < N; ++k)  // the rest are "don't cares"
                 printf("- ");

             printf("LABEL Rev%d_%d_%d",r+1,p,q) ;

             printf(" COST %d\n", costs[p][q]);

        }
    }
}

int main(int argc, char *argv[])
{
    int offset,seed;

/******* EXTRACT COMMAND LINE ARGUMENTS ************/

    if ( (argc < 2) || (argc == 4) || (argc > 6)
	   || ( (argc != 6) && (strncmp( argv[1], "--pcost", 7 )==0) )
	   || ( (argc > 5)  && (strncmp( argv[1], "--pcost", 7 )!=0) ) ) {
      cerr << "If you do not have --pcost as the first command line argument, you must provide 1, 2, or 4 integers on the command line." << endl;
      cerr << "The first integer is the number of pancakes." << endl;
      cerr << "If there are no other integers, operator costs will all be 1." << endl;
      cerr << "If there is one other integer, operator costs will all be that value." << endl;
      cerr << "If there are three additional integers, operator costs will be generated at random, and" << endl;
      cerr << "these three integers are: the minimum cost, the maximum cost, and the random number seed." << endl;
      cerr << "If you have --pcost as the first command line argument, it must be followed by 4 integers as described above." << endl;
      exit(0);
    }

    if (argc==6) { offset=1;} else {offset=0;}

    if( (!sscanf( argv[1+offset], "%d", &N ))
          || N <= 1 ) {
        cerr << "bad number of pancakes (must be at least 2): " << argv[1] << endl;
        exit( -1 );
    }
    if( (argc==6) && (N > MAX_N) ) {
        cerr << "Cannot have more than " << convertInt(MAX_N) << " pancakes for the --pcost option." << endl;
        exit( -1 );
    }
    if (argc == 2) {
        mincost = maxcost = 1;
    } else if (argc == 3) { // uniform costs of a specified value
        if( (!sscanf( argv[2], "%d", &mincost ))
              || mincost < 0 ) {
            cerr << "bad operator cost: " << argv[2] << endl;
            exit( -1 );
        }
        maxcost = mincost;
    } else {              // argc==5 or argc==6 and --pcost is specified
        if( (!sscanf( argv[2+offset], "%d", &mincost ))
              || mincost < 0 ) {
            cerr << "bad minimum operator cost: " << argv[2+offset] << endl;
            exit( -1 );
        }
        if( (!sscanf( argv[3+offset], "%d", &maxcost ))
              || maxcost < 0 ) {
            cerr << "bad maximum operator cost: " << argv[3+offset] << endl;
            exit( -1 );
        }
        if( ( mincost > maxcost )) {
            cerr << "minimum cost must be less than maximum cost" << endl;
            exit( -1 );
        }
        if( (!sscanf( argv[4+offset], "%d", &seed ))
              || seed < 2 ) {
            cerr << "bad random seed (must be 2 or larger): " << argv[4+offset] << endl;
            exit( -1 );
        }
    }

    range = maxcost-mincost+1;
    if (range != 1) { srand(seed); }  // initialize the random number generator
    printcost = (range > 1) || (mincost > 1) ;


/******* CREATE THE PSVN FILE ************/

    cout << "# This file was created by genPancake.cpp with the following command line parameters:\n#";
    for (int i=1; i<argc; ++i) {
        cout << "  " << argv[i] ;
    }
    cout << "\n\n# number of state variables = the number of pancakes" << endl;
    cout << convertInt(N) << endl;

    string Nstring = convertInt(N);
    cout << "\n# All state variables have "  << Nstring << " possible values."<< endl;
    for (int i=0;i<N;i++)  { cout << " " << Nstring; }

    cout << "\n\n";

    if (argc < 6) {
        for (int r=1; r<N; r++)  // create the rule to reverse the first (r+1) positions
        {
            pancake_rule_simple_cost(r);
        }
    } else {

/* create the cost matrix */
        int costs[MAX_N][MAX_N] ;
        for (int p=0; p<N; p++) {
            for (int q=0; q<N; q++) {
		costs[p][q] = mincost + (rand()%range);
            }
        }
        for (int r=1; r<N; r++)  // create the rules to reverse the first (r+1) positions
        {
            pancake_rules_complex_cost(r,costs);
        }
    }
    
    cout << endl << "GOAL " ;
    for (int i=0;i<N;i++)
    {
        cout << convertInt(i) << " ";
    }
    cout << endl;

    return (EXIT_SUCCESS);
}


