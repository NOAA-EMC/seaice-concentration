#include "ncepgrids.h"

// Program to read in climatology for the day and write out
//   an estimated sea ice field.
// Use count >= 15, then condavg for concentration field
// Robert Grumbine 10 March 2014
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
  GRIDTYPE<float> high, countmp, avgtmp;
  FILE *fin, *fout;
  ijpt loc;

  fin = fopen(argv[1],"r");
  for (int i = 0; i < 7; i++) {
    climo[i].binin(fin);
  }
  fclose(fin);

  float landval = 1.57, nonval = 1.57;
  countmp.fromall(climo[count], landval, nonval);
  avgtmp.fromall(climo[CONDAVG], landval, nonval);

  for (loc.j = 0; loc.j < high.ypoints(); loc.j++) {
  for (loc.i = 0; loc.i < high.xpoints(); loc.i++) {
    if (countmp[loc] >= 15) {high[loc] = avgtmp[loc]; }
    else {
      high[loc] = 0.0;
    }
  } 
  }

  fout = fopen(argv[2],"w");
  high.binout(fout);
  fclose(fout);

  return 0;
}
