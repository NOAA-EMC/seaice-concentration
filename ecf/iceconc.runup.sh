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
echo zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz loading modules zzzzzzzzzzzzzzzzzzzzzzz
module purge
module load EnvVars/1.0.3
module load ips/19.0.5.281 impi/19.0.5
module load prod_envir/1.1.0
module load prod_util/1.1.5
module load grib_util/1.1.1
module load bufr_dumplist/2.3.0
module load dumpjb/5.1.0
module load imagemagick/6.9.9-25
module list
echo zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz done loading modules zzzzzzzzzzzzzzzzzz

# Bring the various environment-sensitive definitions out of J jobs and to here:
#NCO refers to these as 'job card' variables
set -x

tagm=20210405
tag=20210406
end=20210408

export HOMEbase=/u/Robert.Grumbine/rgdev
export seaice_analysis_ver=v4.3.0
export HOMEseaice_analysis=$HOMEbase/seaice_analysis.${seaice_analysis_ver}

cd $HOMEseaice_analysis/ecf
. ./jobcards


if [ -z $obsproc_dump_ver ] ; then
  echo null obsproc_dump_ver
  export obsproc_dump_ver=v4.0.0
  export obsproc_shared_bufr_dumplist_ver=v1.4.0
fi

########################################################
#DBN stuff -- now in jobcards
########################################################


#--------------------------------------------------------------------------------------
#The actual running of stuff

while [ $tag -le $end ]
do
  export PDY=$tag
  export PDYm1=$tagm

  export job=seaice_filter
  export DATA=$DATAROOT/${job}.${pid}
  #script handles make: mkdir $DATA
  time ./sms.filter.fake > /u/Robert.Grumbine/noscrub/com/sms.filter.$tag

  export job=seaice_analysis
  export DATA=$DATAROOT/${job}.${pid}
  #script handles make: mkdir $DATA
#Required for dumpjb to run:
  export TMPDIR=$DATA
  time ./sms.fake > /u/Robert.Grumbine/noscrub/com/sms.$tag

#  module load gempak
#  time ../jobs/JICE_GEMPAK > gempak.$tag

  tagm=$tag
  tag=`expr $tag + 1`
  tag=`dtgfix3 $tag`

done
