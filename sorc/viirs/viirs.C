#include "ncepgrids.h"

int main(int argc, char *argv[]) {
  FILE *fin, *fout;
  global_12th<float> conc, sigmaconc, temp, sigmatemp, count;
  int ti, tj;
  float tlat, tlon, tconc, tsigmaconc, ttemp, tsigmatemp, tcount;

  fin = fopen(argv[1],"r");
  if (fin == (FILE *) NULL) {
    printf("failed to open input file %s\n", argv[1]);
    exit(1);
  }
  fout = fopen(argv[2],"w");
  if (fout == (FILE *) NULL) {
    printf("failed to open output file %s\n", argv[2]);
    exit(1);
  }

  while (!feof(fin)) {
    fscanf(fin,"%d %d %f %f %f %f %f %f %f\n", &ti, &tj, &tlat, &tlon, &tconc, &tsigmaconc, &ttemp, &tsigmatemp, &tcount);
    // could apply filter here
    conc[ti,tj]      = tconc;
    sigmaconc[ti,tj] = tsigmaconc;
    temp[ti,tj]      = ttemp;
    sigmatemp[ti,tj] = tsigmatemp;
    count[ti,tj]     = tcount;
  }
  printf("count %f\n",count.gridmax() );

  conc.binout(fout);
  sigmaconc.binout(fout);
  temp.binout(fout);
  sigmatemp.binout(fout);
  count.binout(fout);

  return 0;
}
//0 0 89.95833333333333 0.041666666666666664 99.15856475830078 1.1910030221915704 239.72630615234374 4.2478503154843 5.0
