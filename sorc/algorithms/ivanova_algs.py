# -*- coding: utf-8 -*-
#!/usr/bin/python
import math
import numpy as np
import os

# Please cite https://tc.copernicus.org/articles/9/1797/2015/
#   for original source -- 
#@Article{tc-9-1797-2015,
#AUTHOR = {Ivanova, N. and Pedersen, L. T. and Tonboe, R. T. and Kern, S. and Heygster, G. and Lavergne, T. and S{\o}rensen, A. and Saldo, R. and Dybkj{\ae}r, G. and Brucker, L. and Shokr, M.},
#TITLE = {Inter-comparison and evaluation of sea ice algorithms: towards further  identification of challenges and optimal approach using passive microwave  observations},
#JOURNAL = {The Cryosphere},
#VOLUME = {9},
#YEAR = {2015},
#NUMBER = {5},
#PAGES = {1797--1817},
#URL = {https://tc.copernicus.org/articles/9/1797/2015/},
#DOI = {10.5194/tc-9-1797-2015}
#}

def asi(tb85v, tb85h):
    """The asi ice concentration algorithm
    """
    P0 = 47.0
    P1 = 7.5
    P = tb85v - tb85h
    """method coefficients:"""
    d3=1.64/100000.0
    d2=-0.0016
    d1=0.0192
    d0=0.971
    """concentrations calculation:"""
    ct = d3 * P**3.0 + d2 * P**2.0 + d1 * P + d0
    return ct

def bootstrap_f(tb18v, tb37v, tiepts):
    
    tw18v = tiepts[6]
    tw37v = tiepts[0]
    tfy18v = tiepts[8]
    tfy37v = tiepts[2]
    tmy18v = tiepts[7]
    tmy37v = tiepts[1]  
    
    
    if (tb18v-tw18v)==0:
        cf=np.nan
    else:

        af = (tfy37v - tmy37v)/(tfy18v - tmy18v)
        bf = (tmy37v - af*tmy18v)
        qf = (tb37v - tw37v)/(tb18v - tw18v)
        wf = (tw37v - qf*tw18v)
        ti18vf = (bf - wf)/(qf - af)
        cf = (tb18v - tw18v)/(ti18vf - tw18v)
    return cf
    
def bootstrap_p(tb37v, tb37h, tiepts):
   
    tw37h = tiepts[3]
    tw37v = tiepts[0]
    tfy37h = tiepts[5]
    tfy37v = tiepts[2]
    tmy37h = tiepts[4]
    tmy37v = tiepts[1]
    
    
    if (tb37h-tw37h)==0:
        cp=np.nan
    else:
        ap   = (tfy37v - tmy37v) / (tfy37h - tmy37h)
        bp   = (tmy37v - ap * tmy37h)
        qp   = (tb37v - tw37v) / (tb37h - tw37h)
        wp   = (tw37v - qp * tw37h)
        if (qp - ap)==0:
            cp=np.nan
        else:
            ti37hp = (bp - wp) / (qp - ap)
            ti37vp =  ap * ti37hp + bp
            if (ti37vp - tw37v)==0:
                cp=np.nan
            else:
                cp = (tb37v - tw37v) / (ti37vp - tw37v)
    return cp

def bristol(tb18v, tb37v, tb37h, tiepts):
    """Bristol ice concentration algorithm
    """

    tw18v = tiepts[6]
    tw37h = tiepts[3]
    tw37v = tiepts[0]
    tfy18v = tiepts[8]
    tfy37h = tiepts[5]
    tfy37v = tiepts[2]
    tmy18v = tiepts[7]
    tmy37h = tiepts[4]
    tmy37v = tiepts[1]
    
    xa = tmy37v + (1.045*tmy37h) + (0.525*tmy18v)
    xd = tfy37v + (1.045*tfy37h) + (0.525*tfy18v)
    xh = tw37v + (1.045*tw37h) + (0.525*tw18v)
    xt = tb37v +(1.045*tb37h) + (0.525*tb18v)

    ya = (0.9164*tmy18v) - tmy37v + (0.4965*tmy37h)
    yd = (0.9164*tfy18v) - tfy37v + (0.4965*tfy37h)
    yh = (0.9164*tw18v) - tw37v + (0.4965*tw37h)
    yt = (0.9164*tb18v)- tb37v + (0.4965*tb37h)

    a_ht = (yt - yh)/(xt - xh)
    b_ht = yh - (a_ht*xh)
    a_da = (ya - yd)/(xa - xd)
    b_da = yd - (a_da*xd)

    xi = (b_da - b_ht)/(a_ht - a_da)
    cf = (xt - xh)/(xi - xh)
    c = cf
    return c

