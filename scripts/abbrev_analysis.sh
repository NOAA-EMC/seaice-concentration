#!/bin/ksh
#####################################################################
echo "------------------------------------------------"
echo "This job takes into account of the SSMI BUFR data creates ICE"
echo "Concentration Fields for the Arctic Ocean and Southern"
echo "Continent and adjoining water bodies."
echo "------------------------------------------------"
echo "History:  AUG 1997 - First implementation of this new script."
echo "          NOV 1997 - Add copy to /dcom and /scom for   "
echo "                       reliability and users           " 
echo "          Jan 1998 - Add transmission to OSO of files in   "
echo "                       WMO format                          "
echo "          Sep 1998 - Rename WMO files so that they'll actually transmit"
echo "                   - Y2K changes"
echo "          Oct 2004 - Convert to CCS Phase II "
#####################################################################
# Robert Grumbine 1 March 1995.
#  1 March 1995 - Script for handling the production and 
#  dissemination of all ssmi-derived sea ice concentration files
#
#  28 November 1995 - Modified for producing reanalysis files.
#
#  23 January 1996 - Heavily modified to try to circumvent 
#  missing/overloaded file systems on operational cray, and to be 
#  more robust against steps failing internal to the script.
#
#  2 June 1997. Modified to work from BUFR files.  Relies on 
#  dumpscript, with no failover protection possible.
#
#  6 August 1997 - Script version for operations use.
#
#  3 February 2000 - Conversion to IBM SP.  New file management (ln)
#    and argument changes for seaissmi and filtanal
#
# 13 April 2001 - Update land mask usage
#
#  7 October 2003 - Add generation of graphics for web
#
# 20 May 2004     - Update to new, higher resolution, ice analysis
#
#  1 May 2009     - Add AMSRE decoding and processing
# 19 May 2009     - Remove legacy exsaicmrf.sh from end of this script.
#
# 30 March 2012   - Add SSMI-S processing, convert to vertical structure
# 30 March 2012   - Direct production of grib2 files
#
# 16 August 2017  - Add ASMR2
########################################################################

set -x

### Definition of the Fix fields:
export FNLAND=${FNLAND:-${FIXseaice_analysis}/seaice_nland.map}
export FNLAND127=${FNLAND127:-${FIXseaice_analysis}/seaice_nland127.map}
export FSLAND=${FSLAND:-${FIXseaice_analysis}/seaice_sland.map}
export FSLAND127=${FSLAND127:-${FIXseaice_analysis}/seaice_sland127.map}
export FGLAND5MIN=${FGLAND5MIN:-${FIXseaice_analysis}/seaice_gland5min}
export FPOSTERIORI5=${FPOSTERIORI5:-${FIXseaice_analysis}/seaice_posteriori_5min}
export FPOSTERIORI30=${FPOSTERIORI30:-${FIXseaice_analysis}/seaice_posteriori_30min}
export FDIST=${FDIST:-${FIXseaice_analysis}/seaice_alldist.bin}
export FGSHHS=${FGSHHS:-${FIXseaice_analysis}/seaice_lake_isleout}

cd $DATA

##############################
#
# START FLOW OF CONTROL
#
# 1) Get the Date from /com/date
#    -- Now done in J-job, not here.
# 2) Run the dumpscript to retrieve a day's data
# 3) Process the bufr output into something readable
# 4) Run the analysis on the files
# -- 2,3,4 are repeated for each satellite being used
# 5) Copy the base analyst's (L3) grids to the running location
# 6) Make up the grib files for the polar stereographic data and
#    copy them to running locations
# 7) Construct the modeller's (L4) global sea ice grid
#
##############################


########################################
msg="HAS BEGUN!"
postmsg "$msg"
########################################

mailbody=mailbody.txt
rm -f $mailbody

#----------------------------------------------------------
# noice, imsice -- for disaster preparedness
#   noice  --> make field for case there is no ice information at all
#   imsice --> make field using ims analysis
#   noice  produces noice.$PDY
#   imsice produces imsice.$PDY
if [ ! -f $COMOUT/noice.$PDY -o ! -f $COMOUT/imsice.$PDY ] ; then
  $USHseaice_analysis/noice.sh
  $USHseaice_analysis/imsice.sh
  cp noice.$PDY  $COMOUT/noice.$PDY
  cp imsice.$PDY $COMOUT/imsice.$PDY
