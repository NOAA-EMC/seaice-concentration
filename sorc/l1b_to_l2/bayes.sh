#!/bin/sh

#make
for f in .0 .001 .002 .003 .004 .005 .0055 .006 .008 .01 .015 .02 .025 .03 .04 .05 .06 .07 .08 .09 .10 .11 .12 .14 .16 .18 .19 .20 .21 .22 .24 .26 .27 .28 .29 .30 .32 .36
do
  if [ ! -f alpha$f ] ; then
    ./bayes $f -$f > alpha$f
  fi
  if [ ! -f beta$f ] ; then
    ./bayes -$f $f > beta$f
  fi
done
echo O > f
echo U >> f
grep -f f alpha.* beta.* | sort -nr -k 17,17 > allice 

exit
grep U alpha* | sort -nr -k 17,17 > u.alpha.ice
grep O alpha* | sort -nr -k 17,17 > o.alpha.ice
grep U beta* | sort -nr -k 17,17 > u.beta.ice
grep O beta* | sort -nr -k 17,17 > o.beta.ice

for f in o u
do
  for g in alpha beta
  do
    sort -nr -k 13,13 $f.$g.ice  > $f.$g.land
    sort -nr -k 11,11 $f.$g.ice  > $f.$g.water
  done
done
