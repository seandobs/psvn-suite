
#!/bin/bash

SEARCH=bfs
DOMAIN="tilepuzzle"
#DOMAIN="pancake"
#DOMAIN_ARG1=15
DOMAIN_ARG1=3
DOMAIN_ARG2=4
PROBES=1
WALKS=0
#NO_DUPS=true
STEPS=105
#IGNORE_GOAL=true

PREDICTIONSFILE="3x4_predictions"
PREDSTATSFILE="3x4_pred_stats"
ABSTRACTIONSTATSFILE="3x4_abstraction_stats"

echo "expanded,startseed,predseed,level,num" > $PREDICTIONSFILE
echo "start,g,t,generated,duplicated,expanded,startseed,predseed,level,num" > $PREDSTATSFILE
echo "start,g,t,generated,duplicated,expanded,startseed,predseed,level,num" > $ABSTRACTIONSTATSFILE

for ((S=0; S < 1; S++))
do
    time make run_randomstate \
	 domain=$DOMAIN \
	 domain_arg1=$DOMAIN_ARG1 \
	 domain_arg2=$DOMAIN_ARG2 \
	 f_max=$STEPS \
	 state_seed=$S

    time make run_$SEARCH \
	 domain=$DOMAIN \
	 domain_arg1=$DOMAIN_ARG1 \
	 domain_arg2=$DOMAIN_ARG2 \
	 start=[$S,$STEPS] \
	 no_dups=$NO_DUPS \
	 ignore_goal=$IGNORE_GOAL

    OUT=./outputs/$DOMAIN[$DOMAIN_ARG1,$DOMAIN_ARG2].[$S,$STEPS].${SEARCH}_out
    STATS=./outputs/$DOMAIN[$DOMAIN_ARG1,$DOMAIN_ARG2].[$S,$STEPS].${SEARCH}_stats
    
    F_MAX=$(tail -n1 < $OUT | cut -d "," -f 8)
    
    EXPANDED=$(cat $OUT | tail -n +2 | cut -d, -f4)
    echo $EXPANDED,$S,0,0,0 >> $PREDICTIONSFILE
    awk -v p="$S,0,0,0" -F"," 'BEGIN { OFS = "," } {$8=p; print}' $STATS | tail -n +2 >> $PREDSTATSFILE
    awk -v p="$S,0,0,0" -F"," 'BEGIN { OFS = "," } {$8=p; print}' $STATS | tail -n +2 >> $ABSTRACTIONSTATSFILE

    rm $OUT
    rm $STATS
    for ((P=0; P < 1000; P++))
	do
	    for ((L=$((($DOMAIN_ARG1*$DOMAIN_ARG2) - 1)); L >= 3; L--))
	    do
		
	    for ((N=1; N < 2; N+=1))
	    do
		time make run_${SEARCH}_predict \
		     domain=$DOMAIN \
		     domain_arg1=$DOMAIN_ARG1 \
		     domain_arg2=$DOMAIN_ARG2 \
		     start=[$S,$STEPS] \
		     m=$PROBES \
		     k=$WALKS \
		     f_max=$F_MAX \
		     n=$N \
		     l=$L \
		     no_dups=$NO_DUPS \
		     pred_seed=$P \
		     ignore_goal=$IGNORE_GOAL
		
		OUT=./outputs/$DOMAIN[$DOMAIN_ARG1,$DOMAIN_ARG2].[$S,$STEPS].[$P,$L,$N].${SEARCH}_predict_out
		PREDSTATS=./outputs/$DOMAIN[$DOMAIN_ARG1,$DOMAIN_ARG2].[$S,$STEPS].[$P,$L,$N].${SEARCH}_predict_stats
		ABSTRACTSTATS=./outputs/$DOMAIN[$DOMAIN_ARG1,$DOMAIN_ARG2].[$S,$STEPS].[$P,$L,$N].${SEARCH}_abstract_stats

		EXPANDED=$(cat $OUT | tail -n +2 | cut -d, -f4)
	        echo $EXPANDED,$S,$P,$L,$N >> $PREDICTIONSFILE
		awk -v p="$S,$P,$L,$N" -F"," 'BEGIN { OFS = "," } {$8=p; print}' $PREDSTATS | tail -n +2 >> $PREDSTATSFILE
		awk -v p="$S,$P,$L,$N" -F"," 'BEGIN { OFS = "," } {$8=p; print}' $ABSTRACTSTATS | tail -n +2 >> $ABSTRACTIONSTATSFILE

		rm $OUT
		rm $PREDSTATS
		rm $ABSTRACTSTATS
		
	    done
	done
    done
done
