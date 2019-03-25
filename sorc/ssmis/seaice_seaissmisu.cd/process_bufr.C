#include <cstdio>

/* For error qc 16 March 2004 */
extern int err_19h_range ;
extern int err_19v_range ;
extern int err_22v_range ;
extern int err_37v_range ;
extern int err_37h_range ;
extern int err_92v_range ;
extern int err_92h_range ;
extern int err_150h_range ;
extern int err_19_polar ;
extern int err_37_polar ;
extern int err_92_polar ;
extern int err_lat      ;
extern int err_lon      ;

int ice_add_bufr(ssmis_tmp *north_tmp, ssmis_tmp  *south_tmp, 
                 ssmisupt *a, ssmis_team2_tables &arctic, ssmis_team2_tables &antarctic) {
/* Version of the SDR ice_add_data suited to work with BUFR input */ 
  float tlat, tlon;
  float t19v, t19h, t22v, t37v, t37h, t92v, t92h, t150h;
  float nasa;
  int ilat, jlon, index, satno;

  satno = a->satid ;

/* Peel off data from the record.  Transfer to physical units. */
    tlat = ( (float)a->clat);
    tlon = ( (float)a->clon);
    t19v = ( (float)a->obs[SSMIS_T19V].tmbr );
    t19h = ( (float)a->obs[SSMIS_T19H].tmbr );
    t22v = ( (float)a->obs[SSMIS_T22V].tmbr );
    t37v = ( (float)a->obs[SSMIS_T37V].tmbr );
    t37h = ( (float)a->obs[SSMIS_T37H].tmbr );
    t92v = ( (float)a->obs[SSMIS_T92V].tmbr );
    t92h = ( (float)a->obs[SSMIS_T92H].tmbr );
    t150h = ( (float)a->obs[SSMIS_T150H].tmbr );

/* Changed from -25, +20 to -30, 24 on 16 March 2004 */
      if (tlat > -30. && tlat < 24. ) return 0;
  
      nasa = 0.;
  
      if ((t19v == 0.) && (tlat == 0.) && (tlon == 0.) ) {
        printf("probably shouldn't be here\n"); fflush(stdout);
        return 0;
      } /* done handling the bad data case */
  
  /* Now check for which hemisphere the data are in. */
      if (tlat > 0.) {
        mapll(tlat, tlon, &ilat, &jlon, xorig_NORTH, yorig_NORTH,
              eccen2, slat_NORTH, slon_NORTH, rearth, dx, dy, sgn_NORTH);
        if ( (ilat >= 0) && (ilat <  (NX_NORTH) ) && 
             (jlon >= 0) && (jlon <  (NY_NORTH) )    ) {
          index = ilat+jlon*(NX_NORTH);
          #ifdef TEAM2
          //printf("n2 - team2\n");
          nasa = (int) (0.5 + nasa_team2(t19v, t19h, t22v, 
                                t37v, t37h, t92v, t92h, t150h, arctic, satno)  );
          #else
          nasa = (int) (0.5 + nasa_team(t19v, t19h, t22v, 
                                t37v, t37h, t92v, t92h, t150h, 'n', SSMIS_ANTENNA, satno)   );
          #endif
          if (nasa != BAD_DATA && nasa != WEATHER) {
/* Units Issues Resolved 16 March 2004 */
/* Need the count == 0 test along with flags because flag values can be
   reached for count != 0 and summing up, e.g. values of 89 and 88 to reach
   177 (weather)
   20 April 2004
*/
            if ( ( north_tmp[index].conc_bar == NO_DATA ||
                   north_tmp[index].conc_bar == BAD_DATA ||
                   north_tmp[index].conc_bar == WEATHER  ) && 
                   north_tmp[index].count == 0) {
              if (north_tmp[index].count != 0) {
                printf("conc_bar was %d, count was %d 19v %d\n", 
                    north_tmp[index].conc_bar, 
                    north_tmp[index].count, north_tmp[index].t19v );
              }
              north_tmp[index].t19v = (unsigned int) (0.5+t19v*100); 
              north_tmp[index].t19h = (unsigned int) (0.5+t19h*100); 
              north_tmp[index].t22v = (unsigned int) (0.5+t22v*100); 
              north_tmp[index].t37v = (unsigned int) (0.5+t37v*100); 
              north_tmp[index].t37h = (unsigned int) (0.5+t37h*100); 
              north_tmp[index].t92v = (unsigned int) (0.5+t92v*100); 
              north_tmp[index].t92h = (unsigned int) (0.5+t92h*100); 
              north_tmp[index].t150h = (unsigned int) (0.5+t150h*100); 
              north_tmp[index].conc_bar = (int) (0.5+nasa);
              north_tmp[index].count = 1;
            }
            else {
              north_tmp[index].conc_bar +=   (int) (0.5 + nasa);
              north_tmp[index].t19v += (unsigned int) (0.5+t19v*100); 
              north_tmp[index].t19h += (unsigned int) (0.5+t19h*100); 
              north_tmp[index].t22v += (unsigned int) (0.5+t22v*100); 
              north_tmp[index].t37v += (unsigned int) (0.5+t37v*100); 
              north_tmp[index].t37h += (unsigned int) (0.5+t37h*100); 
              north_tmp[index].t92v += (unsigned int) (0.5+t92v*100); 
              north_tmp[index].t92h += (unsigned int) (0.5+t92h*100); 
              north_tmp[index].t150h += (unsigned int) (0.5+t150h*100); 
              north_tmp[index].count += 1;
            }
          } /* end of adding data if data not equal bad or land */
          else {
            if ( nasa == BAD_DATA && north_tmp[index].count == 0 ) {
              north_tmp[index].conc_bar = BAD_DATA;
            }
            if ( nasa == WEATHER && north_tmp[index].count == 0 ) {
              north_tmp[index].conc_bar = WEATHER;
            }
            north_tmp[index].weather_count += 1;
          }
  
        } /* end of lat-long range test */
      } /* end of north hemisphere work */
  
      else {
  /* South branch : BEWARE!! must have positive latitudes, take -tlat.*/
  /* Mapll fixed 4 June 1997 to accept physical latitudes */
        mapll(tlat, tlon, &ilat, &jlon, xorig_SOUTH, yorig_SOUTH,
              eccen2, slat_SOUTH, slon_SOUTH, rearth, dx, dy, sgn_SOUTH);
        if (ilat >= 0 && ilat < NX_SOUTH && jlon >= 0 && jlon < NY_SOUTH) {
          index = ilat + jlon*(NX_SOUTH);
          #ifdef TEAM2
          //printf("s2 - team2\n");
          nasa = (int) (0.5 + nasa_team2(t19v, t19h, t22v, 
                                t37v, t37h, t92v, t92h, t150h, antarctic, satno)  );
          #else
          nasa = (int) (0.5 + nasa_team(t19v, t19h, t22v, 
                                t37v, t37h, t92v, t92h, t150h, 's', SSMIS_ANTENNA, satno)   );
          #endif
          if (nasa != BAD_DATA && nasa != WEATHER ) {
            if ( ( south_tmp[index].conc_bar == NO_DATA ||
                   south_tmp[index].conc_bar == BAD_DATA ||
                   south_tmp[index].conc_bar == WEATHER  ) && 
                   south_tmp[index].count == 0) {
              south_tmp[index].t19v = (unsigned int) (0.5 + t19v*100); 
              south_tmp[index].t19h = (unsigned int) (0.5 + t19h*100); 
              south_tmp[index].t22v = (unsigned int) (0.5 + t22v*100); 
              south_tmp[index].t37v = (unsigned int) (0.5 + t37v*100); 
              south_tmp[index].t37h = (unsigned int) (0.5 + t37h*100); 
              south_tmp[index].t92v = (unsigned int) (0.5 + t92v*100); 
              south_tmp[index].t92h = (unsigned int) (0.5 + t92h*100); 
              south_tmp[index].t150h = (unsigned int) (0.5 + t150h*100); 
              south_tmp[index].conc_bar = (int) (0.5 + nasa) ;
              south_tmp[index].count = 1;
            }
            else {
              south_tmp[index].t19v += (unsigned int) (0.5 + t19v*100); 
              south_tmp[index].t19h += (unsigned int) (0.5 + t19h*100); 
              south_tmp[index].t22v += (unsigned int) (0.5 + t22v*100); 
              south_tmp[index].t37v += (unsigned int) (0.5 + t37v*100); 
              south_tmp[index].t37h += (unsigned int) (0.5 + t37h*100); 
              south_tmp[index].t92v += (unsigned int) (0.5 + t92v*100); 
              south_tmp[index].t92h += (unsigned int) (0.5 + t92h*100); 
              south_tmp[index].t150h += (unsigned int) (0.5 + t150h*100); 
              south_tmp[index].conc_bar += (int) (0.5 + nasa);
              south_tmp[index].count += 1;
            }
          } /* end of adding data if data not equal bad or land */
          else {
           if ( nasa == BAD_DATA && south_tmp[index].count == 0 ) {
             south_tmp[index].conc_bar = BAD_DATA;
           }
           if ( nasa == WEATHER && south_tmp[index].count == 0 ) {
             south_tmp[index].conc_bar = WEATHER;
           }
           south_tmp[index].weather_count += 1;
          }
  
  
        } /* end of adding data */
  
      } /* end north-south splitting */


  return 0;

}

