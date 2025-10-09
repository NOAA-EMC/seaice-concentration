#include "ncepgrids.h"

// composite N viirs ascii files in to simple binary global grids
//   of concentration, temperature, sigmaconc, sigmatemp, and count
// Along the way also:
//   filter by sst and land mask
//   apply the viirs-specific filter
// Robert Grumbine
// 9 October 2025

int main(int argc, char *argv[]) {
  FILE *fin, *fout;
  global_12th<float> conc, sigmaconc, temp, sigmatemp, count;
  global_12th<float> sst;
  global_12th<unsigned char> mask;
  int fnum, ti, tj;
  ijpt loc;
  latpt ll;
  float tlat, tlon, tconc, tsigmaconc, ttemp, tsigmatemp, tcount;

  fout = fopen(argv[1],"w");
  if (fout == (FILE *) NULL) {
    printf("failed to open output file %s\n", argv[1]);
    exit(1);
  }
  fin = fopen(argv[2],"r");
  if (fin == (FILE *) NULL) {
    printf("failed to open sst file %s\n", argv[2]);
    exit(1);
  }
  sst.binin(fin);
  fclose(fin);

  fin = fopen(argv[3],"r");
  if (fin == (FILE *) NULL) {
    printf("failed to open land mask file %s\n", argv[3]);
    exit(1);
  }
  mask.binin(fin);
  fclose(fin);

  conc.set((float) 0.);
  sigmaconc.set((float) 0.);
  temp.set((float) 0.);
  sigmatemp.set((float) 0.);
  count.set((float) 0.);

  for (fnum = 4; fnum < argc; fnum++) {
    fin = fopen(argv[fnum],"r");
    if (fin == (FILE *) NULL) {
      printf("failed to open input file %d %s\n",fnum, argv[fnum]);
      exit(1);
    }

    while (!feof(fin)) {
      fscanf(fin,"%d %d %f %f %f %f %f %f %f\n", &ti, &tj, &tlat, &tlon, &tconc, &tsigmaconc, &ttemp, &tsigmatemp, &tcount);
      count[ti,tj]     += tcount;
      conc[ti,tj]      += tconc*count[ti,tj];
      temp[ti,tj]      += ttemp*count[ti,tj];
      // need more detailed additions for differing counts RG
      sigmaconc[ti,tj] += tsigmaconc*tsigmaconc * count[ti,tj];
      sigmatemp[ti,tj] += tsigmatemp*tsigmatemp * count[ti,tj];
    }
    printf("count %f\n",count.gridmax() );
  
    fclose(fin);
  }

  for (loc.j = 0; loc.j < count.ypoints() ; loc.j++) {
  for (loc.i = 0; loc.i < count.xpoints() ; loc.i++) {
    if (count[loc] != 0) {
      conc[loc] /= count[loc];
      temp[loc] /= count[loc];
      // need more detailed math for differing counts RG
      sigmaconc[loc] = sqrt(sigmaconc[loc]/count[loc]);
      sigmatemp[loc] = sqrt(sigmatemp[loc]/count[loc]);
    }
    // apply filter here RG
  }
  }

  conc.binout(fout);
  temp.binout(fout);
  sigmaconc.binout(fout);
  sigmatemp.binout(fout);
  count.binout(fout);

  return 0;
}
//0 0 89.95833333333333 0.041666666666666664 99.15856475830078 1.1910030221915704 239.72630615234374 4.2478503154843 5.0