def calval(tb37v,tb18v,tiepts):

    tw18v = tiepts[6]
    tw37v = tiepts[0]
    tfy18v = tiepts[8]
    tfy37v = tiepts[2]
    tmy18v = tiepts[7]
    tmy37v = tiepts[1]
    
        
    A=np.matrix([[tw37v, tw18v, 1.0],\
                [tfy37v, tfy18v, 1.0],\
                [tmy37v, tmy18v, 1.0]])

    b=np.matrix([0.0, 1.0, 1.0]) 
    
    d=A.I * b.T
    C=d[0]*tb37v+d[1]*tb18v+d[2]

    return C   
    
def nasa(tb18v, tb18h, tb37v, tiepts):
    """NASA-Team ice concetration algorithm
    """
    
     
    ow18v = tiepts[6]
    ow18h = tiepts[9]
    ow37v = tiepts[0]
    fy18v = tiepts[8]
    fy18h = tiepts[11]
    fy37v = tiepts[2]
    my18v = tiepts[7]
    my18h = tiepts[10]
    my37v = tiepts[1]

    a0 = - ow18v + ow18h
    a1 =   ow18v + ow18h
    a2 =   my18v - my18h - ow18v + ow18h
    a3 = - my18v - my18h + ow18v + ow18h
    a4 =   fy18v - fy18h - ow18v + ow18h
    a5 = - fy18v - fy18h + ow18v + ow18h

    b0 = - ow37v + ow18v
    b1 =   ow37v + ow18v
    b2 =   my37v - my18v - ow37v + ow18v
    b3 = - my37v - my18v + ow37v + ow18v
    b4 =   fy37v - fy18v - ow37v + ow18v
    b5 = - fy37v - fy18v + ow37v + ow18v

    gr = (tb37v - tb18v)/(tb37v + tb18v)
    pr = (tb18v - tb18h)/(tb18v + tb18h)

    d0 = (-a2*b4) + (a4*b2)
    d1 = (-a3*b4) + (a5*b2)
    d2 = (-a2*b5) + (a4*b3)
    d3 = (-a3*b5) + (a5*b3)

    dd = d0 + d1*pr + d2*gr + d3*pr*gr
    
    f0 = (a0*b2) - (a2*b0)
    f1 = (a1*b2) - (a3*b0)
    f2 = (a0*b3) - (a2*b1)
    f3 = (a1*b3) - (a3*b1)
    m0 = (-a0*b4) + (a4*b0)
    m1 = (-a1*b4) + (a5*b0)
    m2 = (-a0*b5) + (a4*b1)
    m3 = (-a1*b5) + (a5*b1)

    cf = (f0 + f1*pr + f2*gr + f3*pr*gr)/dd
    cm = (m0 + m1*pr + m2*gr + m3*pr*gr)/dd

    cf = cf
    cm = cm
    ct = cm + cf
    return ct
