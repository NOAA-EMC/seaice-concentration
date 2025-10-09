#include "ncepgrids.h"

// args are output file, then ((sigma+data_file)*N), N being unbounded
//
int main(int argc, char *argv[]) {
  FILE *fout;
  FILE *fin;

  global_12th<float> invsigmasum, weightsum;
  global_12th<unsigned char> conc;
  float sigma;

  ijpt loc;
  int fnum;

  fout = fopen(argv[1], "w");
  invsigmasum.set((float)0.);
  weightsum.set((float) 0.);

  for (fnum = 2; fnum < argc; fnum += 2) {
    sigma = atof(argv[fnum]);
    fin = fopen(argv[fnum+1], "r"); 
    if (fin == (FILE*) NULL) {
      printf("failed to open input concentration file %s\n",argv[fnum+1]);
      exit(1);
    }
    conc.binin(fin);
    fclose(fin);

    for (loc.j = 0; loc.j < conc.ypoints() ; loc.j++) {
    for (loc.i = 0; loc.i < conc.xpoints() ; loc.i++) {
      // Requires that concentrations be meaningful, even 0s, or flagged out (ignored)
      if (conc[loc] <= 100) {
	invsigmasum[loc] += 1./sigma;
	weightsum[loc] += conc[loc]/sigma;
      }
    }
    }

  }

  // render and write out
  conc.set((float) NO_DATA);
  for (loc.j = 0; loc.j < conc.ypoints() ; loc.j++) {
  for (loc.i = 0; loc.i < conc.xpoints() ; loc.i++) {
    if (invsigmasum[loc] > 0) {
      conc[loc] = (unsigned char) (0.5 + weightsum[loc]/invsigmasum[loc] );
    } 
  }
  }
  conc.binout(fout);

  return 0;
  }
