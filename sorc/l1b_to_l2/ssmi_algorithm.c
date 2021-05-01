#include <stdio.h>
/* Various defined values/flags/...  Moved here from ncepgrids 2006 Jan 30 */
#define BAD_DATA 166
#define NO_DATA  224
#define MIN_CONC  15
#define MAX_CONC 127
#define MAX_ICE  128


/* For error/filtration statistics */
extern int bad_low;
extern int bad_high;
extern int crop_low;
extern int filt37;
extern int filt22;


void sat_regress(float t19v, float t19h, float t22v, float t37v, float t37h,
              float t85v, float t85h, 
              float *nt19v, float *nt19h, float *nt22v, float *nt37v, 
              float *nt37h, float *nt85v, float *nt85h, int satno);

float nasa_team(float t19v, float t19h, float t22v, 
                float t37v, float t37h, 
                float t85v, float t85h, 
                const char pole, const int ant, const int satno) {
/* Implementation of the NASA Team algorithm with 22 GHz weather
     filter.  C version by Robert Grumbine, based on Fortran code by
     M. Martino.  3/23/94.

C                                                                
C   THIS PROGRAM USES THE TEAM ALGORITHM TO CALCULATE THE TOTAL ICE
C   CONCENTRATION AND THE MULTIYEAR ICE CONCENTRATION.  INPUT ARE 
C   19 VERT, 19 HORZ AND 37 VERT, 37 HORZ BRIGHTNESS TEMPERATURES.
C                                                                 
C     GR FILTER IS .05                        
C    SSMI TIE POINTS  (1=19H, 4=37V)

C                              
*/
/* North Pole tie points  F-17, Walt Meier */
      /* Order is 19H, 19V, 22V, 37V */
/*       float t0n[7] = {116.5, 182.2, 00.0, 206.5, 0., 0., 0.};
      float tfn[7] = {235.4, 251.7, 00.0, 242.7, 0., 0., 0.};
      float tmn[7] = {199.0, 223.4, 00.0, 188.1, 0., 0., 0.};    */
/* NORTH POLE TIE POINTS 08 MAR 91 F-8 */
      float t0n[7] = {100.8, 177.1, 00.0, 201.7, 0., 0., 0.};
      float tfn[7] = {242.8, 258.2, 00.0, 252.8, 0., 0., 0.};
      float tmn[7] = {203.9, 223.2, 00.0, 186.3, 0., 0., 0.};


/* SOUTH POLE TIE POINTS */
/*       float t0s[7] = {118.4, 187.7, 00.0, 208.9, 0., 0., 0.};
      float tfs[7] = {241.1, 256.2, 00.0, 246.4, 0., 0., 0.};
      float tms[7] = {214.8, 246.9, 00.0, 212.6, 0., 0., 0.};    */
/* SOUTH POLE TIE POINTS  F-8 (regress others to this) */
      float t0s[7] = {100.3, 176.6, 00.0, 200.5, 0., 0., 0.};
      float tfs[7] = {237.8, 249.8, 00.0, 243.3, 0., 0., 0.};
      float tms[7] = {193.7, 221.6, 00.0, 190.3, 0., 0., 0.};


/* tie points within function */
      float *T0, *TF, *TM;
      float DW[2], DF[2], DM[2];
      float SW[2], SF[2], SM[2];

/* Local variables */
      float gr37, gr22, polar;
      float nf19, nm19, dd19, fy, my, total;
      float a1, b1, c1, d1, e1, f1, g1, h1, i1, j1, k1, l1 ;
      float nt19v, nt19h, nt22v, nt37v, nt37h, nt85v, nt85h;

/* For new weather filtering 18 August 2010 */
      float grYY;

/* Preliminary check on whether the data are useable */
       if ( (t22v + t19v) == 0. || (t19v + t19h) == 0. || (t37v + t19v)==0. )
       {
         return (float) BAD_DATA;
       }

/*
C   CALCULATE PARAMETERS FOR ICE CONCENTRATION ALGORITHM          
*/                                                                 
                           
      if (pole == 'n') {
        T0 = &t0n[0];
        TF = &tfn[0];
        TM = &tmn[0];
      }
      else if (pole == 's') {
        T0 = &t0s[0];
        TF = &tfs[0];
        TM = &tms[0];
      }
      else {
        printf("specified a pole that doesn't exist!!\n");
        return -1.;
      }  

       DW[0]=T0[1]-T0[0];
       DF[0]=TF[1]-TF[0];
       DM[0]=TM[1]-TM[0];

       SW[0]=T0[1]+T0[0];
       SF[0]=TF[1]+TF[0];
       SM[0]=TM[1]+TM[0];

       DW[1]=T0[3]-T0[1];
       DF[1]=TF[3]-TF[1];
       DM[1]=TM[3]-TM[1];

       SW[1]=T0[3]+T0[1];
       SF[1]=TF[3]+TF[1];
       SM[1]=TM[3]+TM[1];

       a1=DM[0]*DW[1]-DM[1]*DW[0];
       b1=DM[1]*SW[0]-DW[1]*SM[0];
       c1=DW[0]*SM[1]-DM[0]*SW[1];
       d1=SM[0]*SW[1]-SM[1]*SW[0]; 
/*       d1=SM[0]*SW[0]-SM[1]*SW[0]; */

       i1=DF[1]*DW[0]-DF[0]*DW[1];
       j1=DW[1]*SF[0]-DF[1]*SW[0];
       k1=SW[1]*DF[0]-DW[0]*SF[1];
       l1=SF[1]*SW[0]-SF[0]*SW[1];

       e1=DF[0]*(DM[1]-DW[1])+DW[0]*(DF[1]-DM[1])+DM[0]*(DW[1]-DF[1]); 
       f1=DF[1]*(SM[0]-SW[0])+DW[1]*(SF[0]-SM[0])+DM[1]*(SW[0]-SF[0]); 
       g1=DF[0]*(SW[1]-SM[1])+DW[0]*(SM[1]-SF[1])+DM[0]*(SF[1]-SW[1]); 
/*       g1=DF[0]*(SW[0]-SM[1])+DW[0]*(SM[1]-SF[1])+DM[0]*(SF[1]-SW[1]); */
       h1=SF[1]*(SW[0]-SM[0])+SW[1]*(SM[0]-SF[0])+SM[1]*(SF[0]-SW[0]);

/* Recompute the brightness temperatures, if needed, via the Abdalati, 
   1995 calibration of F-8 versus F-11, or Grumbine verification of
   F-11 to F-8 being ok for F13,14,15 as well*/
/* Regressions for F-18, F-16 to F-17 now, too 9 July 2014 */
         sat_regress(t19v, t19h, t22v, t37v, t37h, t85v, t85h, 
                  &nt19v, &nt19h, &nt22v, &nt37v, &nt37h, &nt85v, &nt85h, 
                  satno); 

/* Rely on external weather/land/water filters. Should only be here with
     good reason to believe it's ice we're looking at */

/* Now, finally, compute the ice concentration */
       gr22  =  (nt22v - nt19v) / (nt22v + nt19v);
       polar =  (nt19v - nt19h) / (nt19v + nt19h);
       gr37  =  (nt37v - nt19v) / (nt37v + nt19v);

       total = (float) NO_DATA;

       nf19=a1+b1*polar+c1*gr37+d1*polar*gr37;
       nm19=i1+j1*polar+k1*gr37+l1*polar*gr37;
       dd19=e1+f1*polar+g1*gr37+h1*polar*gr37;
       if (dd19 == 0.) {  printf("zero divisor for concentrations\n");
                          return (float) BAD_DATA; }
       fy = nf19/dd19;
       my = nm19/dd19;
       total = (my + fy)*100.;
       if (total < -20.0 ) {
          bad_low += 1;
          return (float) BAD_DATA;
       }
       if (total <   0.0 ) {
          crop_low += 1;
          total = 0.; 
       }
       if (total > MAX_CONC) {
          bad_high += 1;
          return (float) BAD_DATA;
       }
      
      return total;
}
void sat_regress(float t19v, float t19h, float t22v, float t37v, float t37h,
              float t85v, float t85h, 
              float *nt19v, float *nt19h, float *nt22v, 
              float *nt37v, float *nt37h,
              float *nt85v, float *nt85h, int satno) {
/* Recompute the brightness temperatures if needed for, say, new instruments */

/* Recompute the brightness temperatures via the Abdalati, 1995 calibration
   of F-8 versus F-11, if needed */
   int tsat;
   if (satno > 20) {
     tsat = satno - 233;
   }
   else {
     tsat = satno;
   }

   if (tsat == 8) {
       *nt19h = t19h;
       *nt19v = t19v;
       *nt22v = t22v;
       *nt37h = t37h;
       *nt37v = t37v;
       *nt85v = t85v;
       *nt85h = t85h;
   }
   else if (tsat == 11 || tsat == 13 || tsat == 14 || tsat == 15) {
       *nt19h = 1.013 * t19h + (-1.89);
       *nt19v = 1.013 * t19v + (-2.51);
       *nt22v = 1.014 * t22v + (-2.73);
       *nt37h = 1.024 * t37h + (-4.22);
       *nt37v = 1.000 * t37v + ( 0.052);
       *nt85v = t85v;
       *nt85h = t85h;
   }



   return;
}
