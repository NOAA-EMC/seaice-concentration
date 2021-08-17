C 30 March 1995
       PROGRAM ICECON
C
C GODDARD SPACE FLIGHT CENTER (GSFC) TEAM SEA ICE ALGORITHM PROGRAM 
C
       REAL*4 V37(385,465),V19(385,465),V22(385,465)
       REAL*4 H19(385,465),TOTCON(385,465),MULCON(385,465)
       INTEGER*1 MASK(385,465), OUTPUT(385, 465)
       CHARACTER*80 TITLE,V19FIL,V37FIL,H19FIL,V22FIL,TOTFIL,MYIFIL
       CHARACTER*80 MASKFIL
       CHARACTER*80 MULTITLE,TOTTITLE
       CHARACTER*8 ROOT
       CHARACTER*1 POLE
       INTEGER*2 BEGROW,ENDROW,BEGCOL,ENDCOL,SCALE,COLS,ROWS
       REAL*8 DW(2),DF(2),DM(2),SF(2),T0(7),TF(7),TM(7),SW(2),SM(2),
     *  A1,POLAR,GRAD,B1,C1,D1,I1,J1,K1,L1,E1,F1,G1,H1,
     *  NF19,NM19,DD19,FIRSTYR,MULTIYR,TOTAL,T0N(7),TFN(7),TMN(7),
     *  T0S(7),TFS(7),TMS(7),VERT19,VERT37,HORZ19,VERT22,GRAD22
       INTEGER*1 T1(2)
       INTEGER*2 T2
       EQUIVALENCE (T1(1), T2)
C                                                                
C   THIS PROGRAM USES THE TEAM ALGORITHM TO CALCULATE THE TOTAL ICE
C   CONCENTRATION AND THE MULTIYEAR ICE CONCENTRATION.  INPUT ARE 
C   19 VERT, 19 HORZ AND 37 VERT, 22 VERT BRIGHTNESS TEMPERATURES.
C   WRITTEN FOR 385 X 465 PS GRID ON MARCH 30, 1995 M.G. MARTINO, HSTX
C                                                                 
C     GR FILTER IS .05                        
C    SSMI TIE POINTS  (1=19H, 4=37V)
C    
C
C THE FOLLOWING TIE POINTS ARE VALID FOR THE DMSP-F8 SSM/I INSTRUMENT
C T0 IS TIE POINT FOR OPEN WATER
C TF IS TIE POINT FOR FIRST YEAR ICE
C TM IS TIE POINT FOR MULTI-YEAR ICE
C N INDICATES NORTHERN HEMISPHERE
C S INDICATES SOUTHERN HEMISPHERE
C TIE POINTS ARE FOR (IN ORDER) 19H, 19V, AND 37V. THE ZEROS ARE 
C PLACEHOLDERS FOR UN-USED CHANNELS
C
C NORTH POLE TIE POINTS 08MAR91                           
       DATA T0N/100.8,177.1,00.0,201.7,0.,0.,0./                   
       DATA TFN/242.8,258.2,00.0,252.8,0.,0.,0./                  
       DATA TMN/203.9,223.2,00.0,186.3,0.,0.,0./                  
C SOUTH POLE TIE POINTS 12 FEB 91                          
       DATA T0S/100.3,176.6,00.0,200.5,0.,0.,0./                    
       DATA TFS/237.8,249.8,00.0,243.3,0.,0.,0./                   
       DATA TMS/193.7,221.6,00.0,190.3,0.,0.,0./ 
                         
       T1(1) = 0
       T1(2) = 0
       ROWS = 465
       COLS = 385
       DO 120 I = 1,7
         T0(I) = T0N(I)
         TF(I) = TFN(I)
         TM(I) = TMN(I)
120    CONTINUE

