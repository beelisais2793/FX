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

#ifndef Headder_KBWFilter
#define Headder_KBWFilter

#include "KTransFunc.h"

class KBWLPFilter
{
  public:
  /************************************************************************/  
  static void Make(KTransFunc* pTF, int Order, double aDeltaT, double aHerz, int aReallocate=1)
  {
    if(aHerz<=0)
    {
      if(aReallocate) pTF->Construct(Order , Order);
      pTF->mB[0] = 1. ;
      for(int p=1;p<=pTF->N;p++) pTF->mB[p]=0.;
      for(int p=1;p<=pTF->M;p++) pTF->mA[p]=0.;
      if(aReallocate) pTF->Reset();
      return;
    }
    if(0.5 < aHerz*aDeltaT) logMsg("[Wrn] Cutoff Freq Too High.\n",0,0,0,0,0,0);
    double tW=(2./ aDeltaT) * tan( aDeltaT * PI * aHerz );
    double tG=tW * aDeltaT /2;
    if(Order==1)
    {
    //  Integral Approximation
    //  pTF->mB[0] = 1.- exp(-2. * PI * aHerz * aDeltaT )  ;
    //  pTF->mB[1] = 0 ;
    //  pTF->mA[1] =   - exp(-2. * PI * aHerz * aDeltaT ) ;
      if(aReallocate) pTF->Construct(Order , Order);
      double tW=2./ aDeltaT * tan( aDeltaT * PI * aHerz );
      double tDenomi = 1./(tW + 2./aDeltaT) ;
      pTF->mB[0] = tDenomi * tW;
      pTF->mB[1] = pTF->mB[0];
      pTF->mA[1] = tDenomi * (tW-2./aDeltaT);
      if(aReallocate) pTF->Reset();
      return;
    }
    if(Order==2)
    {
      if(aReallocate) pTF->Construct(Order , Order);
      double tDenomi = 1./(1. + sqrt(2.)*tG + tG*tG );
      pTF->mB[0] = tDenomi * tG*tG;
      pTF->mB[1] = 2.* pTF->mB[0];
      pTF->mB[2] = pTF->mB[0];
      pTF->mA[1] = tDenomi * 2.*(tG*tG-1.);
      pTF->mA[2] = tDenomi * (tG*tG - sqrt(2.)*tG +1. );
      if(aReallocate) pTF->Reset();
      return;
    }
    if(Order==3)
    {
      if(aReallocate) pTF->Construct(Order , Order);
      double tDenomi = 1./(1. + tG)/(1. + tG + tG * tG ) ;
      pTF->mB[0]=tDenomi * ( tG*tG*tG )  ;
      pTF->mB[1]= 3.* pTF->mB[0] ;
      pTF->mB[2]= 3.* pTF->mB[0] ;
      pTF->mB[3]= pTF->mB[0] ;
      pTF->mA[1]=tDenomi * ( -3. - 2.*tG + 2.*tG*tG+ 3.*tG*tG*tG ) ;
      pTF->mA[2]=tDenomi * ( 1. + tG )*(3. - 5.*tG + 3.*tG*tG )  ;
      pTF->mA[3]=tDenomi * ( -1. + 2.*tG - 2.*tG*tG + tG*tG*tG ) ;
      if(aReallocate) pTF->Reset();
      return;
    }
    if(Order==4)
    {
      if(aReallocate) pTF->Construct(Order , Order);
      double tDenomi = 1./(1. + 2.*cos(PI/8)*tG + tG*tG )/(1. + 2.*cos(3.*PI/8)*tG + tG*tG) ;
      pTF->mB[0]= tDenomi * tG*tG*tG*tG ;
      pTF->mB[1]= pTF->mB[0]*4. ;
      pTF->mB[2]= pTF->mB[0]*6. ;
      pTF->mB[3]= pTF->mB[0]*4. ;
      pTF->mB[4]= pTF->mB[0] ;
      pTF->mA[1]= tDenomi* 4.*(-1. + tG*tG)*(1. + (cos(PI/8) +cos(3.*PI/8)) *tG + tG*tG) ;
      pTF->mA[2]= tDenomi* 2.*( 3. - (2.+sqrt(2.))*tG*tG + 3. *tG*tG*tG*tG) ;
      pTF->mA[3]= tDenomi* 4.*(-1. + tG*tG)*(1. - (cos(PI/8)+cos(3*PI/8)) * tG  + tG*tG)   ;
      pTF->mA[4]= tDenomi* (1. - 2.*cos(PI/8)*tG + tG*tG) * (1. - 2.*tG*cos(3.*PI/8) + tG*tG)  ;
      if(aReallocate) pTF->Reset();
      return;
    }
     if(aReallocate) pTF->Reset();
  }
  static void QuickCopy(KTransFunc* pTF, const KTransFunc& aTF)
  {
    {for(int i=0;i<=pTF->N;i++) pTF->mB[i]=aTF.mB[i];}
    {for(int i=0;i<=pTF->M;i++) pTF->mA[i]=aTF.mA[i];}
  }
  /************************************************************************/  
};


#endif
