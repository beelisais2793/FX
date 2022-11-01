/* cpoly.f -- translated by f2c (version 20100827).
   You must link the resulting object file with libf2c:
	on Microsoft Windows system, link with libf2c.lib;
	on Linux or Unix systems, link with .../path/to/libf2c.a -lm
	or, if you install libf2c.a in a standard place, with -lf2c -lm
	-- in that order, at the end of the command line, as in
		cc *.o -lf2c -lm
	Source for libf2c is in /netlib/f2c/libf2c.zip, e.g.,

		http://www.netlib.org/f2c/libf2c.zip
*/

#define tmwtypes_do_not_include_stdbool 1
#include "math.h"
#include "float.h"
#include "f2c.h"
#include "mex.h"

/* Common Block Declarations */

struct {
    doublereal *pr, *pi, *hr, *hi, *qpr, *qpi, *qhr, *qhi,
	    *shr, *shi, sr, si, tr, ti, pvr, pvi, are, mre, eta, 
	    infin;
    integer nn;
} global_;

#define global_1 global_

/* Table of constant values */

static integer c__5 = 5;
static integer c__10 = 10;

/*     ALGORITHM 419 COLLECTED ALGORITHMS FROM ACM. */
/*     ALGORITHM APPEARED IN COMM. ACM, VOL. 15, NO. 02, */
/*     P. 097. */

