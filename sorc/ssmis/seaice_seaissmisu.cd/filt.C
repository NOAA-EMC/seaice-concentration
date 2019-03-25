#include <cstdio>

/* Perform extended weather filtering (per OMB Tech Note 120) on
     SSMI data for sea ice use */
/* Robert Grumbine 4 June 1997 */
 
float gr37(const ssmis *map, const int i, const int j, 
           const int nx, const int ny, const int range);
float gr22(const ssmis *map, const int i, const int j, 
           const int nx, const int ny, const int range);

/* Variables for tracking which filters get used */
/* 16 March 2004 */
extern int efilt_37_n, efilt_37_s;
extern int efilt_22_n, efilt_22_s;

int ssmis_newfilt(ssmis *nmap, ssmis *smap) {
  int i, j;
  float g37[NY_NORTH][NX_NORTH];
  float g22[NY_NORTH][NX_NORTH];
  bool debug;
  unsigned char nconc[NY_NORTH][NX_NORTH], sconc[NY_SOUTH][NX_SOUTH];
  int range;
 
  #ifdef VERBOSE
    debug = true;
  #else
    debug = false;
  #endif
  #ifdef HIRES
    range = 1;
  #else
    range = 0;
  #endif

  ssmis_getfld(nmap, NX_NORTH*NY_NORTH, &nconc[0][0], &g37[0][0], SSMIS_BAR_CONC);
  ssmis_getfld(smap, NX_SOUTH*NY_SOUTH, &sconc[0][0], &g37[0][0], SSMIS_BAR_CONC);

/* Find the northern hemisphere gradient ratio */
  for (j = 0; j < NY_NORTH  ; j++) {
    for (i = 0; i < NX_NORTH  ; i++) {
      g37[j][i] = gr37(nmap, i, j, NX_NORTH, NY_NORTH, range);
      g22[j][i] = gr22(nmap, i, j, NX_NORTH, NY_NORTH, range);
    }
  }
/* Loop over all points.  If, in any case, the 2 pt averaged gradient ratio 
     for 37-19 v is greater than the cut off, then filter out the ice 
     concentration */

  for (j = 1; j < NY_NORTH - 1 ; j++) {
    for (i = 1; i < NX_NORTH - 1 ; i++) {
      if (nconc[j][i] != 0 && nconc[j][i] != BAD_DATA) {
        /* Include this as either second check (low resolution) or
           an appropriately-ranged check (high resolution 
           Robert Grumbine 16 March 2004 */
        if (g37[j][i] > SSMIS_GR37LIM) {
           efilt_37_n += 1;
           nconc[j][i] = WEATHER;
        }
        if (g22[j][i] > SSMIS_GR22LIM) {
           efilt_22_n += 1;
           nconc[j][i] = WEATHER;
        }

        if (nconc[j+1][i] != BAD_DATA) {
          if (g37[j+1][i] + g37[j][i] > 2*SSMIS_GR37LIM ) {
            efilt_37_n += 1;
            nconc[j][i] = WEATHER;
            if (debug) printf("1 resetting %3d %3d \n",i,j);
            continue;
          }
        }
        if (nconc[j-1][i] != BAD_DATA) {
          if (g37[j-1][i] + g37[j][i] > 2*SSMIS_GR37LIM ) {
            efilt_37_n += 1;
            nconc[j][i] = WEATHER;
            if (debug) printf("2 resetting %3d %3d \n",i,j);
            continue;
          }
        }
        if (nconc[j][i-1] != BAD_DATA) {
          if (g37[j][i-1] + g37[j][i] > 2*SSMIS_GR37LIM ) {
            efilt_37_n += 1;
            nconc[j][i] = WEATHER;
            if (debug) printf("3 resetting %3d %3d \n",i,j);
            continue;
          }
        }
        if (nconc[j][i+1] != BAD_DATA) {
          if (g37[j][i+1] + g37[j][i] > 2*SSMIS_GR37LIM ) {
            efilt_37_n += 1;
            nconc[j][i] = WEATHER;
            if (debug) printf("4 resetting %3d %3d \n",i,j);
            continue;
          }
        }
      
      } /* End of filtration testing */

    }
  }

  for (j = 0; j < NY_NORTH; j++) {
    for (i = 0; i < NX_NORTH; i++) {
       nmap[j*NX_NORTH +i].bar_conc = nconc[j][i];
    }
  }

/* Need to put southern filtering in here */
  for (j = 0; j < NY_SOUTH  ; j++) {
    for (i = 0; i < NX_SOUTH  ; i++) {
      g37[j][i] = gr37(smap, i, j, NX_SOUTH, NY_SOUTH, range);
      g22[j][i] = gr22(smap, i, j, NX_SOUTH, NY_SOUTH, range);
    }
  }
/* Note above that we've used the same array for both north and south
   gradients */
  for (j = 1; j < NY_SOUTH - 1; j++) {
    for (i = 1; i < NX_SOUTH - 1; i++) {
      if (sconc[j][i] != 0 && sconc[j][i] != BAD_DATA) {
        /* Include this as either second check (low resolution) or
           an appropriately-ranged check (high resolution 
           Robert Grumbine 16 March 2004 */
        if (g37[j][i] > SSMIS_GR37LIM) {
           efilt_37_s += 1;
           sconc[j][i] = WEATHER;
        }
        if (g22[j][i] > SSMIS_GR22LIM) {
           efilt_22_s += 1;
           sconc[j][i] = WEATHER;
        }

        if (sconc[j+1][i] != BAD_DATA) {
          if (g37[j+1][i] + g37[j][i] > 2*SSMIS_GR37LIM ) {
            efilt_37_s += 1;
            sconc[j][i] = WEATHER;
            if (debug) printf("1 resetting %3d %3d \n",i,j);
            continue;
          }
        }
        if (sconc[j-1][i] != BAD_DATA) {
          if (g37[j-1][i] + g37[j][i] > 2*SSMIS_GR37LIM ) {
            efilt_37_s += 1;
            sconc[j][i] = WEATHER;
            if (debug) printf("2 resetting %3d %3d \n",i,j);
            continue;
          }
        }
        if (sconc[j][i-1] != BAD_DATA) {
          if (g37[j][i-1] + g37[j][i] > 2*SSMIS_GR37LIM ) {
            efilt_37_s += 1;
            sconc[j][i] = WEATHER;
            if (debug) printf("3 resetting %3d %3d \n",i,j);
            continue;
          }
        }
        if (sconc[j][i+1] != BAD_DATA) {
          if (g37[j][i+1] + g37[j][i] > 2*SSMIS_GR37LIM ) {
            efilt_37_s += 1;
            sconc[j][i] = WEATHER;
            if (debug) printf("4 resetting %3d %3d \n",i,j);
            continue;
          }
        }
      
      } /* End of filtration testing */

    }
  }

  for (j = 0; j < NY_SOUTH; j++) {
    for (i = 0; i < NX_SOUTH; i++) {
       smap[j*NX_SOUTH + i].bar_conc = sconc[j][i];
    }
  }

  return 0;
}


