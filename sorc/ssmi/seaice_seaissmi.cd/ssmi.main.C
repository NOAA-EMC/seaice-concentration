#include <cstdio>
#include <cstdlib>
#include <cmath> 

#include "icessmi.h"
#include "icegrids.h"

#include "grid_math.h"
#include "team2.C"

/*C$$$  MAIN PROGRAM DOCUMENTATION BLOCK                            */
/*C                                                                 */
/*C MAIN PROGRAM:  SSMI      CONSTRUCT SEA ICE CONCENTRATION GRIDS  */
/*C   PRGMMR: ROBERT GRUMBINE  ORG: W/NP21    DATE: 97-06-25        */
/*C                                                                 */
/*C ABSTRACT: READ IN SSMI DATA AND CONSTRUCT SEA ICE CONCENTRATION */
/*C    GRIDS USING THE NASA TEAM ALGORITHM.                         */
/*C                                                                 */
/*C PROGRAM HISTORY LOG:                                            */
/*C    97-06-25  ROBERT GRUMBINE                                    */
/*C                                                                 */
/*C USAGE:                                                          */
/*C   FILES ARE HANDLED BY ORDER IN THE INVOKING COMMAND LINE       */ 
/*C   INPUT FILES:                                                  */
/*C      FILE 1  - File with names of SSMI data files.  UNFORMATTED BINARY GENERATED   */
/*C              -   BY SSMI.bufr.X                                 */
/*C      FILE 2  - NORTHERN HEMISPHERE LAND MASK                    */
/*C      FILE 3  - SOUTHERN HEMISPHERE LAND MASK                    */
/*C   OUTPUT FILES:                                                 */
/*C      FILE 4  - NORTHERN HEMISPHERE GRIDDED SSMI data            */
/*C      FILE 5  - SOUTHERN HEMISPHERE GRIDDED SSMI data            */
/*C      FILE 6  - UNMASKED NORTHERN HEMISPHERE ice CONCENTRATIONS  */
/*C      FILE 7  - UNMASKED SOUTHERN HEMISPHERE ice CONCENTRATIONS  */
/*    Additional Arguments:                                         */
/*        8      - Julian Day                                       */
/*        9      - Year                                             */
/*       10      - Satellite number (F11 = 244 .. F16 = 249         */
/*C                                                                 */
/*C SUBPROGRAMS CALLED:                                             */
/*C      UNIQUE: process_bufr, process_short_bufr, check_bufr,      */
/*C              check_short_bufr, zero_bufr, newfilt, pole_fill    */
/*C              getfld, ice_zero, ice_add_data, ice_add_bufr,      */
/*C              ice_avg_data, nasa_team, nasa_team2                */
/*C      LIBRARY:                                                   */
/*C          OMB: mapll, mapxy                                      */
/*C                                                                 */
/*C EXIT STATES:                                                    */
/*C    COND =  0  - SUCCESSFUL RUN                                  */
/*C         = -1  - ERROR OCCURRED.  MESSAGE WRITTED TO STDOUT      */
/*C                                                                 */
/*C REMARKS:                                                        */
/*C                                                                 */
/*C ATTRIBUTES:                                                     */
/*C   LANGUAGE: ANSI STANDARD C                                     */
/*C   MACHINE:  ANY                                                 */
/*C                                                                 */
/*C$$$                                                              */

/* Main program for decoding the ssmi orbital records. */
/* Read in (process_ and check_ procedures) SDR data. */
/* Remap to sea ice grid -- 25.4 km, true at 60, 80 W (North hemisphere)
    100 E (Southern hemisphere) -- ice_ procedures */
/* Compute sea ice concentrations -- nasa_team algorithm */
/* Robert Grumbine 18 December 1994 */
/* revised to handle flat files derived from bufr 6/97 */

/* Updated for new algorithm targeting 12.7 km grid using 85 GHz */
/*   Robert Grumbine 16 March 2004 */

