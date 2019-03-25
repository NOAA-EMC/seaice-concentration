
#define CASTART 49
#define CCSTART 49

extern FILE *tester;

// Elements for the TEAM2 algorithm 
void arctic_tables(ssmi_team2_tables &x);
void antarctic_tables(ssmi_team2_tables &x);
void lookuptable(ssmi_team2_tables &x);

//////////////// Being TEAM2 code:
void arctic_tables(ssmi_team2_tables &arctic) {
  grid2<float> tbmfy(n_atm, n_tb), tbmow(n_atm, n_tb), tbmcc(n_atm, n_tb);
  grid2<float> tbmthin(n_atm, n_tb);
  FILE *fin;
  ijpt loc; 
  float tmp;
  double phi19, phi85;

  fin = fopen("seaice_TBfyark.tab","r");
  for (loc.i = 0; loc.i < n_atm; loc.i++) {
  for (loc.j = 0; loc.j < n_tb; loc.j++) {
    fscanf(fin, "%f", &tmp);
    tbmfy[loc] = tmp;
  }
  }    
  fclose(fin);

  fin = fopen("seaice_TBccark.tab","r");
  for (loc.i = 0; loc.i < n_atm; loc.i++) {
  for (loc.j = 0; loc.j < n_tb; loc.j++) {
    fscanf(fin, "%f", &tmp);
    tbmcc[loc] = tmp;
  }
  }    
  fclose(fin);
  fin = fopen("seaice_TBowark.tab","r");
  for (loc.i = 0; loc.i < n_atm; loc.i++) {
  for (loc.j = 0; loc.j < n_tb; loc.j++) {
    fscanf(fin, "%f", &tmp);
    tbmow[loc] = tmp;
  }
  }    
  fclose(fin);
  fin = fopen("seaice_TBthark.tab","r");
  for (loc.i = 0; loc.i < n_atm; loc.i++) {
  for (loc.j = 0; loc.j < n_tb; loc.j++) {
    fscanf(fin, "%f", &tmp);
    tbmthin[loc] = tmp;
  }
  }    
  fclose(fin);

  /* ROTATION */
  phi19=-0.18;
  phi85=-0.06;

  arctic.tbmfy = tbmfy;
  arctic.tbmow = tbmow;
  arctic.tbmcc = tbmcc;
  arctic.tbmthin = tbmthin;
  arctic.phi19 = phi19;
  arctic.phi85 = phi85;

  return;
}
void antarctic_tables(ssmi_team2_tables &antarctic) {
  grid2<float> tbmfy(n_atm, n_tb), tbmow(n_atm, n_tb), tbmcc(n_atm, n_tb);
  FILE *fin;
  ijpt loc; 
  float tmp;
  double phi19, phi85;

  fin = fopen("seaice_TBfyant.tab","r");
  for (loc.i = 0; loc.i < n_atm; loc.i++) {
  for (loc.j = 0; loc.j < n_tb; loc.j++) {
    fscanf(fin, "%f", &tmp);
    tbmfy[loc] = tmp;
  }
  }    
  fclose(fin);

  fin = fopen("seaice_TBccant.tab","r");
  for (loc.i = 0; loc.i < n_atm; loc.i++) {
  for (loc.j = 0; loc.j < n_tb; loc.j++) {
    fscanf(fin, "%f", &tmp);
    tbmcc[loc] = tmp;
  }
  }    
  fclose(fin);
  fin = fopen("seaice_TBowant.tab","r");
  for (loc.i = 0; loc.i < n_atm; loc.i++) {
  for (loc.j = 0; loc.j < n_tb; loc.j++) {
    fscanf(fin, "%f", &tmp);
    tbmow[loc] = tmp;
  }
  }    
  fclose(fin);

  /* ROTATION */
  phi19=-0.59;
  phi85=-0.4;

  antarctic.tbmfy = tbmfy;
  antarctic.tbmow = tbmow;
  antarctic.tbmcc = tbmcc;
  antarctic.phi19 = phi19;
  antarctic.phi85 = phi85;

  return;
}