else
  cp $COMOUT/imsice.$PDY imsice.$PDY
  cp $COMOUT/noice.$PDY noice.$PDY
fi

#----------------------------------------------------------
#Begin ordinary processing.
#---------------------------------------------------------
#Dummy variables at this point, formerly used and may be useful
# again in the future.  Robert Grumbine 6 August 1997.
jday=1
refyear=2022

#BUFR------------------------------------------------------
#Run the dumpscript to retrieve a day's data
#----------------------------------------------------------
#Cactus:
if [ -z $obsproc_dump_ver ] ; then
  echo null obsproc_dump_ver
  export obsproc_dump_ver=v4.0.0
  export obsproc_shared_bufr_dumplist_ver=v1.4.0
fi

if [ ! -f $COMOUT/ssmisu.ibm ] ; then
  export pgm=dumpjb2
  . prep_step
  #$USHobsproc_dump/dumpjb ${PDY}00 12 ssmisu
  time $DUMPJB ${PDY}00 12 ssmisu
  export err=$?
  if [ $err -ne 0 ] ; then
    msg="WARNING:  Continuing without ssmiu data"
    postmsg "$msg"
    echo "********************************************************************" >> $mailbody
    echo "*** WARNING:  dumpjb returned status $err.  Continue without ssmisu." >> $mailbody
    echo "********************************************************************" >> $mailbody
  fi
  touch ssmisu.ibm # ensures that file exists in rare event that dump is empty
  cp ssmisu.ibm $COMOUT
else
  cp $COMOUT/ssmisu.ibm .
fi

if [ ! -f $COMOUT/amsr2.ibm ] ; then
  export pgm=dumpjb3
  . prep_step
  #$USHobsproc_dump/dumpjb ${PDY}00 12 amsr2
  time $DUMPJB ${PDY}00 12 amsr2
  export err=$?
  if [ $err -ne 0 ] ; then
    msg="WARNING:  Continuing without amsr2"
    postmsg "$msg"
    echo "********************************************************************" >> $mailbody
    echo "*** WARNING:  dumpjb returned status $err.  Continue without amsr2. " >> $mailbody
    echo "********************************************************************" >> $mailbody
  fi
  touch amsr2.ibm # ensures that file exists in rare event that dump is empty
  cp amsr2.ibm $COMOUT
else
  cp $COMOUT/amsr2.ibm amsr2.ibm
fi

# send email if any dumpjb calls returned non-zero status
if [ -s "$mailbody" ] && [ "$SENDEMAIL" = "YES" ]; then
   subject="$job degraded due to missing data"
   mail.py -s "$subject" < $mailbody
fi

#--------------------------------------------------------------------
#Process the bufr output into something readable: bufr --> l1b
#--------------------------------------------------------------------

##################Decode SSMI-S data
if [ ! -f $COMOUT/ssmisu.bufr ] ; then
  export pgm=seaice_ssmisubufr
  . prep_step
  
  export XLFRTEOPTS="unit_vars=yes"
  ln -sf ssmisu.ibm fort.11
  
  startmsg
  time $EXECseaice_analysis/seaice_ssmisubufr >> $pgmout 2> errfile
  mv fort.51 ssmisu.bufr
  export err=$?
  if [ $err -ne 0 ] ; then
    msg="WARNING:  Continuing without ssmisubufr"
    postmsg "$msg"
    echo "********************************************************************" >> $mailbody
    echo "*** WARNING:  ssmisubufr returned status $err.  Continue without ssmisu " >> $mailbody
    echo "********************************************************************" >> $mailbody
  else
    cp ssmisu.bufr $COMOUT/
  fi
else
  cp $COMOUT/ssmisu.bufr .
fi

##################Decode AMSR2 data
if [ ! -f $COMOUT/amsr2.bufr ] ; then
  export pgm=seaice_amsrbufr
  . prep_step
  
  export XLFRTEOPTS="unit_vars=yes"
  ln -sf amsr2.ibm fort.11
  if [ -f fort.52 ] ; then
    rm fort.52
  fi

  startmsg
  time $EXECseaice_analysis/seaice_amsrbufr >> $pgmout 2> errfile
  mv fort.52 amsr2.bufr
  export err=$?;err_chk
  if [ $err -ne 0 ] ; then
    msg="WARNING:  Continuing without amsr2bufr"
    postmsg "$msg"
    echo "********************************************************************" >> $mailbody
    echo "*** WARNING:  amsr2bufr returned status $err.  Continue without amsr2 " >> $mailbody
    echo "********************************************************************" >> $mailbody
  fi
  touch amsr2.bufr 
  cp amsr2.bufr $COMOUT/amsr2.bufr
