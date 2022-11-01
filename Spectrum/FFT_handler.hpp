//Developed by Xiankun Xu: xiankunxu@gmail.com

#include <iostream>
#include <fstream>
#include <cmath>
#include <complex>
#include <cstdlib>
#include <typeinfo>
#include <vector>
#include <stdexcept>
using namespace std;
//This is a handler to implement the FFT and IFFT algorithms on DFT and IDFT
//Definition of DFT: X(k)=sum_{n=0}^{N-1} {x(n)*W^{nk}}  with W=exp(-i 2pi/N)
//Definition of IDFT: x(n)=(1/N)sum_{k=0}^{N-1} {X(k)*W^{-nk}}  with W=exp(-i 2pi/N)

#ifndef DFT_H
#define DFT_H
class DFT
{
private:
	const double pi;
public:
	DFT():pi(atan(1)*4){}
	template <typename iter_data_type, typename iter_complexdouble_type>
	void doDFT(iter_data_type x_beg, std::size_t N_data, iter_complexdouble_type X_beg);
		//DFT operation from x -> X
		//x_beg and X_beg are iterators (pointer, iterator of STL vector or deque) point to two contiguous containers each with N data
		//take DFT on data range denoted by [x_beg, x_beg+N_data).
		//x_beg can point to data type as int, double, and complex<double>
		//The output data will be written to a range in [X_beg, X_beg+N_data)
		//X_beg must point to complex<double> type (since the DFT output about int, double, complex<double> are all in type of complex<double>)
		//Must make sure the X_beg points to a continuous data set which contains at least N_data complex<double>

	template <typename iter_data_type, typename iter_complexdouble_type>
	void doIDFT(iter_data_type X_beg, size_t N_data, iter_complexdouble_type x_beg);
		//DFT operation from X -> x
		//x_beg and X_beg are iterators (pointer, iterator of STL vector or deque) point to two contiguous containers each with N data
		//take IDFT on data range denoted by [X_beg, X_beg+N_data).
		//X_beg can point to data type as int, double, and complex<double>
		//The output data will be written to a range in [x_beg, x_beg+N_data)
		//x_beg must point to complex<double> type (since the IDFT output about int, double, complex<double> are all in type of complex<double>)
		//Must make sure the x_beg points to a continuous data set which contains at least N_data complex<double>
};
template <typename iter_data_type, typename iter_complexdouble_type>
void DFT::doDFT(iter_data_type x_beg, std::size_t N, iter_complexdouble_type X_beg)
{
	complex<double>	z(0,-2*pi/N);
	complex<double> W(exp(z)),Wk(complex<double>(1,0));

	for(iter_complexdouble_type it_X=X_beg; it_X!=X_beg+N; ++it_X)
	{
		complex<double>	Wkl(complex<double>(1,0));
		*it_X=0;

		for(iter_data_type it_x=x_beg; it_x!=x_beg+N; ++it_x)
		{
			*it_X+=(*it_x)*Wkl;
			Wkl*=Wk;
		}
		Wk*=W;
	}
}

template <typename iter_data_type, typename iter_complexdouble_type>
void DFT::doIDFT(iter_data_type X_beg, size_t N, iter_complexdouble_type x_beg)
{
	complex<double>	z(0,2*pi/N);
	complex<double> W(exp(z)),Wk(complex<double>(1,0));
	const double onedN(1.0/N);

	for(iter_complexdouble_type it_x=x_beg; it_x!=x_beg+N; ++it_x)
	{
		complex<double>	Wkl(complex<double>(1,0));
		*it_x=0;

		for(iter_data_type it_X=X_beg; it_X!=X_beg+N; ++it_X)
		{
			*it_x+=(*it_X)*Wkl;
			Wkl*=Wk;
		}
		(*it_x)*=onedN;
		Wk*=W;
	}
}
#endif //DFT_H


#ifndef Radix2_DIF_H
#define Radix2_DIF_H
class Radix2_DIF
{
private:
	int N,L;
	const double pi;
	vector<int> index;
	//After O(Nlog2N) operations, get a reordered X_reordered series.
	//index[i] represents DFT(x)[ index[i] ] = X_reordered[i]

	vector<complex<double> > x1,x2;
	vector<complex<double> > *pcurrent,*plast;
	void calindex();
public:
	Radix2_DIF();
	~Radix2_DIF();

