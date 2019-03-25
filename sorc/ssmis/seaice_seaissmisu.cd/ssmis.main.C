#include <cstdio>
#include <cstdlib>
#include <cmath> 

#include "icessmis.h"
#include "icegrids.h"

#include "grid_math.h"
#include "team2.C"
#include "pole_fill.C"

/*C$$$  MAIN PROGRAM DOCUMENTATION BLOCK                            */
/*C                                                                 */
/*C MAIN PROGRAM:  SSMI-S      CONSTRUCT SEA ICE CONCENTRATION GRIDS  */
/*C   PRGMMR: ROBERT GRUMBINE  ORG: W/NP21    DATE: 97-06-25        */
/*C                                                                 */
/*C ABSTRACT: READ IN SSMI-S DATA AND CONSTRUCT SEA ICE CONCENTRATION */
/*C    GRIDS USING THE NASA TEAM ALGORITHM.                         */
/*C                                                                 */
/*C PROGRAM HISTORY LOG:                                            */
/*C    97-06-25  ROBERT GRUMBINE                                    */
/*C                                                                 */
/*C USAGE:                                                          */
/*C   FILES ARE HANDLED BY ORDER IN THE INVOKING COMMAND LINE       */ 
/*C   INPUT FILES:                                                  */
/*C      FILE 1  - File with names of SSMI-S data files.  UNFORMATTED BINARY GENERATED   */
/*C              -   BY SSMI-S.bufr.X                                 */
/*C      FILE 2  - NORTHERN HEMISPHERE LAND MASK                    */
/*C      FILE 3  - SOUTHERN HEMISPHERE LAND MASK                    */
/*C   OUTPUT FILES:                                                 */
/*C      FILE 4  - NORTHERN HEMISPHERE GRIDDED SSMI-S data            */
/*C      FILE 5  - SOUTHERN HEMISPHERE GRIDDED SSMI-S data            */
/*C      FILE 6  - UNMASKED NORTHERN HEMISPHERE ice CONCENTRATIONS  */
/*C      FILE 7  - UNMASKED SOUTHERN HEMISPHERE ice CONCENTRATIONS  */
/*    Additional Arguments:                                         */
/*        8      - Julian Day                                       */
/*        9      - Year                                             */
/*       10      - Satellite number (F11 = 244 .. F16 = 249         */
/*C                                                                 */
/*C SUBPROGRAMS CALLED:                                             */
/*C      UNIQUE: process_bufr, process_short_bufr, check_bufr,      */
/*C              zero_bufr, newfilt, pole_fill    */
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
/*C   LANGUAGE: STANDARD C++                                     */
/*C   MACHINE:  ANY                                                 */
/*C                                                                 */
/*C$$$                                                              */

/* Main program for decoding the ssmis orbital records. */
/* Derived from ssmi program of 1997-2006 */


/* ************************************************ */
/* The following are for data qc and error tracking */
/* for process_bufr */
int err_19h_range = 0;
int err_19v_range = 0;
int err_22v_range = 0;
int err_37v_range = 0;
int err_37h_range = 0;
int err_92v_range = 0;
int err_92h_range = 0;
int err_150h_range = 0;
int err_19_polar = 0;
int err_37_polar = 0;
int err_92_polar = 0;
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
//float fmax(float x, float y);
//float fmax(float x, float y) {
//  if (x > y) {
//    return x;
//  }
//  return y;
//}

extern float hires(ssmis *map, int nx, int ny, int polei, int polej, 
                      unsigned char *mask) ;