else
  cp $COMOUT/amsr2.bufr .
fi

#---------------------------------------------------------------------
# Translate from l1b to l2 in ssmis
# AMSR2 remains straight shot l1b to l3
# Robert Grumbine 9 April 2021
#---------------------------------------------------------------------

#----------------------------------------------------------
# Run the analysis on the SSMI-S datafiles
# Input files =  delta, $FIXseaice_analysis/seaice_nland127.map, $FIXseaice_analysis/seaice_sland127.map
# Output files = n3ssmi.$PDY, s3ssmi.$PDY, ssmisnorth.$PDY, ssmissouth.$PDY
# Arguments (currently not used) - $jday : Julian Day,
#                                  $refyear : 4 digit year
#           (used)                 285      : satellite number (285 = F17)
# 249 = F16, 286 = F18
#----------------------------------------------------------

#Process SSMI data to analyst (L3) grids

cp $FIXseaice_analysis/seaice_TBthark.tab.ssmisu .
cp $FIXseaice_analysis/seaice_TBowark.tab.ssmisu .
cp $FIXseaice_analysis/seaice_TBowant.tab.ssmisu .
cp $FIXseaice_analysis/seaice_TBfyark.tab.ssmisu .
cp $FIXseaice_analysis/seaice_TBfyant.tab.ssmisu .
cp $FIXseaice_analysis/seaice_TBccark.tab.ssmisu .
cp $FIXseaice_analysis/seaice_TBccant.tab.ssmisu .
echo ssmisu.bufr > delta

#produces L2 files l2out.f285.51.nc, l2out.f286.52.nc
if [ ! -f $COMOUT/l2out.f285.51.nc -o ! -f $COMOUT/l2out.f286.52.nc ] ; then
  export pgm=ssmisu_tol2
  . prep_step
  startmsg
  ln -sf ssmisu.ibm fort.11
  time $EXECseaice_analysis/ssmisu_tol2 >> $pgmout 2> errfile 
  export err=$?;err_chk
  touch l2out.f285.51.nc l2out.f286.52.nc
  cp -p l2out.f285.51.nc $COMOUT/l2out.f285.51.nc
  cp -p l2out.f286.52.nc $COMOUT/l2out.f286.52.nc
else
  cp -p  $COMOUT/l2out.f285.51.nc .
  cp -p  $COMOUT/l2out.f286.52.nc .
fi

# Produce the L3 ssmi-s analysis -- [ns]map.$PDY.f1[78]
export pgm=ssmisu_tol3_f285
. prep_step
startmsg
time $EXECseaice_analysis/ssmisu_tol3 l2out.f285.51.nc nmap.${PDY}.f17 smap.${PDY}.f17 >> $pgmout 2> errfile
export err=$?;err_chk

export pgm=ssmisu_tol3_f286
. prep_step
startmsg
time $EXECseaice_analysis/ssmisu_tol3 l2out.f286.52.nc nmap.${PDY}.f18 smap.${PDY}.f18 >> $pgmout 2> errfile
export err=$?;err_chk


#----------------------------------------------------------
# Run the L1b --> L3 analysis on the AMSR files
# Input files =  amsr.bufr, $FIXseaice_analysis/seaice_nland127.map, $FIXseaice_analysis/seaice_sland127.map
# Output files = n3amsr.$PDY, s3amsr.$PDY, amsr2north.$PDY, amsr2south.$PDY
#New 1 May 2009 -- amsre
#New 16 Aug 2017 -- amsr2
#----------------------------------------------------------
#
cp $FIXseaice_analysis/seaice_TBthark.tab.amsr2 .
cp $FIXseaice_analysis/seaice_TBthant.tab.amsr2 .
cp $FIXseaice_analysis/seaice_TBowark.tab.amsr2 .
cp $FIXseaice_analysis/seaice_TBowant.tab.amsr2 .
cp $FIXseaice_analysis/seaice_TBfyark.tab.amsr2 .
cp $FIXseaice_analysis/seaice_TBfyant.tab.amsr2 .
cp $FIXseaice_analysis/seaice_TBccark.tab.amsr2 .
cp $FIXseaice_analysis/seaice_TBccant.tab.amsr2 .

