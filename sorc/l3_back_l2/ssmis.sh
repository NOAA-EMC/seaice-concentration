hhmm=1130
com2=/com2/omb/prod/sice.

for tag in 20180716 20180715 20180714 20180713 20180712 20180711 20180710 20180709 
do
  ./ssmil3tol2_F17 ${com2}${tag}/seaice.t00z.n3ssmis ${com2}${tag}/seaice.t00z.s3ssmis ${tag}${hhmm} ${tag}${hhmm}_17 > textout17.$tag
  ./ssmil3tol2_F18 ${com2}${tag}/seaice.t00z.n3ssmis18 ${com2}${tag}/seaice.t00z.s3ssmis18 ${tag}${hhmm} ${tag}${hhmm}_18 > textout18.$tag
done

