#include <cmath>
#include <cmath>
#include <cstdio>


/* Elements specific to the high resolution SSMI algorithm and processing */
/* Note that almost the entire program is identical to the low-res version */
/* This is necessarily the case as the goal is to produce something which */
/*   is maximally correspondant to the old processing, just higher resolution */
/* ************************************************ */
int imax(int x, int y);
int imax(int x, int y) {
  if (x > y) {
    return x;
  }
  return y;
}
int imin(int x, int y);
int imin(int x, int y) {
  if (x < y) {
    return x;
  }
  return y;
}
/* ************************************************ */
void landuse(bool *use, unsigned char *mask, ssmis *obs, 
             const int nx, const int ny,
             const int polei, const int polej, const int latrange) ;
void regress(float *test, float *conc, int count, float *a, float *b, 
                                                float *r) ;

float hires(ssmis *map, int nx, int ny, int polei, int polej, 
                      unsigned char *mask) {
  float *conc, *test, a, b, r;
  bool *use;
  float fconc;
  int i, count = 0, tconc, tconc2;
  int latrange = 8*30;  
  float th, tv;

  #ifdef VERBOSE
    printf("entered hires, nx, ny = %d %d\n",nx, ny); fflush(stdout);
  #endif
/* find the points which are usable -- good tb's, not too close to land */
  use  = new bool[nx*ny];
  conc = new float[nx*ny];
  test = new float[nx*ny];
  if (use == (bool*) NULL) {
    printf("failed to new use in hires\n");
    return -1;
  }
  if (conc == (float*) NULL) {
    printf("failed to new conc in hires\n");
    return -1;
  }
  if (test == (float*) NULL) {
    printf("failed to new test in hires\n");
    return -1;
  }
  #ifdef VERBOSE
    printf("past new in hires, polei, polej = %d %d\n",polei, polej); 
    fflush(stdout);
  #endif

  landuse(use, mask, map, nx, ny, polei, polej, latrange);
  #ifdef VERBOSE
    printf("back from landuse\n"); fflush(stdout);
  #endif


  for (i = 0; i < nx*ny; i++) {
    if (use[i]) {
      th = (float) map[i].t92h;
      tv = (float) map[i].t92v;

      if (tv > th) {
        conc[count] = map[i].conc_bar;
        test[count] = sqrt( fabs(tv*tv - th*th) );
        count += 1;
      }
    }
  }
  printf("hires count = %d\n", count); fflush(stdout);

/* Perform the linear regression */
  regress(test, conc, count, &a, &b, &r);
  printf("regression, %f %f  %f\n",a,b,r); fflush(stdout);


  for (i = 0; i < nx*ny; i++) {
    tconc = (int) map[i].conc_bar;
    tconc2 = (int) map[i].bar_conc;
    if (tconc != WEATHER && tconc != BAD_DATA && tconc != NO_DATA &&
        tconc2 != WEATHER && tconc2 != BAD_DATA && tconc2 != NO_DATA) {
      th = (float) map[i].t92h;
      tv = (float) map[i].t92v;
      fconc = a + b * sqrt(fabs(tv*tv - th*th) ); 
      if (fconc < MIN_CONC) fconc = 0;
      if (fconc > MAX_CONC) fconc = BAD_DATA;
      map[i].hires_conc = (unsigned char) (0.5 + fconc);
    }
    else {
      if (tconc2 == WEATHER || tconc == WEATHER) {
        map[i].hires_conc = WEATHER;
      }
      else {
        map[i].hires_conc = (unsigned char) tconc;
      }
      if (map[i].hires_conc != map[i].conc_bar) {
         printf("wild failure in hires %d vs. %d \n",(int) map[i].hires_conc, 
              (int) map[i].conc_bar );
      }
    }
  }

  free(use);
  free(conc);
  free(test);
  /* Return the correlation as a quality parameter */
  return r;
}

void landuse(bool *use, unsigned char *mask, ssmis *obs, 
             const int nx, const int ny,
             const int polei, const int polej, const int latrange) {
  int i, j;
  int k, l, index, tindex, tconc;
  int range = 5;

  #ifdef VERBOSE
    printf("entered landuse\n"); fflush(stdout);
    printf("nx, ny, polei, polej, latrange, range = %d %d %d %d  %d %d\n",
        nx, ny, polei, polej, latrange, range);
    fflush(stdout);
  #endif

  for (j = 0; j < ny; j++) {
  for (i = 0; i < nx; i++) {
    index = i + j*nx;
    #ifdef VERBOSE
      printf("landuse %d %d  %d\n",i,j,index);fflush(stdout);
    #endif 
    use[index] = true;
    tconc = (int) obs[index].conc_bar;
    if (tconc == WEATHER || tconc == BAD_DATA || tconc == NO_DATA ||
        mask[index] == LAND || mask[index] == COAST || tconc > 99 ) {
      use[index] = false;
    }
    else {
      for (l = imax(0,j - range); l <= imin(ny-1,j + range); l++) {
      for (k = imax(0,i - range); k <= imin(nx-1,i + range); k++) {
        tindex = k + l*nx;
        #ifdef VERBOSE
          printf("ranging %3d %3d %6d  %3d %3d %6d  %3d\n",i, j, index, 
                      k, l, tindex, mask[tindex]);fflush(stdout);
        #endif 
        if (mask[tindex ] == LAND || mask[tindex ] == COAST) {
          use[index] = false;
          continue;
        }
      }
      }
    }
/* Now ensure that only high latitude values are used -- exclude Gulf
   Stream storms, etc. */
    if ( use[index] &&
        ((i-polei)*(i-polei)+(j-polej)*(j-polej)) > latrange*latrange) {
      use[index] = false;
    }

  }
  }    

  return;
}
void regress(float *test, float *conc, int count, float *a, float *b, 
             float *r) {
   float sx = 0., sy = 0., sxy = 0., sxx = 0., syy = 0., det;
   float tconc;
   int i;
 
   for (i = 0; i < count; i++) {
      sx += test[i];
      sy += conc[i];
      sxy += test[i]*conc[i];
      sxx += test[i]*test[i];
      syy += conc[i]*conc[i];
   }
   det = count * sxx - sx*sx;
   if (det == 0.) {
     printf("Singular in regress, cannot construct regression.\n");
     *a = 0.0;
     *b = 0.0;
     *r = 0.0;
     return;
   }

   *b = (count *sxy - sx*sy)/det;
   *a = (sy - *b*sx)/count;
   *r = (count *sxy - sx*sy) / sqrt(det) / sqrt(count*syy - sy*sy) ;
   printf("In regress, a, b, r = %f %f %f\n",*a, *b, *r); fflush(stdout);

   sx = 0.;
   sxx = 0.;
   for (i = 0; i < count; i++) {
     sx  +=  *a + (*b)*test[i] - conc[i];
     sxx += (*a + (*b)*test[i] - conc[i])*(*a + (*b)*test[i]-conc[i]);
   }
   printf("sample bias, rms %f %f\n", sx/count, sqrt(sxx/count) ); 
   fflush(stdout); 

   for (i = 0; i < count; i++) {
     tconc = *a + (*b)*test[i];
     if (fabs(tconc - conc[i]) > 3.*sqrt(sxx/count) ) {
        printf("%f %f %f  %5.1f %5.1f %6.1f big delta\n",*a, *b, 
             test[i], conc[i], tconc, tconc - conc[i]);
        fflush(stdout);
     }
   }

   return;
}

int min(int x, int y) {
  if (x < y) { return x; }
  else { return y;}
}