export pgm=seaice_iceamsr2
. prep_step
startmsg
time $EXECseaice_analysis/seaice_iceamsr2 amsr2.bufr $FNLAND127 $FSLAND127 \
           namsr2.$PDY samsr2.$PDY amsr2north6.$PDY amsr2south6.$PDY \
           $FGSHHS $FDIST >> $pgmout 2>errfile
export err=$?;err_chk

# RG: Save point for initial L2, L3 analyses:
# if sendcom ...
#-----------------------------------------------------------
#Copy the base L2 and L3 grids to the output location
#-----------------------------------------------------------

if [ $SENDCOM = "YES" ]
then
  # Raw files -- L3 -- on per-instrument basis
  #L2 ice files -- conc + Tb
  cp l2out.* ${COMOUT}
    
  #L3 with tb etc.
  cp namsr2.${PDY}_hr ${COMOUT}/seaice.t${cyc}z.namsr2.${PDY}_hr
  cp namsr2.${PDY}_lr ${COMOUT}/seaice.t${cyc}z.namsr2.${PDY}_lr
  cp samsr2.${PDY}_hr ${COMOUT}/seaice.t${cyc}z.samsr2.${PDY}_hr
  cp samsr2.${PDY}_lr ${COMOUT}/seaice.t${cyc}z.samsr2.${PDY}_lr
  #L3 ice conc only
  cp amsr2north6.${PDY}   ${COMOUT}/seaice.t${cyc}z.amsr2north6.${PDY}
  cp amsr2south6.${PDY}   ${COMOUT}/seaice.t${cyc}z.amsr2south6.${PDY}
  cp nmap.${PDY}.f1[78] ${COMOUT}
  cp smap.${PDY}.f1[78] ${COMOUT}
fi


#Begin L4-oriented processing -----
#---------------------------------------------------------
# Blend the AMSRE, SSMI, SSMI-S ice concentration analyses
#  -- supercollated L3
# 
# 27 July 2011: Modification, change umask to init in output names
#               the posteriori filter will produce the umask files
# 30 March 2012: use names directly (above), add ssmis
#RG add amsr2 to blend 2017
#---------------------------------------------------------

#debug: exit 1

export pgm=seaice_blend
startmsg
$EXECseaice_analysis/seaice_blend \
  amsr2north6.$PDY nmap.${PDY}.f17 nmap.${PDY}.f18  initnorth12.$PDY \
  amsr2south6.$PDY smap.${PDY}.f17 smap.${PDY}.f18  initsouth12.$PDY \
  $FNLAND127 $FSLAND127 noice.$PDY imsice.$PDY >> $pgmout 2> errfile
export err=$?;err_chk

#----------- End of Blending process ---------------------

#----------- Do the a posteriori filtering -- no dependance on sst field ---
# New 27 July 2011
#
cp $FPOSTERIORI5  . 
cp $FPOSTERIORI30 . 
export pgm=seaice_posteriori_5min.x
startmsg
$EXECseaice_analysis/seaice_posteriori_5min.x seaice_posteriori_5min \
      initnorth12.$PDY initsouth12.$PDY \
      umasknorth12.$PDY umasksouth12.$PDY
export err=$?;err_chk

#-----------------------------------------------------------
#Produce the old resolution files
#-----------------------------------------------------------
export pgm=seaice_north_reduce
startmsg
$EXECseaice_analysis/seaice_north_reduce umasknorth12.$PDY umasknorth.$PDY \
          $FNLAND127 $FNLAND >> $pgmout 2>errfile
export err=$?;err_chk

export pgm=seaice_south_reduce
startmsg
$EXECseaice_analysis/seaice_south_reduce umasksouth12.$PDY umasksouth.$PDY \
          $FSLAND127 $FSLAND >> $pgmout 2>errfile
export err=$?;err_chk

