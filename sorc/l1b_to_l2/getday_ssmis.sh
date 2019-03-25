#!/bin/sh

#Get a day's SSMIS L1b information and write out with concentration in NetCDF L2
#Robert Grumbine

PDY=${PDY:-`date +"%Y%m%d"`}
HH=${HH:-00}
PM=${PM:-12}

module load NetCDF prod_envir prod_util
module load dumpjb bufr_dumplist

#Get the day's L1b ssmi-s data
#  HH is the central hour (UTC)
#  PM is hours plus or minus -- i.e., window half-width, 12 to get a day's data
#  ssmisu specifies the SSMI-S instruments (all of them)
#  ca. 50 seconds on phase 1
dumpjb ${PDY}${HH} ${PM} ssmisu 
  
#Main work of L1b to L2 netcdf -- ca. 500 seconds on phase 1
#  n.b.: Most time is spent in decoding the bufr data
ln -sf ssmisu.ibm fort.11 #input data must be in fortran unit 11
${EXDIR}/ssmis_tol2 > out
mv l2out.f285.51.nc l2.f285.51.${PDY}.nc
