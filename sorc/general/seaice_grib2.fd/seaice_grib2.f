      program seaice_grib2
C General program to make grib output of sea ice concentration grids
C Original by Vera Gerald 11 March 2012
C Modifications to operational sea ice implementation by Robert Grumbine 28 March 2012

      IMPLICIT none

      INTEGER maxpts, lcgrib
      PARAMETER (MAXPTS=10*1000*1000,lcgrib=4*MAXPTS)
      CHARACTER(len=1) :: cgrib(lcgrib),cin(lcgrib)
      real FLD(maxpts)
      real :: coordlist(1)
      integer pds(25),gds(22),parm,disp,parmcat
      integer :: listsec0(3)
      integer :: listsec1(13)
      integer :: igds(5),igdstmpl(200),ipdstmpl(200)
      integer :: ideflist,idefnum
      integer :: idrstmpl(200)
      Logical(KIND=1) bmp(maxpts)
      character(11) envvar
      character(len=8) :: g2file, gifile
c
      INTEGER ierr, ipdsnum, ifhr, ndpts, idrsnum
      INTEGER lengrib, numcoord, ibmap, iyr, imo
      INTEGER ifl1, IFL2, IOS, icnd, iday, icycle
      INTEGER i

C For bacio
      INTEGER newpos, start
      INTEGER MOVA2I

c - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
!  This code reads input that allows the user to encode a field into grib1
!  or grib2. " This code was tested on global multi grid
C - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
C  
      gds = 00
      pds = 00
c
      IFL1=10   ! input data file(unformatted)
      IFL2=20   ! output grib2 gridded file

!      envvar='XLFUNIT_   '
!      write(envvar(9:10),fmt='(I2)') IFL2
!      call getenv(envvar,g2file)
      g2file='fort.20'
      CALL BAOPENW(IFL2,g2file,IOS)
      PRINT *,'opened output, ios, g2file = ',g2file,IOS,g2file
      IF (IOS .NE. 0) THEN
        STOP
      END IF

!      envvar='XLFUNIT_   '
!      write(envvar(9:10),fmt='(I2)') IFL1
!      call getenv(envvar,gifile)
      gifile='fort.10'
      CALL BAOPENR(IFL1,gifile,IOS)
      PRINT *,'opened the input file, IOS = ',gifile,IOS
      IF (IOS .NE. 0) THEN
        STOP
      END IF

c

      OPEN(11, FILE="seaice_pds", FORM="FORMATTED", STATUS="OLD")
c
!  Get runtime and cycle
C
        read(4,444)iyr,imo,iday,icycle
  444   format(6x,i4,3i2)
          rewind 4
c
ccccccccccccccccccccccccccccccccccccccccccc
c
            icnd = 0

c
        !CD PRINT *,'pds1 = ',pds
        READ(11,*) disp
        READ(11,*) parmcat
        READ(11,*) parm
        READ(11,*) ifhr
        READ(11,*) pds(2)
        READ(11,*) pds(4)
        READ(11,*) pds(6)
        READ(11,*) pds(7)
        READ(11,*) pds(13)
        READ(11,*) pds(14)
        READ(11,*) pds(22)
        !CDPRINT *,'pds2 = ',pds

        !CD PRINT *,'gds1 = ',gds
        gds(1:11) = -1
        !CD PRINT *,'gds2a = ',gds 
        READ(12,*) gds(1:11)
        !CD PRINT *,'gds2b = ',gds 
        ndpts = gds(2) * gds(3)   ! NI # rows(lat) & NJ # rows long
        !CD PRINT *,'gds = ',gds
c

CO        read(10,err=998,end=999)disp,parmcat,parm,ifhr,pds,gds,
CO     *  ndpts,fld(1:ndpts),bmp(1:ndpts)
CO        CLOSE(10)

CD        PRINT *,bmp
      
      start = 0
      CALL BAREAD(IFL1, start, ndpts, newpos, cin)
      PRINT *,'have read the file'


      DO i = 1, ndpts
        fld(i) = FLOAT(MOVA2I(cin(i))) / 100.  ! Convert to float, and scale to percent
!CD        PRINT *,'in grib2 fld, mova2i, cin ',i,fld(i), 
!CD     1                           MOVA2I(cin(i)), cin(i)
      ENDDO
      PRINT *,'field max = ',MAXVAL(fld)
c
c
!  create grib message
c
!==    Initialize new GRIB2 message and pack
! GRIB2 sections 0 (Indicator Section) and 1 (Identification  Section)
!== 
c
       listsec0(1)=disp ! Discipline-GRIB Master Table Number (see Code Table 0.0)
c                       ! Met = 0 & Ocean = 10
       listsec0(2)=2 ! GRIB Edition Number (currently 2)
       listsec0(3)=0