/* ************************************************ */
/* The following are for data qc and error tracking */
/* for process_bufr */
int err_stype = 0;
int err_19h_range = 0;
int err_19v_range = 0;
int err_22v_range = 0;
int err_37v_range = 0;
int err_37h_range = 0;
int err_85v_range = 0;
int err_85h_range = 0;
int err_19_polar = 0;
int err_37_polar = 0;
int err_85_polar = 0;
int err_lat      = 0;
int err_lon      = 0;
/* For the algorithm */
int bad_low = 0;
int bad_high = 0;
int crop_low = 0;
int filt37   = 0;
int filt22   = 0;
/* For filtration */
int efilt_37_n = 0;
int efilt_37_s = 0;
int efilt_22_n = 0;
int efilt_22_s = 0;

// Generic file for conducting other training/testing:
FILE *tester;

/* ************************************************ */
//int imax(int x, int y);
//int imax(int x, int y) {
//  if (x > y) {
//    return x;
//  }
//  return y;
//}
//float fmax(float x, float y);
//float fmax(float x, float y) {
//  if (x > y) {
//    return x;
//  }
//  return y;
//}
extern float hires(ssmi *map, int nx, int ny, int polei, int polej, 
                      unsigned char *mask) ;

// For new (2011) weather filtering:
float weather(float &t19v, float &t19h, float &t22v, float &t37v, float &t37h,
             float &t87v, float &t87h, int satno) ;
float old_weather(float &t19v, float &t19h, float &t22v, float &t37v, float &t37h,
             float &t87v, float &t87h, int satno) ;
float new_weather(float &t19v, float &t19h, float &t22v, float &t37v, float &t37h,
             float &t87v, float &t87h, int satno) ;
#include "weather.C"


