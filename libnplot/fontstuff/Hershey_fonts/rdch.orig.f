	integer function rdch(unit,ftarr,maxlen,len)
	parameter (iend=-64,nspl=15)
c	Function to read a font character from the unit given, into the
c	array.  The character number is returned.  If end-of-file, then
c	zero is returned.
	integer unit,ftarr(maxlen),icx(nspl),icy(nspl)
	character*1 sep(nspl)
	ic = 0
	i = 1
10	continue
	   read(unit,1000,end=999) ich,(icx(j),icy(j),sep(j),j=1,nspl)
1000	   format(I5,3X,15(I3,1X,I3,A1))
	   if (ich .ne. 0) then
	      if (ic .ne. 0) then
		 rdch = -1
		 return
	      endif
	      ic = ich
	   endif
	   do 12 k=nspl,1,-1
	      if (sep(k) .eq. ':') go to 14
12	   continue
14	   continue
	   do 15 j=1,k
	      ftarr(i) = icx(j)
	      ftarr(i+1) = icy(j)
	      i = i + 2
	      if (icx(j) .eq. iend .and. icy(j) .eq. iend) then
		 rdch = ic
		 len = i
		 return
	      endif
	      if (i .gt. maxlen) then
		 rdch = -999
		 len = i
		 return
	      endif
15	   continue
	go to 10
c	End-of-file - return 0
999	continue
	rdch = 0
	len = 0
	return
	end
