#!/bin/ksh

#Use IMS ice analysis and conditional climatology to estimate current
#  ice cover
#Robert Grumbine 10 July 2014

#must get DCOM, PDY, FIXseaice_analysis, EXECseaice_analysis from environment
#ims is PDYm1 -- it doesn't run until ~12 hours after the sea ice concentration analysis
echo $DCOM $PDY $PDYm1 $FIXseaice_analysis $EXECseaice_analysis

stag=`echo $PDY | cut -c5-8`

if [ \( $stag -gt 531 \) -a \( $stag -lt 1101 \) ] ; then
  cp $FIXseaice_analysis/gl_zero gl_imsice.$PDY
else
  if [ -f $DCOM/wgrbbul/imssnow96.grb.grib2 ] ; then
    tar xf  $FIXseaice_analysis/gliceclimatology.tgz count.$stag
    cp $DCOM/wgrbbul/imssnow96.grb.grib2 imssnow96.grb.grib2
    $WGRIB2 imssnow96.grb.grib2 | grep ICEC | $WGRIB2 -i imssnow96.grb.grib2 -bin imsice.bin
export pgm=imsice_gl
. prep_step
    $EXECseaice_analysis/imsice_gl count.$stag imsice.bin gl_imsice.$PDY
    export err=$?;err_chk
  else
    cp $FIXseaice_analysis/gl_zero gl_imsice.$PDY
  fi
fi

