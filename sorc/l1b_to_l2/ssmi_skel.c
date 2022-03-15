#include <stdio.h>
#include "netcdf.h"

/* For C++ emulation from C */
typedef int bool;
#define false (1==0)
#define true (1==1)

#include "ssmi2.h"

/* Items for the concentration algorithm: */
#define SSMI_ANTENNA 0
#define SSMI_GR37LIM 0.05
#define SSMI_GR22LIM 0.045
#define BAD_DATA 166
#define WEATHER  177
extern float nasa_team(float t19v, float f19h, float t22v, float t37v, float t37h,
                float t85v, float t85h, const char pole, const int ant,
                const int satno);
#define NFREQS 7
#define LAND 157
#define MIXED 195



int l1b_to_l2(float *tb, int satno, float clat, float clon, float *concentration, int *qc, float *land) ;

extern int tb_filter(float *tb, float *concentration, int *flag, int satno) ;
extern int waters(float tb1, float tb2, float crit, bool under, float *concentration, int *qc, float *land) ;
extern int lands (float tb1, float tb2, float crit, bool under, float *concentration, int *qc, float *land) ;

/* For the algorithm */
int bad_low = 0;
int bad_high = 0;
int crop_low = 0;
int filt37   = 0;
int filt22   = 0;

/* New version 22 August 2018 --
  Write out netcdf
*/
#define NDIMS 1
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); fflush(stdout); }

/* ////////////////  For intercepting binary and writing out flat /////////// */
#ifdef BINOUT
FILE *flatout;
#endif