/* Subroutine */ int cpoly(doublereal *opr, doublereal *opi, integer *degree,
	 doublereal *zeror, doublereal *zeroi, logical *fail)
{
    /* System generated locals */
    integer i__1;
    doublereal d__1, d__2;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    integer i__;
    doublereal zi, zr, xx, yy, bnd, xxx;
    integer cnt1, cnt2;
    doublereal base;
    extern doublereal cmod(doublereal *, doublereal *);
    extern /* Subroutine */ int mcon(doublereal *, doublereal *, doublereal *
	    , doublereal *);
    logical conv;
    doublereal cosr, sinr;
    integer idnn2;
    extern doublereal scale(integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *);
    extern /* Subroutine */ int cdivid(doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *);
    extern doublereal cauchy(integer *, doublereal *, doublereal *);
    doublereal smalno;
    extern /* Subroutine */ int noshft(integer *), fxshft(integer *, 
	    doublereal *, doublereal *, logical *);

    /* Allocate matrices */
    global_1.pr  = mxMalloc(((*degree)+1)*sizeof(doublereal));
    global_1.pi  = mxMalloc(((*degree)+1)*sizeof(doublereal));
    global_1.hr  = mxMalloc(((*degree)+1)*sizeof(doublereal));
    global_1.hi  = mxMalloc(((*degree)+1)*sizeof(doublereal));
    global_1.qpr = mxMalloc(((*degree)+1)*sizeof(doublereal));
    global_1.qpi = mxMalloc(((*degree)+1)*sizeof(doublereal));
    global_1.qhr = mxMalloc(((*degree)+1)*sizeof(doublereal));
    global_1.qhi = mxMalloc(((*degree)+1)*sizeof(doublereal));
    global_1.shr = mxMalloc(((*degree)+1)*sizeof(doublereal));
    global_1.shi = mxMalloc(((*degree)+1)*sizeof(doublereal));
    
/* Added changes from Remark on Algorithm 419 by David H. Withers */
/* CACM (March 1974) Vol 17 No 3 p. 157 */

/* FINDS THE ZEROS OF A COMPLEX POLYNOMIAL. */
/* OPR, OPI  -  DOUBLE PRECISION VECTORS OF REAL AND */
/* IMAGINARY PARTS OF THE COEFFICIENTS IN */
/* ORDER OF DECREASING POWERS. */
/* DEGREE    -  INTEGER DEGREE OF POLYNOMIAL. */
/* ZEROR, ZEROI  -  OUTPUT DOUBLE PRECISION VECTORS OF */
/* REAL AND IMAGINARY PARTS OF THE ZEROS. */
/* FAIL      -  OUTPUT LOGICAL PARAMETER,  TRUE  ONLY IF */
/* LEADING COEFFICIENT IS ZERO OR IF CPOLY */
/* HAS FOUND FEWER THAN DEGREE ZEROS. */
/* THE PROGRAM HAS BEEN WRITTEN TO REDUCE THE CHANCE OF OVERFLOW */
/* OCCURRING. IF IT DOES OCCUR, THERE IS STILL A POSSIBILITY THAT */
/* THE ZEROFINDER WILL WORK PROVIDED THE OVERFLOWED QUANTITY IS */
/* REPLACED BY A LARGE NUMBER. */
/* COMMON AREA */
/* TO CHANGE THE SIZE OF POLYNOMIALS WHICH CAN BE SOLVED, REPLACE */
/* THE DIMENSION OF THE ARRAYS IN THE COMMON AREA. */
/* INITIALIZATION OF CONSTANTS */
    /* Parameter adjustments */
    --zeroi;
    --zeror;
    --opi;
    --opr;

    /* Function Body */
    mcon(&global_1.eta, &global_1.infin, &smalno, &base);
    global_1.are = global_1.eta;
    global_1.mre = sqrt(2.) * 2. * global_1.eta;
    xx = 0.707106781186548;
    yy = -xx;
    cosr = -0.069756473744125;
    sinr = 0.997564050259824;
    *fail = FALSE_;
    global_1.nn = *degree + 1;
/* ALGORITHM FAILS IF THE LEADING COEFFICIENT IS ZERO. */
    if (opr[1] != 0. || opi[1] != 0.) {
	goto L10;
    }
    *fail = TRUE_;
    return 0;
/* REMOVE THE ZEROS AT THE ORIGIN IF ANY. */
L10:
    if (opr[global_1.nn] != 0. || opi[global_1.nn] != 0.) {
	goto L20;
    }
    idnn2 = *degree - global_1.nn + 2;
    zeror[idnn2] = 0.;
    zeroi[idnn2] = 0.;
    --global_1.nn;
    goto L10;
/* MAKE A COPY OF THE COEFFICIENTS. */
L20:
    i__1 = global_1.nn;
    for (i__ = 1; i__ <= i__1; ++i__) {
	global_1.pr[i__ - 1] = opr[i__];
	global_1.pi[i__ - 1] = opi[i__];
	global_1.shr[i__ - 1] = cmod(&global_1.pr[i__ - 1], &global_1.pi[i__ 
		- 1]);
/* L30: */
    }
/* SCALE THE POLYNOMIAL. */
    bnd = scale(&global_1.nn, global_1.shr, &global_1.eta, &global_1.infin, &
	    smalno, &base);
    if (bnd == 1.) {
	goto L40;
    }
    i__1 = global_1.nn;
    for (i__ = 1; i__ <= i__1; ++i__) {
	global_1.pr[i__ - 1] = bnd * global_1.pr[i__ - 1];
	global_1.pi[i__ - 1] = bnd * global_1.pi[i__ - 1];
/* L35: */
    }
/* START THE ALGORITHM FOR ONE ZERO . */
L40:
    if (global_1.nn > 2) {
	goto L50;
    }
/* CALCULATE THE FINAL ZERO AND RETURN. */
    d__1 = -global_1.pr[1];
    d__2 = -global_1.pi[1];
    cdivid(&d__1, &d__2, global_1.pr, global_1.pi, &zeror[*degree], &zeroi[*
	    degree]);
    return 0;
/* CALCULATE BND, A LOWER BOUND ON THE MODULUS OF THE ZEROS. */
L50:
    i__1 = global_1.nn;
    for (i__ = 1; i__ <= i__1; ++i__) {
	global_1.shr[i__ - 1] = cmod(&global_1.pr[i__ - 1], &global_1.pi[i__ 
		- 1]);
/* L60: */
    }
    bnd = cauchy(&global_1.nn, global_1.shr, global_1.shi);
/* OUTER LOOP TO CONTROL 2 MAJOR PASSES WITH DIFFERENT SEQUENCES */
/* OF SHIFTS. */
    for (cnt1 = 1; cnt1 <= 2; ++cnt1) {
/* FIRST STAGE CALCULATION, NO SHIFT. */
	noshft(&c__5);
/* INNER LOOP TO SELECT A SHIFT. */
	for (cnt2 = 1; cnt2 <= 9; ++cnt2) {
/* SHIFT IS CHOSEN WITH MODULUS BND AND AMPLITUDE ROTATED BY */
/* 94 DEGREES FROM THE PREVIOUS SHIFT */
	    xxx = cosr * xx - sinr * yy;
	    yy = sinr * xx + cosr * yy;
	    xx = xxx;
	    global_1.sr = bnd * xx;
	    global_1.si = bnd * yy;
/* SECOND STAGE CALCULATION, FIXED SHIFT. */
	    i__1 = cnt2 * 10;
	    fxshft(&i__1, &zr, &zi, &conv);
	    if (! conv) {
		goto L80;
	    }
/* THE SECOND STAGE JUMPS DIRECTLY TO THE THIRD STAGE ITERATION. */
/* IF SUCCESSFUL THE ZERO IS STORED AND THE POLYNOMIAL DEFLATED. */
	    idnn2 = *degree - global_1.nn + 2;
	    zeror[idnn2] = zr;
	    zeroi[idnn2] = zi;
	    --global_1.nn;
	    i__1 = global_1.nn;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		global_1.pr[i__ - 1] = global_1.qpr[i__ - 1];
		global_1.pi[i__ - 1] = global_1.qpi[i__ - 1];
/* L70: */
	    }
	    goto L40;
L80:
/* IF THE ITERATION IS UNSUCCESSFUL ANOTHER SHIFT IS CHOSEN. */
/* L90: */
	    ;
	}
/* IF 9 SHIFTS FAIL, THE OUTER LOOP IS REPEATED WITH ANOTHER */
/* SEQUENCE OF SHIFTS. */
/* L100: */
    }
