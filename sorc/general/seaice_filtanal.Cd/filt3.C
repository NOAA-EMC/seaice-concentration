#include <cstdio>
#include <cstdlib>
using namespace std;

#include "icessmi.h"
#include "ncepgrids.h"
#include "params.h"

/* Filter the global ice concentration grid according to the SST and
    to the land mask */
/* Note that hooks are in place for filtering the hemispheric ice grids,
    though this isn't done currently. */
/* Robert Grumbine 4 June 1997 */
/* Major revision: change to C++, go to wgrib decoding of sst files
   Robert Grumbine 8 September 1999 */
// Change to 0.5 degree rtg sst 26 April 2004
// Change to 5 minutes ice         August 2004
// Change to 5 minute sst          7 July 2006
// Ensure that SST is in K         28 July 2009

/* Function to fill in the large number of single-point dropouts (bad,
     missing, or weather) that are surrounded by valid ice concentrations */
template <class T>
int gapfill(grid2<T> &x, T lowest_valid, T highest_valid, T flag);
template <class T>
int isolani(metricgrid<T> &x, T flag, T replace); // remove points which 
               //are surrounded by flagged values and replace 
template <class T>
void weather_zero(grid2<T> &x);

//////////////////////////////////
// Something to incorporate in to a library
template <class T, class U>
void gridset(grid2_base<T> &x, grid2_base<U> &y);
//////////////////////////////////


