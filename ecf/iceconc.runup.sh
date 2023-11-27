#!/bin/bash --login
#####
#BSUB -J aice_2021
#BSUB -q "dev"
#BSUB -P RTO-T2O
#BSUB -W 2:59
# #BSUB -W 0:09
#BSUB -o aice.%J
#BSUB -e aice.%J
#BSUB -R "affinity[core(1)]"
#  #BSUB -R "rusage[mem=1024]"
#####


#-----------------------------------------------------------------------------
set -x

tagm=20231120
tag=20231121
end=20231126

export HOMEbase=/u/robert.grumbine/rgdev
export seaice_analysis_ver=v4.4.0
export HOMEseaice_analysis=$HOMEbase/seaice_analysis.${seaice_analysis_ver}

#Use this to override system in favor of my archive:
#export DCOMROOT=/u/robert.grumbine/noscrub/satellites/
#export RGTAG=prod
#export my_archive=true

cd $HOMEseaice_analysis/ecf

#export COMINsst_base=/u/robert.grumbine/noscrub/sst/prod/sst


#--------------------------------------------------------------------------------------
#The actual running of stuff

while [ $tag -le $end ]
do

  time ./day.sh

  export tagm=$tag
  tag=`expr $tag + 1`
  export tag=`$HOME/bin/dtgfix3 $tag`

done
