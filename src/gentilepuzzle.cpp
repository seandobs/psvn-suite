// Generates a tile puzzle domain with the given parameters

#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <stdlib.h>
using namespace std;

int M,N;

string convertInt(int number);
string generate_rule(int i, int j, int k, int l);

int main(int argc, char *argv[])
{

    if (argc < 3)
    {
      cout << "provide M an N as arguments please." <<endl;
      return -1;
    }

    M=atoi(argv[1]); // number of rows
    N=atoi(argv[2]); // number of columns

    string havij;
    havij = "DOMAIN tile ";	
    havij += (convertInt(M*N)+"\n");
    havij += "\t";
    havij += "B ";
    for (int i=1;i<M*N;i++){
    	std::stringstream ss;
	ss << i;
	havij += (ss.str() + " ");
    }
    havij = havij + "\n\n";

    std::stringstream ss;
    ss << (M*N);
    havij += ss.str();
    havij +="\n\n";  

    string tiles_num_str= convertInt(M*N);
    for (int i=0;i<M*N;i++)
        havij=havij+"tile ";
    havij=havij+"\n\n";
    
    for(int i = 0; i< (M*N); i++){
       if((i-N) >= 0) {	// blank can move up
	 for(int j=0;j<(M*N);j++){
		if(i==j)
		      havij = havij +"B ";
		else if(j==(i-N))
		      havij = havij +"X ";
		else
		      havij = havij +"- ";
		    
         }
         havij = havij +"=> ";
	 for(int j=0;j<(M*N);j++){
	 	if(i==j)
		      havij = havij +"X ";
		else if(j==(i-N))
		      havij = havij +"B ";
		else
		      havij = havij +"- ";
	}
	havij = havij + "LABEL UP\n";
      }
    }
	
    for(int i = 0; i< (M*N); i++){     
      if(i%N != 0) {// blank can move left
	 for(int j=0;j<(N*M);j++){
		if(i==j)
		      havij = havij +"B ";
		else if(j==(i-1))
		      havij = havij +"X ";
		else
		      havij = havij +"- ";    
	}
		
	havij = havij +"=> ";
	for(int j=0;j<(N*M);j++){
		if(i==j)
		      havij = havij +"X ";
		else if(j==(i-1))
		      havij = havij +"B ";
		else
		      havij = havij +"- ";
		    
	}
	havij = havij + "LABEL LEFT\n"; 
     }
   }

   for(int i = 0; i< (M*N); i++){
     if(i%N != (N-1)) {// blank can move right
	for(int j=0;j<(M*N);j++){
	   if(i==j)
	      havij = havij +"B ";
	   else if(j==(i+1))
	      havij = havij +"X ";
	   else
	      havij = havij +"- ";
		    
	}
	havij = havij +"=> ";
	for(int j=0;j<(M*N);j++){
	   if(i==j)
	      havij = havij +"X ";
	   else if(j==(i+1))
	      havij = havij +"B ";
	   else
	      havij = havij +"- ";
		    
	}
	havij = havij +"LABEL RIGHT\n";
    }
  }

    for(int i = 0; i< (M*N); i++){
       if((i+N) < (M*N)) {// blank can move down
	 for(int j=0;j<(M*N);j++){
	    if(i==j)
		havij = havij +"B ";
	    else if(j==(i+N))
		havij = havij +"X ";
	    else
	        havij = havij +"- ";
	 }
	 havij = havij +"=> ";
	 for(int j=0;j<(M*N);j++){
	    if(i==j)
		havij = havij +"X ";
	    else if(j==(i+N))
		havij = havij +"B ";
	    else
		havij = havij +"- ";
		    
	 }
	 havij = havij + "LABEL DOWN\n";
	
      }
    }

    havij=havij+"\nGOAL B ";
    for (int i=1; i<M*N;i++)
    {
        havij=havij+ convertInt(i) + " ";
    }
    havij=havij+"\n";
    cout << havij;
}

string generate_rule(int a, int b, int c, int d) //i, j, k, l
{
    //a++;b++;c++;d++;
    char temps[2*M*N+1];
    char temps2[2*M*N+1];
    for (int i=0;i<2*M*N;i+=2)
    {
        temps[i]='-';
        temps[i+1]=' ';
        temps2[i]='-';
        temps2[i+1]=' ';
    }
    temps[2*M*N]=0;
    temps2[2*M*N]=0;
    temps[2*(a*N+b)]='0';
    temps[2*(c*N+d)]='X';
    temps2[2*(a*N+b)]='X';
    temps2[2*(c*N+d)]='0';
    
    string tstr(temps);
    string tstr2(temps2);
    tstr=tstr+"=> "+tstr2+"\n";
    return tstr;
}

string convertInt(int number)
{
   ostringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}

/*
 * Generates MxN-puzzle PSVN files when state variables are tiles in each location
 * standard version
 * Originally Written by Meysam Bastani
 * Re-written by Shahab Jabbari
 * Each rule is about moving the blank tile (e.g., LABEL up means that the blank is moved up)
*/

