#include "ncepgrids.h"

// Apply an a posteriori file (derived from SST flags in this case)
//    to the hemispheric analysis grids and write out the filtered
//    versions
// Robert Grumbine 13 August 2010

float posteriori(psgrid<unsigned char> &x, metricgrid<unsigned char> &flags, psgrid<unsigned char> &y);

int main(int argc, char *argv[]) {
  FILE *inflag, *innh, *insh, *outnh, *outsh;
  GLOBAL<unsigned char> flags;
  NORTH<unsigned char> nh, nhout;
  SOUTH<unsigned char> sh, shout;
  float area = 0.0;

  inflag = fopen(argv[1], "r");
  innh   = fopen(argv[2], "r");
  insh   = fopen(argv[3], "r");
  outnh  = fopen(argv[4], "w");
  outsh  = fopen(argv[5], "w");

  flags.binin(inflag); fclose(inflag);
  nh.binin(innh); fclose(innh);
  sh.binin(insh); fclose(insh);
   
  area  = posteriori(nh, flags, nhout);
  area += posteriori(sh, flags, shout);
  printf("%f thousand km^2 caught in a posteriori filter \n",area / 1.e9 );
  nhout.binout(outnh); fclose(outnh);
  shout.binout(outsh); fclose(outsh);

  return 0;
}
float posteriori(psgrid<unsigned char> &x, metricgrid<unsigned char> &flags, psgrid<unsigned char> &y) {
  float area = 0;
  ijpt iloc, floc;
  latpt ll;

  y = x;

  for (iloc.j = 0; iloc.j < x.ypoints(); iloc.j++) {
  for (iloc.i = 0; iloc.i < x.xpoints(); iloc.i++) {
    ll = x.locate(iloc);
    floc = flags.locate(ll);
    if (flags[floc] == 2 || flags[floc] == 224) {
      x[iloc] = 0;
    }
    else if (flags[floc] == 158 || flags[floc] == 159 || flags[floc] == 160 \
                           || flags[floc] == 161 || flags[floc] == 162 \
                           || flags[floc] == 163 || flags[floc] == 164 ) {
      if (x[iloc] >= 15 && x[iloc] <= 128) {
           area += x.cellarea(iloc); 
           x[iloc] = 1;
           y[iloc] = 0;
      }
      else {
        x[iloc] = 0;
      }
    }
    else if (flags[floc] == 170 || flags[floc] == 171 || flags[floc] == 172) {
      if (x[iloc] >= 15 && x[iloc] <= 128) {
           area += x.cellarea(iloc); 
           x[iloc] = 1;
           y[iloc] = 0;
      }
      else {
        x[iloc] = 0;
      }
    }
    else {
      x[iloc] = 0;
    }
  }
  }

  return area;
}
