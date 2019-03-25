#include <cstdio>
#include <cstring>
using namespace std;

#include "ncepgrids.h"

// info for formatting L2 out:
// SSMI:
#ifdef F15
  #include "icessmi.h"
  #define DTYPE ssmi
  int platform = 15; // dmsp #
  #define NFREQS 7
  float freqs[NFREQS] = {19, 19, 22, 37, 37, 85, 85};
  int   polar[NFREQS] = {1,0,1,1,0,1,0};
#endif

// SSMI-S:
//int platform = (dmsp #)
#if defined(F17) || defined(F18) 
  #include "icessmis.h"
  #define DTYPE ssmis
  #define NFREQS 8
  float freqs[NFREQS] = {19, 19, 22, 37, 37, 92, 92, 150};
  int   polar[NFREQS] = {1,0,1,1,0,1,0, 0};
#endif

#ifdef F17
  int platform = 17; // dmsp #
#endif
#ifdef F18
  int platform = 18; // dmsp #
#endif

// AMSR-E:
//int platform = 0

// AMSR2:
//int platform = 1

void l2out(psgrid<bool> &nmap, grid2<DTYPE> &north, char *dtg, FILE *fout) ;

int main(int argc, char *argv[]) {
  northhigh<bool> nmap;
  grid2<DTYPE> north(nmap.xpoints(), nmap.ypoints() );
  southhigh<bool> smap;
  grid2<DTYPE> south(smap.xpoints(), smap.ypoints() );
  FILE *fin;

  char dtg[12];
  int i;


///////////////  Read in //////////////////////////////
  fin = fopen(argv[1], "r");
  if (fin == (FILE*) NULL) {
    printf("failed to open input file %s\n",argv[1]);
    return 1;
  }
  i = north.binin(fin);
  printf("i = %d\n",i);
  fclose(fin);

  fin = fopen(argv[2], "r");
  if (fin == (FILE*) NULL) {
    printf("failed to open input file %s\n",argv[2]);
    return 2;
  }
  i = south.binin(fin);
  printf("i = %d\n",i);
  fclose(fin);

  strncpy(dtg, argv[3], 12);

///////////////  Done Read in //////////////////////////////

  // sweep through grid and write out pretend L2 version
  FILE *fout;
  fout = fopen(argv[4],"w");
  if (fout == (FILE*) NULL) {
    printf("failed to open output file %s\n",argv[4]);
    return 4;
  }

  l2out(nmap, north, dtg, fout);
  l2out(smap, south, dtg, fout);

  fclose(fout);

  return 0;
}

void l2out(psgrid<bool> &nmap, grid2<DTYPE> &north, char *dtg, FILE *fout) {

  float tb[NFREQS];
  int npts = north.xpoints() * north.ypoints();
  ijpt loc;
  latpt ll;
  float conc;
  int qc = NFREQS;

// Header information:
  fwrite(&npts,     sizeof(int)  ,     1 , fout);
  fwrite(&platform, sizeof(int)  ,     1 , fout);
  fwrite(&qc  ,     sizeof(int)  ,     1 , fout);
  fwrite(freqs,     sizeof(float), NFREQS, fout);
  fwrite(polar,     sizeof(int)  , NFREQS, fout);

  // write out the observation points:
  for (loc.j = 0; loc.j < north.ypoints(); loc.j++) {
  for (loc.i = 0; loc.i < north.xpoints(); loc.i++) {
    ll = nmap.locate(loc);
    tb[0] = north[loc].t19v / 100.0;    
    tb[1] = north[loc].t19h / 100.0;    
    tb[2] = north[loc].t22v / 100.0;    
    tb[3] = north[loc].t37v / 100.0;    
    tb[4] = north[loc].t37h / 100.0;    
    #ifdef F15
      tb[5] = north[loc].t85v / 100.0;    
      tb[6] = north[loc].t85h / 100.0;    
    #elif defined(F17) || defined(F18)
      tb[5] = north[loc].t92v / 100.0;    
      tb[6] = north[loc].t92h / 100.0;    
      tb[7] = north[loc].t150h / 100.0;
    #endif

    printf("%3d %3d %7.3f %7.3f   %6.2f %3d %2d\n",loc.i, loc.j, ll.lat, ll.lon, 
          tb[0], north[loc].bar_conc, north[loc].count);

    conc = north[loc].bar_conc;
    if (conc > 100 && conc < 128) conc = 100;

    conc /= 100.;
    if (north[loc].bar_conc == LAND || north[loc].bar_conc == BAD_DATA || north[loc].bar_conc == NO_DATA) {
      qc = 5;
    }
    else if ( north[loc].bar_conc == WEATHER || north[loc].bar_conc == COAST ) {
      qc = 4;
    }
    else {
      qc = 1;
    }

    // point info
    fwrite(dtg, sizeof(char), 12, fout);
    fwrite(&ll, sizeof(latpt), 1, fout);
    fwrite(tb, sizeof(float), NFREQS, fout);
    fwrite(&conc, sizeof(float), 1, fout);
    fwrite(&qc, sizeof(int), 1, fout);
    
  }
  }

  return ;
}
