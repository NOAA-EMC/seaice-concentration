#include "ncepgrids.h"

// Program to blend multiple source grids, for each hemisphere, on to a unified
//   target grid.
// Robert Grumbine
// 30 March 2012: Extend to having 3 inputs -- AMSR, SSMI, SSMI-S -- per hemiphere.
// 16 August 2017: Change to AMSR2, and SSMIS-17, 18

void blend(metricgrid<unsigned char> &namsr2,
           metricgrid<unsigned char> &nssmis17, metricgrid<unsigned char> &nssmis18, 
           metricgrid<unsigned char> &nout) ;
void blend2(metricgrid<unsigned char> &namsr2,
           metricgrid<unsigned char> &nssmis17, metricgrid<unsigned char> &nssmis18, 
           metricgrid<unsigned char> &nout) ;
void blend(metricgrid<unsigned char> &namsr2, metricgrid<unsigned char> &nssmi, 
           metricgrid<unsigned char> &nssmis17, metricgrid<unsigned char> &nssmis18, 
           metricgrid<unsigned char> &nout) ;
void blend2(metricgrid<unsigned char> &namsr2, metricgrid<unsigned char> &nssmi, 
           metricgrid<unsigned char> &nssmis17, metricgrid<unsigned char> &nssmis18, 
           metricgrid<unsigned char> &nout) ;
void stats(metricgrid<unsigned char> &n, metricgrid<unsigned char> &n2,
           metricgrid<unsigned char> &land) ;

bool somedata(grid2<unsigned char> &x) ;
void splice(global_12th<float> &noice, global_12th<float> &imsice, metricgrid<unsigned char> &x);

