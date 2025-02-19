// Program for deriving sea ice concentrations from AMSR2 observations
// Regression to AMSR-E brightness temperatures provided by Walt Meier 4/2015
// Also update to weather filter
// Robert Grumbine 2 August 2017

#include <cstdio>
#include <stack>
using namespace std;

#include "ncepgrids.h"
#include "amsr2.h"

bool hfok(amsr2_hrpt &hr) ;
bool lfok(amsr2_lrpt &lr) ;
float weather(double &t19v, double &t19h, double &t24v, double &t37v, double &t37h,
             double &t89v, double &t89h) ;
void amsr2_regress_to_amsre(double &v19, double &h19, double &v24, 
               double &v37, double &h37, double &v89, double &h89, float lat) ;

bool notbogus(double &h6p9, double &v6p9, double &h7p3, double &v7p3, double &h11, double &v11,
	       double &h19) ;

#include "amsr2_team2.C"
class amsr2_base {
  public :
    amsr2_spot spot;
  public :
    amsr2_base(void);
};
amsr2_base::amsr2_base() {
  spot.sccf = 0;
  spot.alfr = 0;
  spot.anpo = 0;
  spot.viirsq = 0;
  spot.tmbr   = 0;
  //printf("initialized amsr2_base\n"); fflush(stdout);
}

//class amsr2_hr_accum : public amsr2_base {
//  don't inherit, we're doing a 'composed of' class
class amsr2_hr_accum {
  public :
    int count;
    amsr2_base hr[2];
  public :
    amsr2_hr_accum(void);
};
amsr2_hr_accum::amsr2_hr_accum() {
  count = 0;
  //printf("initialized amsr2_hr\n"); fflush(stdout);
}
class amsr2_lr_accum {
  public :
    int count;
    amsr2_base lr[12];
  public :
    amsr2_lr_accum(void);
};
amsr2_lr_accum::amsr2_lr_accum() {
  count = 0;
  //printf("initialized amsr2_lr\n"); fflush(stdout);
}
// friends:
void hradd(grid2<amsr2_hr_accum> &nh_hr_accum, ijpt loc, amsr2_hrpt &hr);
void lradd(grid2<amsr2_lr_accum> &nh_lr_accum, ijpt loc, amsr2_lrpt &lr);
void hravg(grid2<amsr2_hr_accum> &nh_hr_accum);
void lravg(grid2<amsr2_lr_accum> &nh_lr_accum);
//////////////////////////////////////////////////////////////////////////  