/* THE ZEROFINDER HAS FAILED ON TWO MAJOR PASSES. */
/* RETURN EMPTY HANDED. */
    *fail = TRUE_;
    return 0;
} /* cpoly_ */

/* Subroutine */ int noshft(integer *l1)
{
    /* System generated locals */
    integer i__1, i__2;
    doublereal d__1, d__2;

    /* Local variables */
    integer i__, j, n;
    doublereal t1, t2;
    integer jj, nm1;
    doublereal xni;
    extern doublereal cmod(doublereal *, doublereal *);
    extern /* Subroutine */ int cdivid(doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *);

/* COMPUTES  THE DERIVATIVE  POLYNOMIAL AS THE INITIAL H */
/* POLYNOMIAL AND COMPUTES L1 NO-SHIFT H POLYNOMIALS. */
/* COMMON AREA */
    n = global_1.nn - 1;
    nm1 = n - 1;
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	xni = (doublereal) (global_1.nn - i__);
	global_1.hr[i__ - 1] = xni * global_1.pr[i__ - 1] / (doublereal) n;
	global_1.hi[i__ - 1] = xni * global_1.pi[i__ - 1] / (doublereal) n;
/* L10: */
    }
    i__1 = *l1;
    for (jj = 1; jj <= i__1; ++jj) {
	if (cmod(&global_1.hr[n - 1], &global_1.hi[n - 1]) <= global_1.eta * 
		10. * cmod(&global_1.pr[n - 1], &global_1.pi[n - 1])) {
	    goto L30;
	}
	d__1 = -global_1.pr[global_1.nn - 1];
	d__2 = -global_1.pi[global_1.nn - 1];
	cdivid(&d__1, &d__2, &global_1.hr[n - 1], &global_1.hi[n - 1], &
		global_1.tr, &global_1.ti);
	i__2 = nm1;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    j = global_1.nn - i__;
	    t1 = global_1.hr[j - 2];
	    t2 = global_1.hi[j - 2];
	    global_1.hr[j - 1] = global_1.tr * t1 - global_1.ti * t2 + 
		    global_1.pr[j - 1];
	    global_1.hi[j - 1] = global_1.tr * t2 + global_1.ti * t1 + 
		    global_1.pi[j - 1];
/* L20: */
	}
	global_1.hr[0] = global_1.pr[0];
	global_1.hi[0] = global_1.pi[0];
	goto L50;
/* IF THE CONSTANT TERM IS ESSENTIALLY ZERO, SHIFT H COEFFICIENTS. */
L30:
	i__2 = nm1;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    j = global_1.nn - i__;
	    global_1.hr[j - 1] = global_1.hr[j - 2];
	    global_1.hi[j - 1] = global_1.hi[j - 2];
/* L40: */
	}
	global_1.hr[0] = 0.;
	global_1.hi[0] = 0.;
L50:
	;
    }
    return 0;
} /* noshft_ */

