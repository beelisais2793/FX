/*
 * Find polynomial roots using the Jenkins-Traub method.
 *
 * R = polyroots(C)
 *
 * compile command:
 * mex -O polyroots.c cpoly.c rpoly.c pow_di.c
 *
 * calls the CPOLY/RPOLY functions from the TOMS419 and TOMS493 code
 *
 * Ivo Houtzager
 *
 * references;
 *   - Jenkins, M. A. and Traub, J. F. (1972), Algorithm 419: Zeros of a
 *     Complex Polynomial, Comm. ACM, 15, 97–99. 
 *   - Jenkins, M. A. (1975), Algorithm 493: Zeros of a Real Polynomial, 
 *     ACM TOMS, 1, 178–189.
 */

#include "mex.h"
#include "matrix.h"
#include "polyroots.h"


void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    mwSize m, n;
    mwSignedIndex degree, fail;
    double *Cpr, *Cpi,*Zpr, *Zpi;
    
    /* check for proper number of arguments */
    if (nrhs != 1) {
        mexErrMsgTxt("POLYROOTS requires one argument.");
    }
    if (nlhs > 1) {
        mexErrMsgTxt("Too many output arguments.");
    }
    
    /* Check the input argument */
    if (!mxIsNumeric(prhs[0]) || mxIsSparse(prhs[0]) || mxIsSingle(prhs[0])) {
        mexErrMsgTxt( "Input must be a full vector in double precision." );
    }
    m = mxGetM(prhs[0]);
    n = mxGetN(prhs[0]);
    if ( m>1 && n>1 ) {
        mexErrMsgTxt( "Input must be a vector." );
    }
    
    /* Calculate degree of polynomial */
    degree = m*n-1;
        
    /* real or complex coefficients */
    if (mxIsComplex(prhs[0])) {
        Cpr = mxGetData(prhs[0]);
        Cpi = mxGetImagData(prhs[0]);
        
        /* check for leading zeros */
        while (degree) {
            if (Cpr[0]==0 && Cpi[0]==0) {
                Cpr++;
                Cpi++;
                degree--;
            }
            else {
                break;
            }
        }
        
        /* Create output vector */
        plhs[0] = mxCreateNumericMatrix(degree,1,mxDOUBLE_CLASS,mxCOMPLEX);
        Zpr = mxGetData(plhs[0]);
        Zpi = mxGetImagData(plhs[0]);
        
        /* Calculate zeros of polynomial */
        cpoly(Cpr, Cpi, &degree, Zpr, Zpi, &fail);
        if (fail) {
            mexWarnMsgTxt("CPOLY did not find degree zeros.");
            mexPrintf("Polynomial degree is reset to degree=%d.",degree);
        }
        free_cpoly();
    }
    else {
        Cpr = mxGetData(prhs[0]);
        
        /* check for leading zeros */
        while (degree) {
            if (Cpr[0]==0) {
                Cpr++;
                degree--;
            }
            else {
                break;
            }
        }         
        
        /* Create output vector */
        plhs[0] = mxCreateNumericMatrix(degree,1,mxDOUBLE_CLASS,mxCOMPLEX);
        Zpr = mxGetData(plhs[0]);
        Zpi = mxGetImagData(plhs[0]);
        
        /* Calculate zeros of polynomial */
        rpoly(Cpr, &degree, Zpr, Zpi, &fail);
        if (fail) {
            mexWarnMsgTxt("RPOLY did not find degree zeros.");
            mexPrintf("Polynomial degree is reset to degree=%d.",degree);
        }
        free_rpoly();
    }
}
