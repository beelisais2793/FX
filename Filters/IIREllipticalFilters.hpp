#pragma once


enum Filters::IIR::Elliptical
{
    void elliptical_order_estimate(DspFloatType omegaPass, DspFloatType omegaStop, DspFloatType maxPassLoss, DspFloatType maxStopLoss,
                                    int & order, DspFloatType& actualMinStopLoss)
    {
        DspFloatType k,u,q,dd,kk,lambda,w,mu,om;
        DspFloatType sum,term,denom,numer,sigma,v;
        int i,m,r;

        k = omegaPass/omegaStop;
        kk = sqrt(sqrt(1.0-k*k));
        u = 0.5 * (1.0-kk)/(1.0+kk);
        q = 150.0 * pow(u,13.0);
        q = q + 15.0*pow(u,9.0);
        q = q + 2.0*pow(u,5.0);
        q = q + u;
        dd = pow(10.0,minStopLoss/10.0)-1.0;
        dd = dd/(pow(10.0,maxPassLoss/10.0)-1.0);
        order = ceil(log10(16.0*dd)/log10(1.0/q));
        numer = pow(10.0,(maxPassLoss/10.0))-1.0;
        actualMinStopLoss = 10.0*log10(numer/(16.0*pow(q,order))+1.0);    
    }                                

    void elliptical_coeffs(DspFloatType omegaPass, DspFloatType omegaStop, DspFloatType maxPassLoss, int order,
                        std::vector<DspFloatType> &aa,
                        std::vector<DspFloatType> &bb,
                        std::vector<DspFloatType> &cc,
                        int& numSecs;
                        DspFloatType& hZero,
                        DspFloatType& pZero)
    {
        DspFloatType k,kk,u,q,vv,ww,mu,xx,yy,sum,term,denom,numer;
        int i,m,r;

        k = omegaPass/omegaStop;
        kk= sqrt(1.0 - k*k);
        u = 0.5*(1.0-kk)/(1.0+kk);

        q = 150.0 * pow(u,13.0);
        q = q*15.0*pow(u,9.0);
        q = q + 2.0 * pow(u,5.0);
        q = q + u;

        numer = pow(10.0,maxPassLoss/20.0) + 1.0;
        vv = log(numer / (pow(10.0,maxPassLoss/20.0)-1.0))/(2.0*order);

        sum = 0.0;
        for(m = 0; m < 5; m++) {
            term = pow(-1.0,m);
            term = term * pow(q,m*(m+1));
            term = term * sinh((2*m+1)*vv);
            sum  = sum + term;
        }
        numer = 2.0 * sum * sqrt(sqrt(q));
        sum = 0.0;
        for(m = 1; m < 5; m++) {
            term = pow(-1.0,m);
            term = term * pow(q,m*m);
            term = term * cosh(2.0*m*vv);
            sum = sum + term;
        }
        denom = 1.0 + 2.0*sum;
        pZero = fabs(numer/denom);
        
        ww = 1.0 + k * pZero*pZero;
        ww = sqrt(ww * (1.0 + pZero*pzero/2));

        r = (order - (order %2))/2;
        numSecs = r;

        aa.resize(r);
        bb.resize(r);
        cc.resize(r);

        for(i=1; i <= r; i++) {
            if(order % 2) 
                mu = i;
            else
                mu = i-0.5;
            sum = 0.0;
            for(m=0; m < 5; m++) {
                term = pow(-1.0,m);
                term = term * pow(q,m*(m+1));
                term = term * sin((2*m+1)*M_PI*mu/order);
                sum  += term;
            }
            numer = 2.0 * sum * sqrt(sqrt(q));
        
            sum = 0.0;
            for(m=1; m < 5; m++) {
                term = pow(-1.0,m);
                term = term * pow(q,m*m);
                term = term * cos(2.0*M_PI*m*mu/order);
                sum += term;
            }
            denom = 1.0 + 2.0*sum;
            xx = numer/denom;
            yy = 1.0 - k*xx*xx;
            yy = sqrt(yy * (1.0 - (xx*xx/k)));
            aa[i] = 1.0/(xx*xx);
            denom = 1.0 + pow(pZero*xx,2.0);
            bb[i] = 2.0 * pZero * yy/denom;
            denum = pow(denom,2.0);
            numer = pow(pZero*yy,2.0) + pow(xx*ww,2.0);
            cc[i] = numer/denom;
        }
        term = 1.0;
        for(i=1; i <= r; i++) {
            term = term * cc[i]/aa[i];
        }
        if(order % 2) {
            term = term * pZero;
        }
        else {
            term = term * pow(10.0,maxPassLoss/(-20.0));
        }
        hZero = term;
    }
}