int main(int argc, char *argv[]) {
  northhigh2<unsigned char> namsr2;
  northhigh<unsigned char> nout, nout2, nland;
  northhigh<unsigned char> nssmi;
  northhigh<unsigned char> nssmis17, nssmis18;

  southhigh2<unsigned char> samsr2;
  southhigh<unsigned char> sout, sout2, sland;
  southhigh<unsigned char> sssmi;
  southhigh<unsigned char> sssmis17, sssmis18;

  global_12th<float> noice, imsice;
  FILE *fin, *nfout, *sfout;

// Initialize grids:
  namsr2.set((unsigned char) NO_DATA);
  nssmi.set((unsigned char) NO_DATA);
  nssmis17.set((unsigned char) NO_DATA);
  nssmis18.set((unsigned char) NO_DATA);
  samsr2.set((unsigned char) NO_DATA);
  sssmi.set((unsigned char) NO_DATA);
  sssmis17.set((unsigned char) NO_DATA);
  sssmis18.set((unsigned char) NO_DATA);
  noice.set((float) NO_DATA/100.0);
  imsice.set((float) NO_DATA/100.0);

// Read in the hemispheric grids and open output file
  fin = fopen(argv[1],"r");
  if (fin == (FILE *) NULL) {
    printf("failed to open namsr2 %s\n",argv[1]); fflush(stdout);
  }
  else {
    namsr2.binin(fin);
    fclose(fin);
  }

  fin = fopen(argv[2],"r");
  if (fin == (FILE *) NULL) {
    printf("failed to open nssmi %s\n",argv[2]); fflush(stdout);
  }
  else {
    nssmi.binin(fin);
    fclose(fin);
  }

  fin = fopen(argv[3],"r");
  if (fin == (FILE *) NULL) {
    printf("failed to open nssmis17 %s\n",argv[3]); fflush(stdout);
  }
  else {
    nssmis17.binin(fin);
    fclose(fin);
  }

  fin = fopen(argv[4],"r");
  if (fin == (FILE *) NULL) {
    printf("failed to open nssmis18 %s\n",argv[4]); fflush(stdout);
  }
  else {
    nssmis18.binin(fin);
    fclose(fin);
  }

  nfout = fopen(argv[5],"w");
  if (nfout == (FILE *) NULL) {
    printf("Failed to open the northern hemisphere file %s for writing!\n",argv[4]);
    return 1;
  }

  fin = fopen(argv[6],"r");
  if (fin == (FILE *) NULL) {
    printf("failed to open samsr2 %s\n",argv[6]); fflush(stdout);
  }
  else {
    samsr2.binin(fin);
    fclose(fin);
  }

  fin = fopen(argv[7],"r");
  if (fin == (FILE *) NULL) {
    printf("failed to open sssmi %s\n", argv[7]); fflush(stdout);
  }
  else {
    sssmi.binin(fin);
    fclose(fin);
  }

  fin = fopen(argv[8],"r");
  if (fin == (FILE *) NULL) {
    printf("failed to open sssmis17 %s\n",argv[8]); fflush(stdout);
  }
  else {
    sssmis17.binin(fin);
    fclose(fin);
  }
  fin = fopen(argv[9],"r");
  if (fin == (FILE *) NULL) {
    printf("failed to open sssmis18 %s\n",argv[9]); fflush(stdout);
  }
  else {
    sssmis18.binin(fin);
    fclose(fin);
  }

  sfout = fopen(argv[10],"w");
  if (sfout == (FILE *) NULL) {
    printf("Failed to open the southern hemisphere file %s for writing!\n",argv[10]);
    return 1;
  }
// Land masks:
  fin = fopen(argv[11], "r");
  if (fin == (FILE *) NULL) {
    printf("failed to open nland %s\n",argv[11]);
    return 1;
  }
  nland.binin(fin);
  fclose(fin);
  fin = fopen(argv[12], "r");
  if (fin == (FILE *) NULL) {
    printf("failed to open sland %s\n",argv[12]);
    return 1;
  }
  sland.binin(fin);

// noice (climatology) and imsice (IMS analysis)
  fin = fopen(argv[13], "r");
  if (fin == (FILE *) NULL) {
    printf("failed to open noice file %s\n",argv[13]); fflush(stdout);
  }
  else {
    noice.binin(fin);
    fclose(fin);
  }
  fin = fopen(argv[14], "r");
  if (fin == (FILE*) NULL) {
    printf("failed to open imsice file %s\n",argv[14]); fflush(stdout);
  }
  else {
    imsice.binin(fin);
    fclose(fin);
  }
//////////////////////////////////////////////////////////////////////////

// now have all inputs
  blend(namsr2, nssmis17, nssmis18, nout);
  blend2(namsr2, nssmis17, nssmis18, nout2);
  if (!somedata(nout) ) {
    splice(noice, imsice, nout);
  }

  blend(samsr2, sssmis17, sssmis18, sout);
  blend2(samsr2, sssmis17, sssmis18, sout2);
  if (!somedata(sout) ) {
    splice(noice, imsice, sout);
  }

  nout.binout(nfout);
  sout.binout(sfout);
  fclose(nfout);
  fclose(sfout);

  printf("stats for north intercomparison\n");
  stats(nout, nout2, nland);
  printf("stats for south intercomparison\n");
  stats(sout, sout2, sland);

  return 0;
}
/////////////// Blenders /////////////////////////////////////////////////////
void blend(metricgrid<unsigned char> &namsr2, 
           metricgrid<unsigned char> &nssmis17, metricgrid<unsigned char> &nssmis18,
           metricgrid<unsigned char> &nout) {
  ijpt loc, loc2;
//  latpt ll;
  grid2<int> tmp(nout.xpoints(), nout.ypoints() );
  grid2<int> c(nout.xpoints(), nout.ypoints() );

  tmp.set(0);
  c.set(0);

  for (loc.j = 0; loc.j < namsr2.ypoints(); loc.j++) {
  for (loc.i = 0; loc.i < namsr2.xpoints(); loc.i++) {
    // Pre-bound:
    if (namsr2[loc] > 100 && namsr2[loc] <= 128) namsr2[loc] = 100;
    if (namsr2[loc] <= 100) {
      #ifdef VERBOSE
        ll = namsr2.locate(loc);
        loc2 = nout.locate(ll);
        if (loc2.i < 0 || loc2.i > nout.xpoints()-1 || loc2.j < 0 || loc2.j > nout.ypoints()-1) {
          printf("loc2 failed %d %d vs. orig %d %d\n",loc2.i, loc2.j, loc.i, loc.j);
          fflush(stdout);
          if (loc2.i > nout.xpoints()-1) loc2.i = nout.xpoints()-1;
          if (loc2.j > nout.ypoints()-1) loc2.j = nout.ypoints()-1;
        }
      #endif

      loc2.i = loc.i/2;
      loc2.j = loc.j/2; // this is a cheat -- we know that northhigh2 is identically 2x
      c[loc2]++; 
      tmp[loc2] += namsr2[loc];
    }
  }
  }

  for (loc.j = 0; loc.j < nout.ypoints(); loc.j++) {
  for (loc.i = 0; loc.i < nout.xpoints(); loc.i++) {
    // Pre-bound:
    if (nssmis17[loc] > 100 && nssmis17[loc] <= 128) nssmis17[loc] = 100;
    if (nssmis18[loc] > 100 && nssmis18[loc] <= 128) nssmis18[loc] = 100;

    if (nssmis17[loc] <= 100) {
      c[loc]++; tmp[loc] += nssmis17[loc];
    }
    if (nssmis18[loc] <= 100) {
      c[loc]++; tmp[loc] += nssmis18[loc];
    }


    if (c[loc] != 0) {
      nout[loc] = (int) (((float) tmp[loc] / (float) c[loc]) + 0.5 );
    }
    else {
      nout[loc] = NO_DATA;
    }

// Ensure properly bounded from below:
    if (nout[loc] < MIN_CONC) nout[loc] = 0;
  }
  }

  return;
}

