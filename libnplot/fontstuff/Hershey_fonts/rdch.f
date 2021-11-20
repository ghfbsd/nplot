	integer function icvt(c)
	character*1 c
	if (c .eq. ' ') then
	   icvt = -64
	else
	   icvt = ichar(c) - ichar('R')
	end if
	return
	end
	integer function rdch(unit,ftarr,maxlen,len)
	parameter (nspl=200+36)
c	Function to read a font character from the unit given, into the
c	array.  The character number is returned.  If end-of-file, then
c	zero is returned.
	integer unit,ftarr(maxlen)
	character*1 icx(nspl),icy(nspl)
10	continue
	   read(unit,1000,end=999) ich,ist,(icx(j),icy(j),j=1,min(ist,32))
1000	   format(I5,I3,32(2A1))
	if (ich .eq. 0) go to 10

	   if (ist .gt. nspl) then
	      write(0,1001) ich,ist
1001          format(' Error: too many strokes in ',I4,': ',I4)
	      stop
	   end if
	   l = min(ist,32)
1050       continue
	      if (l .eq. ist)  goto 14
	      k = l + 1
	      l = min(k+35,ist)
	      read(unit,'(36(2A1))',end=999) (icx(j),icy(j),j=k,l)
	   go to 1050
14	   continue
	   i = 1
	   do 15 j=1,ist
	      ftarr(i) = icvt(icx(j))
	      ftarr(i+1) = icvt(icy(j))
	      i = i + 2
15	   continue
           ftarr(i) = icvt(' ')
	   ftarr(i+1) = icvt(' ')
	continue
	rdch = ich
	len = (ist+1)*2
	return
c	End-of-file - return 0
999	continue
	rdch = 0
	len = 0
	return
	end
