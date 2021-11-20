c	Test the polygonal symbols in nplot
	PROGRAM polyplot
	PARAMETER (mxpara=22)
	REAL x(100), y1(100), y2(100), para(mxpara)
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
	Para(13) = -2.0 - 16.0
	CALL Nplot(100, x, y2, ' ', ' ', ' ', Para)
	Para(13) = 0.0
	Para(14) = -1.0
	Para(21) = 5.0
	Para(22) = 5.0
	CALL Nplot(100, x, y1, ' ', ' ', ' ', Para)
	END