#ifdef IBM
int close_nc(int *ncid) {
#elif LINUX
int close_nc_(int *ncid) {
#else
int close_nc_(int *ncid) {
#endif
  return nc_close(*ncid);
}

#ifdef IBM
int open_nc(int *unit, int *ncid, int *platform, int *varid) {
#elif LINUX
int open_nc_(int *unit, int *ncid, int *platform, int *varid) {
#else
int open_nc_(int *unit, int *ncid, int *platform, int *varid) {
#endif
  char fname[24];
  int rec_dimid, dimids[NDIMS];
  float freqs[NFREQS] = { 19.35, 19.35, 22.235, 37.0, 37.0, 85.5, 85.5};
  int polar[NFREQS] = {0, 1, 1, 0, 1, 1, 0};
  int nfreqs = NFREQS;

  int varid_tb19v, varid_tb19h, varid_tb22v, varid_tb37v, varid_tb37h;
  int varid_tb85v, varid_tb85h;
  int varid_lat, varid_lon, varid_sic, varid_qc, varid_land, varid_dtg1, varid_dtg2;
  int varid_satid;
  int retcode;

  #ifdef BINOUT
  flatout = fopen("flatout","w");
  #endif

  sprintf(fname,"l2out.f%03d.%02d.nc",*platform, *unit);

  nc_create(fname, NC_CLOBBER, ncid);
  nc_def_dim(*ncid, "nobs", NC_UNLIMITED, &rec_dimid);
  dimids[0] = rec_dimid;
/*  header information (past nobs) */
   nc_put_att_int(*ncid, NC_GLOBAL, "platform_id", NC_INT, 1, platform);
   nc_put_att_int(*ncid, NC_GLOBAL, "n_frequencies", NC_INT, 1, &nfreqs);
   nc_put_att_int(*ncid, NC_GLOBAL, "polarizations", NC_INT, nfreqs, &polar[0]);
   nc_put_att_float(*ncid, NC_GLOBAL, "frequencies", NC_FLOAT, nfreqs, &freqs[0]);
/* info for each obs: */
  nc_def_var(*ncid, "satid", NC_INT, NDIMS, dimids, &varid_satid);
  nc_def_var(*ncid, "longitude", NC_DOUBLE, NDIMS, dimids, &varid_lon);
  nc_def_var(*ncid, "latitude", NC_DOUBLE, NDIMS, dimids, &varid_lat);
  nc_def_var(*ncid, "ice_concentration", NC_FLOAT, NDIMS, dimids, &varid_sic);
  nc_def_var(*ncid, "quality", NC_INT, NDIMS, dimids, &varid_qc);
  nc_def_var(*ncid, "land_flag", NC_FLOAT, NDIMS, dimids, &varid_land);
  nc_def_var(*ncid, "dtg_yyyymmdd", NC_INT, NDIMS, dimids, &varid_dtg1);
  nc_def_var(*ncid, "dtg_hhmm", NC_INT, NDIMS, dimids, &varid_dtg2);
  nc_def_var(*ncid, "tb_19V", NC_FLOAT, NDIMS, dimids, &varid_tb19v);
  nc_def_var(*ncid, "tb_19H", NC_FLOAT, NDIMS, dimids, &varid_tb19h);
  nc_def_var(*ncid, "tb_22V", NC_FLOAT, NDIMS, dimids, &varid_tb22v);
  nc_def_var(*ncid, "tb_37V", NC_FLOAT, NDIMS, dimids, &varid_tb37v);
  nc_def_var(*ncid, "tb_37H", NC_FLOAT, NDIMS, dimids, &varid_tb37h);
  nc_def_var(*ncid, "tb_85V", NC_FLOAT, NDIMS, dimids, &varid_tb85v);
  nc_def_var(*ncid, "tb_85H", NC_FLOAT, NDIMS, dimids, &varid_tb85h);
/* exit definition section */

  nc_enddef(*ncid);
  varid[0] = varid_lon;
  varid[1] = varid_lat;
  varid[2] = varid_sic;
  varid[3] = varid_qc;
  varid[4] = varid_land;
  varid[5] = varid_dtg1;
  varid[6] = varid_tb19v;
  varid[7] = varid_tb19h;
  varid[8] = varid_tb22v;
  varid[9] = varid_tb37v;
  varid[10] = varid_tb37h;
  varid[11] = varid_tb85v;
  varid[12] = varid_tb85h;
  varid[13] = varid_dtg2;
  varid[14] = varid_satid;

  return 0;
}



/* ssmiout */
#ifdef IBM
void ssmiout_nc
#elif LINUX
void ssmiout_nc_
#else
void ssmiout_nc_
#endif
(int *satno, int *iy, int *im, int *id, int *ihr, int *imins, int *isec, int *iscan,
    int *kscan, float *xlat, float *xlon, int *sftg, int *posn, 
    float *tmp1, float *tmp2, float *tmp3, float *tmp4, float *tmp5, float *tmp6, float *tmp7,
    int *ncid, int *varid, int *nwrites) {

  ssmipt x;
  int bad = 0, k, j;
  int dtg1, dtg2;
  float conc, land, tb[NFREQS];
  int nobs, qc, nfreqs = NFREQS;

  x.year   = (short int)     *iy;
  x.month  = (unsigned char) *im;
  x.day    = (unsigned char) *id;
  x.hour   = (unsigned char) *ihr;
  x.minute = (unsigned char) *imins;
  x.second = (unsigned char) *isec;
  x.satid = (short int) *satno;

  x.clat = *xlat;
  x.clon = *xlon;

  if (x.clat > 90 || x.clon > 360 || x.clon < -360) bad += 1;
  k = 0;
  x.tb[0] = *tmp1;
  x.tb[1] = *tmp2;
  x.tb[2] = *tmp3;
  x.tb[3] = *tmp4;
  x.tb[4] = *tmp5;
  x.tb[5] = *tmp6;
  x.tb[6] = *tmp7;
  for (j = 0; j < 7; j++) {
      if (x.tb[j] > 350) bad += 1;
      tb[j] = x.tb[j];
  }
  /* printf("%d %d %f %f  %f %f\n", x.satid, x.year, x.clat, x.clon, x.tb[0], x.tb[6]);
  fflush(stdout);
  */
 

  size_t start[NDIMS]; start[0] = (size_t) *nwrites;
  size_t count[NDIMS]; count[0] = 1;
  int nerr = 0;
  land = 0.0;

  if (bad == 0 && (x.clat > 24.0 || x.clat < -30.0) ) {
/* Can call the algorithm from here -- thence to write out netcdf */
    nerr = 0;
    nerr = process_bufr(&x);
    if (nerr == 0) {
  
/* could be filtering land, water here, then let l1b_to_l2 do concentration/weather filtering */
/* RG: tmp     nobs = l1b_to_l2(&x, &conc, &qc, &land);
    conc = 1.0;
    qc = 5;
    land = 0.0; 
 */
      nobs = l1b_to_l2(&tb[0], *satno, *xlat, *xlon, &conc, &qc, &land);

      dtg1  = x.year;
      dtg1 *= 100; dtg1 += x.month;
      dtg1 *= 100; dtg1 += x.day;

      dtg2 = x.hour;
      dtg2 *= 100; dtg2 += x.minute;

      nc_put_vara_int  (*ncid, varid[14], start, count, satno);
      nc_put_vara_double(*ncid, varid[0], start, count, &x.clon);
      nc_put_vara_double(*ncid, varid[1], start, count, &x.clat);
      nc_put_vara_float(*ncid, varid[2], start, count, &conc);
      nc_put_vara_int  (*ncid, varid[3], start, count, &qc);
      nc_put_vara_float(*ncid, varid[4], start, count, &land);
      nc_put_vara_int (*ncid, varid[5], start, count, &dtg1);
      nc_put_vara_float(*ncid, varid[6], start, count, &tb[0]);
      nc_put_vara_float(*ncid, varid[7], start, count, &tb[1]);
      nc_put_vara_float(*ncid, varid[8], start, count, &tb[2]);
      nc_put_vara_float(*ncid, varid[9], start, count, &tb[3]);
      nc_put_vara_float(*ncid, varid[10], start, count, &tb[4]);
      nc_put_vara_float(*ncid, varid[11], start, count, &tb[5]);
      nc_put_vara_float(*ncid, varid[12], start, count, &tb[6]);
      nc_put_vara_int (*ncid, varid[13], start, count, &dtg2);

/* For flat binary: */
      #ifdef BINOUT
      fwrite(&x.clon, sizeof(x.clon), 1, flatout);
      fwrite(&x.clat, sizeof(x.clat), 1, flatout);
      fwrite(&conc  , sizeof(conc)  , 1, flatout);
      fwrite(&qc    , sizeof(qc)    , 1, flatout);
      fwrite(&land  , sizeof(land)  , 1, flatout);
      fwrite(&tb[0] , sizeof(float) , nfreqs, flatout);
      #endif
/* /////////////////////////////////////////////////////////////////////// */

      *nwrites += 1;
    }
    /* else {
    debug  printf("nerr = %d\n",nerr);
    } */
  }

 return;
}
int l1b_to_l2(float *tb, int satno, float clat, float clon, float *concentration, int *qc, float *land) {
/*
  float tb[NFREQS];
  tb[0] = (float)a->tb[0];
  tb[1] = (float)a->tb[1];
  tb[2] = (float)a->tb[2];
  tb[3] = (float)a->tb[3];
  tb[4] = (float)a->tb[4];
  tb[5] = (float)a->tb[5];
  tb[6] = (float)a->tb[6];
*/

  *concentration = 1.0;
  *qc = 5;      /* 1-5, 5 is worst */
  *land = 1./4. + 1./16.; /* want this to be about 0.30, but do this for exactness */
  int i, j, flag = 0, sum = 0;

  #ifdef DEBUG
  printf("debug args1 %d %f %f %f %d %f\n",satno, clat, clon, *concentration, *qc, *land);
  fflush(stdout);
  printf("debug args2 %f %f\n",tb[0], tb[1]);
  fflush(stdout);
  #endif

  #ifdef DEBUG
  printf("debug_pre land, qc, conc = %f %d %f  %f\n",*land, *qc, *concentration, tb[0]);
  #endif

/* tb filter: */
   tb_filter(tb, concentration, &flag, satno);
   if (flag == LAND) {
     *land = 0.99;
     *qc   = 3;
     sum += 1;
   }
  #ifdef DEBUG
  printf("debug_h land, qc, conc = %f %d %f  %f\n",*land, *qc, *concentration, tb[0]);
  #endif

/* for delta filtering: */
   float crit;
   bool under;

/* Water filters:
*/
  if (satno == 15 || satno == 248) {
     /* Rebuild for F15, different for F13, F14: */
     /* 19v, 85v, <  -0.090792151111545
        19h, 85h,  <  -0.1756696439150608
        Early 2000's version
     i = 0; j = 5; under = true; crit =  -0.090792151111545;
     sum +=  waters(tb[i], tb[j], crit, under, concentration, qc, land);
     i = 0; j = 6; under = true; crit = -0.1756696439150608; 
     sum +=  waters(tb[i], tb[j], crit, under, concentration, qc, land);
     */
     /* Rebuild for F15, different for F13, F14: 
        (t37v, t85v) < -0.08223462165004075)
        (t37h, t85h) < -0.18216759628719753)
        (t37v, t85h) < -0.05941281460150324)
      8 April 2021
     */
     i = 3; j = 5; crit = -0.08223462165004075; under = true; 
     sum += waters(tb[i], tb[j], crit, under, concentration, qc, land);
     i = 4; j = 6; crit = -0.18216759628719753; under = true;
     sum += waters(tb[i], tb[j], crit, under, concentration, qc, land);
     i = 3; j = 6; crit = -0.05941281460150324; under = true;
     sum += waters(tb[i], tb[j], crit, under, concentration, qc, land);
  }
  else {
     /* F13, F14: */
/* 37v, 85v, <  -0.08223462165004075
   19v, 22v, <  -0.045881619056065914
   37h, 85h, <  -0.17385640469464386
   19v, 85v, <  -0.13226512947467844
   19v, 37v, <  -0.05464559974092431
*/
    under = true;
    i = 3; j = 5; crit = -0.08223462165004075;
     sum +=  waters(tb[i], tb[j], crit, under, concentration, qc, land);
    i = 0; j = 2; crit = -0.045881619056065914;
     sum +=  waters(tb[i], tb[j], crit, under, concentration, qc, land);
    i = 4; j = 6; crit = -0.17385640469464386;
     sum +=  waters(tb[i], tb[j], crit, under, concentration, qc, land);
    i = 0; j = 5; crit = -0.13226512947467844;
     sum +=  waters(tb[i], tb[j], crit, under, concentration, qc, land);
    i = 0; j = 3; crit =  -0.05464559974092431;
     sum +=  waters(tb[i], tb[j], crit, under, concentration, qc, land);
  }
  #ifdef DEBUG
  printf("debug_i land, qc, conc = %f %d %f  %f\n",*land, *qc, *concentration, tb[0]);
  #endif


/* Land filters:
     Rebuild for F15
     19v, 19h, < 0.018077900915434868
     19h, 37v, >  0.05150437355041504
*/
  if (satno == 15 || satno == 248) {
     i = 0; j = 1; under = true; crit = 0.011795428064134385;
     sum +=  lands(tb[i], tb[j], crit, under, concentration, qc, land);
     i = 3; j = 4; under = false; crit = 0.007027243122910009;
     sum +=  lands(tb[i], tb[j], crit, under, concentration, qc, land);
  }
  else {
    /* no further land filter for F13, F14 */
  }

/* prep for regular usage of algorithm -- restore land to 0 */
  if ((*land) == (1./4. + 1./16.) ) {
    *land = 0.0;
  }
 
  #ifdef DEBUG
  printf("debug_j land, qc, conc = %f %d %f  %f\n",*land, *qc, *concentration, tb[0]);
  #endif

  if (*land != 0.0) return 1;
/* only proceed if we didn't filter the point already */

/* //////////////////////////////////////////////////////////////
// done with delta ratio filters and tb filters, ready to call algorithm
////////////////////////////////////////////////////////////// */
  float tlat, tlon;
  float t19v, t19h, t22v, t37v, t37h, t85v, t85h;
  float nasa;
  int ant = SSMI_ANTENNA;
  char pole;

  #ifdef DEBUG
  printf("debug_k land, qc, conc = %f %d %f  %f %f\n",*land, *qc, *concentration, tb[0], t19v);
  #endif

  t19v = ( (float)tb[SSMI_T19V] );
  t19h = ( (float)tb[SSMI_T19H] );
  t22v = ( (float)tb[SSMI_T22V] );
  t37v = ( (float)tb[SSMI_T37V] );
  t37h = ( (float)tb[SSMI_T37H] );
  t85v = ( (float)tb[SSMI_T85V] );
  t85h = ( (float)tb[SSMI_T85H] );
  if (tlat > 0) {
    pole = 'n';
  }
  else {
    pole = 's';
  }
  nasa = nasa_team(t19v, t19h, t22v, t37v, t37h, t85v, t85h, pole, ant, satno);
  if (nasa == BAD_DATA || nasa == WEATHER || nasa < 0. ) {
    *qc = 5;
    *land = 0.5;
    *concentration = 0.0;
  }
  else { /* valid concentration: */
    *qc = 1;
    *land = 0.0;
    *concentration = nasa / 100.;
    if (*concentration > 1.  ) *concentration = 1.0;
    if (*concentration < 0.15) *concentration = 0.0;
  }

  #ifdef DEBUG
  printf("debug_l land, qc, conc = %f %d %f  %f %f\n",*land, *qc, *concentration, tb[0], t19v);
  #endif

  return sum;
}
