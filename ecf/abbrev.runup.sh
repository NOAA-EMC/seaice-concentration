#!/bin/bash 
#####
#PBS -l select=1:ncpus=1
#PBS -l walltime=2:59:00
#PBS -N aice_2024
#PBS -q "dev"
#PBS -j oe
#PBS -A ICE-DEV
#  #PBS -R "rusage[mem=1024]"
#####


#-----------------------------------------------------------------------------
set -x

export tagm=20240212
export tag=20240213
export end=20240223

echo zzz $tagm

export HOMEbase=/u/robert.grumbine/rgdev
export seaice_analysis_ver=v4.6.0
export HOMEseaice_analysis=$HOMEbase/seaice_analysis.${seaice_analysis_ver}

#Use this to override system in favor of my archive:
export DCOMROOT=/u/robert.grumbine/noscrub/satellites/prod/
export RGTAG=prod
export my_archive=true

cd $HOMEseaice_analysis/ecf

#export COMINsst_base=/u/robert.grumbine/noscrub/sst/prod/sst


#--------------------------------------------------------------------------------------
#The actual running of stuff

while [ $tag -le $end ]
do

  time ./abbrevday.sh

  export tagm=$tag
  tag=`expr $tag + 1`
  export tag=`$HOME/bin/dtgfix3 $tag`

done
