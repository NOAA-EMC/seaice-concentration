hhmm=1130
com2=/com2/omb/prod/sice.

for tag in 20180716 20180715 20180714 20180713 20180712 20180711 20180710 20180709 
do
  ./ssmil3tol2 ${com2}${tag}/seaice.t00z.n3ssmi ${com2}${tag}/seaice.t00z.s3ssmi ${tag}${hhmm} ${tag}${hhmm} > textout.$tag
done