/* Subroutine */ int fxshft(integer *l2, doublereal *zr, doublereal *zi, 
	logical *conv)
{
    /* System generated locals */
    integer i__1, i__2;
    doublereal d__1, d__2;

    /* Local variables */
    integer i__, j, n;
    doublereal oti, otr;
    extern doublereal cmod(doublereal *, doublereal *);
    logical pasd, bool, test;
    doublereal svsi, svsr;
    extern /* Subroutine */ int calct(logical *), nexth(logical *), vrshft(
	    integer *, doublereal *, doublereal *, logical *), polyev(
	    integer *, doublereal *, doublereal *, doublereal *, doublereal *,
	     doublereal *, doublereal *, doublereal *, doublereal *);

/* COMPUTES L2 FIXED-SHIFT H POLYNOMIALS AND TESTS FOR */
/* CONVERGENCE. */
/* INITIATES A VARIABLE-SHIFT ITERATION AND RETURNS WITH THE */
/* APPROXIMATE ZERO IF SUCCESSFUL. */
/* L2 - LIMIT OF FIXED SHIFT STEPS */
/* ZR,ZI - APPROXIMATE ZERO IF CONV IS .TRUE. */
/* CONV  - LOGICAL INDICATING CONVERGENCE OF STAGE 3 ITERATION */
/* COMMON AREA */
    n = global_1.nn - 1;
/* EVALUATE P AT S. */
    polyev(&global_1.nn, &global_1.sr, &global_1.si, global_1.pr, 
	    global_1.pi, global_1.qpr, global_1.qpi, &global_1.pvr, &
	    global_1.pvi);
    test = TRUE_;
    pasd = FALSE_;
/* CALCULATE FIRST T = -P(S)/H(S). */
    calct(&bool);
/* MAIN LOOP FOR ONE SECOND STAGE STEP. */
    i__1 = *l2;
    for (j = 1; j <= i__1; ++j) {
	otr = global_1.tr;
	oti = global_1.ti;
/* COMPUTE NEXT H POLYNOMIAL AND NEW T. */
	nexth(&bool);
	calct(&bool);
	*zr = global_1.sr + global_1.tr;
	*zi = global_1.si + global_1.ti;
/* TEST FOR CONVERGENCE UNLESS STAGE 3 HAS FAILED ONCE OR THIS */
/* IS THE LAST H POLYNOMIAL . */
	if (bool || ! test || j == *l2) {
	    goto L50;
	}
	d__1 = global_1.tr - otr;
	d__2 = global_1.ti - oti;
	if (cmod(&d__1, &d__2) >= cmod(zr, zi) * .5) {
	    goto L40;
	}
	if (! pasd) {
	    goto L30;
	}
/* THE WEAK CONVERGENCE TEST HAS BEEN PASSED TWICE, START THE */
/* THIRD STAGE ITERATION, AFTER SAVING THE CURRENT H POLYNOMIAL */
/* AND SHIFT. */
	i__2 = n;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    global_1.shr[i__ - 1] = global_1.hr[i__ - 1];
	    global_1.shi[i__ - 1] = global_1.hi[i__ - 1];
/* L10: */
	}
	svsr = global_1.sr;
	svsi = global_1.si;
	vrshft(&c__10, zr, zi, conv);
	if (*conv) {
	    return 0;
	}
/* THE ITERATION FAILED TO CONVERGE. TURN OFF TESTING AND RESTORE */
/* H,S,PV AND T. */
	test = FALSE_;
	i__2 = n;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    global_1.hr[i__ - 1] = global_1.shr[i__ - 1];
	    global_1.hi[i__ - 1] = global_1.shi[i__ - 1];
/* L20: */
	}
	global_1.sr = svsr;
	global_1.si = svsi;
	polyev(&global_1.nn, &global_1.sr, &global_1.si, global_1.pr, 
		global_1.pi, global_1.qpr, global_1.qpi, &global_1.pvr, &
		global_1.pvi);
	calct(&bool);
	goto L50;
L30:
	pasd = TRUE_;
	goto L50;
L40:
	pasd = FALSE_;
L50:
	;
    }
/* ATTEMPT AN ITERATION WITH FINAL H POLYNOMIAL FROM SECOND STAGE. */
    vrshft(&c__10, zr, zi, conv);
    return 0;
} /* fxshft_ */

