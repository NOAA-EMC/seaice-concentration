#!/bin/bash 
#####
#PBS -l select=1:ncpus=1
#PBS -l walltime=5:59:00
#PBS -N aice
#PBS -q "dev"
#PBS -j oe
#PBS -A ICE-DEV
#  #PBS -R "rusage[mem=1024]"
#####

#-----------------------------------------------------------------------------
set -x

export NRT=NO

export tagm=20251005
export tag=20251006
export end=20251006

export HOMEbase=/u/robert.grumbine/rgdev
export seaice_analysis_ver=v4.5.2

export HOMEseaice_analysis=$HOMEbase/seaice_analysis.${seaice_analysis_ver}

echo zzz tagm = $tagm

#Use this to override system in favor of my archive:
if [ $NRT == 'NO' ] ; then
  echo zzz not running in near real time, use my archives
  export DCOMROOT=/u/robert.grumbine/noscrub/satellites/prod/
  export COMINsst_base=$HOME/noscrub/nsst/
  export RGTAG=dev
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