def nasa2(tb18v, tb18h, tb37v, tb85v, tb85h):
    """The NASA Team 2 ice concentration algorithm
    """
    """NASA Team2 Model Data:"""
    
    tbmow = readtxt2matrix('InputNT2/tbmow.txt')
    tbmfy = readtxt2matrix('InputNT2/tbmfy.txt')
    tbmcc = readtxt2matrix('InputNT2/tbmcc.txt')
    tbmthin = readtxt2matrix('InputNT2/tbmthin.txt')

    gr3719=(tb37v-tb18v)/(tb37v+tb18v)
    """number of atmospheres (models of atmosphere):"""
    n_atm=12
    """Rotation:"""
    """angle in radians between GR-axis and A-B line (FY-MY line) for the PR(19)-GR(37V19V) damain. Arctic:"""
    phi19=-0.18
    """angle in radians between GR-axis and A-B line (FY-MY line) for the PR(85)-GR(37V19V) damain. Arctic:"""
    phi85=-0.06
   
    """Allocate memory:"""
    
    LUT19=readtxt2matrix3D('InputNT2/LUT19_')
    LUT85=readtxt2matrix3D('InputNT2/LUT85_')
    LUT19thin=readtxt2matrix3D('InputNT2/LUT19thin_')	
    LUT85thin=readtxt2matrix3D('InputNT2/LUT85thin_')
    LUTDGR=readtxt2matrix3D('InputNT2/LUTDGR_')
    LUTGR37=readtxt2matrix3D('InputNT2/LUTGR37_')
    camina=np.zeros((n_atm,1))
    ccmina=np.zeros((n_atm,1))
    dmina=np.zeros((n_atm,1))        
                   

    """calculate ice concentrations:"""
    """weights:"""   

    w19=1
    w85=1
    wgr=1

    sinphi19=math.sin(phi19)
    sinphi85=math.sin(phi85)
    cosphi19=math.cos(phi19)
    cosphi85=math.cos(phi85)

    pr19=(tb18v-tb18h)/(tb18v+tb18h)
    pr85=(tb85v-tb85h)/(tb85v+tb85h)

    gr8519v=(tb85v-tb18v)/(tb85v+tb18v)
    gr8519h=(tb85h-tb18h)/(tb85h+tb18h)

    pr19r=-gr3719*sinphi19 + pr19*cosphi19
    pr85r=-gr3719*sinphi85 + pr85*cosphi85
    dgr=gr8519h-gr8519v

    dmin=10000

    for k in range(0,n_atm-1):
        imin=5
        jmin=5
        ca=45
        cc=45
        while ((imin !=0) | (jmin != 0)):
            dmin=10000
            
            for i in range(-1,1):
                
                for j in range(-1,1):
                     cai=ca+i
                     ccj=cc+j
                     if ((cai<121) & (ccj<121) & (cai>=1) & (ccj>=1) & ((cai+ccj)>=0) & ((cai+ccj)<121)):
                         if gr3719 > -0.01:
                             dpr19=pr19r-LUT19thin[k,cai,ccj]
                             dpr85=pr85r-LUT85thin[k,cai,ccj]
                             ddgr=gr3719-LUTGR37[k,cai,ccj]
                         else:
                             dpr19=pr19r-LUT19[k,cai,ccj]
                             dpr85=pr85r-LUT85[k,cai,ccj]
                             ddgr=dgr-LUTDGR[k,cai,ccj]
                    
                         d=w19*dpr19*dpr19+w85*dpr85*dpr85+wgr*ddgr*ddgr
                         if d < dmin:
                             dmin=d
                             imin=i
                             jmin=j
                    
            ca=ca+imin
            cc=cc+jmin
       
        camina[k]=ca
        ccmina[k]=cc
        dmina[k]=dmin
        
    bestk=20
    dmin=1000
    
    for k in range(0,n_atm-1):
        if dmina[k] < dmin:
            dmin=dmina[k]
            bestk=k
    
    CT=(camina[bestk]+ccmina[bestk])/100
    
    
    return CT   

    
def near90(tb85v, tb85h, tiepts):
    tmy85v = tiepts[30]
    tfy85v = tiepts[31]
    tmy85h = tiepts[32]
    tfy85h = tiepts[33]
    tw85v  = tiepts[34]
    tw85h  = tiepts[35]
    

    P = tb85v - tb85h
    P0 = tw85v - tw85h
    P1 = tfy85v - tfy85h
    
    A=np.matrix([[P1**3.0, P1**2.0, P1, 1.0],\
                [P0**3.0, P0**2.0, P0, 1.0],\
                [3.0*P1**3.0, 2.0*P1**2.0, P1, 0.0],\
                [3.0*P0**3.0, 2.0*P0**2.0, P0, 0.0]])
    b=np.matrix([1.0, 0.0, -0.14, -1.14])
    
    d=A.I * b.T 
    
    #d=np.linalg.solve(A,b.T)
    C = d[0] * P**3 + d[1] * P**2 + d[2] * P + d[3]
    return np.float(C)

