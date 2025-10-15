#!/bin/bash 

#export tag=20240110
export tag=${tag:-`date +"%Y%m%d"`}
export NRT=${NRT:-YES}
echo zzz tag, NRT = $tag, $NRT

tagm=`expr $tag - 1`
export tagm=`/u/robert.grumbine/bin/dtgfix3 $tagm`
tagm2=`expr $tagm - 1`
export tagm2=`/u/robert.grumbine/bin/dtgfix3 $tagm2`
export end=$tag
echo initial tag date = $tag

#-----------------------------------------------------------------------------

set -x

# Bring the various environment-sensitive definitions out of J jobs and to here:
#NCO refers to these as 'job card' variables

export HOMEbase=$HOME/rgdev
export seaice_analysis_ver=v4.5.2

export HOMEseaice_analysis=$HOMEbase/seaice_analysis.${seaice_analysis_ver}
#Use this to override system in favor of my archive:
if [ $NRT == 'NO' ] ; then
  export DCOMROOT=/u/robert.grumbine/noscrub/satellites/prod/
  export RGTAG=prod
  export my_archive=true
fi

cd $HOMEseaice_analysis/ecf/
module reset
source ../versions/run.ver
. ./jobcards
module list
echo zzz $jlogfile $DATA $cyc $cycle
echo zzz date pdy= $PDY cyc = $cyc ncepdate = $ncepdate

#--------------------------------------------------------------------------------------
#The actual running of stuff
export KEEPDATA=${KEEPDATA:-NO}

while [ $tag -le $end ]
do
  export PDY=$tag
  export PDYm1=$tagm
  export PDYm2=$tagm2
  echo dates: tag= $tag PDY = $PDY

  export job=seaice_viirs
  export DATA=$DATAROOT/${job}.${pid}

  time ./sms.viirs > /u/robert.grumbine/noscrub/com/sms.viirs.$tag

  tagm2=$tagm
  tagm=$tag
  tag=`expr $tag + 1`
  tag=`/u/robert.grumbine/bin/dtgfix3 $tag`

done
