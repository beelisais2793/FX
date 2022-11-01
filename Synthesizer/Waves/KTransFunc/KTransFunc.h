/* 
 This file is a part of KTransFunc.
 
 Author: Ryo Kikuuwe
 
 Copyright (c) 2000-2015 Ryo Kikuuwe

 KTransFunc is a set of functions that deals with transfer functions
 in the Z-transform domain.
  
 KTransFunc is a free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 KTransFunc is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with SiconosPlugin; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 
 Contact: Ryo Kikuuwe, kikuuwe@ieee.org
*/

#ifndef Headder_KTransFunc
#define Headder_KTransFunc

#include "math.h"

///////////////////////////////////////////////////////////////
class KTransFunc
{
 public:
    int N, M;
  public:
    double * in ;
    double * out ;
    double * mB ;
    double * mA ;
  public:
    KTransFunc() :N(-1),M(-1),in(NULL),out(NULL),mB(NULL),mA(NULL) {}
  /************************************************************************/  
    void Construct(int aN, int aM)
    {
      Destruct();
      N   = aN;
      M   = aM;
      in  = new double[N+1];
      out = new double[M+1];
      mB  = new double[N+1];
      mA  = new double[M+1];
      mA[0] = 1;
      Reset();
    }
  /************************************************************************/  
    void Destruct()
    {
      if(in !=NULL) delete [] in  ; in  = NULL;
      if(out!=NULL) delete [] out ; out = NULL;
      if(mB !=NULL) delete [] mB  ; mB  = NULL;
      if(mA !=NULL) delete [] mA  ; mA  = NULL;
      N=-1;
      M=-1;
    }
  /************************************************************************/  
    virtual ~KTransFunc()
    {
      Destruct();
    }
  /************************************************************************/  
    void Reset()
    {
      if((M==-1)||(N==-1)) return;
      {for(int p=0;p<=N;p++) in [p]=0;}
      {for(int p=0;p<=M;p++) out[p]=0;}
    };
  /************************************************************************/  
    double Work(double inValue)
    {
	if(in == NULL) perror("Transfunc Not Allocated \n");
        {for(int p=N-1 ;p>=0 ;p--) in [p+1]=in[p];}
        {for(int p=M-1 ;p>=0 ;p--) out[p+1]=out[p];}
        in [0] = inValue;
        out[0] = 0  ;
        {for(int p=N ;p>=0 ;p--) out[0] += mB[p] * in [p] ;}
        {for(int p=M ;p>=1 ;p--) out[0] -= mA[p] * out[p] ;}
	return out[0];
    }
  /************************************************************************/  
    void init_multiply(const KTransFunc& T1, const KTransFunc& T2)
    {
      Construct(T1.N + T2.N, T1.M + T2.M);
      {for(int p=0;p<=N   ;p++)mB[p]=0;}
      {for(int p=0;p<=M   ;p++)mA[p]=0;}
      {for(int i=0;i<=T1.N;i++)for(int j=0;j<=T2.N;j++)mB[i+j]+=T1.mB[i]*T2.mB[j];}
      {for(int i=0;i<=T1.M;i++)for(int j=0;j<=T2.M;j++)mA[i+j]+=T1.mA[i]*T2.mA[j];}
    }
  /************************************************************************/  
    void reus_multiply(const KTransFunc& T1)
    {
      KTransFunc tmp;
      tmp.init_multiply(*this, T1);
      this->init_copy(tmp);
    }
  /************************************************************************/  
    void reus_times(const double& k)
    {
      {for(int i=0;i<=N;i++) mB[i] *= k;}
    }
  /************************************************************************/  
    void init_inverse(const KTransFunc& T1)
    {
      Construct(T1.M , T1.N );
      if(T1.mB[0]==0)perror("[Err] KTransFunc not invertible.\n");
      {for(int p=0;p<=N;p++) mB[p]=T1.mA[p]/T1.mB[0];}
      {for(int p=0;p<=M;p++) mA[p]=T1.mB[p]/T1.mB[0];}
    }
  /************************************************************************/  
    void init_identity()
    {
      Construct(0,0);
      mB[0]= 1;
      mA[0]= 1;
    }
  /************************************************************************/  
    void init_copy(const KTransFunc& T)
    {
      Construct(T.N,T.M);
      {for(int i=0;i<=N;i++) mB[i]=T.mB[i];}
      {for(int i=0;i<=M;i++) mA[i]=T.mA[i];}
    }
  /************************************************************************/  
};
///////////////////////////////////////////////////////////////
class KDifferentiate
{
  public:
  static void Make(KTransFunc* const pTF, double aDeltaT) 
  {
    pTF->Construct(1,0);
    pTF->mB[0] =  1./aDeltaT;
    pTF->mB[1] = -1./aDeltaT;
    pTF->Reset();
  }
};
///////////////////////////////////////////////////////////////
#endif