def near90_linear(tb85v, tb85h):
    ct=1.22673-0.02652*(tb85v-tb85h) 
    return ct


def norsex(tb18v,tb37v,tiepts):
    
    SAT = 260.0
    T_sa = 270.0
    T_a = 250.0
    To = 272.0
    
    tau_sa19v = 0.0610
    tau_sa37v = 0.1000
    tau_a19v = 0.0440
    tau_a37v = 0.0700
    
    
    TB_w_19v = tiepts[6]
    TB_w_37v = tiepts[0]
    TB_fy_19v = tiepts[8]
    TB_fy_37v = tiepts[2]
    TB_my_19v = tiepts[7]
    TB_my_37v = tiepts[1]

    #the sea ice temperature
    T_ice = 0.4*SAT + 0.6*To

    #Constants to be used in computing ice concentrations:
    a11 = TB_fy_19v - TB_w_19v
    a21 = TB_fy_37v - TB_w_37v
    a12 = TB_my_19v - TB_w_19v
    a22 = TB_my_37v - TB_w_37v
    d_coef = a11 * a22 - a12 * a21

    #Initialize atmospheric surface temperature: 
    t_atm_surf=SAT

    #interpolate opacity between arctic and subarctic values:
    tau19v = tau_a19v + (t_atm_surf - T_a) * (tau_sa19v - tau_a19v) / (T_sa - T_a)
    tau37v = tau_a37v + (t_atm_surf - T_a) * (tau_sa37v - tau_a37v) / (T_sa - T_a)
    
    #find emitted brightness temperature at the surface by correcting for
    #atmospheric disturbances:
    TB_surf_19v = (tb18v - t_atm_surf * (2.0 * tau19v - tau19v**2.0 + 0.01)) / (1.0 - 2.0 * tau19v + tau19v**2.0 - 0.01)
    TB_surf_37v = (tb37v - t_atm_surf * (2.0 * tau37v - tau37v**2.0 + 0.01)) / (1.0 - 2.0 * tau37v + tau37v**2.0 - 0.01)

    #Find new atmospheric surface brightness temperature and mean surface
    #emissions by solving for first year and multi-year ice concentrations.
    c1 = TB_surf_19v - TB_w_19v
    c2 = TB_surf_37v - TB_w_37v
    Cmy = (a11 * c2 - a21 * c1) / d_coef
    Cfy = (a22 * c1 - a12 * c2) / d_coef
    CT = Cfy + Cmy
    
    t_atm_surf = To + (SAT - To) * CT
    
    #interpolate opacity between arctic and subarctic values:
    tau19v = tau_a19v + (t_atm_surf - T_a) * (tau_sa19v - tau_a19v) / (T_sa - T_a)
    tau37v = tau_a37v + (t_atm_surf - T_a) * (tau_sa37v - tau_a37v) / (T_sa - T_a)
    
    #find emitted brightness temperature at the surface by correcting for
    #atmospheric disturbances:
    TB_surf_19v = (tb18v - t_atm_surf * (2.0 * tau19v - tau19v**2.0 + 0.01)) / (1.0 - 2.0 * tau19v + tau19v**2.0 - 0.01)
    TB_surf_37v = (tb37v - t_atm_surf * (2.0 * tau37v - tau37v**2.0 + 0.01)) / (1.0 - 2.0 * tau37v + tau37v**2.0 - 0.01)

    #Find new atmospheric surface brightness temperature and mean surface
    #emissions by solving for first year and multi-year ice concentrations.
    c1 = TB_surf_19v - TB_w_19v
    c2 = TB_surf_37v - TB_w_37v
    Cmy = (a11 * c2 - a21 * c1) / d_coef
    Cfy = (a22 * c1 - a12 * c2) / d_coef
    CT = Cfy + Cmy
    
    return CT

