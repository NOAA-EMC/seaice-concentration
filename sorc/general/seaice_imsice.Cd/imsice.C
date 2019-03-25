#include "ncepgrids.h"

// Program to read in climatology for the day and write out
//   an estimated sea ice field.
// Use count >= 15, then condavg for concentration field
// Arbitrary input grids
// Robert Grumbine 10 July 2014
//
#define count 0
#define avg   1
#define sumsq 2
#define var   3
#define CONDAVG  4
#define CONSUMSQ 5
#define CONDVAR  6

int main(int argc, char *argv[]) {
  global_ice<float> climo[7];

  GRIDTYPE<float> fromims;
  GRIDTYPE<float> high, countmp, avgtmp;
  
  bedient_north<float> ims(96); // ims ice on 1/96th bedient grid
  FILE *fin, *fout;
  ijpt loc;
  latpt ll;
  float landval = 1.57, nonval = 1.57;

  fin = fopen(argv[1],"r");
  for (int i = 0; i < 7; i++) {
    climo[i].binin(fin);
  }
  fclose(fin);

  countmp.fromall(climo[count], landval, nonval);
  avgtmp.fromall(climo[CONDAVG], landval, nonval);

// Note that ims is northern hemisphere only.
  fin = fopen(argv[2],"r");
  ims.binin(fin);
  fclose(fin);
  fromims.fromall(ims, landval, nonval);

  for (loc.j = 0; loc.j < high.ypoints(); loc.j++) {
  for (loc.i = 0; loc.i < high.xpoints(); loc.i++) {
    ll = high.locate(loc);
    if (ll.lat > 0) {
      if (fromims[loc] > 0 && fromims[loc] <= 1.00) {
        high[loc] = avgtmp[loc] ; }
      else {
        high[loc] = 0.0;
      }
    }
    else {
      if (countmp[loc] >= 15) {
        high[loc] = avgtmp[loc]; 
      }
      else {
        high[loc] = 0.0;
      }
    }

  } 
  }

  fout = fopen(argv[3],"w");
  high.binout(fout);
  fclose(fout);

  return 0;
}
