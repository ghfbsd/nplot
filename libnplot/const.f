	subroutine conzro(s,n)
c nulls out a string of length n
c so it can be 'started' again in const
	character*1 s(1)
	do 1 i=1,n
1	s(i)=char(0)
	return
	end
      subroutine nadjus(str,l)
c nu f77 subroutine to take character string str
c and return its length l
      character*1 str(80),space,null
      space=char(32)
      null=char(0)
      do 5 il = 1,80
	 if (str(il) .eq. null) go to 10
    5 continue
   10 continue
      l = il-1
   15 if (l .eq. 0) return
	 if (str(l) .ne. space) return
	 l = l-1
      go to 15
      end
c
c
c
      subroutine const(as,xs)
c     const - this subroutine appends a space and the character string in xs
c     after the last non-space character in as.  If aggregate length is less
c     than 80 characters, a null character is placed after the catenated
c     text.
      character*1 xs(80),as(80)
      call nadjus(as,l1)
      if (l1 .ge. 80) return
      call nadjus(xs,l2)
      l2 = min(l2,79 - l1)
      if (l1+l2 .lt. 80) then
	 as(l1+1) = ' '
	 do 10 i = 1, l2
	    as(i+1+l1) = xs(i)
   10    continue
	 i = l1+l2+2
	 if (i .le. 80) as(i) = char(0)
      endif
      end

      subroutine conum(as,x,m)
c
c this subroutine appends alphanumeric
c representation of x to the character
c string in as. input m is number of decimal places
c desired.
c
c
      character*1 as(80),xs(80),ds(80)
      call nform(x,ds,m,l)
      do 7 i = 1, l
    7 xs (i) = ds(i)
      do 20 i = l+1,80
   20 xs(i) = ' '
      call const(as,xs)
      return
      end
      subroutine nform(xi,s,m,l)
c     NU f77 subroutine to take a floating point (not integer!)
c     number and convert it to a character string s.
c
c     Parameters:  xi - number to convert
c                   m - number of places to the right of the decimal point
c                   s - string to place result into
c                   l - length of the string (number of nonblank characters)
c
c     Properties:  The value xi will be returned with the number of trailing
c        digits specified.  If all the digits are zero, however, they will
c        be lopped off, but a trailing decimal point is retained.  However,
c        if the number of digits desired is zero, no decimal point is ever
c        supplied.
c
c     Rewritten to use Fortran I/O so to be consistent with Fortran rounding
c     by G. Helffrich 5/87.
      character*80 string
      character*10 format
      character*1 s(80) 
      if (m .lt. 0 .or. m .gt. 77) then
         string = '<Nform: invalid l>'
         l = 20
	 do 20 i=1,20
	    s(i) = string(i:i)
20       continue
         return
      endif
      write(format,100) m
100   format('(F80.',I2.2,')')
      write(string,format) xi
      do 30 i=1,80
	 if (string(i:i) .ne. ' ') then
	    do 25 j=i,80
	       s(j-i+1)=string(j:j)
25          continue
	    l = 80-i+1
	    do 27 j=l,1,-1
	       if (.not. (s(j) .eq. '0' .or. s(j) .eq. ' ')) then
		  if (s(j) .eq. '.') then
		     if (m .eq. 0) then
			s(j) = ' '
			l = j-1
		     else
			l = j
		     endif
		  endif
		  return
	       endif
27          continue
	    return
	 endif
30    continue
      l = 0
      return
      end