#-----------------------------------------------------------
#Construct the summary graphics for display
#-----------------------------------------------------------
export pgm=seaice_north12xpm
startmsg
$EXECseaice_analysis/seaice_north12xpm umasknorth12.$PDY $FNLAND127 nh12.$PDY.xpm \
			    >> $pgmout 2>errfile
export err=$?;err_chk
convert nh12.$PDY.xpm nh12.$PDY.gif

export pgm=seaice_south12xpm
startmsg
$EXECseaice_analysis/seaice_south12xpm umasksouth12.$PDY $FSLAND127 sh12.$PDY.xpm \
			    >> $pgmout 2>errfile
export err=$?;err_chk
convert sh12.$PDY.xpm sh12.$PDY.gif

export pgm=seaice_northxpm
startmsg
$EXECseaice_analysis/seaice_northxpm umasknorth.$PDY $FNLAND nh.$PDY.xpm \
			    >> $pgmout 2>errfile
export err=$?;err_chk
convert nh.$PDY.xpm nh.$PDY.gif

export pgm=seaice_southxpm
startmsg
$EXECseaice_analysis/seaice_southxpm umasksouth.$PDY $FSLAND sh.$PDY.xpm \
			    >> $pgmout 2>errfile
export err=$?;err_chk
convert sh.$PDY.xpm sh.$PDY.gif


# -- ! Moved up
#-----------------------------------------------------------
#Copy the base L2 and L3 grids to the output location
#-----------------------------------------------------------
#if [ $SENDCOM = "YES" ]
#then
#  # Raw files -- L3 -- on per-instrument basis
#  #L2 ice files -- conc + Tb
#  cp l2out.* ${COMOUT}
#
#  #L3 with tb etc.
#  cp namsr2.${PDY}_hr ${COMOUT}/seaice.t${cyc}z.namsr2.${PDY}_hr
#  cp namsr2.${PDY}_lr ${COMOUT}/seaice.t${cyc}z.namsr2.${PDY}_lr
#  cp samsr2.${PDY}_hr ${COMOUT}/seaice.t${cyc}z.samsr2.${PDY}_hr
#  cp samsr2.${PDY}_lr ${COMOUT}/seaice.t${cyc}z.samsr2.${PDY}_lr
#  #L3 ice conc only
#  cp amsr2north6.${PDY}   ${COMOUT}/seaice.t${cyc}z.amsr2north6.${PDY}
#  cp amsr2south6.${PDY}   ${COMOUT}/seaice.t${cyc}z.amsr2south6.${PDY}
#  cp nmap.${PDY}.f1[78] ${COMOUT}
#  cp smap.${PDY}.f1[78] ${COMOUT}
#fi

#-----------------------------------------------------------
#Copy the collated L3 grids to the output location
#-----------------------------------------------------------
if [ $SENDCOM = "YES" ]
then
  #Summary files 
  cp umasknorth12.$PDY  ${COMOUT}/seaice.t${cyc}z.umasknorth12
  cp umasksouth12.$PDY  ${COMOUT}/seaice.t${cyc}z.umasksouth12
  cp umasknorth.$PDY  ${COMOUT}/seaice.t${cyc}z.umasknorth
  cp umasksouth.$PDY  ${COMOUT}/seaice.t${cyc}z.umasksouth
  cp nh12.$PDY.gif    ${COMOUT}/seaice.t${cyc}z.nh12.gif
  cp sh12.$PDY.gif    ${COMOUT}/seaice.t${cyc}z.sh12.gif
  cp nh.$PDY.gif      ${COMOUT}/seaice.t${cyc}z.nh.gif
  cp sh.$PDY.gif      ${COMOUT}/seaice.t${cyc}z.sh.gif
  if [ $SENDDBN = "YES" ]
  then
    $DBNROOT/bin/dbn_alert MODEL OMBICE $job ${COMOUT}/seaice.t${cyc}z.nh12.gif
    $DBNROOT/bin/dbn_alert MODEL OMBICE $job ${COMOUT}/seaice.t${cyc}z.sh12.gif
    $DBNROOT/bin/dbn_alert MODEL OMBICE $job ${COMOUT}/seaice.t${cyc}z.nh.gif
    $DBNROOT/bin/dbn_alert MODEL OMBICE $job ${COMOUT}/seaice.t${cyc}z.sh.gif
  fi
fi

