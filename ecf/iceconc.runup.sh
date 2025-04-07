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

export NRT=NO
export tagm=20240315
export tag=20240316
export end=20240318

export HOMEbase=/u/robert.grumbine/rgdev
export seaice_analysis_ver=v4.5.1

echo zzz tagm = $tagm

export HOMEseaice_analysis=$HOMEbase/seaice_analysis.${seaice_analysis_ver}

echo zzz tagm = $tagm

#Use this to override system in favor of my archive:
if [ $NRT == 'NO' ] ; then
  export DCOMROOT=/u/robert.grumbine/noscrub/satellites/prod/
  export RGTAG=prod
  export my_archive=true
fi

cd $HOMEseaice_analysis/ecf

#--------------------------------------------------------------------------------------
#The actual running of stuff

while [ $tag -le $end ]
do

  time ./day.sh

  export tagm=$tag
  tag=`expr $tag + 1`
  export tag=`$HOME/bin/dtgfix3 $tag`

done