def nrl(tb37v, tb37h):
    """Lo/Naval Research Laboratory (NRL)
    """
       
    A0 = 1.3 # values from Leif
    A1 = 0.019

    c = A0 - A1*(tb37v - tb37h)

    return c

def onechannel(tb6h, tiepts):
    """Simple 1 channel algorithm
    """
    
    fy6h = tiepts[17]
    my6h = tiepts[16]
     
    ow6h = 82.3  
    i6h = (fy6h+my6h)/2   

    ct = (tb6h - ow6h)/(i6h - ow6h)
    return ct

def P90(tb85v, tb85h):
    X=(tb85v-tb85h)
    P=(X-2.63)/0.752
    
    d3=1.64/100000.0
    d2=-0.0016
    d1=0.0192
    d0=0.971
   
    c1 = d3 * P**3.0 + d2 * P**2.0 + d1 * P + d0

    c = c1+(P-8)/700 #to adjust near SIC0
    if (P>48):
        c=-0.026 #to prevent large P85 giving ice
    if (P<8.5):
        c=1.03 #to prevent low P85 losing ice

    return c
    

def pr(tb18v, tb18h, tb37v, tb37h, tiepts):
    """Simple polarization ratio algorithm
    """
    
     
    ow18v = tiepts[6]
    ow18h = tiepts[9]
    ow37v = tiepts[0]
    ow37h = tiepts[3]
    fy18v = tiepts[8]
    fy18h = tiepts[11]
    fy37v = tiepts[2]
    fy37h = tiepts[5]
    my18v = tiepts[7]
    my18h = tiepts[10]
    my37v = tiepts[1]
    my37h = tiepts[4]

    i18v = (fy18v+my18v)/2
    i18h = (fy18h+my18h)/2
    i37v = (fy37v+my37v)/2
    i37h = (fy37h+my37h)/2  

    PR18 = (tb18v - tb18h)/(tb18v + tb18h)
    PR37 = (tb37v - tb37h)/(tb37v + tb37h)

    c18 =  (ow18v*(1 - PR18) - ow18h*(1 + PR18))/(PR18*(i18v + i18h - ow18v - ow18h) - (i18v - i18h - ow18v + ow18h))
    c37 =  (ow37v*(1 - PR37) - ow37h*(1 + PR37))/(PR37*(i37v + i37h - ow37v - ow37h) - (i37v - i37h - ow37v + ow37h))
    c_old = (c18 + c37)/2
    c = c_old/(2-c_old)    

    return c

def tud(tb18v, tb37v, tb85v, tb85h, tiepts):
    """TUD ice concentration alogorithm
    """

    tw18v = tiepts[6]
    tw37v = tiepts[0]
    tfy18v = tiepts[8]
    tfy37v = tiepts[2]
    tmy18v = tiepts[7]
    tmy37v = tiepts[1]

    af = (tfy37v - tmy37v)/(tfy18v - tmy18v)
    bf = (tmy37v - af*tmy18v)
    if (tb18v - tw18v)==0:
        c=np.nan
    else:
        qf = (tb37v - tw37v)/(tb18v - tw18v)
        wf = (tw37v - qf*tw18v)
        ti18vf = (bf - wf)/(qf - af)
        cf = (tb18v - tw18v)/(ti18vf - tw18v)
        c85 = 1.22673-0.02652*(tb85v - tb85h)
        if ((c85>0) & (cf>0.1)):
            c = np.sqrt(cf*c85)
        else:
            c = cf
    return c

def twochannel10(tb10v, tb10h):
    """Simple 2 channel algorithm 10 GHz
    """
    ct=1.33313-0.01686*(tb10v-tb10h)
   
    return ct