int main(int argc, char *argv[]) {
// Reading through file:
  amsr2head head;
  amsr2_spot s[12];
  amsr2_hrpt hr;
  amsr2_lrpt lr;

  southhigh2<float> sgrid;
  southhigh2<unsigned char> sgridchar;
  grid2<amsr2_lr_accum> sh_lr_accum(sgrid.xpoints(), sgrid.ypoints());
  grid2<amsr2_hr_accum> sh_hr_accum(sgrid.xpoints(), sgrid.ypoints());
  northhigh2<float> ngrid;
  northhigh2<unsigned char> ngridchar;
  grid2<amsr2_lr_accum> nh_lr_accum(ngrid.xpoints(), ngrid.ypoints());
  grid2<amsr2_hr_accum> nh_hr_accum(ngrid.xpoints(), ngrid.ypoints());
  
  int i, nok_hf = 0, nok_lf = 0, nread = 0, nobs, nhigh = 0, nlow = 0;
  FILE *fin;
  ijpt loc;
  latpt ll;

//////////////////////////////////////////////////////
// Algorithm Prep For team2
  amsr_team2_tables arctic, antarctic;
  arctic.tbmfy.resize(n_atm, n_tb);
  arctic.tbmow.resize(n_atm, n_tb);
  arctic.tbmcc.resize(n_atm, n_tb);
  arctic.tbmthin.resize(n_atm, n_tb);
  arctic.pole = 'n';
  arctic_tables(arctic);
  lookuptable(arctic);

  antarctic.tbmfy.resize(n_atm, n_tb);
  antarctic.tbmow.resize(n_atm, n_tb);
  antarctic.tbmcc.resize(n_atm, n_tb);
  antarctic.tbmthin.resize(n_atm, n_tb);
  antarctic.pole = 's';
  antarctic_tables(antarctic);
  lookuptable(antarctic);
  printf("Done with getting the team2 tables for amsr2\n");fflush(stdout);
// End Team2 initialization



// Reading:
  fin = fopen(argv[1], "r");
  if ( fin != (FILE*)NULL) {
    printf("successfully opened %s\n",argv[1]); fflush(stdout);
  }
  else {
    printf("could not open input file %s\n",argv[1]); fflush(stdout);
    return 1;
  }
  rewind(fin);

  while (!feof(fin) && nread < 400e6) {

// read in data
    fread(&head, sizeof(head), 1, fin);
    nobs = head.nspots;
    ll.lat = head.clat;
    ll.lon = head.clon;
    fread(&s[0], sizeof(amsr2_spot), nobs, fin);

    if (ll.lat < 25.0 && ll.lat > -40.0) continue;

// parcel out to high res or low res (observations) grids /////////////////
    loc.j = -1; loc.i = -1;
    if (nobs == 12) { // low resolution spots
      nlow += 1;
      lr.head = head;
      for (i = 0; i < nobs; i++) { lr.obs[i] = s[i]; }
      if (lfok(lr)) { 
        nok_lf += 1;
        // this is where to split between grids:
        if (ll.lat > 0) {
          loc = ngrid.locate(ll);
          if (ngrid.in(loc)) {
            lradd(nh_lr_accum, loc, lr);
          }
        }
        else {
          loc = sgrid.locate(ll);
          if (sgrid.in(loc)) {
            lradd(sh_lr_accum, loc, lr);
          }
        }
      }
    }
    if (nobs == 2) { // high resolution spots
      hr.head = head;
      nhigh += 1;
      for (i = 0; i < nobs; i++) { hr.obs[i] = s[i]; }
      if (hfok(hr)) { 
        nok_hf += 1;
        // this is where to split between grids:
        if (ll.lat > 0) {
          loc = ngrid.locate(ll);
          if (ngrid.in(loc)) {
            hradd(nh_hr_accum, loc, hr);
          }
        }
        else {
          loc = sgrid.locate(ll);
          if (sgrid.in(loc)) {
            hradd(sh_hr_accum, loc, hr);
          }
        }
      }
    }
////////////////////
    int outfreq = 1000000;
    if (nread % outfreq == 0) printf("nr = %d k\n",nread/1000); fflush(stdout);
    nread += 1;
  }
  fclose(fin);
  printf("nr = %d nread, # ok high freq = %d  low freq = %d\n",nread, nok_hf, nok_lf);
  fflush(stdout);

  hravg(nh_hr_accum);
  lravg(nh_lr_accum);

  hravg(sh_hr_accum);
  lravg(sh_lr_accum);

  printf("Done with averaging the grids for amsr2\n");fflush(stdout);
    
// Now sweep the grid and wherever there are valid obs, compute ice concentration:
  for (loc.j = 0; loc.j < ngrid.ypoints(); loc.j++) {
  for (loc.i = 0; loc.i < ngrid.xpoints(); loc.i++) {
     if (nh_hr_accum[loc].count == 0 || nh_lr_accum[loc].count == 0) {
       ngrid[loc] = NO_DATA;
     }
     else if (nh_lr_accum[loc].lr[AMSR2_T19V].spot.alfr == 0 &&
              nh_hr_accum[loc].hr[AMSR2_T89V].spot.alfr == 0    ) {
       // land = 0, pure ocean = 1, some gradiations in between
       ngrid[loc] = LAND;
     }
     else {
       ngrid[loc] = nasa_team2(
                           nh_lr_accum[loc].lr[AMSR2_T6p9H].spot.tmbr,
                           nh_lr_accum[loc].lr[AMSR2_T6p9V].spot.tmbr,
                           nh_lr_accum[loc].lr[AMSR2_T7p3H].spot.tmbr,
                           nh_lr_accum[loc].lr[AMSR2_T7p3V].spot.tmbr,
                           nh_lr_accum[loc].lr[AMSR2_T11H].spot.tmbr,
                           nh_lr_accum[loc].lr[AMSR2_T11V].spot.tmbr,

                           nh_lr_accum[loc].lr[AMSR2_T19V].spot.tmbr, 
                           nh_lr_accum[loc].lr[AMSR2_T19H].spot.tmbr, 
                           nh_lr_accum[loc].lr[AMSR2_T24V].spot.tmbr, 
                           nh_lr_accum[loc].lr[AMSR2_T37V].spot.tmbr, 
                           nh_lr_accum[loc].lr[AMSR2_T37H].spot.tmbr, 
                           nh_hr_accum[loc].hr[AMSR2_T89V].spot.tmbr, 
                           nh_hr_accum[loc].hr[AMSR2_T89H].spot.tmbr, arctic, (float) ll.lat);
     }
  }
  }
  
// Southern hemisphere:
  for (loc.j = 0; loc.j < sgrid.ypoints(); loc.j++) {
  for (loc.i = 0; loc.i < sgrid.xpoints(); loc.i++) {
     if (sh_hr_accum[loc].count == 0 || sh_lr_accum[loc].count == 0) {
       sgrid[loc] = NO_DATA;
     }
     else if (sh_lr_accum[loc].lr[AMSR2_T19V].spot.alfr == 0 &&
              sh_hr_accum[loc].hr[AMSR2_T89V].spot.alfr == 0    ) {
       // land = 0, pure ocean = 1, some gradiations in between
       sgrid[loc] = LAND;
     }
     else {
       sgrid[loc] = nasa_team2(
                           sh_lr_accum[loc].lr[AMSR2_T6p9H].spot.tmbr,
                           sh_lr_accum[loc].lr[AMSR2_T6p9V].spot.tmbr,
                           sh_lr_accum[loc].lr[AMSR2_T7p3H].spot.tmbr,
                           sh_lr_accum[loc].lr[AMSR2_T7p3V].spot.tmbr,
                           sh_lr_accum[loc].lr[AMSR2_T11H].spot.tmbr,
                           sh_lr_accum[loc].lr[AMSR2_T11V].spot.tmbr,
                       
                           sh_lr_accum[loc].lr[AMSR2_T19V].spot.tmbr, 
                           sh_lr_accum[loc].lr[AMSR2_T19H].spot.tmbr, 
                           sh_lr_accum[loc].lr[AMSR2_T24V].spot.tmbr, 
                           sh_lr_accum[loc].lr[AMSR2_T37V].spot.tmbr, 
                           sh_lr_accum[loc].lr[AMSR2_T37H].spot.tmbr, 
                           sh_hr_accum[loc].hr[AMSR2_T89V].spot.tmbr, 
                           sh_hr_accum[loc].hr[AMSR2_T89H].spot.tmbr, arctic, (float) ll.lat);
     }
  }
  }

  FILE *fout;
  palette<unsigned char> gg(19, 65);
  char fname[255];

// argv2,3 = n, s 12.7 km land masks
// argv4 = base name for nh
// argv5 = base name for sh 
// argv6 = name for nh iceconc field
// argv7 = name for sh iceconc field
// argv8 = gshhs bounding curves
// argv9 = distance to land file
  sprintf(fname,"%s_hr",argv[4]);
  fout = fopen(fname,"w");
  nh_hr_accum.binout(fout);
  fclose(fout);
  sprintf(fname,"%s_lr",argv[4]);
  fout = fopen(fname,"w");
  nh_lr_accum.binout(fout);
  fclose(fout);

  conv(ngrid, ngridchar);
  fout = fopen(argv[6],"w");
  ngridchar.binout(fout);
  fclose(fout);
  sprintf(fname, "n.xpm");
  ngrid.xpm(fname,7,gg);
  
  sprintf(fname,"%s_hr",argv[5]);
  fout = fopen(fname,"w");
  sh_hr_accum.binout(fout);
  fclose(fout);
  sprintf(fname,"%s_lr",argv[5]);
  fout = fopen(fname,"w");
  sh_lr_accum.binout(fout);
  fclose(fout);

  fout = fopen(argv[7],"w");
  conv(sgrid, sgridchar);
  sgridchar.binout(fout);
  fclose(fout);
  sprintf(fname,"s.xpm");
  sgrid.xpm(fname,7,gg);

  return 0;
}
////////////////////////////////////////////////////////////////////////////////