float gr37(const ssmis *map, const int i, const int j,
                            const int nx, const int ny, const int range)
{
   int index, ti, tj, count = 0;
   float t19v = 0.0, t37v = 0.0;

   if (range != 0) {
     for (tj = j-range ; tj < j+range ; tj++) {
       for (ti = i - range ; ti < i+range; ti++) {
         index = ti + tj*nx;
         if (index < 0 || index >= nx*ny) continue;
         if (map[index].t19v != 0 && map[index].t37v != 0) {
           count += 1;
           t19v += map[index].t19v;
           t37v += map[index].t37v;
         }
       }
     }

     t37v = t37v / count;
     t19v = t19v / count;
   }
   else {
     index = i + j*nx;
     t19v = map[index].t19v;
     t37v = map[index].t37v;
   }

   if (t19v != 0.0 && t37v != 0.0 ) {
     return ((t37v - t19v) / (t37v + t19v));
   }
   else {return 0.0;}

   return 0.0;
}

float gr22(const ssmis *map, const int i, const int j, 
                            const int nx, const int ny, const int range)
{
   int index, ti, tj, count = 0;
   float t19v = 0.0, t22v = 0.0;

   if (range != 0) {
     for (tj = j-range ; tj < j+range ; tj++) {
       for (ti = i - range ; ti < i+range; ti++) {
         index = ti + tj*nx;
         if (index < 0 || index >= nx*ny) continue; 
         if (map[index].t19v != 0 && map[index].t22v != 0) {
           count += 1;
           t19v += map[index].t19v;
           t22v += map[index].t22v;
         }
       }
     }
  
     t22v = t22v / count;
     t19v = t19v / count;
   }
   else {
     index = i + j*nx;
     t19v = map[index].t19v;
     t22v = map[index].t22v;
   }

   if (t19v != 0.0 && t22v != 0.0 ) {
     return ((t22v - t19v) / (t22v + t19v));
   }
   else {return 0.0;}
   
   return 0.0;
}