// Do a blending in sequence of guessed better to worser instruments:
void blend2(metricgrid<unsigned char> &namsr2, 
            metricgrid<unsigned char> &nssmis17,  metricgrid<unsigned char> &nssmis18,
            metricgrid<unsigned char> &nout) {
  ijpt loc;

  for (loc.j = 0; loc.j < nout.ypoints(); loc.j++) {
  for (loc.i = 0; loc.i < nout.xpoints(); loc.i++) {
    if (namsr2[loc] <= 100) {
      nout[loc] = namsr2[loc];
    }
    else if (nssmis17[loc] <= 100 ) {
      nout[loc] = nssmis17[loc];
    }
    else if (nssmis18[loc] <= 100 ) {
      nout[loc] = nssmis18[loc];
    }
    else {
      nout[loc] = NO_DATA;
    }
  }
  }

  return;
}
// ------------------- 3 inputs
void blend(metricgrid<unsigned char> &namsr2, metricgrid<unsigned char> &nssmi, 
           metricgrid<unsigned char> &nssmis17, metricgrid<unsigned char> &nssmis18,
           metricgrid<unsigned char> &nout) {
  ijpt loc, loc2;
//  latpt ll;
  grid2<int> tmp(nout.xpoints(), nout.ypoints() );
  grid2<int> c(nout.xpoints(), nout.ypoints() );

  tmp.set(0);
  c.set(0);

  for (loc.j = 0; loc.j < namsr2.ypoints(); loc.j++) {
  for (loc.i = 0; loc.i < namsr2.xpoints(); loc.i++) {
    // Pre-bound:
    if (namsr2[loc] > 100 && namsr2[loc] <= 128) namsr2[loc] = 100;
    if (namsr2[loc] <= 100) {
      #ifdef VERBOSE
        ll = namsr2.locate(loc);
        loc2 = nout.locate(ll);
        if (loc2.i < 0 || loc2.i > nout.xpoints()-1 || loc2.j < 0 || loc2.j > nout.ypoints()-1) {
          printf("loc2 failed %d %d vs. orig %d %d\n",loc2.i, loc2.j, loc.i, loc.j);
          fflush(stdout);
          if (loc2.i > nout.xpoints()-1) loc2.i = nout.xpoints()-1;
          if (loc2.j > nout.ypoints()-1) loc2.j = nout.ypoints()-1;
        }
      #endif

      loc2.i = loc.i/2;
      loc2.j = loc.j/2; // this is a cheat -- we know that northhigh2 is identically 2x
      c[loc2]++; 
      tmp[loc2] += namsr2[loc];
    }
  }
  }

  for (loc.j = 0; loc.j < nout.ypoints(); loc.j++) {
  for (loc.i = 0; loc.i < nout.xpoints(); loc.i++) {
    // Pre-bound:
    if (nssmi[loc] > 100 && nssmi[loc] <= 128) nssmi[loc] = 100;
    if (nssmis17[loc] > 100 && nssmis17[loc] <= 128) nssmis17[loc] = 100;
    if (nssmis18[loc] > 100 && nssmis18[loc] <= 128) nssmis18[loc] = 100;

    if (nssmi[loc] <= 100) {
      c[loc]++; tmp[loc] += nssmi[loc];
    }
    if (nssmis17[loc] <= 100) {
      c[loc]++; tmp[loc] += nssmis17[loc];
    }
    if (nssmis18[loc] <= 100) {
      c[loc]++; tmp[loc] += nssmis18[loc];
    }


    if (c[loc] != 0) {
      nout[loc] = (int) (((float) tmp[loc] / (float) c[loc]) + 0.5 );
    }
    else {
      nout[loc] = NO_DATA;
    }

// Ensure properly bounded from below:
    if (nout[loc] < MIN_CONC) nout[loc] = 0;
  }
  }

  return;
}