def twochannel18(tb18v, tb18h, tiepts):
    """Simple 2 channel algorithm 18 GHz
    """
         
    ow18v = tiepts[6]
    ow18h = tiepts[9]
    fy18v = tiepts[8]
    fy18h = tiepts[11]
    my18v = tiepts[7]
    my18h = tiepts[10]
    
      
    cf = ((tb18h - ow18h)*(my18v - ow18v) - (tb18v - ow18v)*(my18h - ow18h))/((fy18h - ow18h)*(my18v - ow18v) - (fy18v - ow18v)*(my18h - ow18h))
    cm = ((tb18h - ow18h) - cf*(fy18h - ow18h))/(my18h - ow18h)

    ct = cf + cm
    return ct

def twochannel22(tb22v, tb22h, tiepts):
    """Simple 2 channel algorithm 22 GHz
    """
    
    ow22v = tiepts[24]
    ow22h = tiepts[27]
    fy22v = tiepts[26]
    fy22h = tiepts[29]
    my22v = tiepts[25]
    my22h = tiepts[28]

    cf = ((tb22h - ow22h)*(my22v - ow22v) - (tb22v - ow22v)*(my22h - ow22h))/((fy22h - ow22h)*(my22v - ow22v) - (fy22v - ow22v)*(my22h - ow22h))
    cm = ((tb22h - ow22h) - cf*(fy22h - ow22h))/(my22h - ow22h)

    ct = cf + cm
    return ct

def twochannel37(tb37v, tb37h, tiepts):
    """Simple 2 channel algorithm 37 GHz
    """
     
    ow37v = tiepts[0]
    ow37h = tiepts[3]
    fy37v = tiepts[2]
    fy37h = tiepts[5]
    my37v = tiepts[1]
    my37h = tiepts[4]
    
     
    cf = ((tb37h - ow37h)*(my37v - ow37v) - (tb37v - ow37v)*(my37h - ow37h))/((fy37h - ow37h)*(my37v - ow37v) - (fy37v - ow37v)*(my37h - ow37h))
    cm = ((tb37h - ow37h) - cf*(fy37h - ow37h))/(my37h - ow37h)

    ct = cf + cm
    return ct

def twochannel37_linear(tb37v,tb37h):
    ct=1.21233-0.01879*(tb37v-tb37h)
    return ct
    
def UMass(tb18v,tb37v,tiepts):
#The code is based on C. T. Swift, L. S. Fedor, and R. O. Ramseier, ?An Algorithm 
#to Measure Sea Ice Concentration With Microwave Radiometers,? Journal of Geophysical 
#Research, vol. 90, no. C1, pages 1087 - 1099, 1985.

    
    tw19v = tiepts[6]
    tw37v = tiepts[0]
    tfy19v = tiepts[8]
    tfy37v = tiepts[2]
    tmy19v = tiepts[7]
    tmy37v = tiepts[1]

    """solution of the equations (11)-(12) in Swift et al 1985"""
    """here we use brightness temperatures instead!! (Rasmus 2012)"""
    """e19v=(TB19v-13)./(Ts-12);
       e37v=(TB37v-26)./(Ts-26);"""

    a1 = (tfy19v - tb18v) / (tfy19v - tw19v)
    a2 = (tfy19v - tmy19v) / (tfy19v - tw19v)
    a3 = (tfy37v - tb37v) / (tfy37v - tmy37v)
    a4 = (tfy37v - tw37v) / (tfy37v - tmy37v)
    fw = (a1 - a2 * a3) / (1.0 - a2 * a4)

    Cmy = a3 - fw * a4
    Cfy = 1.0 - fw - Cmy
    CT = Cmy + Cfy
    return CT

# function to read .txt input into a matrix
def readtxt2matrix(filename):
    f = open(filename)
    lines = f.readlines()
    f.close()
    tbmow=np.zeros((13,7))
    for line in lines:
        l = line.strip()
        l2 = l.split(',')    
    k=0
    for i in range(0,13):
        for j in range(0,7):
            tbmow[i,j] = float(l2[k])
            k=k+1
    return tbmow

