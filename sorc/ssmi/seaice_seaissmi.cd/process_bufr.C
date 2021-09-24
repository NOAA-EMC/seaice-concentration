#include <cstdio>

#include "icegrids.h"

/* For error qc 16 March 2004 */
extern int err_stype ;
extern int err_19h_range ;
extern int err_19v_range ;
extern int err_22v_range ;
extern int err_37v_range ;
extern int err_37h_range ;
extern int err_85v_range ;
extern int err_85h_range ;
extern int err_19_polar ;
extern int err_37_polar ;
extern int err_85_polar ;
extern int err_lat      ;
extern int err_lon      ;

int ice_add_bufr(ssmi_tmp *north_tmp, ssmi_tmp  *south_tmp, 
                 ssmi_bufr_line *a, ssmi_team2_tables &arctic, ssmi_team2_tables &antarctic) {
/* Version of the SDR ice_add_data suited to work with BUFR input */ 

  int j, k;
  float tlat, tlon;
  float t19v, t19h, t22v, t37v, t37h, t85v, t85h;
  float nasa;
  int ilat, jlon, index, satno;
  //int stype;

  #ifdef HIRES
     int KMAX = 4;
  #else
     int KMAX = 1;
  #endif

/* Convert to F number.  Satno 246 (native) is F-13 */
  satno = a->satno - 233;

/* Peel off data from the record.  Transfer to physical units. */
  for (j = 0; j < NSCANS; j++) {
    t19v = ( (float)a->full[j].t19v );
    t19h = ( (float)a->full[j].t19h );
    t22v = ( (float)a->full[j].t22v );
    t37v = ( (float)a->full[j].t37v );
    t37h = ( (float)a->full[j].t37h );
    //stype = a->full[j].surface_type;

    for (k = 0; k < KMAX; k++) {
      if (k > 0) {
        tlat = ( (float)a->full[j].hires[k-1].latitude);
        tlon = ( (float)a->full[j].hires[k-1].longitude);
        t85v = ( (float)a->full[j].hires[k-1].t85v );
        t85h = ( (float)a->full[j].hires[k-1].t85h );
      }
      else {
        tlat = ( (float)a->full[j].latitude);
        tlon = ( (float)a->full[j].longitude);
        t85v = ( (float)a->full[j].t85v );
        t85h = ( (float)a->full[j].t85h );
      }

/* Changed from -25, +20 to -30, 24 on 16 March 2004 */
      if (tlat > -30. && tlat < 24. ) continue;
  
      nasa = 0.;
  
      if ((t19v == 0.) && (tlat == 0.) && (tlon == 0.) ) {
        printf("probably shouldn't be here\n"); fflush(stdout);
        continue;
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
                                t37v, t37h, t85v, t85h, arctic, satno)  );
          #else
          nasa = (int) (0.5 + nasa_team(t19v, t19h, t22v, 
                                t37v, t37h, t85v, t85h, 'n', SSMI_ANTENNA, satno)   );
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
              north_tmp[index].t85v = (unsigned int) (0.5+t85v*100); 
              north_tmp[index].t85h = (unsigned int) (0.5+t85h*100); 
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
              north_tmp[index].t85v += (unsigned int) (0.5+t85v*100); 
              north_tmp[index].t85h += (unsigned int) (0.5+t85h*100); 
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
                                t37v, t37h, t85v, t85h, antarctic, satno)  );
          #else
          nasa = (int) (0.5 + nasa_team(t19v, t19h, t22v, 
                                t37v, t37h, t85v, t85h, 's', SSMI_ANTENNA, satno)   );
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
              south_tmp[index].t85v = (unsigned int) (0.5 + t85v*100); 
              south_tmp[index].t85h = (unsigned int) (0.5 + t85h*100); 
              south_tmp[index].conc_bar = (int) (0.5 + nasa) ;
              south_tmp[index].count = 1;
            }
            else {
              south_tmp[index].t19v += (unsigned int) (0.5 + t19v*100); 
              south_tmp[index].t19h += (unsigned int) (0.5 + t19h*100); 
              south_tmp[index].t22v += (unsigned int) (0.5 + t22v*100); 
              south_tmp[index].t37v += (unsigned int) (0.5 + t37v*100); 
              south_tmp[index].t37h += (unsigned int) (0.5 + t37h*100); 
              south_tmp[index].t85v += (unsigned int) (0.5 + t85v*100); 
              south_tmp[index].t85h += (unsigned int) (0.5 + t85h*100); 
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

    } /* end for k loop */

  } /* end for loop */

  return 0;

}

