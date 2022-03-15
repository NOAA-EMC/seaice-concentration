#!/bin/sh

if [ ! -f seaice_fixed_fields.nc ] ; then
  echo need seaice_fixed_fields.nc
  exit 1
fi
if [ ! -d /Volumes/Data/dcom.ssmi85 ] ; then
  echo need /Volumes/Data/dcom.ssmi85
  exit 1
fi
if [ ! -d /Volumes/Data/qdoi/nc ] ; then
  echo need /Volumes/Data/qdoi/nc
  exit 1
fi
if [ ! -f ssmi_tol2 ] ; then
  echo need ssmi_tol2
  exit 1
fi


yy=2003
for mm in 01 02 03 04 05 06 07 08 09 10 11 12
do
  ym=${yy}${mm}
  tag=${ym}01

  ln -sf /Volumes/Data/dcom.ssmi85/${yy}/ssmi85.$tag        fort.14 
  ln -sf /Volumes/Data/qdoi/nc/avhrr-only-v2.$tag.nc avhrr-only.nc
  time ./ssmi_tol2 > out.$tag
  time python3 version3.py > outp.$tag
  mv round1 round1.$tag
  mv l2out.f248.51.nc l2out.ssmi.${tag}.nc
done

