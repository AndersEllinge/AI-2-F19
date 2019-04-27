#!/bin/sh
echo "Starting test"

for i in {1..15}
do
   echo "---------------------"
   echo "Running iteration $i."
   ./../build-ludo-Desktop-Release/ludo 21 3 16 3
   # pop, tournament, genes, generations
done

echo "Ending test"
exit 0
