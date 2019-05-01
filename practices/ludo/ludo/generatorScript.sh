#!/bin/sh
echo "Starting test"

for i in {1..50}
do
   echo "---------------------"
   echo "Running iteration $i."
   ./../build-ludo-Desktop-Release/ludo 100 10 10 16 2
   # pop, tournamentSize, numTournaments, genes, generations
done

echo "Ending test"
exit 0
