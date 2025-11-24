#!/bin/sh

for d in 04 05 06 07 08 09 10 11
do
  for cyc in 00 06 12 18
  do
    export tag=202511$d
    time ./cyc${cyc}.sh > $cyc.$d
  done
done
