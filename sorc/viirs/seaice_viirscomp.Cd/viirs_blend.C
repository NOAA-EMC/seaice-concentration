#include "ncepgrids.h"

int main(int argc, char* argv[]) {
  FILE *finage, *finold, *fintoday, *fout, *foutage;
  global_12th<float> oldconc, oldtemp, newconc, newtemp, outconc, outtemp; 
  global_12th<float> oldsigmaconc, newsigmaconc, outsigmaconc;
  global_12th<float> oldsigmatemp, newsigmatemp, outsigmatemp;
  global_12th<int> oldage, outage;

// Open files and acquire data
  finage   = fopen(argv[1],"r");
  if (finage == (FILE*) NULL) {
    oldage.set(0);
    printf("cold start set oldage to 0\n"); fflush(stdout);
  }
  else {
    oldage.binin(finage);
  }
  printf("maxage %d\n",oldage.gridmax() );
  //debug: return 0;

  finold   = fopen(argv[2], "r");
  if (finold == (FILE*) NULL) {
    printf("could not open %s\n",argv[2]);
    exit(2);
  }
  else {
    printf("opened %s for old data\n",argv[2]);
  }
  oldconc.binin(finold);
  oldsigmaconc.binin(finold);
  oldtemp.binin(finold);
  oldsigmatemp.binin(finold);
  printf("old conc temp %f %f\n",oldconc.gridmax(), oldtemp.gridmax() );
  //debug: return 0;

  fintoday = fopen(argv[3], "r");
  if (fintoday == (FILE*) NULL) {
    printf("could not open %s\n",argv[3]);
    exit(3);
  }
  newconc.binin(fintoday);
  newsigmaconc.binin(fintoday);
  newtemp.binin(fintoday);
  newsigmatemp.binin(fintoday);

  foutage  = fopen(argv[4], "w");
  if (foutage == (FILE*) NULL) {
    printf("could not open %s\n",argv[4]);
    exit(4);
  }
  fout     = fopen(argv[5], "w");
  if (fout == (FILE*) NULL) {
    printf("could not open %s\n",argv[5]);
    exit(5);
  }
////////////////////////////////////////////
  ijpt loc;
  int i,j;

  outage = oldage;
  outage += 1;
  for (loc.j = 0; loc.j < oldage.ypoints(); loc.j++) {
    j = loc.j;
  for (loc.i = 0; loc.i < oldage.xpoints(); loc.i++) {
    i = loc.i;
    if (newconc[i,j] != 0) {
      outage[i,j] = 0;
      outconc[i,j] = newconc[i,j];
      outtemp[i,j] = newtemp[i,j];
      outsigmaconc[i,j] = newsigmaconc[i,j];
      outsigmatemp[i,j] = newsigmatemp[i,j];
    }
    else {
      outconc[i,j] = oldconc[i,j];
      outtemp[i,j] = oldtemp[i,j];
      outsigmaconc[i,j] = oldsigmaconc[i,j];
      outsigmatemp[i,j] = oldsigmatemp[i,j];
    }
  }
  }

  outconc.binout(fout);
  outsigmaconc.binout(fout);
  outtemp.binout(fout);
  outsigmatemp.binout(fout);
  fclose(fout);

  printf("max new age %d\n",outage.gridmax() );
  outage.binout(foutage);
  fclose(foutage);

  return 0;
}
