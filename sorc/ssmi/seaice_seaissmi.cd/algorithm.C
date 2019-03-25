#include <cstdio>

/* For error/filtration statistics */
extern int bad_low;
extern int bad_high;
extern int crop_low;
extern int filt37;
extern int filt22;


void antenna(float *t19v, float *t19h, float *t22v, float *t37v, float *t37h,
             float *t85v, float *t85h);

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
C   COMPLETELY RE-WRITTEN FOR THE PC 16MAY91 M.G.MARTINO, STX CORP.
C   22V-19V GR WEATHER FILTER ADDED 22 JULY 93 BY MGM
C                                                                 
C     GR FILTER IS .05                        
C    SSMI TIE POINTS  (1=19H, 4=37V)
C                              
C   Revised to use the Abdalati regreession coefficients for F-11, 
C     with F-8 tie points.  95/06/22.
C   Revised to make antenna to brightness temperature correction 95/06/22.
*/
/* NORTH POLE TIE POINTS 08 MAR 91 F-8 */                           
      float t0n[7] = {100.8, 177.1, 00.0, 201.7, 0., 0., 0.};
      float tfn[7] = {242.8, 258.2, 00.0, 252.8, 0., 0., 0.};
      float tmn[7] = {203.9, 223.2, 00.0, 186.3, 0., 0., 0.};
/* North Pole tie points Jan 13, 1992 F-11*/
/*      float t0n[7] = { 99.8, 177.3, 00.0, 202.5, 0., 0., 0.};  */                 
/*      float tfn[7] = {239.5, 249.7, 00.0, 244.2, 0., 0., 0.}; */
/*      float tmn[7] = {202.3, 221.3, 00.0, 184.6, 0., 0., 0.}; */

/* SOUTH POLE TIE POINTS 12 FEB 91 F-8 */
      float t0s[7] = {100.3, 176.6, 00.0, 200.5, 0., 0., 0.};
      float tfs[7] = {237.8, 249.8, 00.0, 243.3, 0., 0., 0.};
      float tms[7] = {193.7, 221.6, 00.0, 190.3, 0., 0., 0.};

/* South pole tie points 22 Nov 1994, approved 14 Oct 1994 F-11 */
/*      float t0s[7] = {100.9, 177.1, 00.0, 200.4, 0., 0., 0.}; */ /* open */
/*      float tfs[7] = {236.9, 249.0, 00.0, 242.8, 0., 0., 0.}; */ /* first */
/*      float tms[7] = {193.2, 221.3, 00.0, 190.3, 0., 0., 0.}; */ /* multi */

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
         sat_regress(t19v, t19h, t22v, t37v, t37h, t85v, t85h,
                  &nt19v, &nt19h, &nt22v, &nt37v, &nt37h, &nt85v, &nt85h, 
                  satno); 

/* Add 19 August 2010 -- recover utility of F15 even though team
     weather filter relies on 22V channel that is corrupted */
/* Leaving in the old filter as-is since it will only result in over-filtering
     of ice, vs. letting bad data through */
/* Note that this type of filter is probably a good idea for all SSMI, and
     for reruns */
      if (satno == 15 || satno == 248 ) {
        grYY = (t37v-t85v)/(t37v+t85v);
        if (t85h > 266.875610 || grYY < -0.085 ) {
          total = 0;
          return total;
        }
      }

/* Correct from antenna temperatures to brightness temperatures, if
   needed, following Katz codes of 3/6/95 */
       if (ant == 1) {
         antenna(&nt19v, &nt19h, &nt22v, &nt37v, &nt37h, &nt85v, &nt85h);
       }

