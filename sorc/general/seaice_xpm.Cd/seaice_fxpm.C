#include <cstdio>
using namespace std;

// This is a generalized program to construct sea ice xpm files
//   for any known grid (GRIDTYPE).  GRIDTYPE must be specified in compilation.
// Arguments are: ice_data land_file output_file
// Robert Grumbine 
// Doc: 9 December 2002
//Future: specify data type as well as gridtype

#include "ncepgrids.h"
#ifndef LAND
  #define LAND 157
#endif
#ifndef NO_DATA
  #define NO_DATA 224
#endif


int main(int argc, char *argv[]) {
  FILE *fin;
  GRIDTYPE<unsigned char> yland; 
  GRIDTYPE<float> in, fland; 
  palette<unsigned char> gg(19, 65);

  fin = fopen(argv[1],"r");
  if (fin == (FILE *) NULL ) {
    printf("failed to open input file\n");
    return 1;
  }
  in.binin(fin);
  fclose(fin);
  if (in.average() < 3) in *= 100;
 
  fin = fopen(argv[2], "r");
  if (fin == (FILE *) NULL ) {
    printf("failed to open land file\n");
    return 1;
  }
  yland.binin(fin); 
  fclose(fin);
  conv(yland, fland);

// Added to ensure that land coloring does not bleed on to ocean:
  ijpt loc;
  for (loc.j = 0; loc.j < in.ypoints(); loc.j++) {
  for (loc.i = 0; loc.i < in.xpoints(); loc.i++) {
     if (in[loc] == LAND && yland[loc] != LAND) {
       in[loc] = NO_DATA;
     }
  }
  } 

  in.colorproc(fland, 7, 65, std_ice_coloring);
  in.xpm(argv[3], 1, gg); 

  return 0;
}