int main(int argc, char *argv[]) {
  FILE *in10, *in11, *in12, *out13, *out14, *out15, *in16, *in17, *in18;

  northhigh<unsigned char> nland, nmap;
  southhigh<unsigned char> sland, smap;
  northhigh<float> nf, count, nlandf;
  southhigh<float> sf, scount, slandf;
  global_12th<unsigned char> cout2, gmap, refmap;
  global_12th<float> outmap, altmap;
  global_12th<float> sst; 

  ijpt ijloc, destloc, ijloc2;
  fijpt tloc;
  latpt outloc;
  float filt_temp, flag;
  int range = 0; //min distance to land for a valid concentration
  int filled, index;

  #ifdef VERBOSE
    printf("Entered the filtering program\n"); fflush(stdout);
  #endif
  outmap.set(0.0);
  altmap.set(0.0);
  count.set(0.0);

  in12 = fopen(argv[1],"r");
  in10 = fopen(argv[2],"r");
  in11 = fopen(argv[3],"r");
  in16 = fopen(argv[7],"r");
  in17 = fopen(argv[9],"r");
  in18 = fopen(argv[10],"r");
  #ifdef VERBOSE
    printf("Finished trying to open input files\n"); fflush(stdout);
  #endif
  if (in10 == (FILE*) NULL || in11 == (FILE*) NULL || in12 == (FILE*) NULL || in16 == (FILE*) NULL ||
      in17 == (FILE*) NULL || in18 == (FILE*) NULL ) {
    printf("Failed to open a required input file in file\n");
    if (in10 == (FILE*) NULL) { printf(" - northern hemisphere file\n");}
    if (in11 == (FILE*) NULL) { printf(" - southern hemisphere file\n");}
    if (in12 == (FILE*) NULL) { printf(" - global sst file\n");}
    if (in16 == (FILE*) NULL) { printf(" - global land mask file\n");}
    if (in17 == (FILE*) NULL) { printf(" - n. hemisphere land mask file\n");}
    if (in18 == (FILE*) NULL) { printf(" - s. hemisphere land mask file\n");}
    return -1;
  }

  out13 = fopen(argv[4],"w");
  out14 = fopen(argv[5],"w");
  out15 = fopen(argv[6],"w");
  #ifdef VERBOSE
    printf("Finished trying to open output files\n"); fflush(stdout);
  #endif
  if (out13 == (FILE*) NULL || out14 == (FILE*) NULL || out15 == (FILE*) NULL) {
    printf("Failed to open a required output file in file\n");
    if (out13 == (FILE*) NULL) { printf(" - global ice file\n");}
    if (out14 == (FILE*) NULL) { printf(" - n. hemisphere ice file\n");}
    if (out15 == (FILE*) NULL) { printf(" - s. hemisphere ice file\n");}
    return -1;
  }
    
//Future: strtol
  filt_temp = (float) atof(argv[8]);
  #ifdef VERBOSE
    printf("Converted filtering temperature: %f \n",filt_temp); fflush(stdout);
  #endif

  nmap.binin(in10);
  nland.binin(in17);
  smap.binin(in11);
  sland.binin(in18);
  sst.binin(in12);
  gmap.binin(in16);
  #ifdef VERBOSE
    printf("Finished reading in data files\n"); fflush(stdout);
  #endif
  // Ensure that values have been rescaled to correct range:
  if (nmap.average() < 2.56) nmap *= 100;
  if (smap.average() < 2.56) smap *= 100;
  gridset(nlandf, nland);
  gridset(slandf, sland);

/* Now average from polar stereographic on to the lat-long grid */
/* RG 8 Sept 1999: In C++, this can now be made a trivial function passing
    a metricgrid in, arbitrary mapping and origin */
//Note that this is now (8 Sep 1999) interpolation, rather than drop in 
//    bucket averaging 
// Mask out a range of grid points near land (set by 'range') 
//    -- note that range = 0, as we're accepting up to the very edge
  flag = 224.;
  gridset(nf, nmap);

  for (index = 0; index < nf.xpoints()*nf.ypoints(); index++) {
    if (nf[index] > 100. && nf[index] < MAX_ICE) {
       nf[index] = 100.;
    }
    else if (nf[index] < MIN_CONC) {
       nf[index] = 0.0;
    }
  }
  for (ijloc.j = 0; ijloc.j < nf.ypoints(); ijloc.j++) {
    for (ijloc.i = 0; ijloc.i < nf.xpoints(); ijloc.i++) {
       if (nlandf.anyof((float) LAND, range, ijloc) > 0) {
         nlandf[ijloc] = flag;
       }
    }
  }
  filled  = gapfill(nf, (float)0., (float)100., (float)224.); //no-data points
  filled += gapfill(nf, (float)0., (float)100., (float)166.); //bad-data points
  filled += gapfill(nf, (float)0., (float)100., (float)177.); //weather points
  weather_zero(nf);
  filled += gapfill(nf, (float)0., (float)100., (float)224.); //no-data points
  filled += gapfill(nf, (float)0., (float)100., (float)166.); //bad-data points

  for (index = 0; index < nf.xpoints()*nf.ypoints(); index++) {
       if (nf[index] == WEATHER) {
         nf[index] = 0.;
       }
       if (nf[index] >= MAX_ICE || nlandf[index] == COAST ) {
        nf[index] = 224.;
        nlandf[index] = flag;
       }
  }

  outmap.fromall(nf, nlandf, flag, 224.);
  #ifdef VERBOSE
  for (ijloc.j = 0; ijloc.j < outmap.ypoints(); ijloc.j++) {
  for (ijloc.i = 0; ijloc.i < outmap.xpoints(); ijloc.i++) {
     if (outmap[ijloc] > 100. && outmap[ijloc] < LAND) {
       outloc = outmap.locate(ijloc);
       tloc = nf.locate(outloc);
       printf("after %3d %3d %5.1f  %5.2f %6.2f  %6.2f %6.2f  %f\n",
         ijloc.i, ijloc.j, outmap[ijloc], outloc.lat, outloc.lon, 
         tloc.i, tloc.j, nlandf[tloc]);
     }
  }
  }
  #endif
       

// Mask out a range of grid points near land
// Transfer values, cap, and then fill in gaps.
  gridset(slandf, sland);
  gridset(sf, smap);

  for (index = 0; index < smap.xpoints()*smap.ypoints(); index++) {
       if (sf[index] > 100. && sf[index] < MAX_ICE) {
         sf[index] = 100.;
       }
       else if (sf[index] < MIN_CONC) {
         sf[index] = 0.0;
       }
  }
  for (ijloc.j = 0; ijloc.j < smap.ypoints(); ijloc.j++) {
    for (ijloc.i = 0; ijloc.i < smap.xpoints(); ijloc.i++) {
       if (slandf.anyof((float) LAND, range, ijloc) > 0) slandf[ijloc] = flag;
    }
  }
  filled += gapfill(sf, (float)0., (float)100., (float)224.); //no-data points
  filled += gapfill(sf, (float)0., (float)100., (float)166.); //bad-data points
  filled += gapfill(sf, (float)0., (float)100., (float)177.); //weather points
  weather_zero(sf);
  filled += gapfill(sf, (float)0., (float)100., (float)224.); //no-data points
  filled += gapfill(sf, (float)0., (float)100., (float)166.); //bad-data points
  printf("filled %d points\n",filled);

// 
  for (ijloc.j = 0; ijloc.j < smap.ypoints(); ijloc.j++) {
    for (ijloc.i = 0; ijloc.i < smap.xpoints(); ijloc.i++) {
       if (sf[ijloc] == WEATHER) {
         sf[ijloc] = 0.;
       }
       if (sf[ijloc] >= MAX_ICE || slandf[ijloc] == COAST ) {
        sf[ijloc] = 224.;
        slandf[ijloc] = flag;
       }
    }
  }
  altmap.fromall(sf, slandf, flag, 224.);
  
  for (ijloc.j = 0; ijloc.j < outmap.ypoints(); ijloc.j++) {
  for (ijloc.i = 0; ijloc.i < outmap.xpoints(); ijloc.i++) {
     outloc = outmap.locate(ijloc);
     if (outloc.lat < -20.) outmap[ijloc] = altmap[ijloc];
     if (outmap[ijloc] > 100.1 && outmap[ijloc] != 224.) {
       printf("%3d %3d %f over\n",ijloc.i, ijloc.j, (float) outmap[ijloc]);
     }
  }
  }

  
// Begin SST filtering: Ensure that the SST are in K
   if (sst.gridmax() < 273) {
      printf("adjusting sst from C to K %f\n",sst.gridmax() );
      sst += 273.15;  // adjust for k if needed, program assumes k
   }
    for (ijloc.j = 0; ijloc.j < outmap.ypoints(); ijloc.j++) {
      for (ijloc.i = 0; ijloc.i < outmap.xpoints() ; ijloc.i++) {
        outloc = outmap.locate(ijloc);
        tloc = sst.locate(outloc);
        ijloc2 = tloc;
        if (sst[ijloc2] >= filt_temp && outmap[ijloc] > 0.0 ) {
          outmap[ijloc] = 0.0; 
        }
        cout2[ijloc] = (unsigned char) (outmap[ijloc] + 0.5); 
      }
    }
/* end SST filtering */

/* Now do land mask filtering */
   for (index = 0; index < outmap.xpoints()*outmap.ypoints(); index++) {
     if (gmap[index] == (unsigned char) LAND) cout2[index] = (unsigned char) 0;
   }

/* Write out the filtered global grid */
     cout2.binout(out13);

/* Place sst filtering of polar stereo grids here */

     return 0;

}