/* Subroutine */ int vrshft(integer *l3, doublereal *zr, doublereal *zi, 
	logical *conv)
{
    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    logical b;
    integer i__, j;
    doublereal r1, r2, mp, ms, tp, omp;
    extern doublereal cmod(doublereal *, doublereal *);
    logical bool;
    extern /* Subroutine */ int calct(logical *);
    extern doublereal errev(integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *);
    extern /* Subroutine */ int nexth(logical *);
    doublereal relstp;
    extern /* Subroutine */ int polyev(integer *, doublereal *, doublereal *,
	     doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *);

/* CARRIES OUT THE THIRD STAGE ITERATION. */
/* L3 - LIMIT OF STEPS IN STAGE 3. */
/* ZR,ZI   - ON ENTRY CONTAINS THE INITIAL ITERATE, IF THE */
/* ITERATION CONVERGES IT CONTAINS THE FINAL ITERATE */
/* ON EXIT. */
/* CONV    -  .TRUE. IF ITERATION CONVERGES */
/* COMMON AREA */
    *conv = FALSE_;
    b = FALSE_;
    global_1.sr = *zr;
    global_1.si = *zi;
/* MAIN LOOP FOR STAGE THREE */
    i__1 = *l3;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* EVALUATE P AT S AND TEST FOR CONVERGENCE. */
	polyev(&global_1.nn, &global_1.sr, &global_1.si, global_1.pr, 
		global_1.pi, global_1.qpr, global_1.qpi, &global_1.pvr, &
		global_1.pvi);
	mp = cmod(&global_1.pvr, &global_1.pvi);
	ms = cmod(&global_1.sr, &global_1.si);
	if (mp > errev(&global_1.nn, global_1.qpr, global_1.qpi, &ms, &mp, &
		global_1.are, &global_1.mre) * 20.) {
	    goto L10;
	}
/* POLYNOMIAL VALUE IS SMALLER IN VALUE THAN A BOUND ON THE ERROR */
/* IN EVALUATING P, TERMINATE THE ITERATION. */
	*conv = TRUE_;
	*zr = global_1.sr;
	*zi = global_1.si;
	return 0;
L10:
	if (i__ == 1) {
	    goto L40;
	}
	if (b || mp < omp || relstp >= .05) {
	    goto L30;
	}
/* ITERATION HAS STALLED. PROBABLY A CLUSTER OF ZEROS. DO 5 FIXED */
/* SHIFT STEPS INTO THE CLUSTER TO FORCE ONE ZERO TO DOMINATE. */
	tp = relstp;
	b = TRUE_;
	if (relstp < global_1.eta) {
	    tp = global_1.eta;
	}
	r1 = sqrt(tp);
	r2 = global_1.sr * (r1 + 1.) - global_1.si * r1;
	global_1.si = global_1.sr * r1 + global_1.si * (r1 + 1.);
	global_1.sr = r2;
	polyev(&global_1.nn, &global_1.sr, &global_1.si, global_1.pr, 
		global_1.pi, global_1.qpr, global_1.qpi, &global_1.pvr, &
		global_1.pvi);
	for (j = 1; j <= 5; ++j) {
	    calct(&bool);
	    nexth(&bool);
/* L20: */
	}
	omp = global_1.infin;
	goto L50;
/* EXIT IF POLYNOMIAL VALUE INCREASES SIGNIFICANTLY. */
L30:
	if (mp * .1 > omp) {
	    return 0;
	}
L40:
	omp = mp;
/* CALCULATE NEXT ITERATE. */
L50:
	calct(&bool);
	nexth(&bool);
	calct(&bool);
	if (bool) {
	    goto L60;
	}
	relstp = cmod(&global_1.tr, &global_1.ti) / cmod(&global_1.sr, &
		global_1.si);
	global_1.sr += global_1.tr;
	global_1.si += global_1.ti;
L60:
	;
    }
    return 0;
} /* vrshft_ */

/* Subroutine */ int calct(logical *bool)
{
    /* System generated locals */
    doublereal d__1, d__2;

    /* Local variables */
    integer n;
    doublereal hvi, hvr;
    extern doublereal cmod(doublereal *, doublereal *);
    extern /* Subroutine */ int cdivid(doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *), polyev(
	    integer *, doublereal *, doublereal *, doublereal *, doublereal *,
	     doublereal *, doublereal *, doublereal *, doublereal *);

/* COMPUTES  T = -P(S)/H(S). */
/* BOOL   - LOGICAL, SET TRUE IF H(S) IS ESSENTIALLY ZERO. */
/* COMMON AREA */
    n = global_1.nn - 1;
/* EVALUATE H(S). */
    polyev(&n, &global_1.sr, &global_1.si, global_1.hr, global_1.hi, 
	    global_1.qhr, global_1.qhi, &hvr, &hvi);
    *bool = cmod(&hvr, &hvi) <= global_1.are * 10. * cmod(&global_1.hr[n - 
	    1], &global_1.hi[n - 1]);
    if (*bool) {
	goto L10;
    }
    d__1 = -global_1.pvr;
    d__2 = -global_1.pvi;
    cdivid(&d__1, &d__2, &hvr, &hvi, &global_1.tr, &global_1.ti);
    return 0;
L10:
    global_1.tr = 0.;
    global_1.ti = 0.;
    return 0;
} /* calct_ */