int process_bufr(ssmi_bufr_line *b) {
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
void show_bufr(ssmi_bufr_line *b);

int check_bufr(ssmi_bufr_line *b) {
  int nerr = 0, npts = 0;
  int i;

  #ifdef VERBOSE2
  show_bufr(b);
  #endif

  for (i = 0; i < NSCANS; i++) {
    if ( b->full[i].t19h > 295.0 ||  b->full[i].t19h <  75.0) {
      nerr += 1;
      err_19h_range += 1;
    } 
    if ( b->full[i].t19v > 295.0 ||  b->full[i].t19v < 150.0) {
      nerr += 1;
      err_19v_range += 1;
    } 

    if ( b->full[i].t22v > 295.0 ||  b->full[i].t22v < 150.0) {
      nerr += 1;
      err_22v_range += 1;
    } 

    if ( b->full[i].t37h > 295.0 ||  b->full[i].t37h < 100.0) {
      err_37h_range += 1;
      nerr += 1;
    } 
    if ( b->full[i].t37v > 295.0 ||  b->full[i].t37v < 150.0) {
      err_37v_range += 1;
      nerr += 1;
    } 
  
    if ( b->full[i].t85h > 295.0 ||  b->full[i].t85h < 125.0) {
      err_85h_range += 1;
      nerr += 1;
    } 
    if ( b->full[i].t85v > 295.0 ||  b->full[i].t85v < 150.0) {
      err_85v_range += 1;
      nerr += 1;
    } 

/* Polarization tests: */ 
    if ( b->full[i].t19h > b->full[i].t19v) {
      #ifdef VERBOSE
        printf("failed v > h 19 test\n");
      #endif
      nerr += 1;
      err_19_polar += 1;
    }
    if ( b->full[i].t37h > b->full[i].t37v) {
      #ifdef VERBOSE
        printf("failed v > h 37 test\n");
      #endif
      nerr += 1;
      err_37_polar += 1;
    }
    if ( b->full[i].t85h > b->full[i].t85v) {
      #ifdef VERBOSE
        printf("failed v > h 85 test\n");
      #endif
      nerr += 1;
      err_85_polar += 1;
    }

  
/* Location and surface type tests: */
    if ((int) b->full[i].surface_type > 8 ) {
      nerr += 1;
      err_stype += 1; 
    }
    if ( b->full[i].latitude > 180.) {
       nerr += 1;
       err_lat += 1;
    }
    if ( b->full[i].longitude > 360.) {
       nerr += 1;
       err_lon += 1;
    }

    #ifdef HIRES
    int k;
    for (k = 0; k < 3; k++) {
      if (check_short_bufr(&(b->full[i].hires[k]) ) != 0) {
        nerr += 1;
      }
    }
    #endif

    if (nerr != 0) {
      npts += 1;
      zero_bufr(b, i);
    }

  
  } /* end checking */

  return npts;
  
}

void zero_bufr(ssmi_bufr_line *b, int i) {

   b->full[i].scan_counter = 0;
   b->full[i].latitude     = 0;
   b->full[i].longitude    = 0;
   b->full[i].t19v         = 0;
   b->full[i].t19h         = 0;
   b->full[i].t22v         = 0;
   b->full[i].t37v         = 0;
   b->full[i].t37h         = 0;
   b->full[i].t85v         = 0;
   b->full[i].t85h         = 0;
   b->full[i].surface_type = 0;
   b->full[i].position_num = 0;
   #ifdef HIRES
   int k;
   for (k = 0; k < 3; k++) {
     b->full[i].hires[k].t85v = 0;
     b->full[i].hires[k].t85h = 0;
     b->full[i].hires[k].latitude = 0;
     b->full[i].hires[k].longitude = 0;
   }
   #endif

  return;
}

/* Bounds checking on a short data record */
/* Robert Grumbine 1 March 1995 */

int check_short_bufr(ssmi_short_bufr *c) {
  int nerr = 0;

  if (c->latitude > 180.) {
      nerr += 1;
      err_lat += 1;
  }
  if (c->longitude > 360.) {
      nerr += 1;
      err_lon += 1;
  }
  if ( c->t85v > 295.0 ||  c->t85v < 150.0 ) {
      nerr += 1;
      err_85v_range += 1;
  }
  if ( c->t85h < 125.0 ||  c->t85h > 295.0) {
      err_85h_range += 1;
      nerr += 1;
  }
/* Polarization test:       */
  if ( c->t85h > c->t85v ) {
      nerr += 1;
      err_85_polar += 1;
  }

  if (nerr != 0) {
      c->latitude     = 0;
      c->longitude    = 0;
      c->t85v         = 0;
      c->t85h         = 0;
  }

  return nerr;
  
}
void show_bufr(ssmi_bufr_line *b) {
  
  printf("in bufr, %d %d %d %d %d %d %d %d\n", b->satno, b->year, 
           b->month, b->day, b->hour, b->mins, b->secs, b->scan_no);

}
