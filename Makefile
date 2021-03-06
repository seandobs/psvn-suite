# This is a makefile for the experiment setup for:
# "Using Domain Abstractions to Predict Search Tree Size with Duplicate Detection"
# Usage: make run_astar problem=foo start=bar abstraction=bla 
# Required files:
# 	problems/foo/domain.psvn
#	problems/foo/bar.state
#	problems/foo/abstractions/bla/abs.in
ifndef problem
ifdef domain
problem := $(domain)[
ifdef domain_arg1
problem := $(problem)$(domain_arg1)
ifdef domain_arg2
problem := $(problem),$(domain_arg2)
endif
endif
problem := $(problem)]
endif
endif

ifdef no_dups
dups_flag := --no-duplicate-check
endif

ifdef exact
exact_flag := --exact
endif

ifdef ignore_goal
goal_flag := --ignore-goal
endif




# Compilers and Compiler Options
# ------------------------------
CC = gcc
CXX = g++ -std=c++11
OPT = -O3

# Directories
# -----------------------------
inc := include
src := src
bld := build
bin := bin
in := inputs
abs := abstractions
out := outputs

# Set linker flags
DL = -ldl
DLFLAGS = -fpic -shared -Wl,--export-dynamic


# Default target if none is specified
.PHONY: run_astar run_bfs run_astar_predict run_bfs_predict clean
default: $(bin)/search $(bin)/pdbgen $(bin)/psvn2c $(bin)/abstractor $(bin)/translator $(bin)/predict $(bin)/gentilepuzzle $(bin)/genpancake $(bin)/abstractstate $(bin)/randomstate

# Targets, Prerequisites, and Recipes
# Listed in topological order
#--------------------------------------------------------------

# Compile the object code for the PSVN shared object utility functions
$(bld)/so_util.o: $(src)/so_util.c $(inc)/so_util.h $(inc)/psvn_game_so.h
	mkdir -p $(dir $@)
	$(CC) $(OPT) -c $< -o $@ $(DL)

# Compile the object code for the PSVN helper functions
$(bld)/psvn.o: $(src)/psvn.cpp $(inc)/psvn.hpp
	mkdir -p $(dir $@)
	$(CXX) $(OPT) -c $< -o $@

# Compile the 'psvn2c' API compiler
$(bin)/psvn2c: $(src)/psvn2c.cpp $(inc)/psvn2c.hpp $(bld)/psvn.o $(inc)/psvn.hpp
	mkdir -p $(dir $@)
	$(CXX) $(OPT) $(src)/psvn2c.cpp $(bld)/psvn.o -o $@

# Compile the abstractor
$(bin)/abstractor: $(src)/abstractor.cpp $(inc)/psvn.hpp
	mkdir -p $(dir $@)
	$(CXX) $(OPT) $(src)/abstractor.cpp $(bld)/psvn.o -o $@

# Compile the C++ interface for the PSVN API
$(bld)/domain.o: $(src)/domain.cpp $(inc)/domain.hpp \
		 $(inc)/state.hpp \
		 $(inc)/psvn_game_so.h $(inc)/so_util.h
	mkdir -p $(dir $@)
	$(CXX) $(OPT) -c $(src)/domain.cpp -o $@

$(bld)/state.o: $(src)/state.cpp $(inc)/state.hpp  \
		$(inc)/domain.hpp \
		$(inc)/psvn_game_so.h
	mkdir -p $(dir $@)
	$(CXX) $(OPT) -c $(src)/state.cpp -o $@

$(bld)/state_map.o: $(src)/state_map.cpp $(inc)/state_map.hpp  \
		    $(inc)/domain.hpp $(inc)/state.hpp \
		    $(inc)/psvn_game_so.h
	$(CXX) $(OPT) -c $(src)/state_map.cpp -o $@

$(bld)/abstraction.o: $(src)/abstraction.cpp $(inc)/abstraction.hpp  \
		      $(inc)/domain.hpp $(inc)/state.hpp \
		      $(inc)/psvn_game_so.h
	mkdir -p $(dir $@)
	$(CXX) $(OPT) -c $(src)/abstraction.cpp -o $@

$(bld)/heuristic.o: $(src)/heuristic.cpp $(inc)/heuristic.hpp  \
	            $(inc)/domain.hpp $(inc)/state.hpp $(inc)/state_map.hpp $(inc)/abstraction.hpp
	mkdir -p $(dir $@)
	$(CXX) $(OPT) -c $(src)/heuristic.cpp -o $@