/* Subroutine */ int nexth(logical *bool)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    integer j, n;
    doublereal t1, t2;
    integer nm1;

/* CALCULATES THE NEXT SHIFTED H POLYNOMIAL. */
/* BOOL   -  LOGICAL, IF .TRUE. H(S) IS ESSENTIALLY ZERO */
/* COMMON AREA */
    n = global_1.nn - 1;
    nm1 = n - 1;
    if (*bool) {
	goto L20;
    }
    i__1 = n;
    for (j = 2; j <= i__1; ++j) {
	t1 = global_1.qhr[j - 2];
	t2 = global_1.qhi[j - 2];
	global_1.hr[j - 1] = global_1.tr * t1 - global_1.ti * t2 + 
		global_1.qpr[j - 1];
	global_1.hi[j - 1] = global_1.tr * t2 + global_1.ti * t1 + 
		global_1.qpi[j - 1];
/* L10: */
    }
    global_1.hr[0] = global_1.qpr[0];
    global_1.hi[0] = global_1.qpi[0];
    return 0;
/* IF H(S) IS ZERO REPLACE H WITH QH. */
L20:
    i__1 = n;
    for (j = 2; j <= i__1; ++j) {
	global_1.hr[j - 1] = global_1.qhr[j - 2];
	global_1.hi[j - 1] = global_1.qhi[j - 2];
/* L30: */
    }
    global_1.hr[0] = 0.;
    global_1.hi[0] = 0.;
    return 0;
} /* nexth_ */

/* Subroutine */ int polyev(integer *nn, doublereal *sr, doublereal *si, 
	doublereal *pr, doublereal *pi, doublereal *qr, doublereal *qi, 
	doublereal *pvr, doublereal *pvi)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    integer i__;
    doublereal t;

/* EVALUATES A POLYNOMIAL  P  AT  S  BY THE HORNER RECURRENCE */
/* PLACING THE PARTIAL SUMS IN Q AND THE COMPUTED VALUE IN PV. */
    /* Parameter adjustments */
    --qi;
    --qr;
    --pi;
    --pr;

    /* Function Body */
    qr[1] = pr[1];
    qi[1] = pi[1];
    *pvr = qr[1];
    *pvi = qi[1];
    i__1 = *nn;
    for (i__ = 2; i__ <= i__1; ++i__) {
	t = *pvr * *sr - *pvi * *si + pr[i__];
	*pvi = *pvr * *si + *pvi * *sr + pi[i__];
	*pvr = t;
	qr[i__] = *pvr;
	qi[i__] = *pvi;
/* L10: */
    }
    return 0;
} /* polyev_ */

doublereal errev(integer *nn, doublereal *qr, doublereal *qi, doublereal *ms,
	 doublereal *mp, doublereal *are, doublereal *mre)
{
    /* System generated locals */
    integer i__1;
    doublereal ret_val;

    /* Local variables */
    doublereal e;
    integer i__;
    extern doublereal cmod(doublereal *, doublereal *);

/* BOUNDS THE ERROR IN EVALUATING THE POLYNOMIAL BY THE HORNER */
/* RECURRENCE. */
/* QR,QI - THE PARTIAL SUMS */
/* MS    -MODULUS OF THE POINT */
/* MP    -MODULUS OF POLYNOMIAL VALUE */
/* ARE, MRE -ERROR BOUNDS ON COMPLEX ADDITION AND MULTIPLICATION */
    /* Parameter adjustments */
    --qi;
    --qr;

    /* Function Body */
    e = cmod(&qr[1], &qi[1]) * *mre / (*are + *mre);
    i__1 = *nn;
    for (i__ = 1; i__ <= i__1; ++i__) {
	e = e * *ms + cmod(&qr[i__], &qi[i__]);
/* L10: */
    }
    ret_val = e * (*are + *mre) - *mp * *mre;
    return ret_val;
} /* errev_ */

