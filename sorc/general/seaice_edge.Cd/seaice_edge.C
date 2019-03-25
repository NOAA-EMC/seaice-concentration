#include "ncepgrids.h"

// Analyze a sea ice concentration field to get an estimate of where
//   the sea ice edge is, approximately.  For use in model and
//   analysis automated QC and perhaps other things.
void  edger(metricgrid<unsigned char> &conc, metricgrid<unsigned char> &land,
             mvector<latpt> &analy_edge, int &npts);
void kml_line(mvector<latpt> &x, int npts, FILE *fout) ;

int main(int argc, char *argv[]) {
  FILE *fin1, *fin2, *fout;
  ANALYSIS<unsigned char> conc;
  ANALYSIS<unsigned char> land;
  mvector<latpt> analy_edge(4*4320), fcst_edge(4*4320);
  int npts;

  //read in ice concentration analysis - verification
  //create analysis edge line 
  fin1 = fopen(argv[1], "r");
  if (fin1 == (FILE *) NULL) {
     printf("failed to open %s\n",argv[1]);
     return 1;
  }
  conc.binin(fin1);
  fclose(fin1);

  fin2 = fopen(argv[2], "r");
  if (fin2 == (FILE *) NULL) {
     printf("failed to open %s\n",argv[2]);
     return 1;
  }
  land.binin(fin2);
  fclose(fin2);

  fout = fopen(argv[3], "w");
  if (fout == (FILE *) NULL) {
     printf("failed to open %s\n",argv[3]);
     return 1;
  }

  edger(conc, land, analy_edge, npts);
  kml_line(analy_edge, npts, fout);
  fclose(fout);

  ijpt loc, locp, locm;
  int i;
  for (i = 0; i < npts; i++) {
    loc = conc.locate(analy_edge[i]);
    locp = loc; locp.j++;
    locm = loc; locm.j--;

    printf("%7.3f %8.3f  %3d %3d %3d\n",analy_edge[i].lat, analy_edge[i].lon,
       land[locp], land[loc], land[locm]) ;

  }
  


  return 0;
}

// Find the ice edge.
// (note: assuming only north of interest at the moment.  Should also
//   cover south, and specify hemisphere by argument, or simply
//   do both.
// For generality of algorithm, start at 90N and then descend through
//   each longitude (whatever spacing desired, argument?)  
// Ice edge occurs if we hit a point where the previous point is
//   ice, and the current is water.
// No ice edge occurs if we go from ice to land
// No ice edge if water to ice
// Only the first ice-water transition is counted -- 
//   will create problems if a polynya forms, on the other hand,
//   it is only in this way that we can ensure that consecutive
//   points near the ice edge (a given ice edge) will be
//   consecutive with each other.  (? algorithm development)
void  edger(metricgrid<unsigned char> &conc, metricgrid<unsigned char> &land,
            mvector<latpt> &analy_edge, int &npts) {
  ijpt oldloc, newloc;
  latpt newer, older;
  unsigned char oldconc, newconc;
  float lat, lon, dlat = 1./12., dlon = 1./12.;

  npts = 0;

  // note that this matches specs for the high resolution analysis grid
  for (lon = dlon/2.; lon <= 360. - dlon/2.; lon += dlon) {
    lat = 90. - dlat/2.; //pole-equator
    //lat = 33.0; // equator-pole

    older.lat = lat; 
    older.lon = lon;
    oldloc = conc.locate(older);
    oldconc = conc[oldloc];
    newer.lon = lon;
    // Pole-equator: 
    for (newer.lat = older.lat - dlat; newer.lat > 33.0; newer.lat -= dlat) { 
    // equator-pole:
    //for (newer.lat = older.lat + dlat; newer.lat < 90.0 - dlat/2; newer.lat += dlat) { 
      newloc = conc.locate(newer); // teset for loc being on grid (!)
      // if (! ongrid) escape
      newconc = conc[newloc];
      if (oldconc > 0 && oldconc < 128 && newconc == 0 && land[newloc] == 0 ) {
        // found an edge point
        analy_edge[npts] = newer; // really want to make this an 
                                  //   interpolation between grid points 
                                  // this suffices for development
        npts += 1;
        break; // break out of lat loop to continue for a new longitude
      }
      oldloc = newloc;
      oldconc = newconc;
    }
  }

  printf("found %d points on Arctic edge\n",npts);


// Antarctic edge:

  return;
}
void kml_line(mvector<latpt> &x, int npts, FILE *fout) {
  int i, thinning = 1;
  float lat, lon;

// Header information:
  fprintf(fout, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  fprintf(fout, "<kml xmlns=\"http://www.opengis.net/kml/2.2\" xmlns:gx=\"http://www.google.com/kml/ext/2.2\" xmlns:kml=\"http://www.opengis.net/kml/2.2\" xmlns:atom=\"http://www.w3.org/2005/Atom\">\n");
  fprintf(fout, "<Document>\n");

  fprintf(fout, "<Folder>\n");
  fprintf(fout, "  <name>Ice Edge automated estimate -- Not For Navigation Purposes!</name>\n");
  fprintf(fout, "  <LookAt>\n");
  fprintf(fout, "   <longitude>-165</longitude>\n");
  fprintf(fout, "    <latitude>65</latitude>\n");
  fprintf(fout, "    <range>6400000</range>\n");
  fprintf(fout, "  </LookAt>\n");

  fprintf(fout, " <Placemark>\n");
  fprintf(fout, " <name>Arctic</name>\n");
  fprintf(fout, " <LineString>\n");
  fprintf(fout, "     <tessellate>1</tessellate>\n");
  fprintf(fout, "     <coordinates>\n");

  for (i = 0; i < npts; i++) {
    if (i% thinning == 0) {
      lat = x[i].lat;
      lon = x[i].lon;
      if (lon > 180) {
        lon -= 360.;
      } 
      fprintf(fout,"%f,%f,0\n",lon, lat);
    }
  }
  
  // close up all tags:
  fprintf(fout, "</coordinates>\n");
  fprintf(fout, "</LineString>\n");
  fprintf(fout, "</Placemark>\n");

  fprintf(fout, "</Folder>\n");
  fprintf(fout, "</Document>\n");
  fprintf(fout, "</kml>\n");


  return;
}