	template <typename iter_data_type, typename iter_complexdouble_type>
	void doFFT(iter_data_type x_beg, std::size_t N_data, iter_complexdouble_type X_beg);
		//FFT operation from x -> X
		//x_beg and X_beg are iterators (pointer, iterator of STL vector or deque) point to two contiguous containers each with N data
		//take FFT on data range denoted by [x_beg, x_beg+N_data).
		//x_beg can point to data type as int, double, and complex<double>
		//The output data will be written to a range in [X_beg, X_beg+N_data)
		//X_beg must point to complex<double> type (since the DFT output about int, double, complex<double> are all in type of complex<double>)
		//Must make sure the X_beg points to a continuous data set which contains at least N_data complex<double>

	template <typename iter_data_type, typename iter_complexdouble_type>
	void doIFFT(iter_data_type X_beg, size_t N_data, iter_complexdouble_type x_beg);
		//IFFT operation from X -> x
		//x_beg and X_beg are iterators (pointer, iterator of STL vector or deque) point to two contiguous containers each with N data
		//take IFFT on data range denoted by [X_beg, X_beg+N_data).
		//X_beg can point to data type as int, double, and complex<double>
		//The output data will be written to a range in [x_beg, x_beg+N_data)
		//x_beg must point to complex<double> type (since the IDFT output about int, double, complex<double> are all in type of complex<double>)
		//Must make sure the x_beg points to a continuous data set which contains at least N_data complex<double>
};

Radix2_DIF::Radix2_DIF():
pi(atan(1)*4),pcurrent(0),plast(0) {}

Radix2_DIF::~Radix2_DIF()
{
	pcurrent=0;
	plast=0;
}

void Radix2_DIF::calindex()
{
	for(vector<int>::iterator it=index.begin(); it!=index.end(); ++it) *it=0;	// reassign index to all 0
	int Nsetl(1),Nsetlm1(2);
	for(int l=L;l>0;--l)
	{
		for(int n=0;n<N;++n)
		{
			if(n%Nsetlm1<Nsetl)	index[n]*=2;
			else index[n]=index[n]*2+1;
		}
		Nsetl=Nsetlm1;
		Nsetlm1*=2;
	}
}

template <typename iter_data_type, typename iter_complexdouble_type>
void Radix2_DIF::doFFT(iter_data_type x_beg, size_t N_data, iter_complexdouble_type X_beg)
{
	N=N_data;	//number of data
	L=int(log(N)/log(2)+0.5);	//number of levels, equals to log2(N) if N is in power of 2
	double temp(log(N)/log(2));
	if(abs(temp-L)>1e-6)
	{
		throw runtime_error("The input data size is not an integer power of 2");
	}

	if(N!=index.size())
	{
		index.resize(N);
		calindex();
        x1.resize(N);
        x2.resize(N);
	}

	for(int n=0; n<N; ++n,++x_beg) x1[n]=*x_beg;
		//copy input data into x1, after copying, x_beg points to the end of the input data range.
		//x1[n] is complex double, *x_beg should be in types of int, double, or complex double.

    plast=&x1;
    pcurrent=&x2;

	int Nset(N/2),Nsetlm1(N);

	for(int l=1;l<=L;++l)
	{
		complex<double>	z(0,-2*pi/Nsetlm1);
		complex<double> Wl(exp(z));
		int ig(0),ih(Nset);
		while(ig<N)
		{
			complex<double> Wlj(complex<double>(1,0));
			for(int j=0;j<Nset;++j)
			{
				complex<double>& rig((*plast)[ig]);		//Doing this will reduce the # of finding address, increase the speed.
				complex<double>& rih((*plast)[ih]);
				(*pcurrent)[ig]=rig+rih;
				(*pcurrent)[ih]=(rig-rih)*Wlj;
				Wlj*=Wl;
				++ig;
				++ih;
			}
			ig+=Nset;
			ih+=Nset;
		}
		vector<complex<double> > *ptemp(pcurrent);
		pcurrent=plast;
		plast=ptemp;
		Nsetlm1/=2;
		Nset/=2;
	}

	for(int n=0; n<N; ++n)
		*(X_beg+index[n])=(*plast)[n];
}