C                                                                 
C   CALCULATE PARAMETERS FOR ICE CONCENTRATION ALGORITHM          
C                                                                 
       WRITE(6,17)T0,TF,TM                                        
   17  FORMAT(' TIE POINTS FOR OPEN WATER : '/,' ',7F7.2,/        
     *        '                FY ICE :'/,' ',7F7.2,/             
     *        '                MY ICE :'/,' ',7F7.2)
C
C THIS SECTION COMPUTES COEFFICIENTS FOR USE IN THE ICE ALGORITHM. THE D*
C QUANTITIES ARE ALL DIFFERENCES OF TIE POINTS. THE S* QUANTITIES ARE SUMS
C OF TIE POINTS. A1 THROUGH H1 ARE COEFFICIENTS FOR USE IN THE ALGORITHM
C EQUATIONS
C                           
       DW(1)=T0(2)-T0(1)                                          
       DF(1)=TF(2)-TF(1)                                          
       DM(1)=TM(2)-TM(1)                                          
       SW(1)=T0(2)+T0(1)                                          
       SF(1)=TF(2)+TF(1)                                          
       SM(1)=TM(2)+TM(1)                                          
       DW(2)=T0(4)-T0(2)                                          
       DF(2)=TF(4)-TF(2)                                          
       DM(2)=TM(4)-TM(2)                                          
       SW(2)=T0(4)+T0(2)                                          
       SF(2)=TF(4)+TF(2)                                          
       SM(2)=TM(4)+TM(2)                                          
       A1=DM(1)*DW(2)-DM(2)*DW(1)                                 
       B1=DM(2)*SW(1)-DW(2)*SM(1)                                 
       C1=DW(1)*SM(2)-DM(1)*SW(2)                                 
       D1=SM(1)*SW(2)-SM(2)*SW(1)                                 
       I1=DF(2)*DW(1)-DF(1)*DW(2)                                 
       J1=DW(2)*SF(1)-DF(2)*SW(1)                                 
       K1=SW(2)*DF(1)-DW(1)*SF(2)                                 
       L1=SF(2)*SW(1)-SF(1)*SW(2)                                 
       E1=DF(1)*(DM(2)-DW(2))+DW(1)*(DF(2)-DM(2))+DM(1)*(DW(2)-DF(2)) 
       F1=DF(2)*(SM(1)-SW(1))+DW(2)*(SF(1)-SM(1))+DM(2)*(SW(1)-SF(1)) 
       G1=DF(1)*(SW(2)-SM(2))+DW(1)*(SM(2)-SF(2))+DM(1)*(SF(2)-SW(2)) 
       H1=SF(2)*(SW(1)-SM(1))+SW(2)*(SM(1)-SF(1))+SM(2)*(SF(1)-SW(1))

C      
C READ FILENAMES FROM UNIT 5
C OPEN THEM AND READ THEIR CONTENTS
C
       READ(5,*) MYIFIL
       
       OPEN(UNIT=10,STATUS='UNKNOWN',FORM='SYSTEM',FILE='n19v')
       READ(10) V19
       CLOSE(10) 
             
       OPEN(UNIT=10,STATUS='UNKNOWN',FORM='SYSTEM',FILE='n19h')
       READ(10) H19
       CLOSE(10) 
             
       OPEN(UNIT=10,STATUS='UNKNOWN',FORM='SYSTEM',FILE='n37v')
       READ(10) V37
       CLOSE(10) 
             
       OPEN(UNIT=10,STATUS='UNKNOWN',FORM='SYSTEM',FILE='n22v')
       READ(10) V22
       CLOSE(10) 
             
       OPEN(UNIT=10,STATUS='UNKNOWN',FORM='SYSTEM',FILE='mask')
       READ(10) MASK
       CLOSE(10) 
             
C
C PRODUCE AND WRITE TITLES FOR CONCENTRATION FILES
C
      WRITE(MULTITLE,200) ROOT
      WRITE(TOTTITLE,205) ROOT
