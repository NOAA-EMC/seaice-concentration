#include <stdio.h>

#define NFREQS 7

#define LAND 157
#define MIXED 195

/* For C++ emulation from c */
typedef int bool;
#define false (1==0)
#define true  (1==1)

int tb_filter(float *tb, float *concentration, int *flag) ;

int waters(float tb1, float tb2, float crit, bool under, float *concentration, int *qc, float *land) ;
int lands (float tb1, float tb2, float crit, bool under, float *concentration, int *qc, float *land) ;

int tb_filter(float *tb, float *concentration, int *flag) {

/* Derived from F17: 150 GHz, then usual
  float thot[NFREQS] = {260., 270., 268., 260., 268., 272., 271.} ; 
  float tcold[NFREQS] = {50., 175., 180., 130., 172., 170., 160.};
*/
/* Derived from F15: (50 or 315 are approx ignore) */
                      /* 19v  19h   22v   37v   37h   85v   85h */
  float thot[NFREQS]  = {264., 252., 315., 262., 251., 272., 264.} ; 
  float tcold[NFREQS] = {175.,  96.,  50.,  50., 126., 153., 147.};

  int i, j;
  bool hot = false, cold = false, cold4 = false, is_land = false, mixed = false;

  *flag = 0;
  *concentration = 2.24;

  for (j = 0; j < NFREQS; j++) {
    if (tb[j] > thot[j]) {
      is_land = true;
      #ifdef DEBUG
      printf("land %d %f %f\n",j, tb[j], thot[j]);
      #endif
    }
    if (tb[j] < tcold[j]) {
      is_land = true;
      #ifdef DEBUG
      printf("land %d %f %f\n",j, tb[j], tcold[j]);
      #endif
    }

  } /* nfreqs */

  if (is_land ) {
    *flag = LAND;
    *concentration = 0;
  }

  return 0;
}
/* //////////////////////////////////////////////////////////////// */
int waters(float tb1, float tb2, float crit, bool under, float *concentration, int *qc, float *land) {
  float dr = (tb1-tb2)/(tb1+tb2);

  printf("entered waters, tb = %f %f %f\n",tb1, tb2, dr); fflush(stdout);
/* Filters to flag never-ice water points: */
  if (!under ) {
    if ( dr > crit) {
      *land = 0.01;
      *qc   = 4;
      *concentration = 0.0;
      return 1;
    }
  }
  else {
    if ( dr < crit) {
      *land = 0.02;
      *qc   = 4;
      *concentration = 0.0;
      return 1;
    }
  }

  return 0;
}
/* //////////////////////////////////////////////////////////////////////// */
int lands(float tb1, float tb2, float crit, bool under, float *concentration, int *qc, float *land) {
  float dr = (tb1-tb2)/(tb1+tb2);

/* Filters to flag never-ice land points: */
  if (!under ) {
    if ( dr > crit) {
      *land = 0.97;
      *qc   = 4;
      *concentration = 0.0;
      return 1;
    }
  }
  else {
    if ( dr < crit) {
      *land = 0.98;
      *qc   = 4;
      *concentration = 0.0;
      return 1;
    }
  }

  return 0;
}