template <typename iter_data_type, typename iter_complexdouble_type>
void Radix2_DIF::doIFFT(iter_data_type X_beg, size_t N_data, iter_complexdouble_type x_beg)
{
	N=N_data;	//number of data
	L=int(log(N)/log(2)+0.5);	//number of levels, equals to log2(N) if N is in power of 2
	double temp(log(N)/log(2));
	if(abs(temp-L)>1e-6)
	{
		throw runtime_error("The input data size is not an integer power of 2");
	}

	if(N!=index.size())
	{
		index.resize(N);
		calindex();
        x1.resize(N);
        x2.resize(N);
	}

	for(int n=0; n<N; ++n,++X_beg) x1[n]=*X_beg;
		//copy input data into x1, after copying, X_beg points to the end of the input data range.
		//x1[n] is complex double, *X_beg should be in types of int, double, or complex double.

    plast=&x1;
    pcurrent=&x2;

	complex<double>	z(0,2*pi/N);
	complex<double> Wl(exp(z));
	int Nset(N/2),Nsetlm1(N);

	for(int l=1;l<=L;++l)
	{
		int ig(0),ih(Nset);
		while(ig<N)
		{
			complex<double> Wlj(complex<double>(1,0));
			for(int j=0;j<Nset;++j)
			{
				complex<double>& rig((*plast)[ig]);		//Doing this will reduce the # of finding address, increase the speed.
				complex<double>& rih((*plast)[ih]);
				(*pcurrent)[ig]=rig+rih;
				(*pcurrent)[ih]=(rig-rih)*Wlj;
				Wlj*=Wl;
				++ig;
				++ih;
			}
			ig+=Nset;
			ih+=Nset;
		}
		Wl*=Wl;
		vector<complex<double> > *ptemp(pcurrent);
		pcurrent=plast;
		plast=ptemp;
		Nsetlm1/=2;
		Nset/=2;
	}

	double dtemp(1.0/N);
	for(int n=0; n<N; ++n)
		*(x_beg+index[n])=(*plast)[n]*dtemp;
}
#endif // Radix2_DIF_H


#ifndef Radix2_DIT_H
#define	Radix2_DIT_H
class Radix2_DIT
{
private:
	int N,L;
	const double pi;
	vector<int> index;
	//Before doing FFT, need to reorder x to x_reordered,
	//index[i] represents x_reordered[i] = x [ index[i] ]

	vector<complex<double> > x1,x2;
	vector<complex<double> > *pcurrent,*plast;
	void calindex();
public:
	Radix2_DIT();
	~Radix2_DIT();

	template <typename iter_data_type, typename iter_complexdouble_type>
	void doFFT(iter_data_type x_beg, std::size_t N_data, iter_complexdouble_type X_beg);
		//FFT operation from x -> X
		//x_beg and X_beg are iterators (pointer, iterator of STL vector or deque) point to two contiguous containers each with N data
		//take FFT on data range denoted by [x_beg, x_beg+N_data).
		//x_beg can point to data type as int, double, and complex<double>
		//The output data will be written to a range in [X_beg, X_beg+N_data)
		//X_beg must point to complex<double> type (since the DFT output about int, double, complex<double> are all in type of complex<double>)
		//Must make sure the X_beg points to a continuous data set which contains at least N_data complex<double>

	template <typename iter_data_type, typename iter_complexdouble_type>
	void doIFFT(iter_data_type X_beg, size_t N_data, iter_complexdouble_type x_beg);
		//IFFT operation from X -> x
		//x_beg and X_beg are iterators (pointer, iterator of STL vector or deque) (pointer, iterator of STL vector or deque) point to two contiguous containers each with N data
		//take IFFT on data range denoted by [X_beg, X_beg+N_data).
		//X_beg can point to data type as int, double, and complex<double>
		//The output data will be written to a range in [x_beg, x_beg+N_data)
		//x_beg must point to complex<double> type (since the IDFT output about int, double, complex<double> are all in type of complex<double>)
		//Must make sure the x_beg points to a continuous data set which contains at least N_data complex<double>
};

Radix2_DIT::Radix2_DIT():
pi(atan(1)*4),pcurrent(0),plast(0) {}

Radix2_DIT::~Radix2_DIT()
{
	pcurrent=0;
	plast=0;
}