$(bld)/searchstat.o: $(inc)/searchstat.hpp $(src)/searchstat.cpp  \
		     $(inc)/statetyper.hpp
	mkdir -p $(dir $@)
	$(CXX) $(OPT) -c $(src)/searchstat.cpp -o $@

# Compile the Astar search algorithm
$(bld)/astar.o: $(src)/astar.cpp $(inc)/astar.hpp  \
		$(inc)/state.hpp \
		$(inc)/heuristic.hpp \
		$(inc)/searchstat.hpp
	mkdir -p $(dir $@)
	$(CXX) $(OPT) -c $(src)/astar.cpp -o $@

$(bld)/ss.o: $(src)/ss.cpp $(inc)/ss.hpp  \
		$(inc)/state.hpp \
		$(inc)/heuristic.hpp \
		$(inc)/searchstat.hpp
	mkdir -p $(dir $@)
	$(CXX) $(OPT) -c $(src)/ss.cpp -o $@

$(bld)/probabilitypredictor.o: $(src)/probabilitypredictor.cpp $(inc)/probabilitypredictor.hpp  \
		$(inc)/state.hpp \
		$(inc)/searchstat.hpp \
		$(inc)/astar.hpp \
		$(inc)/abstraction.hpp \
		$(inc)/heuristic.hpp \
		$(inc)/statetyper.hpp
	mkdir -p $(dir $@)
	$(CXX) $(OPT) -c $(src)/probabilitypredictor.cpp -o $@

# Compile the Astar search algorithm
$(bld)/statetyper.o: $(src)/statetyper.cpp $(inc)/statetyper.hpp  \
		$(inc)/state.hpp
	mkdir -p $(dir $@)
	$(CXX) $(OPT) -c $(src)/statetyper.cpp -o $@

# Compile the Planner
$(bin)/search:  $(src)/search.cpp \
		$(inc)/astar.hpp $(bld)/astar.o \
		$(inc)/state.hpp $(bld)/state.o \
		$(inc)/domain.hpp $(bld)/domain.o \
		$(inc)/heuristic.hpp $(bld)/heuristic.o \
		$(inc)/state_map.hpp $(bld)/state_map.o \
		$(inc)/abstraction.hpp $(bld)/abstraction.o \
		$(inc)/searchstat.hpp $(bld)/searchstat.o \
		$(inc)/statetyper.hpp $(bld)/statetyper.o \
		$(inc)/so_util.h $(bld)/so_util.o
	mkdir -p $(dir $@)
	$(CXX) $(OPT) $< $(bld)/astar.o $(bld)/domain.o $(bld)/state.o $(bld)/state_map.o $(bld)/abstraction.o $(bld)/heuristic.o $(bld)/searchstat.o  $(bld)/statetyper.o $(bld)/so_util.o -o $@ $(DL)

# Compile the Planner
$(bin)/abstractstate: $(src)/abstractstate.cpp \
		$(inc)/state.hpp $(bld)/state.o \
		$(inc)/domain.hpp $(bld)/domain.o \
		$(inc)/abstraction.hpp $(bld)/abstraction.o \
		$(inc)/so_util.h $(bld)/so_util.o
	mkdir -p $(dir $@)
	$(CXX) $(OPT) $< $(bld)/domain.o $(bld)/state.o $(bld)/abstraction.o $(bld)/so_util.o -o $@ $(DL)

# Compile the Predictor
$(bin)/predict: $(src)/predict.cpp \
		$(inc)/state.hpp $(bld)/state.o \
		$(inc)/ss.hpp $(bld)/ss.o \
		$(inc)/domain.hpp $(bld)/domain.o \
		$(inc)/heuristic.hpp $(bld)/heuristic.o \
		$(inc)/state_map.hpp $(bld)/state_map.o \
		$(inc)/abstraction.hpp $(bld)/abstraction.o \
		$(inc)/searchstat.hpp $(bld)/searchstat.o \
		$(inc)/statetyper.hpp $(bld)/statetyper.o \
		$(inc)/astar.hpp $(bld)/astar.o \
		$(inc)/probabilitypredictor.hpp $(bld)/probabilitypredictor.o \
		$(inc)/so_util.h $(bld)/so_util.o
	mkdir -p $(dir $@)
	$(CXX) $(OPT) $< $(bld)/ss.o $(bld)/domain.o $(bld)/state.o $(bld)/state_map.o $(bld)/abstraction.o $(bld)/heuristic.o $(bld)/searchstat.o $(bld)/statetyper.o $(bld)/astar.o $(bld)/probabilitypredictor.o $(bld)/so_util.o -o $@ $(DL)

