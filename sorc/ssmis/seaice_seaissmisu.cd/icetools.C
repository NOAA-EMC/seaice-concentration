#include <cstdio>

// unused in ssmis
#define REF_SAT 13

/* Average (as needed) the brightness temperatures and ice concentrations
     on the sea ice grid. */
/* Compute the ice concentration with the averaged ice temperatures (a1 
     files) */
/* Robert Grumbine 10 February 1995 */
/* Apply the minimum concentration cutoff  Robert Grumbine 8 June 2001 */

int ice_avg_data(ssmis_tmp *north_tmp, ssmis_tmp *south_tmp,
                 ssmis *north, ssmis *south, 
                 const int north_pts, const int south_pts, 
                 ssmis_team2_tables &arctic, ssmis_team2_tables &antarctic) { 
  int i, nlim, slim;

  nlim = north_pts;
  slim = south_pts;
  
  for (i = 0; i < nlim ; i++) {
    if (north_tmp[i].count == 0) {
      /* Do it this way to preserve the weather flags we may have set */
      north[i].conc_bar = north_tmp[i].conc_bar;
      north[i].bar_conc = north_tmp[i].conc_bar;
      north[i].hires_conc = north_tmp[i].conc_bar;
      north[i].old_conc = north_tmp[i].conc_bar;
      north[i].t19v = 0;
      north[i].t19h = 0;
      north[i].t22v = 0;
      north[i].t37v = 0;
      north[i].t37h = 0;
      north[i].t92v = 0;
      north[i].t92h = 0;
      north[i].t150h = 0;
      north[i].count = 0;
      north[i].weather_count = north_tmp[i].weather_count;
    }
    else {
      north[i].t19v = (int) (0.5 + (float) north_tmp[i].t19v / \
                                        (float) north_tmp[i].count);
      north[i].t19h = (int) (0.5 + (float) north_tmp[i].t19h / \
                                        (float) north_tmp[i].count);
      north[i].t22v = (int) (0.5 + (float) north_tmp[i].t22v / \
                                        (float) north_tmp[i].count);
      north[i].t37v = (int) (0.5 + (float) north_tmp[i].t37v / \
                                        (float) north_tmp[i].count);
      north[i].t37h = (int) (0.5 + (float) north_tmp[i].t37h / \
                                        (float) north_tmp[i].count);
      north[i].t92v = (int) (0.5 + (float) north_tmp[i].t92v / \
                                        (float) north_tmp[i].count);
      north[i].t92h = (int) (0.5 + (float) north_tmp[i].t92h / \
                                        (float) north_tmp[i].count);
      north[i].t150h = (int) (0.5 + (float) north_tmp[i].t150h / \
                                        (float) north_tmp[i].count);
      north[i].conc_bar = (int) (0.5 + (float) north_tmp[i].conc_bar / 
                                       (float) north_tmp[i].count);
      north[i].count    = north_tmp[i].count;
      north[i].weather_count = north_tmp[i].weather_count;

      #ifdef TEAM2
        //printf("n - team2\n");
        north[i].bar_conc = (unsigned char) (0.5 + nasa_team2(
          (float)north[i].t19v / 100.,
          (float)north[i].t19h / 100.,
          (float)north[i].t22v / 100.,
          (float)north[i].t37v / 100.,
          (float)north[i].t37h / 100.,
          (float)north[i].t92v / 100.,
          (float)north[i].t92h / 100.,
          (float)north[i].t150h / 100.,
          arctic, REF_SAT ));
        // need to test on locale

        north[i].old_conc = (unsigned char) (0.5 + nasa_team(
          (float)north[i].t19v / 100.,
          (float)north[i].t19h / 100.,
          (float)north[i].t22v / 100.,
          (float)north[i].t37v / 100.,
          (float)north[i].t37h / 100.,
          (float)north[i].t92v / 100.,
          (float)north[i].t92h / 100., 
          (float)north[i].t150h / 100., 
          'n', SSMIS_ANTENNA, REF_SAT
        ) ); 

      #else
        north[i].old_conc = (unsigned char) (0.5 + nasa_team2(
          (float)north[i].t19v / 100.,
          (float)north[i].t19h / 100.,
          (float)north[i].t22v / 100.,
          (float)north[i].t37v / 100.,
          (float)north[i].t37h / 100.,
          (float)north[i].t92v / 100.,
          (float)north[i].t92h / 100.,
          (float)north[i].t150h / 100.,
          arctic, REF_SAT ));
        // need to test on locale

        north[i].bar_conc = (unsigned char) (0.5 + nasa_team(
          (float)north[i].t19v / 100.,
          (float)north[i].t19h / 100.,
          (float)north[i].t22v / 100.,
          (float)north[i].t37v / 100.,
          (float)north[i].t37h / 100.,
          (float)north[i].t92v / 100.,
          (float)north[i].t92h / 100., 
          (float)north[i].t150h / 100., 
          'n', SSMIS_ANTENNA, REF_SAT
        ) ); 

      #endif

      #ifdef VERBOSE2
        printf("averaged data, %4d, %5d %5d %5d %5d %3d %3d %5d %3d\n",
        i, north[i].t19v, north[i].t19h, north[i].t22v, north[i].t37v,
        north[i].conc_bar, north[i].bar_conc, 
        north[i].conc_bar - north[i].bar_conc, north[i].count);
      #endif 
      
    }
  }

  for (i = 0; i < slim ; i++) {
    if (south_tmp[i].count == 0) {
      south[i].conc_bar = south_tmp[i].conc_bar;
      south[i].bar_conc = south_tmp[i].conc_bar;
      south[i].hires_conc = south_tmp[i].conc_bar;
      south[i].old_conc = south_tmp[i].conc_bar;
      south[i].t19v = 0;
      south[i].t19h = 0;
      south[i].t22v = 0;
      south[i].t37v = 0;
      south[i].t37h = 0;
      south[i].t92v = 0;
      south[i].t92h = 0;
      south[i].t150h = 0;
      south[i].count = 0;
      south[i].weather_count = south_tmp[i].weather_count;
    }
    else {
      south[i].t19v = (int) (0.5 + (float) south_tmp[i].t19v / 
                                        (float) south_tmp[i].count);
      south[i].t19h = (int) (0.5 + (float) south_tmp[i].t19h /
                                        (float)  south_tmp[i].count);
      south[i].t22v = (int) (0.5 + (float) south_tmp[i].t22v / 
                                        (float) south_tmp[i].count);
      south[i].t37v = (int) (0.5 + (float) south_tmp[i].t37v / 
                                        (float) south_tmp[i].count);
      south[i].t37h = (int) (0.5 + (float) south_tmp[i].t37h / 
                                        (float) south_tmp[i].count);
      south[i].t92v = (int) (0.5 + (float) south_tmp[i].t92v / 
                                        (float) south_tmp[i].count);
      south[i].t92h = (int) (0.5 + (float) south_tmp[i].t92h / 
                                        (float) south_tmp[i].count);
      south[i].t150h = (int) (0.5 + (float) south_tmp[i].t150h / 
                                        (float) south_tmp[i].count);
      south[i].conc_bar = (int) (0.5 + (float)south_tmp[i].conc_bar / 
                                       (float)south_tmp[i].count);
      south[i].count    = south_tmp[i].count;
      south[i].weather_count = south_tmp[i].weather_count;

      #ifdef TEAM2
      //printf("s - team2\n");
      south[i].bar_conc = (unsigned char) (0.5 + nasa_team2(
        (float)south[i].t19v / 100.,
        (float)south[i].t19h / 100.,
        (float)south[i].t22v / 100.,
        (float)south[i].t37v / 100.,
        (float)south[i].t37h / 100.,
        (float)south[i].t92v / 100.,
        (float)south[i].t92h / 100.,
        (float)south[i].t150h / 100.,
        antarctic, REF_SAT ));

      south[i].old_conc = (unsigned char) (0.5 + nasa_team(
        (float)south[i].t19v / 100. ,
        (float)south[i].t19h  / 100.,
        (float)south[i].t22v  / 100.,
        (float)south[i].t37v  / 100.,
        (float)south[i].t37h  / 100.,
        (float)south[i].t92v  / 100.,
        (float)south[i].t92h  / 100., 
        (float)south[i].t150h  / 100., 
        's', SSMIS_ANTENNA, REF_SAT
       )  ) ; 

      #else

      south[i].old_conc = (unsigned char) (0.5 + nasa_team2(
        (float)south[i].t19v / 100.,
        (float)south[i].t19h / 100.,
        (float)south[i].t22v / 100.,
        (float)south[i].t37v / 100.,
        (float)south[i].t37h / 100.,
        (float)south[i].t92v / 100.,
        (float)south[i].t92h / 100.,
        (float)south[i].t150h / 100.,
        antarctic, REF_SAT ));

      south[i].bar_conc = (unsigned char) (0.5 + nasa_team(
        (float)south[i].t19v / 100. ,
        (float)south[i].t19h  / 100.,
        (float)south[i].t22v  / 100.,
        (float)south[i].t37v  / 100.,
        (float)south[i].t37h  / 100.,
        (float)south[i].t92v  / 100.,
        (float)south[i].t92h  / 100., 
        (float)south[i].t150h  / 100., 
        's', SSMIS_ANTENNA, REF_SAT
       )  ) ; 

       #endif
    }
  }

/* Minimum concentration cutoffs  8 June 2001 */
  for (i = 0; i < nlim; i++) {
     if (north[i].conc_bar < MIN_CONC) north[i].conc_bar = 0;
     if (north[i].bar_conc < MIN_CONC) north[i].bar_conc = 0;
     if (north[i].hires_conc < MIN_CONC) north[i].hires_conc = 0;
     if (north[i].old_conc < MIN_CONC) north[i].old_conc = 0;
  }
  for (i = 0; i < slim; i++) {
     if (south[i].conc_bar < MIN_CONC) south[i].conc_bar = 0;
     if (south[i].bar_conc < MIN_CONC) south[i].bar_conc = 0;
     if (south[i].hires_conc < MIN_CONC) south[i].hires_conc = 0;
     if (south[i].old_conc < MIN_CONC) south[i].old_conc = 0;
  }

/* Weather filter based on counts of good vs bad data on point  30 April 2004 */
/* By way of above computation, bar_conc can be weather even if conc_bar is not */
  for (i = 0; i < nlim; i++) {
     if (north[i].bar_conc == WEATHER) {
       north[i].conc_bar = WEATHER;
       north[i].bar_conc = WEATHER;
       north[i].hires_conc = WEATHER;
       north[i].old_conc = WEATHER;
     }
     if (north[i].weather_count >= north[i].count / 2 && 
         north[i].bar_conc <= MAX_CONC ) {
       north[i].conc_bar = WEATHER;
       north[i].bar_conc = WEATHER;
       north[i].hires_conc = WEATHER;
       north[i].old_conc = WEATHER;
     }      
  }
  for (i = 0; i < slim; i++) {
     if (south[i].bar_conc == WEATHER) {
       south[i].conc_bar = WEATHER;
       south[i].bar_conc = WEATHER;
       south[i].hires_conc = WEATHER;
       south[i].old_conc = WEATHER;
     }
     if (south[i].weather_count >= south[i].count / 2 && 
         south[i].bar_conc <= MAX_CONC ) {
       south[i].conc_bar = WEATHER;
       south[i].bar_conc = WEATHER;
       south[i].hires_conc = WEATHER;
       south[i].old_conc = WEATHER;
     }      
  }

  #ifdef VERBOSE
    printf("About to leave ice_avg_data\n");
    fflush(stdout);
  #endif

  return 0;

}