void Radix2_DIT::calindex()
{
	vector<int> index_new(N), index_old(N);
	for(size_t i=0; i<index_old.size(); ++i) index_old[i]=i;
	vector<int> *pold(&index_old), *pnew(&index_new), *ptemp(0);

	int Nsetl(N/2),Nsetlm1(N);
	for(int l=1;l<=L;++l)
	{
		int ibegin(0);
		while(ibegin<N)
		{
			for(int j=0;j<Nsetl;++j)
			{
				(*pnew)[ibegin+j]=(*pold)[ibegin+j*2];
				(*pnew)[ibegin+j+Nsetl]=(*pold)[ibegin+j*2+1];
			}
			ibegin+=Nsetlm1;
		}

		Nsetlm1=Nsetl;
		Nsetl/=2;
		ptemp=pnew;
		pnew=pold;
		pold=ptemp;
	}
	index=(*pold);
}

template <typename iter_data_type, typename iter_complexdouble_type>
void Radix2_DIT::doFFT(iter_data_type x_beg, size_t N_data, iter_complexdouble_type X_beg)
{
	N=N_data;	//number of data
	L=int(log(N)/log(2)+0.5);	//number of levels, equals to log2(N) if N is in power of 2
	double temp(log(N)/log(2));
	if(abs(temp-L)>1e-6)
	{
		throw runtime_error("The input data size is not an integer power of 2");
	}

	if(N!=index.size())
	{
		index.resize(N);
		calindex();
        x1.resize(N);
        x2.resize(N);
	}

	for(int i=0; i<N; ++i) x1[i]= *(x_beg+index[i]);
		//x1[n] is complex double, *x_beg should be in types of int, double, or complex double.
    plast=&x1;
    pcurrent=&x2;

	int Nsetl(2),Nsetlp1(1);
	for(int l=L-1;l>=0;--l)
	{
		complex<double>	z(0,-2*pi/Nsetl);
		complex<double> Wl(exp(z));
		int ig(0),ih(Nsetlp1);
		while(ig<N)
		{
			complex<double> Wlk(1,0);
			for(int k=0;k<Nsetlp1;++k)
			{
				complex<double> ctemp((*plast)[ih]*Wlk);
				complex<double>& rig((*plast)[ig]);		//Doing this will reduce the # of finding address, increase the speed.
				(*pcurrent)[ig]=rig+ctemp;
				(*pcurrent)[ih]=rig-ctemp;
				Wlk*=Wl;
				++ig;
				++ih;
			}
			ig+=Nsetlp1;
			ih+=Nsetlp1;
		}
		vector<complex<double> > *ptemp(pcurrent);
		pcurrent=plast;
		plast=ptemp;
		Nsetlp1=Nsetl;
		Nsetl*=2;
	}

	for(int i=0; i<N; ++i) *(X_beg+i) = (*plast)[i];
}


template <typename iter_data_type, typename iter_complexdouble_type>
void Radix2_DIT::doIFFT(iter_data_type X_beg, size_t N_data, iter_complexdouble_type x_beg)
{
	N=N_data;	//number of data
	L=int(log(N)/log(2)+0.5);	//number of levels, equals to log2(N) if N is in power of 2
	double temp(log(N)/log(2));
	if(abs(temp-L)>1e-6)
	{
		throw runtime_error("The input data size is not an integer power of 2");
	}

	if(N!=index.size())
	{
		index.resize(N);
		calindex();
        x1.resize(N);
        x2.resize(N);
	}

	for(int i=0; i<N; ++i) x1[i]= *(X_beg+index[i]);
		//x1[n] is complex double, *x_beg should be in types of int, double, or complex double.
    plast=&x1;
    pcurrent=&x2;

	int Nsetl(2),Nsetlp1(1);
	for(int l=L-1;l>=0;--l)
	{
		complex<double>	z(0,2*pi/Nsetl);
		complex<double> Wl(exp(z));
		int ig(0),ih(Nsetlp1);
		while(ig<N)
		{
			complex<double> Wlk(1,0);
			for(int k=0;k<Nsetlp1;++k)
			{
				complex<double> ctemp((*plast)[ih]*Wlk);
				complex<double>& rig((*plast)[ig]);		//Doing this will reduce the # of finding address, increase the speed.
				(*pcurrent)[ig]=rig+ctemp;
				(*pcurrent)[ih]=rig-ctemp;
				Wlk*=Wl;
				++ig;
				++ih;
			}
			ig+=Nsetlp1;
			ih+=Nsetlp1;
		}
		vector<complex<double> > *ptemp(pcurrent);
		pcurrent=plast;
		plast=ptemp;
		Nsetlp1=Nsetl;
		Nsetl*=2;
	}

	double dtemp(1.0/N);
	for(int i=0; i<N; ++i) *(x_beg+i) = (*plast)[i]*dtemp;
}
#endif // Radix2_DIT_H


