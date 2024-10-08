#!/bin/bash --login

#export tag=20240110
export tag=${tag:-`date +"%Y%m%d"`}

tagm=`expr $tag - 1`
export tagm=`/u/robert.grumbine/bin/dtgfix3 $tagm`
export end=$tag
echo initial tag date = $tag

#-----------------------------------------------------------------------------

set -xe

# Bring the various environment-sensitive definitions out of J jobs and to here:
#NCO refers to these as 'job card' variables

export HOMEbase=/u/robert.grumbine/rgdev
export seaice_analysis_ver=v4.4.0
export HOMEseaice_analysis=$HOMEbase/seaice_analysis.${seaice_analysis_ver}

cd $HOMEseaice_analysis/ecf/
module reset
source ../versions/run.ver
. ./jobcards
module list
echo $jlogfile $DATA $cyc $cycle
echo date pdy= $PDY ncepdate = $ncepdate


echo date before obsproc: $PDY
if [ -z $obsproc_dump_ver ] ; then
  echo null obsproc_dump_ver
  export obsproc_dump_ver=v4.0.0
  export obsproc_shared_bufr_dumplist_ver=v1.4.0
fi
echo tag = $tag date after obsproc: $PDY

#--------------------------------------------------------------------------------------
#The actual running of stuff
export KEEPDATA=YES

while [ $tag -le $end ]
do
  export PDY=$tag
  export PDYm1=$tagm
  echo dates: tag= $tag PDY = $PDY

  export job=seaice_filter
  export DATA=$DATAROOT/${job}.${pid}
  #Required for dumpjb to run:
  export TMPDIR=$DATA

  #debug: 
  time ./sms.filter.fake > /u/robert.grumbine/noscrub/com/sms.filter.$tag

  #debug: exit

  export job=seaice_analysis
  export DATA=$DATAROOT/${job}.${pid}
  time ./sms.fake > /u/robert.grumbine/noscrub/com/sms.$tag

#  module load gempak
#  time ../jobs/JICE_GEMPAK > gempak.$tag

  tagm=$tag
  tag=`expr $tag + 1`
  tag=`/u/robert.grumbine/bin/dtgfix3 $tag`

done
