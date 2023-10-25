#!/bin/sh
#
# Metafile Script : ice_edge_vgf.sh - produce ice edge VG file for OPC ops desks
#
# Log :
# J. Carr/PMB      12/12/2004    Pushed into production.
# F.Achorn/OPC     06/04/2008    Modified to point to the correct gdfile (vg files were blank)
#                                Corrected DBNET alerts
#
# Set up Local Variables
#
set -x
#
export PS4='ICE_EDGE_VGF:$SECONDS + '
workdir="${DATA}/ICE_EDGE_VGF"
mkdir ${workdir}
cd ${workdir}

#XXW sh ${utilscript}/setup.sh
setup.sh

mdl=ice
gdfile=$COMOUT/ice_${PDY}${cyc}
PDY2=`echo $PDY | cut -c3-`

export DBN_ALERT_TYPE=VGF
export DBN_ALERT_SUBTYPE=OPC

atlicename="atl_ice_edge_${PDY2}.vgf"
pacicename="pac_ice_edge_${PDY2}.vgf"

device1="vg|${atlicename}"
device2="vg|${pacicename}"

# Make vgf files...contour the 50% coverage line

gdplot2_vg << EOF > $DATA/$pgmout
GDFILE	= ${gdfile}
GDATTIM	= last
GLEVEL	= 0
GVCORD	= none
PANEL	= 0
SKIP	= 0
SCALE	= 0
GDPFUN  = sgt(ice,0)  ! mask(sgt(ice,0),sea)
TYPE	= c
CONTUR	= 2
CINT	= .1/.50/.50
LINE	= 16/1/5/0    ! 21/1/5/0
FINT	= 0
FLINE	=
HILO	=
HLSYM	=
CLRBAR	=
WIND	=
REFVEC	=
TITLE	= 16/-1/all ice info!21/-2/with sea mask
TEXT	= .8/1/.8/hw
CLEAR	= YES
GAREA	= 31;-100;89;10
PROJ	= mer
MAP	=
LATLON	=
DEVICE	= $device1
STNPLT	=
SATFIL	=
RADFIL	=
LUTFIL	=
STREAM	=
POSN	= 0
COLORS	= 11
MARKER	= 0
GRDLBL	= 0
FILTER	= n
li
run

GAREA   = 30;130;89;-110
PROJ    = mer
DEVICE  = $device2
li
run

exit
EOF

if [ $SENDCOM = "YES" ] ; then
    mv *.vgf ${COMOUT}
    if [ $SENDDBN = "YES" ] ; then
        ${DBNROOT}/bin/dbn_alert ${DBN_ALERT_TYPE} ${DBN_ALERT_SUBTYPE} $job ${COMOUT}/atl_ice_edge_${PDY2}.vgf
        ${DBNROOT}/bin/dbn_alert ${DBN_ALERT_TYPE} ${DBN_ALERT_SUBTYPE} $job ${COMOUT}/pac_ice_edge_${PDY2}.vgf
    fi
fi
exit