int process_bufr(ssmisupt *b) {
/* Process the bufr data records, which will eventually include the short 
    data as well. 
   Only processing is to check for qc purposes.
*/
  return check_bufr(b);
}

/* Perform some bounds checking for the long data */
/* Order of checking is related to the frequency with which
   a given field is erroneous */
/* Note that typically if one number is wrong, so are several others */
/* Robert Grumbine 10 February 1994 */
void show_bufr(ssmisupt *b);

int check_bufr(ssmisupt *b) {
  int nerr = 0, npts = 0;

  #ifdef VERBOSE2
  show_bufr(b);
  #endif

    if ( b->obs[SSMIS_T19H].tmbr > 295.0 ||  b->obs[SSMIS_T19H].tmbr <  75.0) {
      nerr += 1;
      err_19h_range += 1;
    } 
    if ( b->obs[SSMIS_T19V].tmbr > 295.0 ||  b->obs[SSMIS_T19V].tmbr < 150.0) {
      nerr += 1;
      err_19v_range += 1;
    } 

    if ( b->obs[SSMIS_T22V].tmbr > 295.0 ||  b->obs[SSMIS_T22V].tmbr < 150.0) {
      nerr += 1;
      err_22v_range += 1;
    } 

    if ( b->obs[SSMIS_T37H].tmbr > 295.0 ||  b->obs[SSMIS_T37H].tmbr < 100.0) {
      err_37h_range += 1;
      nerr += 1;
    } 
    if ( b->obs[SSMIS_T37V].tmbr > 295.0 ||  b->obs[SSMIS_T37V].tmbr < 150.0) {
      err_37v_range += 1;
      nerr += 1;
    } 
  
    if ( b->obs[SSMIS_T92H].tmbr > 295.0 ||  b->obs[SSMIS_T92H].tmbr < 125.0) {
      err_92h_range += 1;
      nerr += 1;
    } 
    if ( b->obs[SSMIS_T92V].tmbr > 295.0 ||  b->obs[SSMIS_T92V].tmbr < 150.0) {
      err_92v_range += 1;
      nerr += 1;
    } 
    if ( b->obs[SSMIS_T150H].tmbr > 295.0 ||  b->obs[SSMIS_T150H].tmbr < 100.0) {
      err_150h_range += 1;
      nerr += 1;
    } 

/* Polarization tests: */ 
    if ( b->obs[SSMIS_T19H].tmbr > b->obs[SSMIS_T19V].tmbr) {
      #ifdef VERBOSE
        printf("failed v > h 19 test %6.2f %6.2f \n",b->obs[SSMIS_T19H].tmbr, b->obs[SSMIS_T19V].tmbr);
        fflush(stdout);
      #endif
      nerr += 1;
      err_19_polar += 1;
    }
    if ( b->obs[SSMIS_T37H].tmbr > b->obs[SSMIS_T37V].tmbr) {
      #ifdef VERBOSE
        printf("failed v > h 37 test %6.2fH %6.2fV \n",b->obs[SSMIS_T37H].tmbr, b->obs[SSMIS_T37V].tmbr);
        fflush(stdout);
      #endif
      nerr += 1;
      err_37_polar += 1;
    }
    if ( b->obs[SSMIS_T92H].tmbr > b->obs[SSMIS_T92V].tmbr) {
      #ifdef VERBOSE
        printf("failed v > h 92 test\n");
      #endif
      nerr += 1;
      err_92_polar += 1;
    }

  
/* Location and surface type tests: */
    if ( b->clat > 180.) {
       nerr += 1;
       err_lat += 1;
    }
    if ( b->clon > 360.) {
       nerr += 1;
       err_lon += 1;
    }

    if (nerr != 0) {
      npts += 1;
      zero_bufr(b);
    }

  
  return npts;
  
}

void zero_bufr(ssmisupt *b) {

   b->clat     = 0;
   b->clon     = 0;
   b->obs[SSMIS_T19V].tmbr         = 0;
   b->obs[SSMIS_T19H].tmbr         = 0;
   b->obs[SSMIS_T22V].tmbr         = 0;
   b->obs[SSMIS_T37V].tmbr         = 0;
   b->obs[SSMIS_T37H].tmbr         = 0;
   b->obs[SSMIS_T92V].tmbr         = 0;
   b->obs[SSMIS_T92H].tmbr         = 0;
   b->obs[SSMIS_T150H].tmbr        = 0;

  return;
}

void show_bufr(ssmisupt *b) {
  
  printf("in bufr, %3d %4d %2d %2d %2d %2d %2d \n", b->satid, b->year, 
           b->month, b->day, b->hour, b->minute, b->second);

}
