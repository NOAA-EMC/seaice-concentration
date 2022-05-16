#!/bin/sh

if [ -z $MMAB_BASE ] ; then
  echo MMAB_BASE for mmablib has not been defined
  exit
 else
  echo in general, MMAB_BASE= $MMAB_BASE
fi

if [ ! -f makeall.mk ] ; then
  cp ../makeall.mk .
  if [ $? -ne 0 ] ; then
    echo could not find makeall.mk, aborting
    exit 1
  fi
fi

if [ -z $MMAB_LIBF4 ] ; then
  export MMAB_LIBF4=$MMAB_BASE/libombf_4.a
fi

set -x
for d in seaice_blend.Cd seaice_edge.Cd seaice_filtanal.Cd seaice_ice2grib.fd seaice_icegrid.Cd seaice_posteriori.Cd seaice_psgnorth.fd seaice_psgsouth.fd seaice_reduce.Cd seaice_xpm.Cd seaice_regions.Cd seaice_monitor.Cd seaice_grib2.fd seaice_imsice.Cd seaice_noice.Cd
do
  cd $d
  make
  cd ..
done