bool hfok(amsr2_hrpt &hr) {
  bool tmp = true;
  if (hr.obs[0].tmbr > 285. || hr.obs[1].tmbr > 285.) tmp = false;
  if (hr.obs[0].tmbr > hr.obs[1].tmbr ) tmp = false;

  return tmp;
}
bool lfok(amsr2_lrpt &lr) {
  bool tmp = true;
// range test
  for (int i = 0; i < 12; i++) {
    if (lr.obs[i].tmbr > 285.) tmp = false;
  }
// polarization test
  for (int i = 0; i < 12; i+= 2) {
    if (lr.obs[i].tmbr > lr.obs[i+1].tmbr ) tmp = false;
  }

  return tmp;
}
void hradd(grid2<amsr2_hr_accum> &nh_hr_accum, ijpt loc, amsr2_hrpt &hr) {
  nh_hr_accum[loc].count += 1;
  for (int i = 0; i < 2; i++) {
    nh_hr_accum[loc].hr[i].spot.sccf += hr.obs[i].sccf;
    nh_hr_accum[loc].hr[i].spot.alfr += hr.obs[i].alfr;
    nh_hr_accum[loc].hr[i].spot.anpo += hr.obs[i].anpo;
    nh_hr_accum[loc].hr[i].spot.viirsq += hr.obs[i].viirsq;
    nh_hr_accum[loc].hr[i].spot.tmbr += hr.obs[i].tmbr;
  }
  return;
}
void lradd(grid2<amsr2_lr_accum> &nh_lr_accum, ijpt loc, amsr2_lrpt &lr) {
  nh_lr_accum[loc].count += 1;
  for (int i = 0; i < 12; i++) {
    nh_lr_accum[loc].lr[i].spot.sccf += lr.obs[i].sccf;
    nh_lr_accum[loc].lr[i].spot.alfr += lr.obs[i].alfr;
    nh_lr_accum[loc].lr[i].spot.anpo += lr.obs[i].anpo;
    nh_lr_accum[loc].lr[i].spot.viirsq += lr.obs[i].viirsq;
    nh_lr_accum[loc].lr[i].spot.tmbr += lr.obs[i].tmbr;
  }
  return;
}
void hravg(grid2<amsr2_hr_accum> &nh_hr_accum) {
  ijpt loc;
  for (loc.j = 0; loc.j < nh_hr_accum.ypoints(); loc.j++) {
  for (loc.i = 0; loc.i < nh_hr_accum.xpoints(); loc.i++) {
    if (nh_hr_accum[loc].count != 0) {
      for (int i = 0; i < 2; i++) {
        nh_hr_accum[loc].hr[i].spot.sccf /= nh_hr_accum[loc].count ;
        nh_hr_accum[loc].hr[i].spot.alfr /= nh_hr_accum[loc].count ;
        nh_hr_accum[loc].hr[i].spot.anpo /= nh_hr_accum[loc].count ;
        nh_hr_accum[loc].hr[i].spot.viirsq /= nh_hr_accum[loc].count ;
        nh_hr_accum[loc].hr[i].spot.tmbr /= nh_hr_accum[loc].count ;
      }
    }
  }
  }
  return;
}
void lravg(grid2<amsr2_lr_accum> &nh_lr_accum) {
  ijpt loc;
  for (loc.j = 0; loc.j < nh_lr_accum.ypoints(); loc.j++) {
  for (loc.i = 0; loc.i < nh_lr_accum.xpoints(); loc.i++) {
    if (nh_lr_accum[loc].count != 0) {
      for (int i = 0; i < 12; i++) {
        nh_lr_accum[loc].lr[i].spot.sccf /= nh_lr_accum[loc].count ;
        nh_lr_accum[loc].lr[i].spot.alfr /= nh_lr_accum[loc].count ;
        nh_lr_accum[loc].lr[i].spot.anpo /= nh_lr_accum[loc].count ;
        nh_lr_accum[loc].lr[i].spot.viirsq /= nh_lr_accum[loc].count ;
        nh_lr_accum[loc].lr[i].spot.tmbr /= nh_lr_accum[loc].count ;
      }
    }
  }
  }
  return;
}
/////////////////////////////////////////////////////

