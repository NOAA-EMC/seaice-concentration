#ifndef ICEAMSR
  #include "amsrice.h"
#endif

#define CASTART 49
#define CCSTART 49

extern FILE *tester;

// Elements for the TEAM2 algorithm 
void arctic_tables(amsr_team2_tables &x);
void antarctic_tables(amsr_team2_tables &x);
void lookuptable(amsr_team2_tables &x);

//////////////// Begin TEAM2 code:
void arctic_tables(amsr_team2_tables &arctic) {
// equivalent to get_lut in the original nt2 file
  grid2<float> tbmfy(n_atm, n_tb), tbmow(n_atm, n_tb), tbmcc(n_atm, n_tb);
  grid2<float> tbmthin(n_atm, n_tb);
  FILE *fin;
  ijpt loc; 
  float tmp;
  double phi19, phi89;

  fin = fopen("seaice_TBfyark.tab.amsr2","r");
  for (loc.i = 0; loc.i < n_atm; loc.i++) {
  for (loc.j = 0; loc.j < n_tb; loc.j++) {
    fscanf(fin, "%f", &tmp);
    tbmfy[loc] = tmp;
  }
  }    
  fclose(fin);

  fin = fopen("seaice_TBccark.tab.amsr2","r");
  for (loc.i = 0; loc.i < n_atm; loc.i++) {
  for (loc.j = 0; loc.j < n_tb; loc.j++) {
    fscanf(fin, "%f", &tmp);
    tbmcc[loc] = tmp;
  }
  }    
  fclose(fin);
  fin = fopen("seaice_TBowark.tab.amsr2","r");
  for (loc.i = 0; loc.i < n_atm; loc.i++) {
  for (loc.j = 0; loc.j < n_tb; loc.j++) {
    fscanf(fin, "%f", &tmp);
    tbmow[loc] = tmp;
  }
  }    
  fclose(fin);
  fin = fopen("seaice_TBthark.tab.amsr2","r");
  for (loc.i = 0; loc.i < n_atm; loc.i++) {
  for (loc.j = 0; loc.j < n_tb; loc.j++) {
    fscanf(fin, "%f", &tmp);
    tbmthin[loc] = tmp;
  }
  }    
  fclose(fin);

  /* ROTATION */
  phi19=-0.18;
  phi89=-0.06;

  arctic.tbmfy = tbmfy;
  arctic.tbmow = tbmow;
  arctic.tbmcc = tbmcc;
  arctic.tbmthin = tbmthin;
  arctic.phi19 = phi19;
  arctic.phi89 = phi89;

  return;
}
void antarctic_tables(amsr_team2_tables &antarctic) {
// equivalent to get_lut in the original nt2 file
  grid2<float> tbmfy(n_atm, n_tb), tbmow(n_atm, n_tb), tbmcc(n_atm, n_tb);
  grid2<float> tbmthin(n_atm, n_tb);
  FILE *fin;
  ijpt loc; 
  float tmp;
  double phi19, phi89;

  fin = fopen("seaice_TBfyant.tab.amsr2","r");
  for (loc.i = 0; loc.i < n_atm; loc.i++) {
  for (loc.j = 0; loc.j < n_tb; loc.j++) {
    fscanf(fin, "%f", &tmp);
    tbmfy[loc] = tmp;
  }
  }    
  fclose(fin);

  fin = fopen("seaice_TBccant.tab.amsr2","r");
  for (loc.i = 0; loc.i < n_atm; loc.i++) {
  for (loc.j = 0; loc.j < n_tb; loc.j++) {
    fscanf(fin, "%f", &tmp);
    tbmcc[loc] = tmp;
  }
  }    
  fclose(fin);
  fin = fopen("seaice_TBowant.tab.amsr2","r");
  for (loc.i = 0; loc.i < n_atm; loc.i++) {
  for (loc.j = 0; loc.j < n_tb; loc.j++) {
    fscanf(fin, "%f", &tmp);
    tbmow[loc] = tmp;
  }
  }    
  fclose(fin);
  fin = fopen("seaice_TBthant.tab.amsr2","r");
  for (loc.i = 0; loc.i < n_atm; loc.i++) {
  for (loc.j = 0; loc.j < n_tb; loc.j++) {
    fscanf(fin, "%f", &tmp);
    tbmthin[loc] = tmp;
  }
  }    
  fclose(fin);

  /* ROTATION */
  phi19=-0.59;
  phi89=-0.4;

  antarctic.tbmfy = tbmfy;
  antarctic.tbmow = tbmow;
  antarctic.tbmcc = tbmcc;
  antarctic.tbmthin  = tbmthin ;
  antarctic.phi19 = phi19;
  antarctic.phi89 = phi89;

  return;
}