#-----------------------------------------------------------
# Make up the grib files for the polar stereographic data and
# copy them to running locations
#-----------------------------------------------------------
#For grib1:
# Get the century from /com/date's file
echo $PDY | cut -c1-2 > psin
echo $PDY | cut -c3-4  >> psin
echo $PDY | cut -c5-6  >> psin
echo $PDY | cut -c7-8  >> psin
cat psin

#For Grib2:
echo 'DATE  '$PDY'00' > idate
cp $PARMseaice_analysis/seaice_pds .

for pole in north12 south12 north south
do

  #Grib1 and WMO File construction
  export pgm=seaice_psg$pole
  . prep_step

  rm fort.*
  ln -sf umask${pole}.$PDY        fort.11
  ln -sf ${pole}psg.$PDY          fort.51
  ln -sf wmo${pole}psg.${PDY}.grb fort.52

  startmsg
  $EXECseaice_analysis/seaice_psg${pole} < psin >> $pgmout 2>errfile
  export err=$?;err_chk
  
done

for pole in north12 south12 north south
do

  #Grib2 construction:
  export pgm=seaice_grib2
  . prep_step

  export XLFRTEOPTS="unit_vars=yes"
  ln -sf idate fort.4
  ln -sf umask$pole.${PDY} fort.10
  ln -sf seaice.t${cyc}z.${pole}psg.grib2 fort.20
  ln -sf $PARMseaice_analysis/gds.umask$pole fort.12
  startmsg
  $EXECseaice_analysis/seaice_grib2 >> $pgmout 2> errfile
  export err=$?;err_chk

done


if [ $SENDCOM = "YES" ]
then
  cp north12psg.${PDY} ${COMOUT}/seaice.t${cyc}z.north12psg
  cp south12psg.${PDY} ${COMOUT}/seaice.t${cyc}z.south12psg
  cp northpsg.${PDY} ${COMOUT}/seaice.t${cyc}z.northpsg
  cp southpsg.${PDY} ${COMOUT}/seaice.t${cyc}z.southpsg
  cp seaice.t${cyc}z.northpsg.grib2 ${COMOUT}
  cp seaice.t${cyc}z.southpsg.grib2 ${COMOUT}
  cp seaice.t${cyc}z.north12psg.grib2 ${COMOUT}
  cp seaice.t${cyc}z.south12psg.grib2 ${COMOUT}


  ##########################
  # Conver to grib2 format
  ##########################
  for fil in north12psg south12psg northpsg southpsg
  do
     $WGRIB2 ${COMOUT}/seaice.t${cyc}z.${fil}.grib2 -s > ${COMOUT}/seaice.t${cyc}z.${fil}.grib2.idx
  done

  if [ $SENDDBN = "YES" ]
  then
    for fil in north12psg south12psg northpsg southpsg
    do
      $DBNROOT/bin/dbn_alert MODEL OMBICE $job ${COMOUT}/seaice.t${cyc}z.${fil}

      if [ $SENDDBN = YES ]
      then

      $DBNROOT/bin/dbn_alert MODEL OMBICE_GB2 $job ${COMOUT}/seaice.t${cyc}z.${fil}.grib2
      $DBNROOT/bin/dbn_alert MODEL OMBICE_GB2_WIDX $job ${COMOUT}/seaice.t${cyc}z.${fil}.grib2.idx

      fi

    done
  fi
fi

if [ $SENDCOM = "YES" ]
then
  cp wmonorthpsg.${PDY}.grb ${COMOUTwmo}/seaice.t${cyc}z.wmonorthpsg.seaice_analysis_00
  cp wmosouthpsg.${PDY}.grb ${COMOUTwmo}/seaice.t${cyc}z.wmosouthpsg.seaice_analysis_00
  if [ $SENDDBN = YES ]
  then
    $DBNROOT/bin/dbn_alert GRIB_LOW seaice_analysis $job ${COMOUTwmo}/seaice.t${cyc}z.wmonorthpsg.seaice_analysis_00
    $DBNROOT/bin/dbn_alert GRIB_LOW seaice_analysis $job ${COMOUTwmo}/seaice.t${cyc}z.wmosouthpsg.seaice_analysis_00
  fi
fi

echo zzzzz completed the L2 and L3 analyses zzzzzzzzzzzzzzzz