void lookuptable(ssmi_team2_tables &tab) {
  int ca, cb, k;
  float caf, cbf;

  float tb19h, tb19v, tb37v, tb85h, tb85v;
  float tb19ht, tb19vt, tb37vt, tb85ht, tb85vt;

  for (int i = 0; i < n_atm; i++) {
    tab.LUT19[i].resize(101, 101);
    tab.LUT85[i].resize(101, 101);
    tab.LUT19thin[i].resize(101, 101);
    tab.LUT85thin[i].resize(101, 101);
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
        tb85h=(1.-caf-cbf)*tab.tbmow[k+nx*5]+caf*tab.tbmfy[k+nx*5]+cbf*tab.tbmcc[k+nx*5];
        tb85v=(1.-caf-cbf)*tab.tbmow[k+nx*6]+caf*tab.tbmfy[k+nx*6]+cbf*tab.tbmcc[k+nx*6];

        tb19ht=(1.-caf-cbf)*tab.tbmow[k+nx*0]+caf*tab.tbmfy[k+nx*0]+cbf*tab.tbmthin[k+nx*0];
        tb19vt=(1.-caf-cbf)*tab.tbmow[k+nx*1]+caf*tab.tbmfy[k+nx*1]+cbf*tab.tbmthin[k+nx*1];
        tb37vt=(1.-caf-cbf)*tab.tbmow[k+nx*4]+caf*tab.tbmfy[k+nx*4]+cbf*tab.tbmthin[k+nx*4];
        tb85ht=(1.-caf-cbf)*tab.tbmow[k+nx*5]+caf*tab.tbmfy[k+nx*5]+cbf*tab.tbmthin[k+nx*5];
        tb85vt=(1.-caf-cbf)*tab.tbmow[k+nx*6]+caf*tab.tbmfy[k+nx*6]+cbf*tab.tbmthin[k+nx*6];

        tab.LUT19[k][loc]=-((tb37v-tb19v)/(tb37v+tb19v))*sin(tab.phi19)+
                         ((tb19v-tb19h)/(tb19v+tb19h))*cos(tab.phi19);
        tab.LUT85[k][loc]=-((tb37v-tb19v)/(tb37v+tb19v))*sin(tab.phi85)+
                          ((tb85v-tb85h)/(tb85v+tb85h))*cos(tab.phi85);
        tab.LUT19thin[k][loc]=-((tb37vt-tb19vt)/(tb37vt+tb19vt))*sin(tab.phi19)+
                         ((tb19vt-tb19ht)/(tb19vt+tb19ht))*cos(tab.phi19);
        tab.LUT85thin[k][loc]=-((tb37vt-tb19vt)/(tb37vt+tb19vt))*sin(tab.phi85)+
                          ((tb85vt-tb85ht)/(tb85vt+tb85ht))*cos(tab.phi85);
        tab.LUTDGR[k][loc]=(tb85h-tb19h)/(tb85h+tb19h)-(tb85v-tb19v)/(tb85v+tb19v);
        tab.LUTGR37[k][loc]=(tb37vt-tb19vt)/(tb37vt+tb19vt);

      } /* k */
    }/* cb */
  }/* ca */

  return ;
}

float nasa_team2(float v19, float h19, float v22, float v37, float h37, 
                 float v85, float h85, ssmi_team2_tables &tab, int satno) {
  
    double sinphi19,sinphi85;
    double cosphi19,cosphi85;

    double v19i,h19i,v22i,v37i,v85i,h85i;
    double pr19,pr85,gr3719,gr2219,gr8519v,gr8519h;
    double pr19r,pr85r,dgr;
    double dpr19,dpr85,ddgr;
    float icecon;
    float x = 999, y=999;
    float w19 = 1., w85 = 1., wgr = 1.;
    int ca, cc, camina[n_atm], ccmina[n_atm];
    double dmin, dmina[n_atm], d;
    int i, j, k, imin, jmin, bestk;
    int cai, ccj;

    sinphi19=sin(tab.phi19);
    sinphi85=sin(tab.phi85);
    cosphi19=cos(tab.phi19);
    cosphi85=cos(tab.phi85);

    if ((v19 > 50.)&&(v85 > 50.) ) {
      #ifdef TESTER
        fwrite(&tab.pole, sizeof(char), 1, tester);
        fwrite(&v19, sizeof(char), 1, tester);
        fwrite(&h19, sizeof(float), 1, tester);
        fwrite(&v22, sizeof(float), 1, tester);
        fwrite(&v37, sizeof(float), 1, tester);
        fwrite(&h37, sizeof(float), 1, tester);
        fwrite(&v85, sizeof(float), 1, tester);
        fwrite(&h85, sizeof(float), 1, tester);
      #endif

      /*** If data are valid and no land ***/
        v19i=(double) v19;
        h19i=(double) h19;
        v22i=(double) v22;
        v37i=(double) v37;
        v85i=(double) v85;
        h85i=(double) h85;

        gr3719=(v37i-v19i)/(v37i+v19i);
        gr2219=(v22i-v19i)/(v22i+v19i);

        if ((gr3719 < 0.05) && (gr2219 < 0.045)){
        /*** if passed the weather filters ***/
          pr19=(v19i-h19i)/(v19i+h19i);
          pr85=(v85i-h85i)/(v85i+h85i);

          gr8519v=(v85i-v19i)/(v85i+v19i);
          gr8519h=(h85i-h19i)/(h85i+h19i);

          pr19r=-gr3719*sinphi19 + pr19*cosphi19;
          pr85r=-gr3719*sinphi85 + pr85*cosphi85;
          dgr=gr8519h-gr8519v;

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

                if((tab.pole == 'n')&&(gr3719 > -0.01)&&
                   (((x > 0)&&(x < 80)&&(y >100)&&(y <190))||
                   ((x > 80)&&(x <160)&&(y >0)&&(y <130)))){
                     /** Only pixels of Bering Sea (top)
                     and Sea of Okhotsk (bottom **/
                  dpr19=pr19r-tab.LUT19thin[k][loc];
                  dpr85=pr85r-tab.LUT85thin[k][loc];
                  ddgr=gr3719-tab.LUTGR37[k][loc];
                } 
                else {
                  dpr19=pr19r-tab.LUT19[k][loc];
                  dpr85=pr85r-tab.LUT85[k][loc];
                  ddgr=dgr-tab.LUTDGR[k][loc];
                }

                d=w19*dpr19*dpr19+w85*dpr85*dpr85+wgr*ddgr*ddgr;

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

    } /*endif*/
    else icecon= 177;   /** Weather **/
  }/* endif*/
  else icecon = 224;     /** Missing data **/

  return icecon;
}