void lookuptable(amsr_team2_tables &tab) {
// extracted from the original get_lut to its own function:
  int ca, cb, k;
  float caf, cbf;

  float tb19h, tb19v, tb37v, tb89h, tb89v;
  float tb19ht, tb19vt, tb37vt, tb89ht, tb89vt;

  for (int i = 0; i < n_atm; i++) {
    tab.LUT19[i].resize(101, 101);
    tab.LUT89[i].resize(101, 101);
    tab.LUT19thin[i].resize(101, 101);
    tab.LUT89thin[i].resize(101, 101);
    tab.LUTDGR[i].resize(101, 101);
    tab.LUTGR37[i].resize(101, 101);
  }

    
  int nx = tab.tbmow.xpoints();
  ijpt loc;

  for (ca=0;ca<101;ca++) {
    loc.i = ca;
    for (cb=0;cb<(101-ca);cb++) {
      loc.j = cb;
      caf=ca/100.0;
      cbf=cb/100.0;
      for (k=0;k<n_atm;k++){

        tb19h=(1.-caf-cbf)*tab.tbmow[k+nx*0]+caf*tab.tbmfy[k+nx*0]+cbf*tab.tbmcc[k+nx*0];
        tb19v=(1.-caf-cbf)*tab.tbmow[k+nx*1]+caf*tab.tbmfy[k+nx*1]+cbf*tab.tbmcc[k+nx*1];
        tb37v=(1.-caf-cbf)*tab.tbmow[k+nx*4]+caf*tab.tbmfy[k+nx*4]+cbf*tab.tbmcc[k+nx*4];
        tb89h=(1.-caf-cbf)*tab.tbmow[k+nx*5]+caf*tab.tbmfy[k+nx*5]+cbf*tab.tbmcc[k+nx*5];
        tb89v=(1.-caf-cbf)*tab.tbmow[k+nx*6]+caf*tab.tbmfy[k+nx*6]+cbf*tab.tbmcc[k+nx*6];

        tb19ht=(1.-caf-cbf)*tab.tbmow[k+nx*0]+caf*tab.tbmfy[k+nx*0]+cbf*tab.tbmthin[k+nx*0];
        tb19vt=(1.-caf-cbf)*tab.tbmow[k+nx*1]+caf*tab.tbmfy[k+nx*1]+cbf*tab.tbmthin[k+nx*1];
        tb37vt=(1.-caf-cbf)*tab.tbmow[k+nx*4]+caf*tab.tbmfy[k+nx*4]+cbf*tab.tbmthin[k+nx*4];
        tb89ht=(1.-caf-cbf)*tab.tbmow[k+nx*5]+caf*tab.tbmfy[k+nx*5]+cbf*tab.tbmthin[k+nx*5];
        tb89vt=(1.-caf-cbf)*tab.tbmow[k+nx*6]+caf*tab.tbmfy[k+nx*6]+cbf*tab.tbmthin[k+nx*6];

        tab.LUT19[k][loc]= -((tb37v-tb19v)/(tb37v+tb19v))*sin(tab.phi19)+
                            ((tb19v-tb19h)/(tb19v+tb19h))*cos(tab.phi19);
        tab.LUT89[k][loc]= -((tb37v-tb19v)/(tb37v+tb19v))*sin(tab.phi89)+
                            ((tb89v-tb89h)/(tb89v+tb89h))*cos(tab.phi89);
        tab.LUT19thin[k][loc]= -((tb37vt-tb19vt)/(tb37vt+tb19vt))*sin(tab.phi19)+
                                ((tb19vt-tb19ht)/(tb19vt+tb19ht))*cos(tab.phi19);
        tab.LUT89thin[k][loc]= -((tb37vt-tb19vt)/(tb37vt+tb19vt))*sin(tab.phi89)+
                                ((tb89vt-tb89ht)/(tb89vt+tb89ht))*cos(tab.phi89);
        tab.LUTDGR[k][loc] = (tb89h-tb19h)/(tb89h+tb19h)-(tb89v-tb19v)/(tb89v+tb19v);
        tab.LUTGR37[k][loc]= (tb37vt-tb19vt)/(tb37vt+tb19vt);

      } /* k */
    }/* cb */
  }/* ca */

  return ;
}

