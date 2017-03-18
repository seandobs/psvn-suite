# psvn-suite
This is a suite of tools implementing an A* planner, and the 'Stratified Sampling with Duplicate Probabilities' search tree size predictor.

This work was done for my BsC Honours Dissertation at the University of Auckland (2016).

The suite may be compiled by running `make` in the root directory

The Suite:
* `bin/gen%DOMAIN%` - Takes domain parameters. Outputs a PSVN domain (%DOMAIN% can be 'pancake' or 'tilepuzzle')
* `bin/translator` - Takes an output.sas file from Fast Downward. Outputs a PSVN Domain
* `bin/abstractor` - Takes a PSVN domain, and a PSVN abstraction. Generates the abstracted PSVN domain.
* `bin/psvn2c` - Takes a PSVN domain. Outputs the C API for it. The API can be compiled to a shared object (.so) with gcc
* `bin/pdbgen` - Takes a domain.so, and filename. Generates a PDB for that domain, saves it on disk with the filename.
* `bin/randomstate` - Takes a domain.so, and a number N. Performs a random walk N steps backward from the goal state. Outputs the last state expanded.
* `bin/abstractstate` - Takes a domain.so, a PSVN abstraction, and a state. Outputs the abstract version of that state
* `bin/predict` - Takes a domain.so, an initial state, the parameters for SSDP, and optionally a PDB and PSVN abstraction. Outputs a prediction for the size of the search tree, if we were to run A* with duplicate detection.
* `bin/search` - Takes a domain.so, an initial state, the parameters for SSDP, and optionally a PDB and PSVN abstraction. Runs A* starting from the initial state, searching for a solution in the domain, using the PDB as a heuristic.