/* ************************************************ */
int main(int argc, char *argv[]) {
  ssmi_bufr_line  a_bufr_line;
  FILE *fin, *input, *outn, *outs, *nland, *sland;
  FILE *nraw, *sraw;
  char fname[80];

  ssmi *south, *north;
  ssmi_tmp *north_tmp, *south_tmp;
  float  rfld_n[NY_NORTH][NX_NORTH];
  float  rfld_s[NY_SOUTH][NX_SOUTH];
  unsigned char  fld_n[NY_NORTH][NX_NORTH];
  unsigned char  fld_s[NY_SOUTH][NX_SOUTH];

  int north_pts, south_pts;
  int i, j, nerrs, nproc;
  int jday, satno, ref_year;
  int lines_used = 0, err_spots = 0;
// For team2
  ssmi_team2_tables arctic, antarctic;
                                                                                
// Team2 initialization:
  arctic.tbmfy.resize(n_atm, n_tb);
  arctic.tbmow.resize(n_atm, n_tb);
  arctic.tbmcc.resize(n_atm, n_tb);
  arctic.tbmthin.resize(n_atm, n_tb);
  arctic.pole = 'n';
  arctic_tables(arctic);
  lookuptable(arctic);

  antarctic.tbmfy.resize(n_atm, n_tb);
  antarctic.tbmow.resize(n_atm, n_tb);
  antarctic.tbmcc.resize(n_atm, n_tb);
  antarctic.tbmthin.resize(n_atm, n_tb);
  antarctic.pole = 's';
  antarctic_tables(antarctic);
  lookuptable(antarctic);
// End Team2 initialization



/* New the ssmi arrays */
  north_tmp = new ssmi_tmp[NX_NORTH*NY_NORTH]; 
  south_tmp = new ssmi_tmp[NX_SOUTH*NY_SOUTH]; 
  north     = new ssmi[NX_NORTH*NY_NORTH]; 
  south     = new ssmi[NX_SOUTH*NY_SOUTH]; 
  if (north == (ssmi*) NULL || south == (ssmi*) NULL || 
      south_tmp == (ssmi_tmp*) NULL || north_tmp == (ssmi_tmp*) NULL) {
    printf("Failed to new one of the ssmi fields!\n");
    return -1;
  }
 
/* Open data files */
  fin = fopen(argv[1], "r");
  nland = fopen(argv[2], "r");
  sland = fopen(argv[3], "r");
  outn  = fopen(argv[4], "w");
  outs  = fopen(argv[5], "w");
  nraw  = fopen(argv[6], "w");
  sraw  = fopen(argv[7], "w");
  if (fin == (FILE *) NULL   || 
       outn == (FILE *) NULL ||  outs == (FILE *) NULL || 
      nland == (FILE *) NULL || sland == (FILE *) NULL ||
       nraw == (FILE *) NULL ||  sraw == (FILE *) NULL)
  {
    printf("Failed to open one of the nine files!\n");
    if (fin == (FILE *) NULL ) {printf(" - input file %s\n",argv[1]);}
    if (nland == (FILE *) NULL ) {printf(" - north land in %s\n", argv[2]);}
    if (sland == (FILE *) NULL ) {printf(" - south land in %s\n", argv[3]);}
    if (outn == (FILE *) NULL ) {printf(" - north out %s\n", argv[4]);}
    if (outs == (FILE *) NULL ) {printf(" - south out %s\n", argv[5]);}
    if (nraw == (FILE *) NULL ) {printf(" - north raw out %s\n", argv[6]);}
    if (sraw == (FILE *) NULL ) {printf(" - south raw out %s\n", argv[7]);}
    return -1;
  }

  #ifdef TESTER
    tester = fopen("tester","w");
  #endif

  jday   = atoi(argv[8]);
  ref_year = atoi(argv[9]);
  satno    = atoi(argv[10]);
  printf("jday = %d, year = %d, satellite number %d\n",jday, ref_year, satno);

/* Set internal constants */
  nproc = NORBITS;
  north_pts = (NX_NORTH)*(NY_NORTH);
  south_pts = (NX_SOUTH)*(NY_SOUTH);
  ice_zero(north_tmp, south_tmp, north_pts, south_pts);

/* Read in the land masks fld_n, fld_s */
  fread(fld_n, sizeof(unsigned char), north_pts, nland);
  fread(fld_s, sizeof(unsigned char), south_pts, sland);


  while ( !feof(fin) ) {
/* Read in name of next file to process */
    fscanf(fin, "%s\n", fname);
    printf("Opened %s in orbital process loop\n", fname);
    input = fopen(fname, "r");
    if ( input == (FILE *) NULL ) {
       printf("Failed to open %s from orbit process loop\n", fname);
       break;
    }
  

/* Begin stripping out header */
    fseek(input, (long)(0) , SEEK_SET);

/* Begin stripping out data */
    for (i = 0; i < nproc ; i++)
    while (!feof(input) )
    {

      /* Add data only if data is within window -- Now assumed that the*/
      /* date-time filtering has been done prior to input */
        j = 0;
        while (!feof(input) )
        {  
          #ifdef VERBOSE2
            printf("Process record number %d\n",j); fflush(stdout); 
          #endif
          j++;
          fread(&a_bufr_line, sizeof(ssmi_bufr_line), 1, input);
          
          #ifdef VERBOSE
            printf("%d Satno = %d\n",j, a_bufr_line.satno);
          #endif
/* ZZZZ  Move to using all satellites appropriately 16 March 2004      */ 
/* Orig          if (a_bufr_line.satno == satno) {                     */
/* The test vs. 233 is a dummy.  The DMSP F series has satno = 233 + F */
          if (a_bufr_line.satno == satno ) {
            nerrs = process_bufr(&a_bufr_line);
            lines_used += 1;
            err_spots += nerrs;
            #ifdef VERBOSE
            if (nerrs != 0) {
              printf("%2d errors in record %6d of orbit %2d\n",nerrs, j, i);
              fflush(stdout);
            }
            #endif

            ice_add_bufr(north_tmp, south_tmp, &a_bufr_line, arctic, antarctic); 
          }
        }
  
    } /* end processing the orbits */

    fclose(input);
  } /* End while - reading files */
  
  /* Compute averaged tb and concentrations */
  ice_avg_data(north_tmp, south_tmp, north, south, north_pts, south_pts, arctic, antarctic);

/* Apply newer filtering techniques 10/29/95 BG */
/* Revise for hires grids 16 March 2004 */
// Remove 10/31/2011 for new structure of filtering:  newfilt(north, south);

  #ifdef HIRES
/* Now that we have a clean grid, work on the high resolution version */
    hires(north, NX_NORTH, NY_NORTH, (int) (0.5+polei_NORTH), 
                                     (int) (0.5+polej_NORTH), &fld_n[0][0]);
    hires(south, NX_SOUTH, NY_SOUTH, (int) (0.5+polei_SOUTH), 
                                     (int) (0.5+polej_SOUTH), &fld_s[0][0]);
    getfld(north, north_pts, &fld_n[0][0], &rfld_n[0][0], SSMI_HIRES_CONC);
    getfld(south, south_pts, &fld_s[0][0], &rfld_s[0][0], SSMI_HIRES_CONC);
  #else
    getfld(north, north_pts, &fld_n[0][0], &rfld_n[0][0], SSMI_BAR_CONC);
    getfld(south, south_pts, &fld_s[0][0], &rfld_s[0][0], SSMI_BAR_CONC);
  #endif

/* Fill in pole for output purposes */
  ssmi_pole_fill(&fld_n[0][0], 1);
  ssmi_pole_fill(&fld_s[0][0], 2);
  fwrite(fld_n, sizeof(unsigned char), north_pts, nraw);
  fwrite(fld_s, sizeof(unsigned char), south_pts, sraw);

/* Mask out the land now */
  ice_mask(north, south, north_pts, south_pts, &fld_n[0][0], &fld_s[0][0]);

/* Write out full ssmi and concentration data files */
  fwrite(north, sizeof(ssmi), north_pts, outn);
  fwrite(south, sizeof(ssmi), south_pts, outs);
  fclose(outn);
  fclose(outs);

/* Expt: Write out the error counts by type */
  if (lines_used > 0) {
    printf("Total lores spots: %8d  Error spots %8d fraction %6.4f\n",
            lines_used*NSCANS, err_spots, 
            (float) err_spots / (float) (NSCANS*lines_used) );
    printf("Error stats\n");
    printf("  stype %d lat %d lon %d\n",err_stype, err_lat, err_lon);
    printf("  range 19v %5d 19h %5d 22v %5d 37v %5d 37h %5d 85v %5d 85h %5d\n",
      err_19v_range, err_19h_range, err_22v_range,
      err_37v_range, err_37h_range, err_85v_range, err_85h_range);
    printf("  polar 19GHz %5d 37GHz %5d 85GHz %6d\n",
      err_19_polar, err_37_polar, err_85_polar);
    printf("  bad_low %4d crop_low %4d bad high %4d\n",bad_low, crop_low, bad_high);
    printf("  filt37 %d filt22 %d efilt_37_n %6d efilt_37_s %6d\n",
      filt37, filt22, efilt_37_n, efilt_37_s);
  }
  else {
    printf("Did not find data for satellite %2d\n",satno - 233);
  }

/* Expt: Write out the data count maps */
  #ifdef VERBOSE
    outn = fopen("count.n", "w");
    outs = fopen("count.s", "w");
    getfld(north, north_pts, &fld_n[0][0], &rfld_n[0][0], COUNT);
    getfld(south, south_pts, &fld_s[0][0], &rfld_s[0][0], COUNT);
    fwrite(fld_n, sizeof(unsigned char), north_pts, outn);
    fwrite(fld_s, sizeof(unsigned char), south_pts, outs);
    fclose(outn);
    fclose(outs);
  #endif

  return 0;

}

#include "algorithm.C"
#include "hires.C"
#include "getfld.C"
#include "icetools.C"
#include "process_bufr.C"
#include "filt.C"
#include "pole_fill.C"