float nasa_team2(float v19, float h19, float v24, float v37, float h37, 
                 float v89, float h89, amsr_team2_tables &tab, float lat) {
// nt2 in NASA original
  
    double sinphi19,sinphi89;
    double cosphi19,cosphi89;

    double v19i,h19i,v24i,v37i,h37i,v89i,h89i;
    double pr19,pr89,gr3719,gr8919v,gr8919h;
    double pr19r,pr89r,dgr;
    double dpr19,dpr89,ddgr;
    float icecon = BAD_DATA;

    float w19 = 1., w89 = 1., wgr = 1.;
    int ca, cc, camina[n_atm], ccmina[n_atm];
    double dmin, dmina[n_atm], d;
    int i, j, k, imin, jmin, bestk;
    int cai, ccj;

// x, y are used in the original TEAM2 to index locations in the
//Bering sea and Sea of Okhotsk, where the thin ice algorithm 
//can be used.
//    float x = 999, y=999;

    sinphi19=sin(tab.phi19);
    sinphi89=sin(tab.phi89);
    cosphi19=cos(tab.phi19);
    cosphi89=cos(tab.phi89);

    if ((v19 > 50.)&&(v89 > 50.) ) {
      #ifdef TESTER
        fwrite(&tab.pole, sizeof(char), 1, tester);
        fwrite(&v19, sizeof(float), 1, tester);
        fwrite(&h19, sizeof(float), 1, tester);
        fwrite(&v24, sizeof(float), 1, tester);
        fwrite(&v37, sizeof(float), 1, tester);
        fwrite(&h37, sizeof(float), 1, tester);
        fwrite(&v89, sizeof(float), 1, tester);
        fwrite(&h89, sizeof(float), 1, tester);
      #endif

      /*** If data are valid and no land ***/
        v19i=(double) v19;
        h19i=(double) h19;
        v24i=(double) v24;
        v37i=(double) v37;
        h37i=(double) h37;
        v89i=(double) v89;
        h89i=(double) h89;
        amsr2_regress_to_amsre(v19i, h19i, v24i, v37i, h37i, v89i, h89i, lat);

        gr3719=(v37i-v19i)/(v37i+v19i);

// NOTE: in older code, only the 24ghz filter is used ... why? ... other is commented out
// Newer team2 code returns to both filters
        if (weather(v19i, h19i, v24i, v37i, h37i, v89i, h89i) != WEATHER) {
        /*** if passed the weather filters ***/
          pr19=(v19i-h19i)/(v19i+h19i);
          pr89=(v89i-h89i)/(v89i+h89i);

          gr8919v=(v89i-v19i)/(v89i+v19i);
          gr8919h=(h89i-h19i)/(h89i+h19i);

          pr19r=-gr3719*sinphi19 + pr19*cosphi19;
          pr89r=-gr3719*sinphi89 + pr89*cosphi89;
          dgr=gr8919h-gr8919v;

          dmin=10000.;

          for (k=0;k<n_atm;k++){

          imin=5;
          jmin=5;
          ca=CASTART;
          cc=CCSTART;
          ijpt loc;
          do {

            dmin=10000.;

            for (i=-1;i<=1;i++){
            for (j=-1;j<=1;j++){
              cai=ca+i;
              ccj=cc+j; 
              loc.i = cai;
              loc.j = ccj;
              if((cai < 101)&&(ccj < 101)&&(cai >=0)&&(ccj >=0)&&
                      ((cai+ccj) >=0)&&((cai+ccj) < 101)){

                // The newer (Jan 2009) NASA Team2 algorithm uses only the gr3719
                //   value to decide whether to use the thin ice algorithm.
                // It also applies the thin ice algorithm to both poles.  Hence
                //   This section can not be pole-independant, and just respond
                //   to the thin ice vs. not-thin
                // Robert Grumbine 11 August 2010
                //old if((tab.pole == 'n')&&(gr3719 > -0.01)&&
                //old    (((x > 0)&&(x < 80)&&(y >100)&&(y <190))||
                //old    ((x > 80)&&(x <160)&&(y >0)&&(y <130)))){
                //old      /** Only pixels of Bering Sea (top)
                //old      and Sea of Okhotsk (bottom **/
                if (gr3719 > -0.01) {
                  dpr19=pr19r-tab.LUT19thin[k][loc];
                  dpr89=pr89r-tab.LUT89thin[k][loc];
                  ddgr=gr3719-tab.LUTGR37[k][loc];
                } 
                else {
                  dpr19=pr19r-tab.LUT19[k][loc];
                  dpr89=pr89r-tab.LUT89[k][loc];
                  ddgr=dgr-tab.LUTDGR[k][loc];
                }

                d=w19*dpr19*dpr19+w89*dpr89*dpr89+wgr*ddgr*ddgr;

                if (d < dmin){
                  dmin=d;
                  imin=i;
                  jmin=j;
                }
              } /*endif*/
            } /*endfor*/
            }/*endfor*/
            ca=ca+imin;
            cc=cc+jmin;
       } /*do */ while ((imin !=0)||(jmin != 0));

       camina[k]=ca;
       ccmina[k]=cc;
       dmina[k]=dmin;

       #ifdef TESTER
         fwrite(&ca, sizeof(int), 1, tester);
         fwrite(&cc, sizeof(int), 1, tester);
         fwrite(&dmina , sizeof(double), 1, tester);
         fwrite(&k , sizeof(int), 1, tester);
       #endif

       } /*k*/

       bestk=20;
       dmin=1000;
       for (k=0;k<n_atm;k++){
         if (dmina[k] < dmin){
           dmin=dmina[k];
           bestk=k;
         }
       }

       icecon=(camina[bestk]+ccmina[bestk]);
       // Newer Team2 added this test, not sure it's needed
       if (icecon > 100) {
         printf("ice concentration over 100 %f\n",(float) icecon);
         icecon = NO_DATA;
       }

    } /*endif*/
    else {
      icecon = WEATHER;   /** Weather **/
    }
  }/* endif*/
  else {
    icecon = NO_DATA;     /** Missing data **/
  }

  return icecon;
}