/* Now, finally, compute the ice concentration */
       gr22  =  (nt22v - nt19v) / (nt22v + nt19v);
       polar =  (nt19v - nt19h) / (nt19v + nt19h);
       gr37  =  (nt37v - nt19v) / (nt37v + nt19v);

       total = (float) NO_DATA;
       //if ( (gr37 <= (float) GR37LIM) && (gr22 <= (float) GR22LIM) ) {
       //if ( old_weather(nt19v, nt19h, nt22v, nt37v, nt37h, nt85v, nt85h, satno) != WEATHER) {
       // Note that the new weather filter was developed against the antenna temperatures,
       //   rather than the brightness temperatures (t rather than nt)
       if ( new_weather(t19v, t19h, t22v, t37v, t37h, t85v, t85h, satno) != WEATHER) {
           if (new_weather(t19v, t19h, t22v, t37v, t37h, t85v, t85h, satno) == 0) return 0;

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
       }
       else {
/* Set weather filtered points to weather flag 
   Tracking of which filter was triggered added 16 March 2004 */
        if (gr37 > SSMI_GR37LIM) filt37 += 1;
        if (gr22 > SSMI_GR22LIM) filt22 += 1;
        total = (float) WEATHER; 
       }
      
      return total;
}
void antenna(float *TB19V, float *TB19H, float *TB22V, 
             float *TB37V, float *TB37H, float *TB85V, float *TB85H)
{
      float YES19V, YES19H, YES22V, YES37V, YES37H, YES85V, YES85H;

      float AP19V = 0.969, AP19H = .969, AP22V = .974;
      float AP37V = .986, AP37H = .986,  AP85V = .988, AP85H = .988;
      float BP19V = .00473, BP19H = .00415, BP22V = .0107; 
      float BP37V = .0217, BP37H = .02612, BP85V = .01383, BP85H = .01947;

      float C19V, C19H, C22V, C37V, C37H, C85V, C85H;
      float D19V, D19H, D22V, D37V, D37H, D85V, D85H;

      C19V = 1/(AP19V*(1-BP19V));
      C19H = 1/(AP19H*(1-BP19H));
      C22V = 1/(AP22V*(1-BP22V));
      C37V = 1/(AP37V*(1-BP37V));
      C37H = 1/(AP37H*(1-BP37H));
      C85V = 1/(AP85V*(1-BP85V));
      C85H = 1/(AP85H*(1-BP85H));
      D19V = C19V*BP19V;
      D19H = C19H*BP19H;
      D22V = C22V*BP22V;
      D37V = C37V*BP37V;
      D37H = C37H*BP37H;
      D85V = C85V*BP85V;
      D85H = C85H*BP85H;

        YES19V = C19V * (*TB19V) - D19V * (*TB19H);
        YES19H = C19H * (*TB19H) - D19H * (*TB19V);
        YES22V = C22V * (*TB22V) - D22V * (0.653 * (*TB19H) + 96.6);
        YES37V = C37V * (*TB37V) - D37V * (*TB37H);
        YES37H = C37H * (*TB37H) - D37H * (*TB37V);
        YES85V = C85V * (*TB85V) - D85V * (*TB85H);
        YES85H = C85H * (*TB85H) - D85H * (*TB85V);

     *TB19V = YES19V;
     *TB19H = YES19H;
     *TB22V = YES22V;
     *TB37V = YES37V;
     *TB37H = YES37H;
     *TB85V = YES85V;
     *TB85H = YES85H;

     return;
}     
void sat_regress(float t19v, float t19h, float t22v, float t37v, float t37h,
              float t85v, float t85h,
              float *nt19v, float *nt19h, float *nt22v, 
              float *nt37v, float *nt37h,
              float *nt85v, float *nt85h, int satno) {
/* Recompute the brightness temperatures via the Abdalati, 1995 calibration
   of F-8 versus F-11, if needed */
   if (satno == 8) {
       *nt19h = t19h;
       *nt19v = t19v;
       *nt22v = t22v;
       *nt37h = t37h;
       *nt37v = t37v;
       *nt85v = t85v;
       *nt85h = t85h;
   }
   else if (satno == 11 || satno == 13 || satno == 14 || satno == 15) { 
       *nt19h = 1.013 * t19h + (-1.89);
       *nt19v = 1.013 * t19v + (-2.51);
       *nt22v = 1.014 * t22v + (-2.73);
       *nt37h = 1.024 * t37h + (-4.22);
       *nt37v = 1.000 * t37v + ( 0.052);
       *nt85v = t85v;
       *nt85h = t85h;
   }
   else {
     printf("unknown satellite\n");
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