# function to read .txt input into a 3Dmatrix
def readtxt2matrix3D(f1):
    
    ext='.txt'
    n_atm=12
    LUT19=np.zeros((12,100,100))
    for k1 in range(0,n_atm):
        f2=str(k1)
        filename=os.path.join(f1+f2+ext)
        f = open(filename)
        lines = f.readlines()
        f.close()
        
        for line in lines:
            l2 = line.split(',')
    
        k=0
        for i in range(0,100):
            for j in range(0,100):
                LUT19[k1,i,j] = float(l2[k])
                k=k+1
    return LUT19


# TL (14.01.2013) separate algorithms from main function to allow reuse 
#    of the algos by importing the ic_algs.py file from elsewhere
if __name__ == '__main__':    
    
    
    filename = 'Input/SIC-RRDP-N/DTU-SIC1-AMSR-N-RRDP.text-2008'
    f1 = open(filename)
    
    lines = f1.readlines()
    f1.close()
    
    f2 = open('Output_N/DTU-SIC1-AMSR-N-RRDP.text-2008-algs31012013.txt', 'wt')

    k=0
    for line in lines:
        l = line.strip()
        f2.write(l)
        k=k+1
        l2 = l.split(',')
        try:
            v1 = float(l2[0])
        except:
            print 'not a valid line'
            if k==1:
                f2.write('%s\n' % (',ASI,Bootstrap_f,Bootstrap_p,Bristol,CalVal,NASA_Team,NASA_Team2,Near90GHz,Near90_lin,NORSEX,NRL,One_channel,P90,PR,TUD,Two_channel10,Two_channel18,Two_channel22,Two_channel37,Two_channel37_lin,UMass_AES'))
            else:
                f2.write('\n')
        else:
            print 'Valid line'
            H = filename[15]
            filetype = filename[26:28]
            
            if filetype == 'AM' or filetype == '15': 
                tb6h = float(l2[10])
                tb10v = float(l2[11])
                tb10h = float(l2[12])
                tb18v = float(l2[13])
                tb18h = float(l2[14])
                tb22v = float(l2[15])
                tb22h = float(l2[16])
                tb37v = float(l2[17])
                tb37h = float(l2[18])
                tb85v = float(l2[19])
                tb85h = float(l2[20])
            
            if filetype == 'SS': 
                tb18v = float(l2[9])
                tb18h = float(l2[10])
                tb22v = float(l2[11])
                tb37v = float(l2[12])
                tb37h = float(l2[13])
                tb85v = float(l2[14])
                tb85h = float(l2[15])
            
            if filetype == 'SM': 
                tb6h = float(l2[9])
                tb10v = float(l2[12])
                tb10h = float(l2[11])
                tb18v = float(l2[14])
                tb18h = float(l2[13])
                tb22v = float(l2[16])
                tb22h = float(l2[15])
                tb37v = float(l2[18])
                tb37h = float(l2[17])
            # UB thin ice:
            if filetype == 'MS': 
                tb6h = float(l2[12])
                tb10v = float(l2[13])
                tb10h = float(l2[14])
                tb18v = float(l2[15])
                tb18h = float(l2[16])
                tb22v = float(l2[17])
                tb22h = float(l2[18])
                tb37v = float(l2[19])
                tb37h = float(l2[20])
                tb85v = float(l2[21])
                tb85h = float(l2[22])
            # 85% files:
            if filetype == '85': 
                tb6h = float(l2[14])
                tb10v = float(l2[15])
                tb10h = float(l2[16])
                tb18v = float(l2[17])
                tb18h = float(l2[18])
                tb22v = float(l2[19])
                tb22h = float(l2[20])
                tb37v = float(l2[21])
                tb37h = float(l2[22])
                tb85v = float(l2[23])
                tb85h = float(l2[24])

            """tiepts: w37v    my37v   fy37v    w37h  my37h    fy37h   w18v   my18v    fy18v   w18h    my18h  fy18h    w6v     my6v    fy6v    w6h   my6h    fy6h    w10v    my10v   fy10v  w10h    my10h  fy10h    w22v    my22v  fy22v    w22h"""               
            """my22h   fy22h   my85v    fy85v my85h    fy85h  w85v  w85h"""
            if H == 'N':
                # Northern Hemisphere tie-points:
                tiepts = [209.81, 187.18, 246.29, 145.29, 175.72, 235.15, 183.72, 219.66, 251.56, 108.46, 201.66, 237.16, 161.35, 242.91, 251.59, 82.13, 218.74, 232.14, 167.34, 235.26, 250.89, 88.26, 212.47, 234.16, 196.41, 208.82, 250.18, 128.23, 192.09, 236.42, 178.01, 229.52, 169.52, 220.94, 243.20, 196.94]
            else:
                # Southern Hemisphere tie-points:
                tiepts = [209.81, 187.18, 246.29, 145.29, 175.72, 235.15, 183.72, 219.66, 251.56, 108.46, 201.66, 237.16, 161.35, 242.91, 251.59, 82.13, 218.74, 232.14, 167.34, 235.26, 250.89, 88.26, 212.47, 234.16, 196.41, 208.82, 250.18, 128.23, 192.09, 236.42, 178.01, 229.52, 169.52, 220.94, 243.20, 196.94]

            if filetype == 'SM':
                CT_asi = 12
                CT_nasa2 = 12
                CT_near90 = 12
                CT_near90_linear = 12
                CT_P90 = 12
                CT_tud = 12
            else:
                CT_asi = asi(tb85v, tb85h)*100
                CT_nasa2 = nasa2(tb18v, tb18h, tb37v, tb85v, tb85h)*100
                #CT_nasa2 = 12
	        CT_near90 = near90(tb85v, tb85h, tiepts)*100
                CT_near90_linear = near90_linear(tb85v, tb85h)*100
                CT_P90 = P90(tb85v, tb85h)*100
                CT_tud = tud(tb18v, tb37v, tb85v, tb85h, tiepts)*100

            if filetype == 'SS':
                CT_onechannel = 12
                CT_twochannel10 = 12
                CT_twochannel22 = 12
            else:
                CT_onechannel = onechannel(tb6h, tiepts)*100
                CT_twochannel10 = twochannel10(tb10v, tb10h)*100
                CT_twochannel22 = twochannel22(tb22v, tb22h, tiepts)*100

            CT_bootstrap_f = bootstrap_f(tb18v, tb37v, tiepts)*100
	    CT_bootstrap_p = bootstrap_p(tb37v, tb37h, tiepts)*100
	    CT_bristol = bristol(tb18v, tb37v, tb37h, tiepts)*100
	    CT_calval = calval(tb37v,tb18v,tiepts)*100
	    CT_nasa = nasa(tb18v, tb18h, tb37v, tiepts)*100
            CT_norsex = norsex(tb18v,tb37v,tiepts)*100
	    CT_nrl = nrl(tb37v, tb37h)*100
	    CT_pr = pr(tb18v, tb18h, tb37v, tb37h, tiepts)*100
	    CT_twochannel18 = twochannel18(tb18v, tb18h, tiepts)*100
	    CT_twochannel37 = twochannel37(tb37v, tb37h, tiepts)*100
            CT_twochannel37_linear = twochannel37_linear(tb37v,tb37h)*100
	    CT_UMass = UMass(tb18v,tb37v,tiepts)*100
            

            f2.write(' %1.6f %1.6f %1.6f %1.6f %1.6f %1.6f %1.6f %1.6f %1.6f %1.6f %1.6f %1.6f %1.6f %1.6f %1.6f %1.6f %1.6f %1.6f %1.6f %1.6f %1.6f\n' % (CT_asi, CT_bootstrap_f, CT_bootstrap_p, CT_bristol, CT_calval, CT_nasa, CT_nasa2, CT_near90, CT_near90_linear, CT_norsex, CT_nrl, CT_onechannel, CT_P90, CT_pr, CT_tud, CT_twochannel10, CT_twochannel18, CT_twochannel22, CT_twochannel37, CT_twochannel37_linear, CT_UMass))

    f2.close()
    