# Compile the PDB generator
$(bin)/pdbgen: $(src)/pdbgen.cpp \
	$(inc)/state.hpp $(bld)/state.o \
	$(inc)/abstraction.hpp $(bld)/abstraction.o \
		  $(inc)/domain.hpp $(bld)/domain.o \
		  $(inc)/state_map.hpp $(bld)/state_map.o \
		  $(inc)/so_util.h $(bld)/so_util.o

	mkdir -p $(dir $@)
	$(CXX) $(OPT) $< $(bld)/domain.o $(bld)/state.o $(bld)/abstraction.o $(bld)/state_map.o $(bld)/so_util.o -o $@ $(DL)

#Compile the SAS2PSVN translator
$(bin)/translator: $(src)/translator.cpp
	mkdir -p $(dir $@)
	$(CXX) $(OPT) $< -o $@

# Compile the domain generators
$(bin)/gen%: $(src)/gen%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(OPT) $< -o $@

# Compile the random state generator
$(bin)/randomstate: $(src)/randomstate.cpp \
		$(inc)/state.hpp $(bld)/state.o \
		$(inc)/abstraction.hpp $(bld)/abstraction.o \
		$(inc)/domain.hpp $(bld)/domain.o \
		$(inc)/so_util.h $(bld)/so_util.o
	mkdir -p $(dir $@)
	$(CXX) $(OPT) $< $(bld)/domain.o $(bld)/state.o $(bld)/abstraction.o $(bld)/so_util.o -o $@ $(DL)

$(in)/$(problem)/domain.psvn: $(bin)/gen$(domain)
	mkdir -p $(dir $@)
	$(bin)/gen$(domain) $(domain_arg1) $(domain_arg2) > $@

# Generate an abstraction
$(in)/$(problem)/$(abs)/$(abstraction)/domain.psvn $(in)/$(problem)/$(abs)/$(abstraction)/domain.abst: $(bin)/abstractor $(in)/$(problem)/domain.psvn $(in)/$(problem)/$(abs)/$(abstraction)/abs.in
	mkdir -p $(dir $@)
	$(bin)/abstractor $(in)/$(problem)/domain.psvn $(in)/$(problem)/$(abs)/$(abstraction)/domain < $(in)/$(problem)/$(abs)/$(abstraction)/abs.in

$(in)/$(problem)/psvn_game_so.h: $(inc)/psvn_game_so.h
	mkdir -p $(dir $@)
	cp $< $@

$(in)/$(problem)/psvn2c_common.c: $(src)/psvn2c_common.c
	mkdir -p $(dir $@)
	cp $< $@

$(in)/$(problem)/$(abs)/$(abstraction)/psvn_game_so.h: $(inc)/psvn_game_so.h
	mkdir -p $(dir $@)
	cp $< $@

$(in)/$(problem)/$(abs)/$(abstraction)/psvn2c_common.c: $(src)/psvn2c_common.c
	mkdir -p $(dir $@)
	cp $< $@

# Generate the PSVN API C code for the problem
%/api.c: %/domain.psvn $(bin)/psvn2c
	mkdir -p $(dir $@)
	$(bin)/psvn2c --backwards_moves --fwd_history_len=0 < $< > $@


%/api.so: %/api.c %/psvn_game_so.h %/psvn2c_common.c
	mkdir -p $(dir $@)
	$(CC) $(OPT) $< -o $@ $(DLFLAGS) $(DL)

# Generate the pdb for $(abstraction)
$(in)/$(problem)/$(abs)/$(abstraction)/pdb: $(bin)/pdbgen $(in)/$(problem)/$(abs)/$(abstraction)/api.so
	mkdir -p $(dir $@)
	$(bin)/pdbgen \
	$(in)/$(problem)/$(abs)/$(abstraction)/api.so \
	$(in)/$(problem)/$(abs)/$(abstraction)/pdb


.FORCE:

# Run Astar on $(problem) with the pdb given by $(abstraction) starting from $(start)
$(out)/$(problem).$(start).$(abstraction).astar_out $(out)/$(problem).$(start).$(abstraction).astar_stats: $(bin)/search \
$(in)/$(problem)/api.so \
$(in)/$(problem)/$(start).state \
$(in)/$(problem)/$(abs)/$(abstraction)/pdb \
$(in)/$(problem)/$(abs)/$(abstraction)/domain.abst
	mkdir -p $(dir $@)
	$(bin)/search \
	--domain $(in)/$(problem)/api.so \
	--start $(in)/$(problem)/$(start).state \
	--pdb $(in)/$(problem)/$(abs)/$(abstraction)/pdb \
	--abstraction $(in)/$(problem)/$(abs)/$(abstraction)/domain.abst \
	--stats $(out)/$(problem).$(start).$(abstraction).astar_stats \
	$(dups_flag) \
	$(goal_flag) > $@