template <class T>
int gapfill(grid2<T> &x, T lowest_valid, T highest_valid, T flag) {
  ijpt loc, ip1, jp1, im1, jm1;
  ijpt tip1, tjp1, tim1, tjm1;
  int count = 0, passno=0, valid, tcount = 0;
  T tmp;
  int xlim = x.xpoints(), ylim= x.ypoints();

  ip1.i =  1; ip1.j =  0;
  im1.i = -1; im1.j =  0;
  jp1.i =  0; jp1.j =  1;
  jm1.i =  0; jm1.j = -1;

//This portion does a 1 point fill -- point surrounded by valid points.
//Must skip bounding 0, [xy]lim as we're looking +- 1 
  count = 0;
  for (loc.j = 1; loc.j < ylim -1 ; loc.j++) {
      tjp1.j = loc.j + 1;
      tjm1.j = loc.j - 1;
  for (loc.i = 1; loc.i < xlim -1 ; loc.i++) {
    if (x[loc] == flag) {
      tip1 = loc ; tip1 += ip1;
      tim1 = loc ; tim1 += im1;
      tjp1.i = loc.i ; 
      tjm1.i = loc.i ; 
      valid = (x[tip1] >= lowest_valid && x[tip1] <= highest_valid ) +
              (x[tim1] >= lowest_valid && x[tim1] <= highest_valid ) +
              (x[tjp1] >= lowest_valid && x[tjp1] <= highest_valid ) +
              (x[tjm1] >= lowest_valid && x[tjm1] <= highest_valid ) ;
      if (valid == 4) {
        tmp =  0.25 * (x[tip1] + x[tim1] + x[tjp1] + x[tjm1]);
        x[loc] = tmp;
        if (x[loc] < MIN_CONC) x[loc] = 0.0;
        count += 1;

        if (x[loc] > highest_valid && x[loc] < LAND) {
          printf("Gapfill failure, %3d %3d %f \n",loc.i, loc.j, x[loc]);
        }
      }
    }
  }
  }

// This one is to be iterated as it is possible to fill in portions that
//   can later be used to fill in further.
  passno = 0;
  do {
    tcount = 0;
    passno += 1;
    for (loc.j = 1; loc.j < ylim -1 ; loc.j++) {
      tjp1.j = loc.j + 1;
      tjm1.j = loc.j - 1;
    for (loc.i = 1; loc.i < xlim -1 ; loc.i++) {
      if (x[loc] == flag) {
        tip1 = loc ; tip1 += ip1;
        tim1 = loc ; tim1 += im1;
        tjp1.i = loc.i ; 
        tjm1.i = loc.i ; 
        valid = (x[tip1] >= lowest_valid && x[tip1] <= highest_valid ) +
                (x[tim1] >= lowest_valid && x[tim1] <= highest_valid ) +
                (x[tjp1] >= lowest_valid && x[tjp1] <= highest_valid ) +
                (x[tjm1] >= lowest_valid && x[tjm1] <= highest_valid ) ;
        if (valid == 4) {
          tmp =  0.25 * (x[tip1] + x[tim1] + x[tjp1] + x[tjm1]);
          x[loc] = tmp;
          if (x[loc] < MIN_CONC) x[loc] = 0.0;
        if (x[loc] > highest_valid && x[loc] < LAND) {
          printf("Gapfill failure, %3d %3d %f \n",loc.i, loc.j, x[loc]);
        }
          tcount += 1;
        }
        else if (valid == 3) {
          tmp = 0.;
          if (x[tip1] >= lowest_valid && x[tip1] <= highest_valid ) tmp += x[tip1];
          if (x[tim1] >= lowest_valid && x[tim1] <= highest_valid ) tmp += x[tim1];
          if (x[tjp1] >= lowest_valid && x[tjp1] <= highest_valid ) tmp += x[tjp1];
          if (x[tjm1] >= lowest_valid && x[tjm1] <= highest_valid ) tmp += x[tjm1];
          tmp /= valid;
          tcount += 1;
          x[loc] = tmp;
          if (x[loc] < MIN_CONC) x[loc] = 0.0;
        if (x[loc] > highest_valid && x[loc] < LAND) {
          printf("Gapfill failure, %3d %3d %f \n",loc.i, loc.j, x[loc]);
        }
        }
      }
    }
    }
    count += tcount;
    #ifdef VERBOSE
      printf("Gapfill Pass %d tcount %d\n",passno, tcount);
    #endif

  } while (tcount > 0 && passno < 10);

  return count;
}