// Do a blending in sequence of guessed better to worser instruments:
void blend2(metricgrid<unsigned char> &namsr2, metricgrid<unsigned char> &nssmi, 
            metricgrid<unsigned char> &nssmis17,  metricgrid<unsigned char> &nssmis18,
            metricgrid<unsigned char> &nout) {
  ijpt loc;

  for (loc.j = 0; loc.j < nout.ypoints(); loc.j++) {
  for (loc.i = 0; loc.i < nout.xpoints(); loc.i++) {
    if (namsr2[loc] <= 100) {
      nout[loc] = namsr2[loc];
    }
    else if (nssmi[loc] <= 100 ) {
      nout[loc] = nssmi[loc];
    }
    else if (nssmis17[loc] <= 100 ) {
      nout[loc] = nssmis17[loc];
    }
    else if (nssmis18[loc] <= 100 ) {
      nout[loc] = nssmis18[loc];
    }
    else {
      nout[loc] = NO_DATA;
    }
  }
  }

  return;
}
//////////////// Utilities ////////////////////////////////////////////
void stats(metricgrid<unsigned char> &n, metricgrid<unsigned char> &n2, metricgrid<unsigned char> &land) {
  ijpt loc;
  double sum = 0, sumsq = 0;
  int count = 0;
  
  for (loc.j = 0; loc.j < n.ypoints(); loc.j++) {
  for (loc.i = 0; loc.i < n.xpoints(); loc.i++) {
    if ((n[loc] != n2[loc]) && (land[loc] == 0) ) {
      count += 1;
      sum += (n[loc] - n2[loc]);
      sumsq += (n[loc] - n2[loc])*(n[loc] - n2[loc]);
    }
  }
  }
  printf("count %d mean, rms difference %f %f\n",count, sum/count, sqrt(sumsq/count) );

  return;
}
bool somedata(grid2<unsigned char> &x) {
  int i;
  for (i = 0; i < x.xpoints()*x.ypoints(); i++) {
    if (x[i] != NO_DATA) return true;
  }
  return false;
} 
void splice(global_12th<float> &noice, global_12th<float> &imsice, 
            metricgrid<unsigned char> &x) {
  ijpt loc, tloc;
  latpt ll;

  for (loc.j = 0; loc.j < x.ypoints(); loc.j++) {
  for (loc.i = 0; loc.i < x.xpoints(); loc.i++) {
    if (x[loc] == NO_DATA) {
      ll = x.locate(loc);
      tloc = imsice.locate(ll);
      if (imsice[tloc] <= 1.00) {
        x[loc] = (int) (0.5 + 100.*imsice[tloc]);
      }
      else if (noice[tloc] <= 1.00) {
        x[loc] = (int) (0.5 + 100.*noice[tloc]);
      }
    }
  }
  }

  return;
}