# Run BFS on $(problem) starting from $(start)
$(out)/$(problem).$(start).bfs_out $(out)/$(problem).$(start).bfs_stats: $(bin)/search \
$(in)/$(problem)/api.so \
$(in)/$(problem)/$(start).state
	mkdir -p $(dir $@)
	$(bin)/search \
	--domain $(in)/$(problem)/api.so \
	--start $(in)/$(problem)/$(start).state \
	--stats $(out)/$(problem).$(start).bfs_stats \
	$(dups_flag) \
	$(goal_flag) > $@

$(out)/$(problem).$(start).$(abstraction).astar_predict_out $(out)/$(problem).$(start).$(abstraction).astar_predict_stats $(out)/$(problem).$(start).$(abstraction).astar_abstract_stats: $(bin)/predict \
$(in)/$(problem)/api.so \
$(in)/$(problem)/$(start).state \
$(in)/$(problem)/$(abs)/$(abstraction)/pdb \
$(in)/$(problem)/$(abs)/$(abstraction)/domain.abst
	mkdir -p $(dir $@)
	$(bin)/predict \
	--domain $(in)/$(problem)/api.so \
	--pdb $(in)/$(problem)/$(abs)/$(abstraction)/pdb \
	--abstraction $(in)/$(problem)/$(abs)/$(abstraction)/domain.abst \
	--probes $(m) \
	--walks $(k) \
	--f-max $(f_max) \
	--start $(in)/$(problem)/$(start).state \
	--pred-stats $(out)/$(problem).$(start).$(abstraction).astar_predict_stats \
	--abstract-stats $(out)/$(problem).$(start).$(abstraction).astar_abstract_stats \
	$(goal_flag) > $@

$(out)/$(problem).$(start).[$(pred_seed),$(l),$(n)].bfs_predict_out $(out)/$(problem).$(start).[$(pred_seed),$(l),$(n)].bfs_predict_stats  $(out)/$(problem).$(start).[$(pred_seed),$(l),$(n)].bfs_abstract_stats: $(bin)/predict \
$(in)/$(problem)/api.so \
$(in)/$(problem)/$(start).state
	mkdir -p $(dir $@)
	$(bin)/predict \
	--domain $(in)/$(problem)/api.so \
	--probes $(m) \
	--walks $(k) \
	--f-max $(f_max) \
	--num-abstractions $(n) \
	--level-abstractions $(l) \
	--rng-seed $(pred_seed) \
	--start $(in)/$(problem)/$(start).state \
	--pred-stats $(out)/$(problem).$(start).[$(pred_seed),$(l),$(n)].bfs_predict_stats \
	--abstract-stats $(out)/$(problem).$(start).[$(pred_seed),$(l),$(n)].bfs_abstract_stats \
	$(goal_flag) > $@

$(in)/$(problem)/[$(state_seed),$(f_max)].state: $(bin)/randomstate \
$(in)/$(problem)/api.so
	mkdir -p $(dir $@)
	$(bin)/randomstate \
	--domain $(in)/$(problem)/api.so \
	--rng-seed $(state_seed) \
	--f-max $(f_max) > $@

run_astar: $(out)/$(problem).$(start).$(abstraction).astar_out

run_astar_predict: $(out)/$(problem).$(start).$(abstraction).astar_predict_out

run_bfs: $(out)/$(problem).$(start).bfs_out

run_bfs_predict: $(out)/$(problem).$(start).[$(pred_seed),$(l),$(n)].bfs_predict_out

run_randomstate: $(in)/$(problem)/[$(state_seed),$(f_max)].state

# Clean up
clean:
	rm -Rf $(bin)
	rm -Rf $(bld)
	rm -Rf $(out)
	find -type f -path './$(in)/*/api.c' -exec rm {} +
	find -type f -path './$(in)/*/api.so' -exec rm {} +
	find -type f -path './$(in)/*/psvn_game_so.h' -exec rm {} +
	find -type f -path './$(in)/*/psvn2c_common.c' -exec rm {} +
	find -type f -path './$(in)/*/abstraction/*/domain.abst' -exec rm {} +
	find -type f -path './$(in)/*/abstraction/*/domain.psvn' -exec rm {} +
	find -type f -path './$(in)/*/abstraction/*/psvn_game_so.h' -exec rm {} +
	find -type f -path './$(in)/*/abstraction/*/psvn2c_common.c' -exec rm {} +
