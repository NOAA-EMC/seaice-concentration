PROGRAM amsr2lr
      IMPLICIT none
!From Vera Gerald program of 18 Dec 2013
!F90 and other modifications R. Grumbine
      
      INTEGER iunt
      PARAMETER ( iunt = 12 )

      CHARACTER(8) SUBSET
      CHARACTER(80)  HS1, HS2, HS3, Htmp

      CHARACTER*(*)   FLT
      PARAMETER     ( FLT = 'bufr.tab' )

!      REAL HDR(9), XLOC(2,1), PRDLR(5,12), PRDHR(5,2)
      REAL*8 HDR(9), XLOC(2,1), PRDLR(5,12), PRDHR(5,2)
      DATA HS1, HS2, HS3, Htmp &
    / 'SAID SIID YEAR MNTH DAYS HOUR MINU ORBN SLNM               ', &
      'CLATH CLONH                                                ', &
      'SCCF ALFR VIIRSQ ANPO TMBR                                 ', &
      'MTYP                                                       '/
!
      INTEGER icount, iu, ntank, is
      REAL sccf, alfr, virsq, anpo, btmp
      DOUBLE PRECISION slon, slat
  
      INTEGER itk, lubfi, idate, iret, isad, issid, myr, mon, mda, mhr
      INTEGER minu, iorbn, iscanl, idatc
      INTEGER nchan, err

      INTEGER openout, outunit, creturns
      LOGICAL ok, hfok, lfok, weatherok
!.................................

       OPEN  ( UNIT = iunt, FILE = FLT )

       CALL W3TAGB('DCODAMSR2',0095,0333,0077,'NP11   ')
      
      outunit = 52
      OPEN (outunit, FORM="UNFORMATTED", STATUS="NEW")
      creturns = openout(outunit)

      icount = 0
      iu = 11
      ntank = 1

      DO ITK = 1, NTANK
!-----------------------------------------------------------------------
!                       DECODE BUFR FILE
!-----------------------------------------------------------------------
!  OPEN THE BUFR INPUT FILE
!  ------------------------
      LUBFI=IU

      CALL OPENBF(LUBFI,'IN',iunt)
      CALL READMG(LUBFI,SUBSET,IDATE,iret)
      IF(iret.NE.0) THEN
        CALL   closbf(lubfi)
        IF (icount.lt.1) then
          WRITE(6, *) 'READMG ERROR W/ BUFR MSG:iret=', iret
          err = 23
          STOP
!          CALL exit(23)
          go to 99
        ENDIF
      ENDIF
!
      PRINT*, 'READING DATA FOR ', IDATE
!
!  READ A SUBSET - READ A MESSAGE WHEN NO SUBSETS - END WHEN NO MESSAGES
!  ---------------------------------------------------------------------
  150 CONTINUE
      CALL READSB(LUBFI, iret)
      IF(iret.NE.0) THEN
         CALL READMG(LUBFI, SUBSET, IDATE, iret)
         IF(iret.NE.0) GOTO 100
         GOTO 150
      ENDIF
!
      if(icount .LE. 5e8) go to 250
      PRINT *, ' ***** NUMBER OF OBS EXCEEDED 5e8 *****'
      STOP 

  250 CONTINUE
!  -----------------------------------------------------------------
!  CALL UFBINT, UFBREP TO GET THE DATA
!
!   SAID SIID YEAR MNTH DAYS HOUR MINU ORBN SLNM 
!   CLATH CLONH                                                ',
!   SCCF ALFR VIIRSQ ANPO TMBR  
!  -----------------------------------------------
      CALL UFBINT(LUBFI,HDR, 9,  1,iret, HS1)
      CALL UFBrep(LUBFI,XLOC,2,  1,iret, HS2)
      CALL UFBrep(LUBFI,PRDLR,5,12,iret, HS3)
!      PRINT *,'iret hs3 = ',iret
      nchan = iret
!
      isad   = NINT(hdr(1))
      issid  = NINT(hdr(2))
      myr    = NINT(hdr(3))
      mon    = NINT(hdr(4))
      mda    = NINT(hdr(5))
      mhr    = NINT(hdr(6))
      minu   = NINT(hdr(7))
      iorbn  = NINT(hdr(8))
      iscanl = NINT(hdr(9))

!
        SLAT = XLOC(1,1)
        SLON = XLOC(2,1)

        if(slon.lt.-180.0.or.slon.gt.180.0) go to 151

          icount = icount + 1