#ifndef Radix3_DIF_H
#define Radix3_DIF_H
class Radix3_DIF
{
private:
	int N,L;
	const double pi;
	vector<int> index;
	//After O(Nlog2N) operations, get a reordered X_reordered series.
	//index[i] represents DFT(x)[ index[i] ] = X_reordered[i]

	vector<complex<double> > x1,x2;
	vector<complex<double> > *pcurrent,*plast;
	void calindex();
public:
	Radix3_DIF();
	~Radix3_DIF();

	template <typename iter_data_type, typename iter_complexdouble_type>
	void doFFT(iter_data_type x_beg, std::size_t N_data, iter_complexdouble_type X_beg);
		//FFT operation from x -> X
		//x_beg and X_beg are iterators (pointer, iterator of STL vector or deque) point to two contiguous containers each with N data
		//take FFT on data range denoted by [x_beg, x_beg+N_data).
		//x_beg can point to data type as int, double, and complex<double>
		//The output data will be written to a range in [X_beg, X_beg+N_data)
		//X_beg must point to complex<double> type (since the DFT output about int, double, complex<double> are all in type of complex<double>)
		//Must make sure the X_beg points to a continuous data set which contains at least N_data complex<double>

	template <typename iter_data_type, typename iter_complexdouble_type>
	void doIFFT(iter_data_type X_beg, size_t N_data, iter_complexdouble_type x_beg);
		//IFFT operation from X -> x
		//x_beg and X_beg are iterators (pointer, iterator of STL vector or deque) point to two contiguous containers each with N data
		//take IFFT on data range denoted by [X_beg, X_beg+N_data).
		//X_beg can point to data type as int, double, and complex<double>
		//The output data will be written to a range in [x_beg, x_beg+N_data)
		//x_beg must point to complex<double> type (since the IDFT output about int, double, complex<double> are all in type of complex<double>)
		//Must make sure the x_beg points to a continuous data set which contains at least N_data complex<double>
};

Radix3_DIF::Radix3_DIF():
pi(atan(1)*4),pcurrent(0),plast(0) {}

Radix3_DIF::~Radix3_DIF()
{
	pcurrent=0;
	plast=0;
}

void Radix3_DIF::calindex()
{
	for(vector<int>::iterator it=index.begin(); it!=index.end(); ++it) *it=0;	// reassign index to all 0
	int Nsetl(1),Nsetlm1(3);
	for(int l=L;l>0;--l)
	{
		for(int n=0;n<N;++n)
		{
			int residue(n%Nsetlm1);
			if(residue<Nsetl)	index[n]*=3;
			else if(residue<Nsetl*2)	index[n]=index[n]*3+1;
			else index[n]=index[n]*3+2;
		}
		Nsetl=Nsetlm1;
		Nsetlm1*=3;
	}
}

template <typename iter_data_type, typename iter_complexdouble_type>
void Radix3_DIF::doFFT(iter_data_type x_beg, size_t N_data, iter_complexdouble_type X_beg)
{
	N=N_data;	//number of data
	L=int(log(N)/log(3)+0.5);	//number of levels, equals to log2(N) if N is in power of 2
	double temp(log(N)/log(3));
	if(abs(temp-L)>1e-6)
	{
		throw runtime_error("The input data size is not an integer power of 3");
	}

	if(N!=index.size())
	{
		index.resize(N);
		calindex();
        x1.resize(N);
        x2.resize(N);
	}

	for(int n=0; n<N; ++n,++x_beg) x1[n]=*x_beg;
		//copy input data into x1, after copying, x_beg points to the end of the input data range.
		//x1[n] is complex double, *x_beg should be in types of int, double, or complex double.

    plast=&x1;
    pcurrent=&x2;

	complex<double>	coeff1(exp(complex<double>(0,-2*pi/3)));
	complex<double>	coeff2(exp(complex<double>(0,-4*pi/3)));
	complex<double>	z(0,-2*pi/N);
	complex<double> Wl(exp(z));
	complex<double> Wl2(exp(z*2.0));
	int Nset(N/3),Nsetlm1(N);

	for(int l=1;l<=L;++l)
	{
		int ia(0),ib(Nset),ic(Nset+Nset);
		while(ia<N)
		{
			complex<double> Wln(complex<double>(1,0));
			complex<double> Wln2(complex<double>(1,0));
			for(int n=0;n<Nset;++n)
			{
				complex<double>& ria((*plast)[ia]);		//Doing this will reduce the # of finding address, increase the speed.
				complex<double>& rib((*plast)[ib]);
				complex<double>& ric((*plast)[ic]);

				(*pcurrent)[ia]=ria+rib+ric;
				(*pcurrent)[ib]=(ria+rib*coeff1+ric*coeff2)*Wln;
				(*pcurrent)[ic]=(ria+rib*coeff2+ric*coeff1)*Wln2;
				Wln*=Wl;
				Wln2*=Wl2;
				++ia;
				++ib;
				++ic;
			}
			double itemp(Nset*2);
			ia+=itemp;
			ib+=itemp;
			ic+=itemp;
		}
		Wl=pow(Wl,3);
		Wl2=pow(Wl2,3);
		vector<complex<double> > *ptemp(pcurrent);
		pcurrent=plast;
		plast=ptemp;
		Nsetlm1/=3;
		Nset/=3;
	}

	for(int n=0; n<N; ++n)
		*(X_beg+index[n])=(*plast)[n];
}