doublereal cauchy(integer *nn, doublereal *pt, doublereal *q)
{
    /* System generated locals */
    integer i__1;
    doublereal ret_val, d__1;

    /* Builtin functions */
    double log(doublereal), exp(doublereal);

    /* Local variables */
    doublereal f;
    integer i__, n;
    doublereal x, df, dx, xm;

/* CAUCHY COMPUTES A LOWER BOUND ON THE MODULI OF THE ZEROS OF A */
/* POLYNOMIAL - PT IS THE MODULUS OF THE COEFFICIENTS. */
    /* Parameter adjustments */
    --q;
    --pt;

    /* Function Body */
    pt[*nn] = -pt[*nn];
/* COMPUTE UPPER ESTIMATE OF BOUND. */
    n = *nn - 1;
    x = exp((log(-pt[*nn]) - log(pt[1])) / (doublereal) n);
    if (pt[n] == 0.) {
	goto L20;
    }
/* IF NEWTON STEP AT THE ORIGIN IS BETTER, USE IT. */
    xm = -pt[*nn] / pt[n];
    if (xm < x) {
	x = xm;
    }
/* CHOP THE INTERVAL (0,X) UNITL F LE 0. */
L20:
    xm = x * .1;
    f = pt[1];
    i__1 = *nn;
    for (i__ = 2; i__ <= i__1; ++i__) {
	f = f * xm + pt[i__];
/* L30: */
    }
    if (f <= 0.) {
	goto L40;
    }
    x = xm;
    goto L20;
L40:
    dx = x;
/* DO NEWTON ITERATION UNTIL X CONVERGES TO TWO DECIMAL PLACES. */
L50:
    if ((d__1 = dx / x, abs(d__1)) <= .005) {
	goto L70;
    }
    q[1] = pt[1];
    i__1 = *nn;
    for (i__ = 2; i__ <= i__1; ++i__) {
	q[i__] = q[i__ - 1] * x + pt[i__];
/* L60: */
    }
    f = q[*nn];
    df = q[1];
    i__1 = n;
    for (i__ = 2; i__ <= i__1; ++i__) {
	df = df * x + q[i__];
/* L65: */
    }
    dx = f / df;
    x -= dx;
    goto L50;
L70:
    ret_val = x;
    return ret_val;
} /* cauchy_ */

doublereal scale(integer *nn, doublereal *pt, doublereal *eta, doublereal *
	infin, doublereal *smalno, doublereal *base)
{
    /* System generated locals */
    integer i__1;
    doublereal ret_val;

    /* Builtin functions */
    double sqrt(doublereal), log(doublereal), pow_di(doublereal *, integer *);

    /* Local variables */
    integer i__, l;
    doublereal x, hi, sc, lo, min__, max__;

/* RETURNS A SCALE FACTOR TO MULTIPLY THE COEFFICIENTS OF THE */
/* POLYNOMIAL. THE SCALING IS DONE TO AVOID OVERFLOW AND TO AVOID */
/* UNDETECTED UNDERFLOW INTERFERING WITH THE CONVERGENCE */
/* CRITERION.  THE FACTOR IS A POWER OF THE BASE. */
/* PT - MODULUS OF COEFFICIENTS OF P */
/* ETA,INFIN,SMALNO,BASE - CONSTANTS DESCRIBING THE */
/* FLOATING POINT ARITHMETIC. */
/* FIND LARGEST AND SMALLEST MODULI OF COEFFICIENTS. */
    /* Parameter adjustments */
    --pt;

    /* Function Body */
    hi = sqrt(*infin);
    lo = *smalno / *eta;
    max__ = 0.;
    min__ = *infin;
    i__1 = *nn;
    for (i__ = 1; i__ <= i__1; ++i__) {
	x = pt[i__];
	if (x > max__) {
	    max__ = x;
	}
	if (x != 0. && x < min__) {
	    min__ = x;
	}
/* L10: */
    }
/* SCALE ONLY IF THERE ARE VERY LARGE OR VERY SMALL COMPONENTS. */
    ret_val = 1.;
    if (min__ >= lo && max__ <= hi) {
	return ret_val;
    }
    x = lo / min__;
    if (x > 1.) {
	goto L20;
    }
    sc = 1. / (sqrt(max__) * sqrt(min__));
    goto L30;
L20:
    sc = x;
    if (*infin / sc > max__) {
	sc = 1.;
    }
L30:
    l = (integer) (log(sc) / log(*base) + .5f);
    ret_val = pow_di(base, &l);
    return ret_val;
} /* scale_ */

