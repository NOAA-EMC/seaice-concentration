#include <stdio.h>

#define NFREQS 7

#define LAND 157
#define MIXED 195

/* For C++ emulation from c */
typedef int bool;
#define false (1==0)
#define true  (1==1)

int tb_filter(float *tb, float *concentration, int *flag, int satno) ;

int waters(float tb1, float tb2, float crit, bool under, float *concentration, int *qc, float *land) ;
int lands (float tb1, float tb2, float crit, bool under, float *concentration, int *qc, float *land) ;

int tb_filter(float *tb, float *concentration, int *flag, int satno) {

/* Derived from F15: (50 or 315 are approx ignore) */
                          /* 19v  19h   22v   37v   37h   85v   85h */
  float hot_f15[NFREQS]  = {268., 259., 315., 265., 257., 264., 264.} ; 
  float cold_f15[NFREQS] = {175.,  96.,  50., 184., 126., 163., 156.};

/* For F13, F14: */
  float hot_f13[NFREQS]  = {270., 263., 270., 267., 262., 270., 263.} ; 
  float cold_f13[NFREQS] = {176.,  50., 185., 195.,  50., 184., 174.};

  float *thot, *tcold;
  int i, j;
  bool hot = false, cold = false, is_land = false;

  *flag = 0;
  *concentration = 2.24;

  if (satno == 15 || satno == 248) {
    thot  = &hot_f15[0];
    tcold = &cold_f15[0];
  }
  else {
    thot  = &hot_f13[0];
    tcold = &cold_f13[0];
  }

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

  #ifdef DEBUG
  printf("debug: entered waters, tb = %f %f %f\n",tb1, tb2, dr); fflush(stdout);
  #endif
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