/* Apply a land mask to the concentrations (but not the brightness 
     temperatures ) */
/* Robert Grumbine 28 March 1995 */

int ice_mask( ssmis *north, ssmis *south, 
              const int north_pts, const int south_pts, 
              unsigned char *nmap, unsigned char *smap    ) { 
  int i, nlim, slim;

  nlim = north_pts;
  slim = south_pts;
  
  for (i = 0; i < nlim ; i++) {
    if ( ((int) nmap[i]) == ((int) LAND) ) {
      north[i].conc_bar = LAND ;
      north[i].bar_conc = LAND ;
      north[i].hires_conc = LAND ;
      north[i].old_conc = LAND ;
    }
    if ( ((int) nmap[i]) == ((int) COAST) ) {
      north[i].conc_bar = COAST ;
      north[i].bar_conc = COAST ;
      north[i].hires_conc = COAST ;
      north[i].old_conc = COAST ;
    }
  }

  for (i = 0; i < slim ; i++) {
    if (smap[i] == LAND) {
      south[i].conc_bar = LAND ;
      south[i].bar_conc = LAND ;
      south[i].hires_conc = LAND ;
      south[i].old_conc = LAND ;
    }
    if ( ((int) smap[i]) == ((int) COAST) ) {
      south[i].conc_bar = COAST ;
      south[i].bar_conc = COAST ;
      south[i].hires_conc = COAST ;
      south[i].old_conc = COAST ;
    }
  }

  return 0;

}

/* Zero out the information in the temporary data files for ssmis 
      average computation */
/* Robert Grumbine 15 December 1994 */

int ice_zero(ssmis_tmp *north_tmp, ssmis_tmp *south_tmp, 
              const int north_pts, const int south_pts)
{
  int i;
  ssmis_tmp blank;

  blank.t19v = 0;
  blank.t19h = 0;
  blank.t22v = 0;
  blank.t37v = 0;
  blank.t37h = 0;
  blank.t92v = 0;
  blank.t92h = 0;
  blank.t150h = 0;
  blank.conc_bar = NO_DATA;
  blank.hires_bar = NO_DATA;
  blank.old_conc_bar = NO_DATA;
  blank.count    = 0;
  blank.weather_count    = 0;

  for (i = 0; i < north_pts; i++) { 
    north_tmp[i] = blank;
  }

  for (i = 0; i < south_pts; i++) { 
    south_tmp[i] = blank;
  }

  return 0;

}
