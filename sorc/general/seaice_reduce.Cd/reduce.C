#include <cstdio>
using namespace std;

#include "ncepgrids.h"
#include "params.h"

// Take information on a high resolution grid and transfer as much as
//   possible to a low resolution grid.  Assumes that the grids are
//   integer multiples of each other.
// Robert Grumbine 30 April 2004

int main(int argc, char *argv[]) {
  GRIDLOW<unsigned char> lotmp, count, mask;
  GRIDLOW<int> lores;
  GRIDHIGH<unsigned char> hires, himask;

  ijpt loc, lowloc;
  ijpt lowerl, ur;
  FILE *fin, *fout, *fhimask, *fmask;

  fin = fopen(argv[1], "r");
  fout = fopen(argv[2], "w");
  fhimask = fopen(argv[3], "r");
  fmask = fopen(argv[4], "r");

  if (fin == (FILE *) NULL) {
    printf ("failed to open the hires ice file %s\n", argv[1]);
    return 1;
  }
  if (fout == (FILE *) NULL) {
    printf ("failed to open the output file %s\n", argv[2]);
    return 2;
  }
  if (fhimask == (FILE *) NULL) {
    printf("failed to open the hires land mask file %s\n", argv[3]);
    return 3;
  }
  if (fmask == (FILE *) NULL) {
    printf("failed to open the lores land mask file %s\n", argv[4]);
    return 4;
  }
  
  hires.binin(fin);
  fclose(fin);
  himask.binin(fhimask);
  fclose(fhimask);
  mask.binin(fmask);
  fclose(fmask);
  
// If any point on the high resolution grid is not land or coast, use
//   its concentration for the low resolution grid.

  int ratio = hires.xpoints() / mask.xpoints();

  count.set((unsigned char) 0);
  for (loc.j = 0; loc.j < hires.ypoints(); loc.j++) {
  lowloc.j = loc.j / ratio;
  for (loc.i = 0; loc.i < hires.xpoints(); loc.i++) {
    lowloc.i = loc.i / ratio;
    if (hires[loc] < MAX_CONC && himask[loc] != LAND &&
        himask[loc] != COAST  &&   mask[lowloc] != LAND ) { 
      if (hires[loc] > 100) {
        lores[lowloc] += 100;
      }
      else {
        lores[lowloc] += hires[loc];
      }
      count[lowloc] += 1;
    }
  }
  }

// Now construct percentages as possible:
  for (loc.j = 0; loc.j < lores.ypoints(); loc.j++) {
  for (loc.i = 0; loc.i < lores.xpoints(); loc.i++) {
    if (count[loc] != 0 ) {
      lores[loc] /= count[loc];
      if (lores[loc] < MIN_CONC) lores[loc] = 0;
    }
    else {
      // Change to LAND from NO_DATA, for ease on downstream non-ice users 23 March 2010
      lores[loc] = LAND;
    }

//remove coast flagging on low res grid -- if high res had something
//  it considered reliable, use it.
    if (mask[loc] == LAND ) {
      lores[loc] = mask[loc];
    }

    lotmp[loc] = (unsigned char) lores[loc];
  }
  }

  lotmp.binout(fout);
  fclose(fout);

  return 0;
}
