#!/bin/sh

#Get a day's SSMIS L1b information and write out with concentration in NetCDF L2
#Robert Grumbine

PDY=${PDY:-`date +"%Y%m%d"`}
HH=${HH:-00}
PM=${PM:-12}
EXDIR=${EXDIR:-.}
OUTDIR=${OUTDIR:-/u/Robert.Grumbine/noscrub/l2}

#module load EnvVars/1.0.3 ips/19.0.5.281 impi/19.0.5
#module load bufr/11.3.1 NetCDF/4.5.0 w3nco/2.2.0
#module load dumpjb/5.1.0 bufr_dumplist/2.3.0
#env | grep -i dcom > ~/dcoms

if [ $PDY -ge 20210809 ] ;
  echo F15 retired 20210809
  exit
fi

set -x
# Run if we don't already have:
if [ ! -f ${OUTDIR}/l2.ssmi.${PDY}.nc ] ; then

  #Get the day's L1b data
  #  HH is the central hour (UTC)
  #  PM is hours plus or minus -- i.e., window half-width, 12 to get a day's data
  #  ssmisu specifies the SSMI-S instruments (all of them)
  #  ssmit  specifies the SSMI   instruments (all of them)
  #  ca. 50 seconds on phase 1
  dumpjb ${PDY}${HH} ${PM} ssmit 
    
  #Main work of L1b to L2 netcdf -- ca. 500 seconds on phase 1
  #  n.b.: Most time is spent in decoding the bufr data
  ln -sf ssmit.ibm fort.14 #input data must be in fortran unit 14
  time ${EXDIR}/ssmi_tol2 > out.248
  mv l2out.f248.51.nc ${OUTDIR}/l2.ssmi.${PDY}.nc

fi
