/* 
 * Author: Shahab Jabbari
 * 
 * Generates a PSVN encoding from the output of FD's translator
 */

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <string.h>

using namespace std;

string convertInt(int number)
{
    ostringstream ss;//create a stringstream
    ss << number;//add number to the stream
    return ss.str();//return a string with the contents of the stream
}

int main(int argc, char *argv[])
{
    string havij = "";
    string goal = "";
    if (argc < 2)
    {
        cout << "provide the input .sas file as an argument please." << endl;
        return -1;
    }

    FILE* f = fopen(argv[1], "r");
    if (f == NULL)
    {
        printf("Error: cannot read from the problem file %s \n", argv[1]);
        return 1;
    }

    char line[1024];
    // number of variables in the psvn encoding
    int noVariables = -1;

    // this checks for the number and domain of variables
    bool variable_mode = false;
    // this is true when we are reading the start state from the input
    bool start_mode = false;
    // this is true when we are reading the goal state from the input
    bool goal_mode = false;
    // this is true when we are reading the operator from the input
    bool operator_mode = false;

    int norules = 0;

    while (!feof(f))
    {
        if (fgets(line, sizeof line, f) == NULL)
        {
            break;
        }

        if (variable_mode)
        {
            noVariables = atoi(line);
            // ad number of variables
            havij = havij + (convertInt(noVariables) + "\n\n");

            // add the domain for each variable
            for (int i = 0; i < noVariables; ++i)
            {
                if (fgets(line, sizeof line, f) == NULL)
                {
                    printf("Error: cannot read from the problem file %s \n",
                            argv[1]);
                    return 1;
                }

                char * pch;
                pch = strtok(line, " ");
                pch = strtok(NULL, " ");
                int var_domain = atoi(pch);
                havij = havij + (convertInt(var_domain) + " ");

            }
            havij = havij + "\n\n";
            cout << havij;
            havij = "";
            variable_mode = false;
        }
        else if (start_mode)
        {
            havij = havij + "#START ";
            havij = havij + (convertInt(atoi(line)) + " ");
            for (int i = 0; i < noVariables - 1; ++i)
            {
                if (fgets(line, sizeof line, f) == NULL)
                {
                    printf("Error: cannot read from the problem file %s \n",
                            argv[1]);
                    return 1;
                }
                havij = havij + (convertInt(atoi(line)) + " ");
            }
            havij = havij + "\n\n";
            cout << havij;
            havij = "";
            start_mode = false;
        }
        else if (goal_mode)
        {
            goal = goal + "GOAL ";
            int goal_vars = atoi(line);
            int goals[noVariables];
            for (int i = 0; i < noVariables; i++)
                goals[i] = -1;
            for (int i = 0; i < goal_vars; i++)
            {
                if (fgets(line, sizeof line, f) == NULL)
                {
                    printf("Error: cannot read from the problem file %s \n",
                            argv[1]);
                    return 1;
                }
                char * pch;
                pch = strtok(line, " ");
                int t1 = atoi(pch);
                pch = strtok(NULL, " ");
                int t2 = atoi(pch);
                goals[t1] = t2;
            }
            for (int i = 0; i < noVariables; i++)
            {
                if (goals[i] == -1)
                    goal = goal + "- ";
                else
                    goal = goal + (convertInt(goals[i]) + " ");
            }
            goal = goal + "\n\n";
            goal_mode = false;
        }
        else if (operator_mode)
        {
            norules++;
            for (int i = 0; i < 1024; i++)
            {
                if (line[i] == '\n')
                {
                    line[i] = '\0';
                    break;
                }
                else if (line[i] == ' ')
                    line[i] = '_';

            }
            string label = "";
            label = label + line;
            /*	string label = "";
             char * pch;
             pch = strtok (line," ");
             label = (label + pch+"_");
             while(pch != NULL){
             pch = strtok (NULL, " ");
             label = (label + pch+"_");
             }
             */
            if (fgets(line, sizeof line, f) == NULL)
            {
                printf("Error: cannot read from the problem file %s \n",
                        argv[1]);
                return 1;
            }

            // those that should be true for the operator to be executable and remain the same at the end of the operator
            int noPrevail = atoi(line);
            int LHS[noVariables];
            int RHS[noVariables];
            for (int i = 0; i < noVariables; i++)
            {
                LHS[i] = -1;
                RHS[i] = -1;
            }
            for (int i = 0; i < noPrevail; ++i)
            {
                if (fgets(line, sizeof line, f) == NULL)
                {
                    printf("Error: cannot read from the problem file %s \n",
                            argv[1]);
                    return 1;
                }
                char * pch;
                pch = strtok(line, " ");
                int t1 = atoi(pch);
                pch = strtok(NULL, " ");
                int t2 = atoi(pch);
                RHS[t1] = t2;
                LHS[t1] = t2;
            }

            if (fgets(line, sizeof line, f) == NULL)
            {
                printf("Error: cannot read from the problem file %s \n",
                        argv[1]);
                return 1;
            }

            // the number of variables that their value is changed by applying this rule
            int effects = atoi(line);
            for (int i = 0; i < effects; ++i)
            {
                if (fgets(line, sizeof line, f) == NULL)
                {
                    printf("Error: cannot read from the problem file %s \n",
                            argv[1]);
                    return 1;
                }
                char * pch;
                pch = strtok(line, " ");
                int t1 = atoi(pch);
                pch = strtok(NULL, " ");
                int t2 = atoi(pch);
                pch = strtok(NULL, " ");
                int t3 = atoi(pch);
                pch = strtok(NULL, " ");
                int t4 = atoi(pch);
                if (t1 != 0)
                {
                    printf(
                            "Error: The current version does not support conditional effects");
                    return 1;
                }
                LHS[t2] = t3;
                RHS[t2] = t4;
            }

            //get Cost, the last line before end
            if (fgets(line, sizeof line, f) == NULL)
            {
                printf("Error: cannot read from the problem file %s \n",
                        argv[1]);
                return 1;
            }
            int cost = atoi(line);

            for (int i = 0; i < noVariables; ++i)
            {
                if (LHS[i] == -1)
                    havij = havij + "- ";
                else
                    havij = havij + (convertInt(LHS[i]) + " ");
            }

            havij = havij + "=> ";
            for (int i = 0; i < noVariables; ++i)
            {
                if (RHS[i] == -1)
                    havij = havij + "- ";
                else
                    havij = havij + (convertInt(RHS[i]) + " ");
            }
            havij = havij + ("LABEL " + label) + (" COST " + convertInt(cost) + "\n");
            //havij = havij + "\n";
            //cout << havij << endl;
            cout << havij;
            havij = "";
            operator_mode = false;
        }

        if (strncmp(line, "begin_variables", 14) == 0)
        {
            variable_mode = true;
        }
        else if (strncmp(line, "begin_state", 10) == 0)
        {
            start_mode = true;
        }
        else if (strncmp(line, "begin_goal", 9) == 0)
        {
            goal_mode = true;
        }
        else if (strncmp(line, "begin_operator", 13) == 0)
        {
            operator_mode = true;
        }

    }

    fclose(f);
    cout << endl << goal << endl;

    return (EXIT_SUCCESS);
}

