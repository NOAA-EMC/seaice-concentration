#include "ncepgrids.h"

// composite N viirs ascii files in to simple binary global grids
//   of concentration, temperature, sigmaconc, sigmatemp, and count
// Along the way also:
//   filter by land mask
//   apply the viirs-specific filter
// Robert Grumbine
// 9 October 2025

int main(int argc, char *argv[]) {
  FILE *fin, *fout;
  global_12th<float> conc, sigmaconc, temp, sigmatemp, count;
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
    printf("failed to open land mask file %s\n", argv[2]);
    exit(1);
  }
  mask.binin(fin);
  fclose(fin);

  conc.set((float) 0.);
  sigmaconc.set((float) 0.);
  temp.set((float) 0.);
  sigmatemp.set((float) 0.);
  count.set((float) 0.);

  for (fnum = 3; fnum < argc; fnum++) {
    fin = fopen(argv[fnum],"r");
    if (fin == (FILE *) NULL) {
      printf("failed to open input file %d %s\n",fnum, argv[fnum]);
      break;
    }

    while (!feof(fin)) {
      fscanf(fin,"%d %d %f %f %f %f %f %f %f\n", &ti, &tj, &tlat, &tlon, &tconc, &tsigmaconc, &ttemp, &tsigmatemp, &tcount);
      loc.i = ti;
      loc.j = tj;
      if (tconc > 100 || tcount == 0) {
        printf("file %d  %d %d %f %f\n",fnum, ti, tj, tconc, tcount);
      }
      conc[loc]      += tconc*tcount;
      temp[loc]      += ttemp*tcount;
      count[loc]     += tcount;
      // need more detailed additions for differing counts RG
      sigmaconc[loc] += tsigmaconc*tsigmaconc * tcount;
      sigmatemp[loc] += tsigmatemp*tsigmatemp * tcount;
    }
    //debug: printf("%d count %f\n",fnum, count.gridmax() );
  
    fclose(fin);
  }

  float scale = 1.;
  int fcount = 0;
  global_12th<unsigned char> cout; // for compatibility with system, make this uchar
  cout.set((unsigned char) NO_DATA);

  for (loc.j = 0; loc.j < count.ypoints() ; loc.j++) {
  for (loc.i = 0; loc.i < count.xpoints() ; loc.i++) {
    if (count[loc] != 0) {
      conc[loc] /= count[loc];
      temp[loc] /= count[loc];
      ll = conc.locate(loc);
      scale = count[loc] / cos(M_PI/180.*ll.lat);
      // need more detailed math for differing counts RG
      sigmaconc[loc] = sqrt(sigmaconc[loc]/count[loc]);
      sigmatemp[loc] = sqrt(sigmatemp[loc]/count[loc]);

    // apply filter 
      if ( !(temp[loc] < 268.545 && scale > 125.005) || mask[loc] > 0 ) {
        fcount += 1;
        conc[loc] = NO_DATA;
        temp[loc] = NO_DATA;
        sigmaconc[loc] = NO_DATA;
        sigmatemp[loc] = NO_DATA;
        count[loc] = 0;
      }

      cout[loc] = (unsigned char) (0.5 + conc[loc]);
    }

    // diagnostic range check
    if (count[loc] > 0) {
      if ((cout[loc] > 100 && (cout[loc] != 224)) || cout[loc] <= 0.) {
        printf("cout out of range %d %d %d %f\n", loc.i, loc.j, cout[loc], count[loc]);
      } 
    }

  }
  }
  //debug: printf("Filtered out %d points\n",fcount);

//dev  conc.binout(fout);
//dev  //debug: printf("conc %f %f\n", conc.gridmax(NO_DATA), conc.gridmin() );
  cout.binout(fout);
  //debug: printf("cout %d %d\n", cout.gridmax(NO_DATA), cout.gridmin() );

  temp.binout(fout);
  //debug: printf("temp %f %f\n", temp.gridmax(NO_DATA), temp.gridmin() );
  sigmaconc.binout(fout);
  sigmatemp.binout(fout);
  count.binout(fout);

  return 0;
}