/* ************************************************ */
int main(int argc, char *argv[]) {
  ssmisupt  a_bufr_line;
  FILE *fin, *input, *outn, *outs, *nland, *sland;
  FILE *nraw, *sraw;
  char fname[80];

  ssmis *south, *north;
  ssmis_tmp *north_tmp, *south_tmp;
  float  rfld_n[NY_NORTH][NX_NORTH];
  float  rfld_s[NY_SOUTH][NX_SOUTH];
  unsigned char  fld_n[NY_NORTH][NX_NORTH];
  unsigned char  fld_s[NY_SOUTH][NX_SOUTH];

  int north_pts, south_pts;
  int i, j, nerrs, nproc;
  int jday, satno, ref_year;
  int lines_used = 0, err_spots = 0;
// For team2
  ssmis_team2_tables arctic, antarctic;
                                                                                
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



/* New the ssmis arrays */
  north_tmp = new ssmis_tmp[NX_NORTH*NY_NORTH]; 
  south_tmp = new ssmis_tmp[NX_SOUTH*NY_SOUTH]; 
  north     = new ssmis[NX_NORTH*NY_NORTH]; 
  south     = new ssmis[NX_SOUTH*NY_SOUTH]; 
  if (north == (ssmis*) NULL || south == (ssmis*) NULL || 
      south_tmp == (ssmis_tmp*) NULL || north_tmp == (ssmis_tmp*) NULL) {
    printf("Failed to new one of the ssmis fields!\n");
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
  printf("size of ssmisupt = %d\n",(int) sizeof(ssmisupt) ); fflush(stdout);

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
    printf("Opened %s in orbital process loop\n", fname); fflush(stdout);
    input = fopen(fname, "r");
    if ( input == (FILE *) NULL ) {
       printf("Failed to open %s from orbit process loop\n", fname); fflush(stdout);
       break;
    }
  

/* Begin stripping out header */
    fseek(input, (long)(0) , SEEK_SET);

/* Begin stripping out data */
    for (i = 0; i < nproc ; i++)
    while (!feof(input) ) {

      /* Add data only if data is within window -- Now assumed that the*/
      /* date-time filtering has been done prior to input */
        j = 0;
        while (!feof(input) ) {  
          #ifdef VERBOSE2
            printf("Process record number %d\n",j); fflush(stdout);
          #endif
          j++;
          fread(&a_bufr_line, sizeof(ssmisupt), 1, input);
          
          #ifdef VERBOSE2
            printf("%d Satno = %d\n",j, a_bufr_line.satid);
          #endif

          if (a_bufr_line.satid == satno ) {
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
  printf("seaice_seaissmisu Done reading all input files\n"); fflush(stdout);
  
  /* Compute averaged tb and concentrations */
  ice_avg_data(north_tmp, south_tmp, north, south, north_pts, south_pts, arctic, antarctic);


  #ifdef HIRES
/* Now that we have a clean grid, work on the high resolution version */
    hires(north, NX_NORTH, NY_NORTH, (int) (0.5+polei_NORTH), 
                                     (int) (0.5+polej_NORTH), &fld_n[0][0]);
    hires(south, NX_SOUTH, NY_SOUTH, (int) (0.5+polei_SOUTH), 
                                     (int) (0.5+polej_SOUTH), &fld_s[0][0]);
    ssmis_getfld(north, north_pts, &fld_n[0][0], &rfld_n[0][0], SSMIS_HIRES_CONC);
    ssmis_getfld(south, south_pts, &fld_s[0][0], &rfld_s[0][0], SSMIS_HIRES_CONC);
  #else
    ssmis_getfld(north, north_pts, &fld_n[0][0], &rfld_n[0][0], SSMIS_BAR_CONC);
    ssmis_getfld(south, south_pts, &fld_s[0][0], &rfld_s[0][0], SSMIS_BAR_CONC);
  #endif

  printf("past getting concentration fields in to fld grids \n"); fflush(stdout);

/* Fill in pole for output purposes */
  ssmis_pole_fill(&fld_n[0][0], 1);
  ssmis_pole_fill(&fld_s[0][0], 2);
  fwrite(fld_n, sizeof(unsigned char), north_pts, nraw);
  fwrite(fld_s, sizeof(unsigned char), south_pts, sraw);

/* Mask out the land now */
  ice_mask(north, south, north_pts, south_pts, &fld_n[0][0], &fld_s[0][0]);

/* Write out full ssmis and concentration data files */
  fwrite(north, sizeof(ssmis), north_pts, outn);
  fwrite(south, sizeof(ssmis), south_pts, outs);
  fclose(outn);
  fclose(outs);

// From here on is doing some system monitoring/checking:

/* Expt: Write out the error counts by type */
  if (lines_used > 0) {
    printf("Total lores spots: %8d  Error spots %8d fraction %6.4f\n",
            lines_used, err_spots, 
            (float) err_spots / (float) (lines_used) );
    printf("Error stats\n");
    printf("  lat %d lon %d\n",err_lat, err_lon);
    printf("  range 19v %5d 19h %5d 22v %5d 37v %5d 37h %5d 92v %5d 92h %5d 150h %5d\n",
      err_19v_range, err_19h_range, err_22v_range,
      err_37v_range, err_37h_range, err_92v_range, err_92h_range, err_150h_range);
    printf("  polar 19GHz %5d 37GHz %5d 92GHz %6d \n",
      err_19_polar, err_37_polar, err_92_polar);
    printf("  bad_low %4d crop_low %4d bad high %4d\n",bad_low, crop_low, bad_high);
    printf("  filt37 %d filt22 %d efilt_37_n %6d efilt_37_s %6d\n",
      filt37, filt22, efilt_37_n, efilt_37_s);
  }
  else {
    printf("Did not find data for satellite %3d\n",satno );
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