/* Subroutine */ int cdivid(doublereal *ar, doublereal *ai, doublereal *br, 
	doublereal *bi, doublereal *cr, doublereal *ci)
{
    doublereal d__, r__, t;
    extern /* Subroutine */ int mcon(doublereal *, doublereal *, doublereal *
	    , doublereal *);
    doublereal infin;

/* COMPLEX DIVISION C = A/B, AVOIDING OVERFLOW. */
    if (*br != 0. || *bi != 0.) {
	goto L10;
    }
/* DIVISION BY ZERO, C = INFINITY. */
    mcon(&t, &infin, &t, &t);
    *cr = infin;
    *ci = infin;
    return 0;
L10:
    if (abs(*br) >= abs(*bi)) {
	goto L20;
    }
    r__ = *br / *bi;
    d__ = *bi + r__ * *br;
    *cr = (*ar * r__ + *ai) / d__;
    *ci = (*ai * r__ - *ar) / d__;
    return 0;
L20:
    r__ = *bi / *br;
    d__ = *br + r__ * *bi;
    *cr = (*ar + *ai * r__) / d__;
    *ci = (*ai - *ar * r__) / d__;
    return 0;
} /* cdivid_ */

doublereal cmod(doublereal *r__, doublereal *i__)
{
    /* System generated locals */
    doublereal ret_val, d__1;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    doublereal ai, ar;

/* MODULUS OF A COMPLEX NUMBER AVOIDING OVERFLOW. */
    ar = abs(*r__);
    ai = abs(*i__);
    if (ar >= ai) {
	goto L10;
    }
/* Computing 2nd power */
    d__1 = ar / ai;
    ret_val = ai * sqrt(d__1 * d__1 + 1.);
    return ret_val;
L10:
    if (ar <= ai) {
	goto L20;
    }
/* Computing 2nd power */
    d__1 = ai / ar;
    ret_val = ar * sqrt(d__1 * d__1 + 1.);
    return ret_val;
L20:
    ret_val = ar * sqrt(2.);
    return ret_val;
} /* cmod_ */

/* Subroutine */ int mcon(doublereal *eta, doublereal *infiny, doublereal *
	smalno, doublereal *base)
{
/* MCON PROVIDES MACHINE CONSTANTS USED IN VARIOUS PARTS OF THE */
/* PROGRAM. THE USER MAY EITHER SET THEM DIRECTLY OR USE THE */
/* STATEMENTS BELOW TO COMPUTE THEM. THE MEANING OF THE FOUR */
/* CONSTANTS ARE - */
/* ETA       THE MAXIMUM RELATIVE REPRESENTATION ERROR */
/* WHICH CAN BE DESCRIBED AS THE SMALLEST POSITIVE */
/* FLOATING-POINT NUMBER SUCH THAT 1.0D0 + ETA IS */
/* GREATER THAN 1.0D0. */
/* INFINY    THE LARGEST FLOATING-POINT NUMBER */
/* SMALNO    THE SMALLEST POSITIVE FLOATING-POINT NUMBER */
/* BASE      THE BASE OF THE FLOATING-POINT NUMBER SYSTEM USED */
/* LET T BE THE NUMBER OF BASE-DIGITS IN EACH FLOATING-POINT */
/* NUMBER(DOUBLE PRECISION). THEN ETA IS EITHER .5*B**(1-T) */
/* OR B**(1-T) DEPENDING ON WHETHER ROUNDING OR TRUNCATION */
/* IS USED. */
/* LET M BE THE LARGEST EXPONENT AND N THE SMALLEST EXPONENT */
/* IN THE NUMBER SYSTEM. THEN INFINY IS (1-BASE**(-T))*BASE**M */
/* AND SMALNO IS BASE**N. */
/* THE VALUES FOR BASE,T,M,N BELOW CORRESPOND TO THE IBM/360. */
    *base = DBL_RADIX;
    *eta = DBL_EPSILON;
    *infiny = DBL_MAX;
    *smalno = DBL_MIN;
    return 0;
} /* mcon_ */

/* Subroutine */ void free_cpoly()
{
    mxFree(global_1.pr);
    mxFree(global_1.pi);
    mxFree(global_1.hr);
    mxFree(global_1.hi);
    mxFree(global_1.qpr);
    mxFree(global_1.qpi);
    mxFree(global_1.qhr);
    mxFree(global_1.qhi);
    mxFree(global_1.shr);
    mxFree(global_1.shi);
}

