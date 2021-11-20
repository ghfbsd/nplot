c	Test the polygonal symbols in nplot
	PROGRAM polyplot
	PARAMETER (mxpara=22)
	REAL x(100), y1(100), y2(100), x3(10), y3(10), para(mxpara)
	INTEGER I

	DO 100 I=1, 100
	    X(i) = i/5
	    y1(i) = x(i)**2
	    y2(i) = 5*x(i)
100	  CONTINUE

	DO 200 I=1,mxpara
200	    Para(i) = 0.0
	Para(13) = -8.0
	Para(14) = 3.0
	

	CALL Nplot(100, x, y1, ' ', ' ', ' ', Para)

c     Test out Nshad
	x3(1) = 5
	y3(1) = 50
	x3(2) = 10
	y3(2) = 50
	x3(3) = 10
	y3(3) = 200
	x3(4) = 5
	y3(4) = 200
	x3(5) = 5
	y3(5) = 50
	CALL Nshad(5, x3, y3, 2, Para)

	Para(13) = -2.0 - 16.0
	CALL Nplot(100, x, y2, ' ', ' ', ' ', Para)
	Para(13) = 0.0
	Para(14) = -1.0
	Para(21) = 5.0
	Para(22) = 5.0
	CALL Nplot(100, x, y1, ' ', ' ', ' ', Para)

	END
