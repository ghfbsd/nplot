c	Program to read NBS format fonts and plot them.
	parameter (maxchr=2000)
	parameter (maxstk=150,maxcrd=maxchr*2*maxstk,iend=-64)
	parameter (xw=8.0,yw=6.0,ippb=110,ibpl=10,ibph=10)
	parameter (bxmx=49,bymx=49,bybot=10)
	logical debug
	parameter (debug=.false.)
c	xw = x plot width
c	yw = y plot width
c	ippb = points per box
c	ibpl = boxes per length
c	ibph = boxes per height
	dimension para(22)
	integer code(maxchr),ibeg(maxchr),text(maxcrd),rdch
	character lbl*20
c	Functions to make offsetting of data easier.  Y-values seem to be
c       based on the top of the character down; convert to bottom up.
	xp(ipar) = xorg + ipar
	yp(ipar) = yorg - ipar
	do 1 i=1,22
	   para(i) = 0.0
1	continue
	para(1) = xw
	para(2) = yw
	para(3) = 0.0
	para(4) = 0.0
	para(5) = ippb*ibpl
	para(6) = ippb*ibph
c	No grid or box, please
	para(16) = -1.0
	para(17) = 1.0
	para(18) = 1.0
c
c	Having initialized the plot, read a bunch of characters.  Stop at
c	end-of-file.
	ich = 1
	inx = 1
10	continue
	   kode = rdch(5,text(inx),maxcrd-inx+1,len)
	   if (kode .eq. 0) goto 100
	   if (kode .lt. 0) then
	      write(6,*) ' ** Error: Return code ',kode,' from rdch'
	      stop
	   endif
	   code(ich) = kode
           ibeg(ich) = inx
	   ich = ich + 1
	   if (ich .gt. maxchr) then
	      write(6,*) ' ** Too many characters, stopping at ',maxchr
	      go to 100
	   endif
	   inx = inx + len
	   if (inx .gt. maxcrd) then
	      write(6,*) ' ** Too many strokes total, truncating to ',
     +                   maxchr
	      go to 100
	   endif
	go to 10
c
100	continue
	ich = ich - 1
	write(6,101) ich
101	format(1X,I4,' characters read.')
	if (debug) then
	   do 400 i=1,ich
	      write(6,*) code(i),ibeg(i),(text(j),j=ibeg(i),ibeg(i)+10)
400	   continue
	endif
	do 199 i=1,ich
	   im1 = i - 1
	   if (mod(im1,ibph*ibpl) .eq. 0) then
c	      Start a new page of the plot
	      para(12) = 0.0
	      call nplot(1,0.0,0.0,' ',' ',' ',para)
	   endif
	   iyorg = mod(im1/ibph,ibph)
	   ixorg = mod(im1,ibph)
	   bxorg = ippb*ixorg
	   byorg = ippb*iyorg
	   xorg = bxorg + ippb/2.0
	   yorg = byorg + ippb/2.0 + bybot
	   write(lbl,'(i4,a)') code(i),char(0)
	   para(14) = 5.0
	   call plabel(bxorg,byorg,lbl,para)
	   j = ibeg(i) + 2
	   if (text(j) .eq. iend .and. text(j+1) .eq. iend) go to 199
	   call nmove(xp(text(j)),yp(text(j+1)),para)
	   j = j + 2
110	   continue
	      if (text(j) .eq. iend) then
		 if (text(j+1) .eq. iend) goto 199
		 call nmove(xp(text(j+2)),yp(text(j+3)),para)
		 j = j + 4
	      else
		 call nline(xp(text(j)),yp(text(j+1)),para)
		 j = j + 2
	      endif
	      if (j .gt. maxcrd) then
		 write(6,*) ' ** Error: too many points in ',code(i)
		 stop
	      endif
	   go to 110
199	continue
c
200	continue
	stop
	end
