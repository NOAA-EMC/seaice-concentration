#!/bin/ksh

#Extract conditional ice climatology for high res global
#Robert Grumbine 10 July 2014

#must get PDY, FIXseaice_analysis, EXECseaice_analysis from environment
stag=`echo $PDY | cut -c5-8`

tar xf  $FIXseaice_analysis/counts.tgz count.$stag
export pgm=noice
. prep_step
$EXECseaice_analysis/noice count.$stag noice.$PDY
    export err=$?;err_chk