template <typename iter_data_type, typename iter_complexdouble_type>
void Radix3_DIF::doIFFT(iter_data_type X_beg, size_t N_data, iter_complexdouble_type x_beg)
{
	N=N_data;	//number of data
	L=int(log(N)/log(3)+0.5);	//number of levels, equals to log2(N) if N is in power of 2
	double temp(log(N)/log(3));
	if(abs(temp-L)>1e-6)
	{
		throw runtime_error("The input data size is not an integer power of 3");
	}

	if(N!=index.size())
	{
		index.resize(N);
		calindex();
        x1.resize(N);
        x2.resize(N);
	}

	for(int n=0; n<N; ++n,++X_beg) x1[n]=*X_beg;
		//copy input data into x1, after copying, X_beg points to the end of the input data range.
		//x1[n] is complex double, *X_beg should be in types of int, double, or complex double.

    plast=&x1;
    pcurrent=&x2;

	complex<double>	coeff1(exp(complex<double>(0,2*pi/3)));
	complex<double>	coeff2(exp(complex<double>(0,4*pi/3)));
	complex<double>	z(0,2*pi/N);
	complex<double> Wl(exp(z));
	complex<double> Wl2(exp(z*2.0));
	int Nset(N/3),Nsetlm1(N);

	for(int l=1;l<=L;++l)
	{
		int ia(0),ib(Nset),ic(Nset+Nset);
		while(ia<N)
		{
			complex<double> Wln(complex<double>(1,0));
			complex<double> Wln2(complex<double>(1,0));
			for(int n=0;n<Nset;++n)
			{
				complex<double>& ria((*plast)[ia]);		//Doing this will reduce the # of finding address, increase the speed.
				complex<double>& rib((*plast)[ib]);
				complex<double>& ric((*plast)[ic]);

				(*pcurrent)[ia]=ria+rib+ric;
				(*pcurrent)[ib]=(ria+rib*coeff1+ric*coeff2)*Wln;
				(*pcurrent)[ic]=(ria+rib*coeff2+ric*coeff1)*Wln2;
				Wln*=Wl;
				Wln2*=Wl2;
				++ia;
				++ib;
				++ic;
			}
			double itemp(Nset*2);
			ia+=itemp;
			ib+=itemp;
			ic+=itemp;
		}
		Wl=pow(Wl,3);
		Wl2=pow(Wl2,3);
		vector<complex<double> > *ptemp(pcurrent);
		pcurrent=plast;
		plast=ptemp;
		Nsetlm1/=3;
		Nset/=3;
	}

	double dtemp(1.0/N);
	for(int n=0; n<N; ++n)
		*(x_beg+index[n])=(*plast)[n]*dtemp;
}
#endif // Radix3_DIF_H


#ifndef FFT_handler_H
#define FFT_handler_H
class FFT_handler
{
private:
	DFT		dft_object;
	Radix2_DIF	r2DIF_object;
	Radix2_DIT	r2DIT_object;
	Radix3_DIF r3DIF_object;
public:
	template <typename iter_data_type, typename iter_complexdouble_type>
	void dft(iter_data_type x_beg, std::size_t N, iter_complexdouble_type X_beg)
	{
		dft_object.doDFT(x_beg, N, X_beg);
	}

