#!/bin/bash 
#####
#PBS -l select=1:ncpus=1
#PBS -l walltime=5:59:00
#PBS -N viirs_nrt
#PBS -q "dev"
#PBS -j oe
#PBS -A ICE-DEV
#  #PBS -R "rusage[mem=1024]"
#####

#-----------------------------------------------------------------------------
set -x

export NRT=YES
export KEEPDATA=NO

export tagm=20251006
export tag=20251007
export end=20251013

#-----------------------------------------------------------------------------
export HOMEbase=$HOME/rgdev
export seaice_analysis_ver=v4.5.2

export HOMEseaice_analysis=$HOMEbase/seaice_analysis.${seaice_analysis_ver}

#Use this to override system in favor of my archive:
if [ $NRT == 'NO' ] ; then
  echo zzz not running in near real time, use my archives
  export DCOMROOT=/u/robert.grumbine/noscrub/satellites/prod/
  export RGTAG=dev
  export my_archive=true
else
  echo zzz running in near real time, use operations
fi

cd $HOMEseaice_analysis/ecf

export cyc=00
#--------------------------------------------------------------------------------------
#The actual running of stuff

while [ $tag -le $end ]
do

  for cyc in 00 06 12 18
  do
    time ./viirsday.sh
  done

  export tagm=$tag
  tag=`expr $tag + 1`
  export tag=`$HOME/bin/dtgfix3 $tag`

done
