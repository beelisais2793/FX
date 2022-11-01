// Time Domain convolution
// linear/acyclic
// circular/cyclic
// periodic
// non-periodic
#pragma once

namespace DSP::Convolution
{
    // linear or acyclic
    std::vector<float> linear_convolution(float * h, float * x, int M, int N)
    {
        std::vector<float> y(M*N-1);
        
        //Compute Convolution
        for(int i=0;i<M+N-1;i++)
        {  y[i]=0;
            for(j=0;j<=i;j++)
                y[i]+=x[j]*h[i-j];
        }
        return y;
    }
    void linear_convolution(float * h, float * x, float * y, int M, int N)
    {                
        //Compute Convolution
        for(int i=0;i<M+N-1;i++)
        {  y[i]=0;
            for(j=0;j<=i;j++)
                y[i]+=x[j]*h[i-j];
        }
        return y;
    }
    // circular or cyclic
    std::vector<float> circular_convolution(float * h, float * x, int M, int N)
    {
        std::vector<float> y(M);    
        for(n=0;n < M;n++)
        { y[n]=0;
        for(k=0;k < N;k++)
            {if(k>n)
                j=n-k+l1;
            else
                j=n-k;
            y[n]=y[n]+x[k]*h[j];
            }
        }        
    }   
    void circular_convolution(float * h, float * x, float * y, int M, int N)
    {        
        for(n=0;n < M;n++)
        { y[n]=0;
        for(k=0;k < N;k++)
            {if(k>n)
                j=n-k+l1;
            else
                j=n-k;
            y[n]=y[n]+x[k]*h[j];
            }
        }     
    }   
}