#!/bin/ksh

#Extract conditional ice climatology for Great Lakes wave model
#Robert Grumbine 10 July 2014

#must get PDY, FIXsice, EXECsice from environment
stag=`echo $PDY | cut -c5-8`

if [ \( $stag -gt 531 \) -a \( $stag -lt 1101 \) ] ; then
  cp $FIXseaice_analysis/gl_zero gl_noice.$PDY
else
  tar xf $FIXseaice_analysis/gliceclimatology.tgz count.$stag
export pgm=noice_gl
. prep_step
  $EXECseaice_analysis/noice_gl count.$stag gl_noice.$PDY
    export err=$?;err_chk
fi
