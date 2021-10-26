#!/bin/ksh
###################################################################
echo "----------------------------------------------------"
echo "exnawips - convert NCEP GRIB files into GEMPAK Grids"
echo "----------------------------------------------------"
echo "History: Mar 2000 - First implementation of this new script."
echo "S Lilly: May 2008 - add logic to make sure that all of the "
echo "                    data produced from the restricted ECMWF"
echo "                    data on the CCS is properly protected."
#####################################################################

set -xa

cd $DATA

msg="Begin job for $job"
postmsg "$msg"

#
NAGRIB_TABLE=$HOMEseaice_analysis/fix/nagrib.tbl

#
create_gempak () {
  if [ $RUN == "ice" ]; then
      NAGRIB=${OS_BIN}/nagrib_nc
  else
      NAGRIB=${OS_BIN}/nagrib2
  fi
  if [ "$RUN" == "ice" ] ; then
    cpyfil=`echo $entry  | awk 'BEGIN {FS="|"} {print $2}'`
    garea=`echo $entry   | awk 'BEGIN {FS="|"} {print $3}'`
    gbtbls=`echo $entry  | awk 'BEGIN {FS="|"} {print $4}'`
    maxgrd=`echo $entry  | awk 'BEGIN {FS="|"} {print $5}'`
    kxky=`echo $entry    | awk 'BEGIN {FS="|"} {print $6}'`
    grdarea=`echo $entry | awk 'BEGIN {FS="|"} {print $7}'`
    proj=`echo $entry    | awk 'BEGIN {FS="|"} {print $8}'`
    output=`echo $entry  | awk 'BEGIN {FS="|"} {print $9}'`
  else
    cpyfil=`echo $entry  | awk 'BEGIN {FS="|"} {print $2}'`
    garea=`echo $entry   | awk 'BEGIN {FS="|"} {print $3}'`
    gbtbls=`echo $entry  | awk 'BEGIN {FS="|"} {print $4}'`
    maxgrd=`echo $entry  | awk 'BEGIN {FS="|"} {print $5}'`
    kxky=`echo $entry    | awk 'BEGIN {FS="|"} {print $6}'`
    grdarea=`echo $entry | awk 'BEGIN {FS="|"} {print $7}'`
    proj=`echo $entry    | awk 'BEGIN {FS="|"} {print $8}'`
    output=`echo $entry  | awk 'BEGIN {FS="|"} {print $9}'`
    g2diag=all
    overwr=yes
  fi  
  pdsext=no
  
  maxtries=15
  fhcnt=$fstart
  while [ $fhcnt -le $fend ] ; do
    if [ $fhcnt -ge 100 ] ; then
      typeset -Z3 fhr
    else
      typeset -Z2 fhr
    fi
    fhr=$fhcnt
  
    if [ $RUN == "ice" ] ; then    
      GRIBIN=$COMIN/${model}${RUN}.${cycle}.grb
      GEMGRD=${RUN}_${PDY}${cyc}
    else
      GRIBIN=$COMIN/seaice.${cycle}.5min.grb.grib2
      GEMGRD=${RUN2}_${PDY}${cyc}.grd
    fi
  
    if [ -z $RUN ] ; then
        echo  "RUN not defined"
    else
        GRIBIN_chk=$GRIBIN
    fi
  
    icnt=1
    while [ $icnt -lt 1000 ]
    do
      if [ -r $GRIBIN_chk ] ; then
        break
      else
        let "icnt=icnt+1"
        sleep 20
      fi
      if [ $icnt -ge $maxtries ]
      then
        msg="ABORTING after 1 hour of waiting for F$fhr to end."
        err_exit $msg
      fi
    done
  
    cp $GRIBIN grib$fhr
    export pgm="$NAGRIB F$fhr"
    startmsg
  
    $NAGRIB << EOF
     GBFILE   = grib$fhr
     INDXFL   =
     GDOUTF   = $GEMGRD
     PROJ     = $proj
     GRDAREA  = $grdarea
     KXKY     = $kxky
     MAXGRD   = $maxgrd
     CPYFIL   = $cpyfil
     GAREA    = $garea
     OUTPUT   = $output
     G2TBLS   = $gbtbls
     G2DIAG   = $g2diag
     OVERWR   = $overwr
    l
    r
EOF
    export err=$?;err_chk
  
    #####################################################
    # GEMPAK DOES NOT ALWAYS HAVE A NON ZERO RETURN CODE
    # WHEN IT CAN NOT PRODUCE THE DESIRED GRID.  CHECK
    # FOR THIS CASE HERE.
    #####################################################
    if [ $model != " " ] ; then
      ls -l $GEMGRD
      export err=$?;export pgm="GEMPAK CHECK FILE";err_chk
    fi
    
    gpend
  
    if [ $SENDCOM = "YES" ] ; then
       mv $GEMGRD $COMOUT/$GEMGRD
       if [ $SENDDBN = "YES" ] ; then
          $DBNROOT/bin/dbn_alert MODEL ${DBN_ALERT_TYPE} $job \
          $COMOUT/$GEMGRD
       else
         echo "##### DBN_ALERT_TYPE is: ${DBN_ALERT_TYPE} #####"
       fi
    fi
  
    let fhcnt=fhcnt+finc
  done
}

for product in ice ice_5min; do 
    entry=`grep "^${product} " $NAGRIB_TABLE | awk 'index($1,"#") != 1 {print $0}'`
    RUN=$(echo $entry|sed 's/ //g'|awk -F"|" '{print $1}')
    create_gempak
done


#####################################################################
# GOOD RUN
set +x
echo "**************JOB $RUN NAWIPS COMPLETED NORMALLY ON THE IBM"
echo "**************JOB $RUN NAWIPS COMPLETED NORMALLY ON THE IBM"
echo "**************JOB $RUN NAWIPS COMPLETED NORMALLY ON THE IBM"
set -x
#####################################################################

msg='Job completed normally.'
echo $msg
postmsg "$msg"

############################### END OF SCRIPT #######################