c
       listsec1(1)=7       ! 7  Id of orginating centre (Common Code Table C-1)
       listsec1(2)=0 !"EMC"! Id of orginating sub-centre (local table)/Table C/on388
       listsec1(3)=2       ! GRIB Master Tables Version Number (Code Table 1.0)
       listsec1(4)=1       !per Brent! GRIB Local Tables Version Number (Code Table 1.
       listsec1(5)=1       ! Significance of Reference Time (Code Table 1.2)
       listsec1(6)= iyr     ! Reference Time - Year (4 digits)
       listsec1(7)= imo     ! Reference Time - Month
       listsec1(8)= iday    ! Reference Time - Day
       listsec1(9)= icycle  ! Reference Time - Hour(cycle:0,6,12,18)
       listsec1(10)= 0      ! Reference Time - Minute
       listsec1(11)= 0      ! Reference Time - Second
       listsec1(12)=0      ! Production status of data (Code Table 1.3)
       listsec1(13)=1      ! Type of processed data (Code Table 1.4)
c
      call gribcreate(cgrib,lcgrib,listsec0,listsec1,ierr)
      if (ierr.ne.0) then
        write(6,*) ' ERROR creating new GRIB2 field = ',ierr
        stop 2
      endif
c
! assume predefined grid for simplicity
!
!==> Pack up Grid Definition Section (Section 3) add to GRIB2 message.
c
c    Lat/long grid
        if(gds(1).eq.0)then
c
           igds(1)=0                  !Source of grid definition (see Code Table 3.0)
           igds(2)=gds(2)*gds(3)      ! num of grid points
           igds(3)=0                  !Number of octets needed for each additional grid points dfn
           igds(4)=00                !Interpretation of list for optional points definition (Code Table 3.11)
           igds(5)=0                  !Grid Definition Template Number (Code Table 3.1)
c
           idefnum = 0
           ideflist=0       !Used if igds(3) .ne. 0. Dummy array otherwise
c
           igdstmpl(1)=6
           igdstmpl(2)=0    
           igdstmpl(3)=0
           igdstmpl(4)=0
           igdstmpl(5)=0
           igdstmpl(6)=0
           igdstmpl(7)=0
           igdstmpl(8)=gds(2)         ! num points along parallel
           igdstmpl(9)=gds(3)         ! num points along meridian
           igdstmpl(10)=0
           igdstmpl(11)=0
           igdstmpl(12)=gds(4)*1000   ! lat of first grid point
cccc
           if ( gds(5).lt.0 ) then       ! Lon of 1st grid point
              igdstmpl(13)=(360000+gds(5))*1000    ! convert W to E
           else
              igdstmpl(13)=gds(5)*1000
           endif
cccc
           igdstmpl(14)=0                 ! Resolution and Component flags
           if ( btest(gds(6),7) ) igdstmpl(14)=48
           if ( btest(gds(6),3) ) igdstmpl(14)=igdstmpl(14)+8
ccccc
           igdstmpl(15)=gds(7)*1000   ! lat of last grid point
ccccc
           if ( gds(8).lt.0 ) then       ! Lon of last grid point
              igdstmpl(16)=(360000+gds(8))*1000    ! convert W to E
           else
              igdstmpl(16)=gds(8)*1000
           endif
cc
           igdstmpl(17)=gds(9)*1000   ! Increment of lat
           igdstmpl(18)=gds(10)*1000  ! Increment of long
           igdstmpl(19)=gds(11)       ! scanning mode
cccc
      elseif(gds(1).eq.5) then
c
c   Polar sterographic grid
c
           idefnum=0
           igds(1)=0                 ! grid def specfied in template
           igds(2)=gds(2)*gds(3)   ! num of grid points
           igds(3)=0                 ! octets for further grid definition
           igds(4)=0                 ! interpretation of optional list
           igds(5)=20                 ! Grid Definition Template number
c
c.. radius of earth
c
             igdstmpl(1)=6
           igdstmpl(2)=0
           igdstmpl(3)=0
           igdstmpl(4)=0
           igdstmpl(5)=0
           igdstmpl(6)=0
           igdstmpl(7)=0
           igdstmpl(8)=gds(2)              ! Nx
           igdstmpl(9)=gds(3)              ! Ny
           igdstmpl(10)=gds(4)*1000        ! Lat of 1st grid point
           if ( gds(5).lt.0 ) then         ! Lon of 1st grid point
              igdstmpl(11)=(360000+gds(5))*1000    ! convert W to E
           else
              igdstmpl(11)=gds(5)*1000
           endif
           igdstmpl(12)=0                   ! Resolution and Component flags
           if ( btest(gds(6),7) ) igdstmpl(12)=48
           if ( btest(gds(6),3) ) igdstmpl(12)=igdstmpl(12)+8
           igdstmpl(13)=60000000            ! Lat where Dx and Dy specified
           if ( btest(gds(10),7) ) igdstmpl(13)=-60000000
           if ( gds(7).lt.0 ) then         ! Lon of orientation
              igdstmpl(14)=(360000+gds(7))*1000    ! convert W to E
           else
              igdstmpl(14)=gds(7)*1000
           endif
           igdstmpl(15)=gds(8)*1000        ! Dx
           igdstmpl(16)=gds(9)*1000        ! Dy
           igdstmpl(17)=gds(10)            ! Projection Center Flag
           igdstmpl(18)=gds(11)            ! Scanning mode
c
        else
           ierr=3
        endif
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
      call addgrid(cgrib,lcgrib,igds,igdstmpl,200,ideflist,
     &             idefnum,ierr)
      if (ierr.ne.0) then
        write(6,*) ' ERROR adding GRIB2 grid = ',ierr
        stop 3
      endif
c
! assume PDT 4.0 for simplicity
c
       ipdsnum=0    !  anly or fcst at a hozontal level or level
c
! Parameter category (see Code Table 4.1)
c
        ipdstmpl(1) = parmcat   ! 0 = ocean / 2 = meteo 
c
c    Get parameter number (see Code Table 4.2)
c
        ipdstmpl(2) = parm  
c
c  Type of generating process: analysis or forecast(see code Table 4.3)
c
c anal(0), init(1), fcst(2), bias corr fcst(3), ensemb fcst(4), etc

c  For  Model/Forecast fields

        ipdstmpl(3)=2     ! forecast
        ipdstmpl(4)=0     !background generating process identifier
!                         (defined by originating Center)

        ipdstmpl(5)=pds(2) ! Generating process or Model Number
!                            (defined by originating Center)

        ipdstmpl(6)=0     ! hours of observational data cutoff after reference time
        ipdstmpl(7)=0     ! minutes of observational data cutoff after reference time

        ipdstmpl(8)=pds(13)   !indicator of unit of time range (see Code Table 4.4) 
        ipdstmpl(9)= pds(14)  !forecast time in units defined by pdstmpl(8)

        print * ,' level= ',pds(6),' pds(7)= ',pds(7)

        ipdstmpl(10)=101      ! type of level (see Code Table 4.5) 1st level
        ipdstmpl(11)=0        ! scale factor of pdstmpl(10) 
        ipdstmpl(12)=0        ! scaled value of pdstmpl(10)

        ipdstmpl(13)=255   ! type of level (See Code Table 4.5) 2nd level
        ipdstmpl(14)=0     ! scale factor of ipdstmpl(13)
        ipdstmpl(15)=0     ! scaled value of ipdstmpl(13)

ccccccccccccccccccccc
      numcoord=0
        coordlist= 0.0     !needed for hybrid vertical coordinate
! set bitmap flag
        if (btest(pds(4),6)) then
          ibmap=0
        else
          ibmap=255    ! Bitmap indicator ( see Code Table 6.0 )
          bmp=.true.
        endif
      idrsnum=40    ! Data Rep Template Number ( see Code Table 5.40 ; grd pnt data)
c
      idrstmpl=0
c
!********************************************************************************
! idrstmpl(1): reference value(R) (IEEE 32-bit floating-point value)             *
! idrstmpl(2): binary scale factor (E)                                           *
! idrstmpl(3): decimal scale factor (D)                                          *
! idrstmpl(4): number of bits used for each packed value for simple packing      *
!              or for each group reference value for complex packing or          *
!              spatial differencing                                              *
! idrstmpl(5): type of original field values (See Code Table 5.1)                *
!********************************************************************************
c
      idrstmpl(3)=pds(22) ! binary scale factor (E)
c
      call addfield(cgrib,lcgrib,ipdsnum,ipdstmpl,200,
     &              coordlist,numcoord,idrsnum,idrstmpl,200,fld,
     &              ndpts,ibmap,bmp,ierr)
      if (ierr.ne.0) then
          write(6,*) ' ERROR adding GRIB2 field = ',ierr
          stop 4
      endif


       print*,'IPDSTMPL ',ipdsnum,idrsnum,ipdstmpl(1:25)

       print*,'IGDSTMPL ',igds(1:5),igdstmpl(1:22)

!
! End GRIB2 field
!
      call gribend(cgrib,lcgrib,lengrib,ierr)
      if (ierr.ne.0) then
        write(6,*) ' ERROR ending new GRIB2 message = ',ierr
        stop 5
      endif
      print *,' writing ',lengrib,' bytes...'
      call wryte(IFL2,lengrib,cgrib)
      PRINT *,'back from wryte'

      icnd = 0    ! no read error 
  
      if (icnd.ne.0) then
        write(6,*) ' ERROR READING binary file = ',icnd
      endif

      call baclose(IFL2,ierr)

      stop
      end