	template <typename iter_data_type, typename iter_complexdouble_type>
	void idft(iter_data_type X_beg, size_t N, iter_complexdouble_type x_beg)
	{
		dft_object.doIDFT(X_beg, N, x_beg);
	}

	template <typename iter_data_type, typename iter_complexdouble_type>
	void fft_radix2_dif(iter_data_type x_beg, size_t N_data, iter_complexdouble_type X_beg)
	{
		r2DIF_object.doFFT(x_beg, N_data, X_beg);
	}

	template <typename iter_data_type, typename iter_complexdouble_type>
	void ifft_radix2_dif(iter_data_type X_beg, size_t N_data, iter_complexdouble_type x_beg)
	{
		r2DIF_object.doIFFT(X_beg, N_data, x_beg);
	}

	template <typename iter_data_type, typename iter_complexdouble_type>
	void fft_radix2_dit(iter_data_type x_beg, size_t N_data, iter_complexdouble_type X_beg)
	{
		r2DIT_object.doFFT(x_beg, N_data, X_beg);
	}

	template <typename iter_data_type, typename iter_complexdouble_type>
	void ifft_radix2_dit(iter_data_type X_beg, size_t N_data, iter_complexdouble_type x_beg)
	{
		r2DIT_object.doIFFT(X_beg, N_data, x_beg);
	}

	template <typename iter_data_type, typename iter_complexdouble_type>
	void fft_radix3_dif(iter_data_type x_beg, size_t N_data, iter_complexdouble_type X_beg)
	{
		r3DIF_object.doFFT(x_beg, N_data, X_beg);
	}

	template <typename iter_data_type, typename iter_complexdouble_type>
	void ifft_radix3_dif(iter_data_type X_beg, size_t N_data, iter_complexdouble_type x_beg)
	{
		r3DIF_object.doIFFT(X_beg, N_data, x_beg);
	}

	template <typename T>
	void convolution(const vector<T>& x, const vector<T>& y, vector<T>& xyConvo);
	//calculate xyConvo(n) = sum_{m=0}^{M-1} x(m)y(n-m), with k=0, 1, ..., N-1.
	//y has size M+N-1, its actual subscript range is 0, ... M+N-2.
	//However, we should view that y[j] has subscript range j =-M+1, ..., -1, 0, 1, ... N-1.
	//y_beg is actually an iterator points to y[-M+1]

	template <typename retn_type, typename iter_complexdouble_type>
	void numerical_fourier_transform(retn_type(*func)(double), double tl, double tr, size_t Nt, double fl, double fr, iter_complexdouble_type H_beg, size_t Nf);
};

