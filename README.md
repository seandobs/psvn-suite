# psvn-suite
This is a suite of tools implementing an A* planner, and the 'Stratified Sampling with Duplicate Probabilities' search tree size predictor. These tools are all based around the PSVN problem domain description language, and the PSVN API.

I wrote this software to help complete the Dissertation for my BSc (Hons.) in CS  at the University of Auckland in 2016. This project is not maintained, and was not written with the intention of being useful outside of the scope of my Uni work.

Compile with `make`

The Suite:
* `bin/gen%DOMAIN%` - Takes domain parameters. Outputs a PSVN domain (%DOMAIN% can be 'pancake' or 'tilepuzzle')
* `bin/translator` - Takes an output.sas file from Fast Downward. Outputs a PSVN Domain
* `bin/abstractor` - Takes a PSVN domain, and a PSVN abstraction. Generates the abstracted PSVN domain.
* `bin/psvn2c` - Takes a PSVN domain. Outputs the C API for it. The API can be compiled to a shared object (.so) with gcc
* `bin/pdbgen` - Takes a domain.so, and filename. Generates a PDB for that domain, saves it on disk with the filename.
* `bin/randomstate` - Takes a domain.so, and a number N. Performs a random walk N steps backward from the goal state. Outputs the last state expanded.
* `bin/abstractstate` - Takes a domain.so, a PSVN abstraction, and a state. Outputs the abstract version of that state
* `bin/predict` - Takes a domain.so, an initial state, the parameters for SSDP, and optionally a PDB and PSVN abstraction. Outputs a prediction for the size of the search tree, if we were to run A\* with duplicate detection.
* `bin/search` - Takes a domain.so, an initial state, and optionally a PDB and PSVN abstraction. Runs A\* starting from the initial state, searching for a solution in the domain, using the PDB as a heuristic.