! Add some checking for quality and being in latitudes suitable for sea
! ice.  31 July 2017 Robert Grumbine
          ok = .TRUE.
          IF (nchan .EQ. 2) THEN
            ok = hfok(PRDLR)
          ELSE
            ok = lfok(PRDLR)
          ENDIF 
          ok = ok .AND. (slat > 25.0 .OR. slat < -40.0) !
    
          IF (ok) THEN
            CALL headerout(isad,myr,mon,mda,mhr,minu,slat,slon, nchan)
            DO is = 1,nchan
               sccf  = PRDLR(1,is)
               alfr  = PRDLR(2,is)
               virsq = PRDLR(3,is)
               anpo  = PRDLR(4,is) 
               btmp  = PRDLR(5,is) 
               CALL spotout(sccf, alfr, virsq, anpo, btmp)
            ENDDO
          ENDIF

         IF (MOD(icount,INT(1e5)) .EQ. 0) THEN
           WRITE(51,*)isad,myr,mon,mda,mhr,minu,slat,slon, nchan
         ENDIF
  50     FORMAT(1x,i3,1x,i4,3i2,i3,1x,2f11.5,I3)
!         WRITE(52+icount/5000000) sccf/1.e9,alfr,virsq,anpo,btmp
!         IF (MOD(icount,INT(1e5)) .EQ. 0) THEN
!           WRITE(51,54) sccf/1.e9,alfr,virsq,anpo,btmp
!         ENDIF
   54 FORMAT(1x,F6.2,1x,4F13.4)

  151             continue

!       get next report
      GO TO 150

  100 CONTINUE
!
         CALL   closbf(lubfi)
         IF (icount.lt.1) then
           WRITE(6,*) 'READMG ERROR W/ BUFR MSG:iret=',iret
           err = 23
!           CALL exit(23)
           STOP
           go to 99
         endif
!
      WRITE(6,*) 'AFTER WOOLENS INTERFACE:  icount= ',icount
!
      ENDDO
!
!.................
      WRITE(6,692) icount
  692 FORMAT(1x,'END OF JOB','count= ',i8)

      CALL W3TAGE('DCODNET3')

   99 CONTINUE
END
!---------------------------------------------------------------
LOGICAL FUNCTION hfok(obs)
      IMPLICIT none
      REAL*8 obs(5,2) ! i = parameters, 5 = tmbr; j = channel
      LOGICAL tmp
      tmp = .TRUE.
      IF (obs(5,1) > 285 .OR. obs(5,2) > 285. ) tmp = .FALSE.
      IF (obs(5,1) > obs(5,2) ) tmp = .FALSE.
      hfok = tmp
      RETURN 
      END
      LOGICAL FUNCTION lfok(obs)
      IMPLICIT none
      REAL*8 obs(5,12) ! i = parameters, 5 = tmbr; j = channel
      LOGICAL tmp
      INTEGER i
      tmp = .TRUE.

      DO i = 1, 6
        IF (obs(5,2*i-1) > 285 .OR. obs(5,2*i) > 285 ) tmp = .FALSE.
        IF (obs(5,2*i-1) > obs(5,2*i) ) tmp = .FALSE.
      ENDDO

      lfok = tmp
      RETURN 
      END
      LOGICAL FUNCTION weatherok(obs, lat) 
! Weather filter.  Should only apply after regressions to AMSRE have
! been performed.
      IMPLICIT none
      REAL AMSR_GR37LIM, AMSR_GR24LIM
      PARAMETER (AMSR_GR37LIM = 0.046)
      PARAMETER (AMSR_GR24LIM = 0.045)
      REAL *8 obs(5,12), amsre(5,12)
      REAL *8 lat
      LOGICAL tmp
      REAL gr3719, gr2419

      amsre = obs
      CALL regress(amsre, lat)
      tmp = .FALSE.
      gr3719 = (amsre(5,12) - amsre(5,8) ) / (amsre(5,12)+amsre(5,8))
      gr2419 = (amsre(5,10) - amsre(5,8) ) / (amsre(5,10)+amsre(5,8))
    
      IF ((gr3719 < AMSR_GR37LIM) .AND. (gr2419 < AMSR_GR24LIM) ) THEN
        tmp = .TRUE.
      ENDIF
      weatherok = tmp
      RETURN
      END
      SUBROUTINE regress(amsre, lat) 
      IMPLICIT none
      REAL *8 amsre(5,12)
      REAL *8 lat
!Perform limited regression -- just those channels that are used in
!weather filter
      IF (lat > 0) THEN
        amsre(5, 8) = amsre(5, 8)*1.031 - 9.710
        amsre(5,10) = amsre(5,10)*0.999 - 1.706
        amsre(5,12) = amsre(5,12)*0.997 - 2.610
      ELSE
        amsre(5, 8) = amsre(5, 8)*1.032 -10.013
        amsre(5,10) = amsre(5,10)*0.993 - 0.987
        amsre(5,12) = amsre(5,12)*0.995 - 2.400
      ENDIF

      RETURN
END
