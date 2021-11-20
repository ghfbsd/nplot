	PROGRAM Testpat
	REAL X(5), Y(5), Para(20)
	INTEGER Pat, Row, Col, NumPat
	CHARACTER*70 Label
	PARAMETER (NumPat=39)

	DO 100 I=1,20
100	    Para(i) = 0.0
	Para(1)  = 9.5
	Para(2)  = 6.5
	Para(3)  = 0.0
	Para(4)  = 0.0
	Para(5)  = 9.5
	Para(6)  = 6.5
	Para(17) = 1.0
	Para(18) = 1.0
	Para(15) = 6.0
	CALL Nplot(1, 0.0, 0.0, ' ', ' ', 'Nplot Patterns', Para)
	Para(15) = 0.0

	DO 1000 Pat=0, NumPat-1
	    Row = Pat/9
	    Col = MOD(Pat, 9)
	    X(1) = Col*1.00 + 0.25
	    Y(1) = Row*1.125 + 0.25
	    X(2) = X(1) + 0.75
	    Y(2) = Y(1)
	    X(3) = X(1) + 0.75
	    Y(3) = Y(1) + 0.75
	    X(4) = X(1)
	    Y(4) = Y(1) + 0.75
	    X(5) = X(1)
	    Y(5) = Y(1)
	    CALL Npat(5, X, Y, Pat, Para)
	    Para(15) = 3.0
	    CALL Nplot(5, X, Y, ' ', ' ', ' ', Para)
	    CALL Conzro(Label, 70)
	    CALL Conum(Label, FLOAT(Pat), 0)
	    IF (Pat .GT. 9)     X(1) = X(1) - .10
	    Para(15) = 6.0
	    CALL Nlabel(X(1) + .17, Y(1) - .2, Label, Para)
1000	  CONTINUE

	END