// Make function to do the computations/filtering regarding weather.
// Isolate the decision to this, rather than the embedded structure
//   (in to nasa_team and team2) in the prior renditions of weather
//   filtering.
// Proximally prompted by the F-15 new filter, but makes sense for
//   any system
// Version for AMSR2
float weather(double &t19v, double &t19h, double &t24v, double &t37v, double &t37h,
             double &t89v, double &t89h) {
    float gr37, gr24;
    float amsr2_gr37lim = 0.046;
    float amsr2_gr24lim = 0.045;

    gr37 = (t37v - t19v) / (t37v + t19v);
    gr24 = (t24v - t19v) / (t24v + t19v);
    if (gr37 < amsr2_gr37lim && gr24 < amsr2_gr24lim) {
      return 0;
    }
    else {
      return WEATHER;
    }

}
bool notbogus(double &h6p9, double &v6p9, double &h7p3, double &v7p3, double &h11, double &v11,
	       double &h19) {
  return (
    h6p9 > 219 &&
    v6p9 > 244 &&
    h7p3 > 220 &&
    v7p3 > 244 &&
    h11  > 219 &&
    v11  > 244 &&
    h19  > 227    );
} 


void amsr2_regress_to_amsre(double &v19, double &h19, double &v24, 
                            double &v37, double &h37, double &v89, double &h89, float lat) {
  if (lat > 0) {
    v19 = v19 * 1.031 - 9.710;
    h19 = h19 * 1.001 - 1.104;
    v24 = v24 * 0.999 - 1.706;
    v37 = v37 * 0.997 - 2.610;
    h37 = h37 * 0.996 - 2.687;
    v89 = v89 * 0.989 + 0.677;
    h89 = h89 * 0.977 + 3.184;
  }
  else {
    v19 = v19 * 1.032 - 10.013;
    h19 = h19 * 1.000 - 1.320;
    v24 = v24 * 0.993 - 0.987;
    v37 = v37 * 0.995 - 2.400;
    h37 = h37 * 0.994 - 2.415;
    v89 = v89 * 0.975 + 4.239;
    h89 = h89 * 0.969 + 4.935;
  }

  return;
}