template <typename T>
void FFT_handler::convolution(const vector<T>& x, const vector<T>& y, vector<T>& xyConvo)
{
	int M(x.size());
	int Mnextpow2(pow(2,ceil(log2(M))));
	int N(xyConvo.size());
	if(y.size()!=M+N-1)
	{
		throw runtime_error("In FFT convolution, the size of data sets are not agreement");
	}

	int p(0),p1(1);
	double minOper,minOper1;
	minOper=(ceil(double(N)/(pow(2,p)*Mnextpow2-M+1))*2+1)*pow(2,p)*Mnextpow2*(p+log2(Mnextpow2));

	while(true)
	{
		minOper1=(ceil(double(N)/(pow(2,p1)*Mnextpow2-M+1))*2+1)*pow(2,p1)*Mnextpow2*(p1+log2(Mnextpow2));
		if(minOper1<minOper)
		{
			p=p1;
			minOper=minOper1;
			++p1;
		}
		else
			break;
	}
	int L(pow(2,p)*Mnextpow2);		//The length of the DFT and IDFT
	int K(L-M+1);					//The length of the sections to divide the N convolution data.
	int nsec(int(N/K));				//# of sections, 0~nsec, where 0~(nsec-1) are all K-long, and the last has length N-(nsec-1)*K.

	vector<T> xextend(x.begin(),x.end()),ysec(L,0);
	vector<complex<double> > Xextend(L,0),Ysec(L,0);
	xextend.resize(L,0);	//resize xextended to size L, added members have value 0
	fft_radix2_dif(xextend.begin(), xextend.size(), Xextend.begin());

	const typename vector<T>::const_iterator it_y0(y.begin()+M-1);	//corresponding to &y[0].

	for(int isec=0;isec<nsec;++isec)
	{
		typename vector<T>::iterator it_ysec(ysec.begin());
		for(int i=isec*K;i<(isec+1)*K;++i)
		{
			*it_ysec = *(it_y0+i);
			++it_ysec;
		}
		for(int i=isec*K-M+1;i<isec*K;++i)
		{
			*it_ysec = *(it_y0+i);
			++it_ysec;
		}
		fft_radix2_dif(ysec.begin(), ysec.size(), Ysec.begin());

		vector<complex<double> >::iterator it_Ysec(Ysec.begin()), it_Xextend(Xextend.begin());
		while(it_Ysec!=Ysec.end())
		{
			(*it_Ysec)*=(*it_Xextend);
			++it_Ysec;
			++it_Xextend;
		}
		ifft_radix2_dif(Ysec.begin(),Ysec.size(),ysec.begin());

		it_ysec=ysec.begin();
		for(int i=isec*K;i<(isec+1)*K;++i)
		{
			xyConvo[i]=*it_ysec;
			++it_ysec;
		}
	}

	{
		typename vector<T>::iterator it_ysec(ysec.begin());
		for(int i=nsec*K;i<N;++i)
		{
			*it_ysec = *(it_y0+i);
			++it_ysec;
		}
		for(int i=N;i<=(nsec+1)*K-1;++i)
		{
			*it_ysec = 0;
			++it_ysec;
		}
		for(int i=nsec*K-M+1;i<nsec*K;++i)
		{
			*it_ysec = *(it_y0+i);
			++it_ysec;
		}
		fft_radix2_dif(ysec.begin(), ysec.size(), Ysec.begin());

		vector<complex<double> >::iterator it_Ysec(Ysec.begin()), it_Xextend(Xextend.begin());
		while(it_Ysec!=Ysec.end())
		{
			(*it_Ysec)*=(*it_Xextend);
			++it_Ysec;
			++it_Xextend;
		}
		ifft_radix2_dif(Ysec.begin(),Ysec.size(),ysec.begin());

		it_ysec=ysec.begin();
		for(int i=nsec*K;i<N;++i)
		{
			xyConvo[i]=*it_ysec;
			++it_ysec;
		}
	}
}


template <typename retn_type, typename iter_complexdouble_type>
void FFT_handler::numerical_fourier_transform(retn_type(*hfun)(double), double tl, double tr, size_t Nt, double fl, double fr, iter_complexdouble_type H_beg, size_t Nf)
{
	const double pi(atan(1)*4);
	//The information of h(t)
	double a(tl),b(tr);		//Have to make sure h(t)=0 in the range [a,b]
	size_t M(Nt);
	double beta((b-a)/M);
	vector<double> t(M);
	vector<retn_type> h(M);
	for(size_t n=0; n<t.size(); ++n)
	{
		t[n]=a+0.5*beta+n*beta;		//2nd accuracy
		h[n]=hfun(t[n]);
	}

	//The information of H(f)
	double c(fl),d(fr);			//The range [c,d] is the range we want to get H(f),can be set arbitrary
	size_t N(Nf);
	double gamma((d-c)/N);
	vector<double> f(N);
	for(size_t k=0; k<f.size(); ++k)
	{
		f[k]=c+k*gamma;
	}

	//The information of the convolution data sequences.
	vector<complex<double> > y(M), z(M+N-1),yzConvo(N);
	for(size_t n=0; n<y.size(); ++n)
	{
		complex<double> z1(0,-2*pi*n*beta*(c+0.5*gamma*n));
		y[n]=h[n]*exp(z1);
	}
	for(size_t n=0; n<z.size() ;++n)
	{
		int itemp(n-(M-1));
		complex<double> z1(0,pi*gamma*beta*itemp*itemp);
		z[n]=exp(z1);
	}

	convolution(y,z,yzConvo);

	a=a+0.5*beta;		//2nd accuracy
	for(size_t k=0; k<yzConvo.size(); ++k)
	{
		complex<double> z1(0,-2*pi*(a*c+a*gamma*k+0.5*gamma*beta*k*k));
		*(H_beg+k)=beta*exp(z1)*yzConvo[k];
	}
}
#endif // FFT_handler_H
