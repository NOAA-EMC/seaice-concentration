#include "ncepgrids.h"
#include "avhrr.h"

#define ch1  0
#define ch2  1
#define ch3a 2
#define ch3b 2
#define ch4 3
#define ch5 4

// program to develop an gacavhrr-based sea ice filter.
// This is a strictly negative filter -- it will set ice concentration
//   to zero in areas which cannot be ice.  But it will not construct ice in
//   places that might be.

int main(int argc, char *argv[]) {
  FILE *fconc, *favhrr, *fage, *foutice, *foutfree;
  global_12th<unsigned char> conc, age;
  ijpt loc;
  latpt ll;
  gacpt x;
  int i, k = 0;
  float tk = 273.15;

  fconc = fopen(argv[1],"r");
  fage  = fopen(argv[2], "r");
  favhrr = fopen(argv[3], "r");
  if (fconc == (FILE *) NULL || fage == (FILE *) NULL || favhrr == (FILE*)NULL) {
    printf("failed to open an input file, %s %s %s\n",argv[1], argv[2], argv[3]);
    return 1;
  }

  conc.binin(fconc);
  age.binin(fage);

  while (!feof(favhrr)) {
    fread(&x, sizeof(x), 1, favhrr);
    if (x.obs[ch5].tmbr < (tk+20) && x.obs[ch4].tmbr < (tk+20)) {

      ll.lat = x.clat;
      ll.lon = x.clon;
      loc = conc.locate(ll);

// Construct albedo grids:
      if (x.obs[ch5].tmbr < (tk+5) || x.obs[ch4].tmbr < (tk+5)) {
        k++;
      }
      else {
        // this is the real filter -- warm, but not absurdly so, tb in ch4 and ch5
        if (conc[loc] != 0 || age[loc] != 0) { 
          printf("%8.3f %7.3f %3d %2d %4d %4d",ll.lon, ll.lat, conc[loc], age[loc], loc.i, loc.j);
          for (i = 0; i < 2; i++) {
            printf("%3d ",x.obs[i].albedo);
          }
          i = 2;
          printf("%3d %6.2f ",x.obs[i].albedo, x.obs[i].tmbr);
          for (i = 3; i < 5; i++) {
            printf(" %6.2f ",x.obs[i].tmbr);
          }
          printf("\n");
        }
        age[loc] = 0;
        conc[loc] = 0;
      }
    }

    
  }

  foutice = fopen(argv[4], "w");
  foutfree = fopen(argv[5], "w");
  conc.binout(foutice);
  age.binout(foutfree);
  fclose(foutice);
  fclose(foutfree);

  return 0;
}