template <class T>
int isolani(metricgrid<T> &x, T flag, T replace) {
  int count = 0;
  ijpt loc, ip1, jp1, im1, jm1 ;
  ijpt  tip1, tjp1, tim1, tjm1 ;

  ip1.i =  1; ip1.j = 0;
  im1.i = -1; ip1.j = 0;
  jp1.i =  0; jp1.j =  1;
  jm1.i =  0; jm1.j = -1;

  for (loc.j = 1; loc.j < x.ypoints() - 1; loc.j++) {
  for (loc.i = 1; loc.i < x.xpoints() - 1; loc.i++) {
    if ( x[loc] != flag && x[loc] != replace) {
       tip1 = loc ; tip1 += ip1;
       tim1 = loc ; tim1 += im1;
       tjp1 = loc ; tjp1 += jp1;
       tjm1 = loc ; tip1 += jm1;
       if (x[tip1] == flag && x[tim1] == flag && x[tjp1] == flag && 
           x[tjm1] == flag) {
         x[loc] = replace;
         count += 1;
       }
    }
  }
  }
  return count;
}
template <class T>
void weather_zero(grid2<T> &x) {
  ijpt loc;
  for (loc.j = 0; loc.j < x.ypoints(); loc.j++) {
  for (loc.i = 0; loc.i < x.xpoints(); loc.i++) {
    if (x[loc] == WEATHER) x[loc] = 0;
  }
  }
  return;
}

//Set grid x = grid y, regardless of types
template <class T, class U>
void gridset(grid2_base<T> &x, grid2_base<U> &y) {
  int i, npts = x.xpoints()*x.ypoints();

  if (x.xpoints() != y.xpoints() || x.ypoints() != y.ypoints() ) {
    x.set((T) 0);
  }
  else {
    for (i = 0; i < npts; i++) {
       x[i] = y[i];
    }
  }
  return;
} 