200   FORMAT('TCD SSM/I DAY ',A8,' MULTIYR ICE CON')
205   FORMAT('TCD SSM/I DAY ',A8,' TOTAL   ICE CON')

C
      WRITE(6,*) 'GENERATING ICE CONCENTRATIONS'
C 
C DOUBLE LOOP TO PROCESS ENTIRE IMAGE
C
      DO 100 J = 1,ROWS
        DO 110 I = 1,COLS
C
C CONVERT ALL BRIGHTNESS TEMPS TO REAL NUMBERS claibrate F-11 to F-8
C
          VERT19 = REAL(V19(I,J))*1.013 + (-2.51)
          HORZ19 = REAL(H19(I,J))*1.013 + (-1.89)
          VERT22 = REAL(V22(I,J))*1.014 + (-2.73)
          VERT37 = REAL(V37(I,J))*1.000 + (0.052)
C
C COMPUTE POLARIZATION RATIO FOR 19GHZ, AND 37-19GHZ GRADIENT RATIO
C COMPUTE 22-19GHZ GRADIENT RATIO FOR WEATHER FILTER
C
          GRAD22 = ((VERT22-VERT19)/(VERT22+VERT19))
          POLAR =  ((VERT19-HORZ19)/(VERT19+HORZ19))
          GRAD  =  ((VERT37-VERT19)/(VERT37+VERT19))
C
C CHECK FOR MISSING DATA AND LANDMASK
C
          T1(2) = MASK(I,J)
          t1(1) = 0
          IF (T2 .LE. 120) THEN
C
C GR < 0.05  OR GR22-19 < 0.045 WEATHER EFFECTS FILTERS
C IF BOTH GR'S ARE NOT ABOVE CUTOFF VALUE, PERFORM ICE CON EQUATIONS
C OTHERWISE, SET TO ZERO CONCENTRATION
C
           IF ((GRAD .LE. 0.05) .AND. (GRAD22 .LE. 0.045)) THEN
               NF19=A1+B1*POLAR+C1*GRAD+D1*POLAR*GRAD            
               NM19=I1+J1*POLAR+K1*GRAD+L1*POLAR*GRAD            
               DD19=E1+F1*POLAR+G1*GRAD+H1*POLAR*GRAD            
               FIRSTYR=NF19/DD19                             
               MULTIYR=NM19/DD19
               TOTAL=MULTIYR+FIRSTYR 
C
C IF CONCENTRATION GREATER THAN 100% OR LESS THAN 0%                  
C CHANGE TO 100 OR 0, RESPECTIVELY
C                                             
               IF(MULTIYR.LE.0) MULTIYR=0                                 
               IF(MULTIYR.GT.1) MULTIYR=1                                 
               IF(TOTAL.LE.0) TOTAL=0                                     
               IF(TOTAL.GT.1) TOTAL=1                                     
               IF(MULTIYR.GT.TOTAL) MULTIYR=TOTAL                         
C
               TOTCON(I,J) = TOTAL * 1000.
               MULCON(I,J) = MULTIYR * 1000.
           ELSE
               MULCON(I,J) = 0
               TOTCON(I,J) = 0
           END IF                             
          ELSE
C
C APPLY LAND MASK, AND RECORD MISSING DATA IN ICE CON GRID
C
           MULCON(I,J) = T2*10
           TOTCON(I,J) = T2*10
          END IF                  
          OUTPUT(I,J) = (MULCON(I,J)+0.5) / 10    
110     CONTINUE
100   CONTINUE
C
C WRITE MULTIYEAR ICE TO FILE
C
      WRITE(6,*) 'WRITING TO FILE', MYIFIL
      OPEN(UNIT=12,STATUS='UNKNOWN',FORM='SYSTEM',FILE=MYIFIL)
      WRITE(12) OUTPUT 
      CLOSE(12)
      
      WRITE(6,*) 'NORMAL END OF PROGRAM. GOOD DAY!!' 
      STOP
      END
