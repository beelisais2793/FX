
#pragma once

#include <stdio.h>
#include <math.h>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <complex>
#include <cmath>

using namespace std;


class Polynom
{
public:
    int degree;
    double* koef;

    Polynom();
    Polynom(int d, double* k);
    ~Polynom();
    void deletePoly();

    void SetPoly(int d, double* k);
    void copyPoly(Polynom& ob);
    Polynom operator/(Polynom& ob);
    Polynom operator*(Polynom& ob);
    Polynom operator-(Polynom& ob);
    Polynom operator%(Polynom& ob);
    Polynom& operator=(const Polynom& ob);
    void printPoly();
    void deleteBy_a0();
    Polynom derivative();
    double value(double x);
    complex <double> value_complex(complex <double> x);
    void removeMultipleRoots(void);
};


class ComplexPolynom
{
public:
    int degree;
    complex <double>* koef;

    ComplexPolynom();
    ComplexPolynom(int d, complex <double>* k);
    ~ComplexPolynom();
    void deletePoly();

    void SetPoly(int d, complex <double>* k);
    void copyPoly(ComplexPolynom& ob);
    ComplexPolynom getcopyPoly();
    ComplexPolynom operator/(ComplexPolynom& ob);
    ComplexPolynom operator*(ComplexPolynom& ob);
    ComplexPolynom operator*(complex <double> c);
    ComplexPolynom operator-(ComplexPolynom& ob);
    ComplexPolynom operator+(ComplexPolynom& ob);
    ComplexPolynom operator%(ComplexPolynom& ob);
    ComplexPolynom& operator=(const ComplexPolynom& ob);
    void printPoly();
    void deleteBy_a0();
    ComplexPolynom derivative();
    complex <double> value_complex(complex <double> x);
    void removeMultipleRoots(void);
    ComplexPolynom TransformToCauchyPoly();
    complex <double> SolveNewton(ComplexPolynom cp, complex <double> left, complex <double> right);
    complex <double> get_s();
    ComplexPolynom NomrPoly();
    complex <double> findOneRoot_Newton_CP(ComplexPolynom poly, complex <double> left, complex <double> right);
};




#define NOROOT 11111111111
#define EPSILON 0.00000001

class RootSeeker
{
private:
    double* roots;
    complex <double>* complex_roots;
    Polynom p;
    ComplexPolynom cp;
    double a, b;
    complex <double> a_c, b_c;
    int cr;
    bool correctPoly;

public:
    RootSeeker() {};

    void SetPoly(int d, double* coef);
    void SetPoly(int d, complex <double>* coef);
    Polynom SetPoly_(int d, double* coef);
    ComplexPolynom SetPoly_(int d, complex <double>* coefs);
    void SetPoly(Polynom poly);
    void printPoly();
    void printCPoly();
    void SetInterval(double a, double b);
    void countOfRoots();
    double* getRoots();
    void printRoots();

    static double findOneRoot_FalsePosition(Polynom& poly, double left, double right);
    static double findOneRoot_Newton(Polynom& poly, double left, double right);
    static double findOneRoot_Secant(Polynom& poly, double x0, double x1);
    static double findOneRoot_Muller(Polynom& poly, double left, double right);
    static double findOneRoot_Halley(Polynom& poly, double left, double right);

    static complex <double> findOneRoot_Halley_Complex(Polynom& poly, complex <double> left, complex <double> right);
    static complex <double> findOneRoot_Halley_Complex(ComplexPolynom& poly, complex <double> left, complex <double> right);

    static complex <double> findOneRoot_Newton_Complex(Polynom& poly, complex <double> left, complex <double> right);
    static complex <double> findOneRoot_Newton_Complex(ComplexPolynom& poly, complex <double> left, complex <double> right);

    static complex <double> findOneRoot_Secant_Complex(Polynom& poly, complex <double> x0, complex <double> x1);
    static complex <double> findOneRoot_Secant_Complex(ComplexPolynom& poly, complex <double> x0, complex <double> x1);

    static complex <double> findOneRoot_FalsePosition_Complex(Polynom& poly, complex <double> left, complex <double> right);
    static complex <double> findOneRoot_FalsePosition_Complex(ComplexPolynom& poly, complex <double> left, complex <double> right);

    static complex <double> findOneRoot_Muller_Complex(Polynom& poly, complex <double> left, complex <double> right);
    static complex <double> findOneRoot_Muller_Complex(ComplexPolynom& poly, complex <double> left, complex <double> right);

    static complex <double>* DurandKerner(Polynom& poly, int deg);
    static complex <double>* DurandKerner(ComplexPolynom& poly, int deg);

    complex <double> findOneRootJenkinsTraub(ComplexPolynom c_poly);

    void findRoots(double (*methodName)(Polynom&, double, double));
};

Polynom::Polynom()
{
    degree = 0;
    koef = new double[degree + 1];
    koef[0] = 0;
}

Polynom::Polynom(int d, double* k)
{
    degree = d;
    koef = new double[degree + 1];
    for (int i = 0; i <= degree; i++)
    {
        koef[i] = k[i];
    }
}

Polynom::~Polynom()
{
    degree = 0;
    //delete[] koef;
}

void Polynom::deletePoly()
{
    degree = 0;
    delete[] koef;
}

void Polynom::deleteBy_a0()
{
    for (int i = 0; i <= degree; i++)
    {
        koef[i] /= koef[degree];
    }
}

Polynom Polynom::derivative()
{
    Polynom p;
    int d = degree - 1;
    double* coef;
    coef = new double[d + 1];
    for (int i = 0; i <= d; i++)
    {
        coef[i] = koef[i + 1] * (i + 1);
    }
    p.SetPoly(d, coef);
    return p;
}

void Polynom::SetPoly(int d, double* k)
{
    degree = d;
    koef = new double[degree + 1];
    for (int i = 0; i <= degree; i++)
    {
        koef[i] = k[i];
    }
}

Polynom& Polynom::operator=(const Polynom& ob)
{
    degree = ob.degree;
    koef = new double[degree + 1];
    for (int i = 0; i <= degree; i++)
    {
        koef[i] = ob.koef[i];
    }

    return *this;
}

void Polynom::copyPoly(Polynom& ob)
{
    ob.degree = degree;
    ob.koef = new double[ob.degree + 1];
    for (int i = 0; i <= ob.degree; i++)
    {
        ob.koef[i] = koef[i];
    }
}

void Polynom::printPoly()
{
    cout << koef[degree] << "x^" << degree;
    for (int i = degree - 1; i >= 0; i--)
    {
        cout << " + " << koef[i] << "x^" << i;
    }
    cout << "\n";
}

Polynom Polynom::operator /(Polynom& ob)
{

    bool inAlgoritm = true;

    Polynom temp;
    Polynom ob_1;
    Polynom ob_2;
    Polynom ob_4;

    temp.degree = degree - ob.degree;
    temp.koef = new double[temp.degree + 1];
    memset(temp.koef, 0, (temp.degree + 1) * sizeof(double));

    ob_1.degree = degree;
    ob_1.koef = new double[degree + 1];
    for (int i = degree; i >= 0; i--)
    {
        ob_1.koef[i] = koef[i];
    }

    ob_2.degree = ob.degree;
    ob_2.koef = new double[ob.degree + 1];
    for (int i = ob.degree; i >= 0; i--)
    {
        ob_2.koef[i] = ob.koef[i];
    }

    ob_4.degree = ob_1.degree;
    ob_4.koef = new double[ob_1.degree + 1];

    double mnojnik;
    int k = 0;
    int i, j;
    while (inAlgoritm)
    {
        for (int i = ob.degree; i >= 0; i--)
        {
            ob_4.koef[i] = ob.koef[i];
        }

        if (ob_2.degree < ob_1.degree)
        {
            for (i = ob_1.degree, j = ob_2.degree; i >= 0; i--, j--)
            {
                if (j < 0)
                {
                    ob_4.koef[i] = 0;
                }
                else
                {
                    ob_4.koef[i] = ob_2.koef[j];
                }
            }
        }

        mnojnik = ob_1.koef[ob_1.degree] / ob_4.koef[ob_1.degree];

        temp.koef[temp.degree - k] = mnojnik;
        k++;

        for (int i = 0; i <= ob_1.degree; i++)
        {
            ob_4.koef[i] *= mnojnik;
        }

        for (int i = 0; i <= ob_1.degree; i++)
        {
            ob_1.koef[i] -= ob_4.koef[i];
        }

        ob_1.degree--;
        if (ob_2.degree > ob_1.degree)
        {
            inAlgoritm = false;
        }
    }

    return temp;
}

Polynom Polynom::operator %(Polynom& ob)
{

    bool inAlgoritm = true;

    Polynom temp;
    Polynom ob_1;
    Polynom ob_2;
    Polynom ob_4;

    temp.degree = degree - ob.degree;
    temp.koef = new double[temp.degree + 1];
    memset(temp.koef, 0, (temp.degree + 1) * sizeof(double));

    ob_1.degree = degree;
    ob_1.koef = new double[degree + 1];
    for (int i = degree; i >= 0; i--)
    {
        ob_1.koef[i] = koef[i];
    }

    ob_2.degree = ob.degree;
    ob_2.koef = new double[ob.degree + 1];
    for (int i = ob.degree; i >= 0; i--)
    {
        ob_2.koef[i] = ob.koef[i];
    }

    ob_4.degree = ob_1.degree;
    ob_4.koef = new double[ob_1.degree + 1];

    double mnojnik;
    int k = 0;
    int i, j;
    while (inAlgoritm)
    {
        for (int i = ob.degree; i >= 0; i--)
        {
            ob_4.koef[i] = ob.koef[i];
        }

        if (ob_2.degree < ob_1.degree)
        {
            for (i = ob_1.degree, j = ob_2.degree; i >= 0; i--, j--)
            {
                if (j < 0)
                {
                    ob_4.koef[i] = 0;
                }
                else
                {
                    ob_4.koef[i] = ob_2.koef[j];
                }
            }
        }

        mnojnik = ob_1.koef[ob_1.degree] / ob_4.koef[ob_1.degree];

        temp.koef[temp.degree - k] = mnojnik;
        k++;

        for (int i = 0; i <= ob_1.degree; i++)
        {
            ob_4.koef[i] *= mnojnik;
        }

        for (int i = 0; i <= ob_1.degree; i++)
        {
            ob_1.koef[i] -= ob_4.koef[i];
        }

        ob_1.degree--;
        if (ob_2.degree > ob_1.degree)
        {
            inAlgoritm = false;
        }
    }

    return ob_1;
}

double Polynom::value(double x)
{
    double y = 0;
    for (int i = 0; i <= degree; i++)
    {
        y += koef[i] * powl(x, i);
    }
    return y;
}

complex <double> Polynom::value_complex(complex <double> x)
{
    complex <double> y(0, 0);
    for (int i = 0; i <= degree; i++)
    {
        y += koef[i] * pow(x, i);
    }
    return y;
}

void Polynom::removeMultipleRoots()
{
    Polynom p;
    this->copyPoly(p);

    Polynom r1, r2, r3;
    p.copyPoly(r1);
    r2 = p.derivative();

    while (true)
    {
        r3 = r1 % r2;

        if (r3.degree <= 0)
        {
            break;
        }

        r1.deletePoly();
        r2.copyPoly(r1);
        r2.deletePoly();
        r3.copyPoly(r2);
        r3.deletePoly();
    }

    if (r3.koef[0] == 0)
    {
        if (r2.degree > 0)
        {
            this->deletePoly();
            *this = p / r2;
        }
    }
    else
    {
        if (r3.degree > 0)
        {
            this->deletePoly();
            *this = p / r3;
        }
    }
}

Polynom Polynom::operator *(Polynom& ob)
{
    Polynom res;
    res.degree = degree + ob.degree;
    res.koef = new double[res.degree + 1];

    memset(res.koef, 0, (res.degree + 1) * sizeof(double));

    for (int i = 0; i <= ob.degree; i++)
    {
        for (int j = 0; j <= degree; j++)
        {
            res.koef[i + j] += ob.koef[i] * koef[j];
        }
    }

    return res;
}

Polynom Polynom::operator -(Polynom& ob)
{
    Polynom res;
    res.degree = degree;
    if (degree - ob.degree > 0) 
    {
        res.degree = degree;
    }
    else if (degree - ob.degree < 0) 
    {
        res.degree = ob.degree;
    }
    else 
    {
        res.degree = degree;
        for (int i = 0; i <= degree; i++)
        {
            if (koef[i] == ob.koef[i]) 
            {
                res.degree -= 1;
            }
        }
    }
    res.koef = new double[res.degree + 1];

    memset(res.koef, 0, (res.degree + 1) * sizeof(double));

    for (int i = 0; i <= degree; i++)
    {
        for (int j = 0; j <= ob.degree; j++)
        {
            res.koef[i + j] += koef[i] - ob.koef[j];
        }
    }

    return res;
}

ComplexPolynom::ComplexPolynom()
{
    degree = 0;
    koef = new complex <double>[degree + 1];
    koef[0] = 0;
}

ComplexPolynom::ComplexPolynom(int d, complex <double>* k)
{
    degree = d;
    koef = new complex <double>[degree + 1];
    for (int i = 0; i <= degree; i++)
    {
        koef[i] = k[i];
    }
}

ComplexPolynom::~ComplexPolynom()
{
    degree = 0;
    //delete[] koef;
}

void ComplexPolynom::deletePoly()
{
    degree = 0;
    delete[] koef;
}

void ComplexPolynom::deleteBy_a0()
{
    for (int i = 0; i <= degree; i++)
    {
        koef[i] /= koef[degree];
    }
}

ComplexPolynom ComplexPolynom::derivative()
{
    ComplexPolynom cp;
    int d = degree - 1;
    complex <double>* coef;
    coef = new complex <double>[d + 1];
    for (int i = 0; i <= d; i++)
    {
        coef[i] = koef[i + 1] * (double)(i + 1);
    }
    cp.SetPoly(d, coef);
    return cp;
}

void ComplexPolynom::SetPoly(int d, complex <double>* k)
{
    degree = d;
    koef = new complex <double>[degree + 1];
    for (int i = 0; i <= degree; i++)
    {
        koef[i] = k[i];
    }
}

ComplexPolynom& ComplexPolynom::operator=(const ComplexPolynom& ob)
{
    degree = ob.degree;
    koef = new complex <double>[degree + 1];
    for (int i = 0; i <= degree; i++)
    {
        koef[i] = ob.koef[i];
    }

    return *this;
}

void ComplexPolynom::copyPoly(ComplexPolynom& ob)
{
    ob.degree = degree;
    ob.koef = new complex <double>[ob.degree + 1];
    for (int i = 0; i <= ob.degree; i++)
    {
        ob.koef[i] = koef[i];
    }
}

ComplexPolynom ComplexPolynom::getcopyPoly()
{
    ComplexPolynom res;
    res.degree = degree;
    res.koef = new complex <double>[res.degree + 1];
    for (int i = 0; i <= res.degree; i++)
    {
        res.koef[i] = koef[i];
    }
    return res;
}

void ComplexPolynom::printPoly()
{
    cout << "(" << koef[degree].real() << " + " << koef[degree].imag() << " * i" << ")" << "* x^" << degree;
    for (int i = degree - 1; i >= 0; i--)
    {
        cout << " + " << "(" << koef[i].real() << " + " << koef[i].imag() << " * i" << ")" << "*" << "x^" << i;
    }
    cout << "\n";
}

ComplexPolynom ComplexPolynom::operator /(ComplexPolynom& ob)
{

    bool inAlgoritm = true;

    ComplexPolynom temp;
    ComplexPolynom ob_1;
    ComplexPolynom ob_2;
    ComplexPolynom ob_4;

    temp.degree = degree - ob.degree;
    temp.koef = new complex <double>[temp.degree + 1];
    memset(temp.koef, 0, (temp.degree + 1) * sizeof(complex <double>));

    ob_1.degree = degree;
    ob_1.koef = new complex <double>[degree + 1];
    for (int i = degree; i >= 0; i--)
    {
        ob_1.koef[i] = koef[i];
    }

    ob_2.degree = ob.degree;
    ob_2.koef = new complex <double>[ob.degree + 1];
    for (int i = ob.degree; i >= 0; i--)
    {
        ob_2.koef[i] = ob.koef[i];
    }

    ob_4.degree = ob_1.degree;
    ob_4.koef = new complex <double>[ob_1.degree + 1];

    complex <double> mnojnik;
    int k = 0;
    int i, j;
    while (inAlgoritm)
    {
        for (int i = ob.degree; i >= 0; i--)
        {
            ob_4.koef[i] = ob.koef[i];
        }

        if (ob_2.degree < ob_1.degree)
        {
            for (i = ob_1.degree, j = ob_2.degree; i >= 0; i--, j--)
            {
                if (j < 0)
                {
                    ob_4.koef[i] = 0;
                }
                else
                {
                    ob_4.koef[i] = ob_2.koef[j];
                }
            }
        }

        mnojnik = ob_1.koef[ob_1.degree] / ob_4.koef[ob_1.degree];

        temp.koef[temp.degree - k] = mnojnik;
        k++;

        for (int i = 0; i <= ob_1.degree; i++)
        {
            ob_4.koef[i] *= mnojnik;
        }

        for (int i = 0; i <= ob_1.degree; i++)
        {
            ob_1.koef[i] -= ob_4.koef[i];
        }

        ob_1.degree--;
        if (ob_2.degree > ob_1.degree)
        {
            inAlgoritm = false;
        }
    }

    return temp;
}

ComplexPolynom ComplexPolynom::operator %(ComplexPolynom& ob)
{

    bool inAlgoritm = true;

    ComplexPolynom temp;
    ComplexPolynom ob_1;
    ComplexPolynom ob_2;
    ComplexPolynom ob_4;

    temp.degree = degree - ob.degree;
    temp.koef = new complex <double>[temp.degree + 1];
    memset(temp.koef, 0, (temp.degree + 1) * sizeof(complex <double>));

    ob_1.degree = degree;
    ob_1.koef = new complex <double>[degree + 1];
    for (int i = degree; i >= 0; i--)
    {
        ob_1.koef[i] = koef[i];
    }

    ob_2.degree = ob.degree;
    ob_2.koef = new complex <double>[ob.degree + 1];
    for (int i = ob.degree; i >= 0; i--)
    {
        ob_2.koef[i] = ob.koef[i];
    }

    ob_4.degree = ob_1.degree;
    ob_4.koef = new complex <double>[ob_1.degree + 1];

    complex <double> mnojnik;
    int k = 0;
    int i, j;
    while (inAlgoritm)
    {
        for (int i = ob.degree; i >= 0; i--)
        {
            ob_4.koef[i] = ob.koef[i];
        }

        if (ob_2.degree < ob_1.degree)
        {
            for (i = ob_1.degree, j = ob_2.degree; i >= 0; i--, j--)
            {
                if (j < 0)
                {
                    ob_4.koef[i] = 0;
                }
                else
                {
                    ob_4.koef[i] = ob_2.koef[j];
                }
            }
        }

        mnojnik = ob_1.koef[ob_1.degree] / ob_4.koef[ob_1.degree];

        temp.koef[temp.degree - k] = mnojnik;
        k++;

        for (int i = 0; i <= ob_1.degree; i++)
        {
            ob_4.koef[i] *= mnojnik;
        }

        for (int i = 0; i <= ob_1.degree; i++)
        {
            ob_1.koef[i] -= ob_4.koef[i];
        }

        ob_1.degree--;
        if (ob_2.degree > ob_1.degree)
        {
            inAlgoritm = false;
        }
    }

    return ob_1;
}

complex <double> ComplexPolynom::value_complex(complex <double> x)
{
    complex <double> y(0, 0);
    for (int i = 0; i <= degree; i++)
    {
        y += koef[i] * pow(x, i);
    }
    return y;
}

void ComplexPolynom::removeMultipleRoots()
{
    ComplexPolynom p;
    this->copyPoly(p);

    ComplexPolynom r1, r2, r3;
    p.copyPoly(r1);
    r2 = p.derivative();

    while (true)
    {
        r3 = r1 % r2;

        if (r3.degree <= 0)
        {
            break;
        }

        r1.deletePoly();
        r2.copyPoly(r1);
        r2.deletePoly();
        r3.copyPoly(r2);
        r3.deletePoly();
    }

    if (r3.koef[0] == 0.)
    {
        if (r2.degree > 0)
        {
            this->deletePoly();
            *this = p / r2;
        }
    }
    else
    {
        if (r3.degree > 0)
        {
            this->deletePoly();
            *this = p / r3;
        }
    }
}

ComplexPolynom ComplexPolynom::operator *(ComplexPolynom& ob)
{
    ComplexPolynom res;
    res.degree = degree + ob.degree;
    res.koef = new complex <double>[res.degree + 1];

    memset(res.koef, 0, (res.degree + 1) * sizeof(complex <double>));

    for (int i = 0; i <= ob.degree; i++)
    {
        for (int j = 0; j <= degree; j++)
        {
            res.koef[i + j] += ob.koef[i] * koef[j];
        }
    }

    return res;
}

ComplexPolynom ComplexPolynom::operator *(complex <double> c)
{
    ComplexPolynom res;
    res.degree = degree;
    res.koef = new complex <double>[res.degree + 1];

    memset(res.koef, 0, (res.degree + 1) * sizeof(complex <double>));

    for (int i = 0; i <= degree; i++)
    {
        res.koef[i] += c * koef[i];
    }

    return res;
}

ComplexPolynom ComplexPolynom::operator -(ComplexPolynom& ob)
{
    ComplexPolynom res;
    res.degree = degree;
    if (degree - ob.degree > 0)
    {
        res.degree = degree;
    }
    else if (degree - ob.degree < 0)
    {
        res.degree = ob.degree;
    }
    else
    {
        res.degree = degree;
        for (int i = 0; i <= degree; i++)
        {
            if (koef[i] == ob.koef[i])
            {
                res.degree -= 1;
            }
        }
    }
    res.koef = new complex <double>[res.degree + 1];

    memset(res.koef, 0, (res.degree + 1) * sizeof(complex <double>));

    for (int i = 0; i <= degree; i++)
    {
        for (int j = 0; j <= ob.degree; j++)
        {
            res.koef[i + j] += koef[i] - ob.koef[j];
        }
    }

    return res;
}

ComplexPolynom ComplexPolynom::operator +(ComplexPolynom& ob)
{
    ComplexPolynom res;
    res.degree = degree;
    if (degree > ob.degree)
    {
        res.degree = degree;
    }
    else if (degree < ob.degree)
    {
        res.degree = ob.degree;
    }
    else
    {
        res.degree = degree;
        for (int i = 0; i <= degree; i++)
        {
            if (koef[i] == -ob.koef[i])
            {
                res.degree -= 1;
            }
        }
    }
    res.koef = new complex <double>[res.degree + 1];

    memset(res.koef, 0, (res.degree + 1) * sizeof(complex <double>));

    for (int i = 0; i <= degree; i++)
    {
        for (int j = 0; j <= ob.degree; j++)
        {
            res.koef[i + j] += koef[i] + ob.koef[j];
        }
    }

    return res;
}

ComplexPolynom ComplexPolynom::TransformToCauchyPoly() 
{
    ComplexPolynom cauchy_poly;
    cauchy_poly.degree = degree;
    bool normalize = false;
    complex <double> norm_c = 0., val;
    for (int i = 0; i < degree + 1; i++) 
    {
        if (i == 0)
        {
            val = koef[i];
            if (abs(val) != 1)
            {
                normalize = true;
                norm_c = val;
            }
            cauchy_poly.koef[i] = 1;
        }
        else if (i == degree) 
        {
            val = koef[i];
            if (normalize) 
            {
                val /= 1. * norm_c;
            }
            val = -val;
            cauchy_poly.koef[i] = val;
        }
        else 
        {
            val = koef[i];
            if (normalize)
            {
                val /= 1. * norm_c;
            }
            cauchy_poly.koef[i] = val;
        }
    }
    return cauchy_poly;
}

complex <double> ComplexPolynom::SolveNewton(ComplexPolynom cp, complex <double> left, complex <double> right)
{
    ComplexPolynom pd = cp.derivative();

    complex <double> x0 = (left + right) / 2.0;

    if (pd.value_complex(x0) == 0.0)
    {
        cout << "Method diverges" << endl;
        return -1;
    }

    complex <double> xn = x0 - cp.value_complex(x0) / pd.value_complex(x0);

    while (abs(xn - x0) > 0.00001)
    {
        x0 = xn;
        xn = xn - cp.value_complex(xn) / pd.value_complex(xn);
    }
    if (fabs(xn.real()) < 0.00001)
    {
        xn.real(0);
    }
    else if (fabs(xn.imag()) < 0.00001)
    {
        xn.imag(0);
    }
    return xn;
}

complex <double> ComplexPolynom::get_s()
{
    ComplexPolynom cauchy = TransformToCauchyPoly();
    complex <double> c(0.3, 0.5), c1(0.7, 0.9), c2(0.0, 1.0);
    complex <double> beta = SolveNewton(cauchy, c, c1);
    double r = ((double)rand() / (RAND_MAX)) * 2 * 3.14;
    return beta * exp(r * c2);
}

ComplexPolynom ComplexPolynom::NomrPoly() 
{
    ComplexPolynom res;
    res.degree = degree;
    for (int i = 0; i < degree + 1; i++) 
    {
        res.koef[i] = (koef[i]) / (koef[0]);
    }
    return res;
}




void RootSeeker::SetPoly(int d, double* coefs)
{
    p.deletePoly();

    int degr = d;
    for (int i = degr; i > 0; i--)
    {
        if (coefs[i] == 0)
        {
            d--;
        }
        else
        {
            break;
        }
    }
    if (d == 0)
    {
        cout << "Your polynom is incorrect\n";
        correctPoly = false;
        p.SetPoly(degr, coefs);
    }
    else
    {
        correctPoly = true;
        p.SetPoly(d, coefs);
    }
}

void RootSeeker::SetPoly(int d, complex <double>* coefs)
{
    cp.deletePoly();

    int degr = d;
    for (int i = degr; i > 0; i--)
    {
        if (coefs[i] == 0.)
        {
            d--;
        }
        else
        {
            break;
        }
    }
    if (d == 0)
    {
        cout << "Your polynom is incorrect\n";
        correctPoly = false;
        cp.SetPoly(degr, coefs);
    }
    else
    {
        correctPoly = true;
        cp.SetPoly(d, coefs);
    }
}

Polynom RootSeeker::SetPoly_(int d, double* coefs)
{
    p.deletePoly();

    int degr = d;
    for (int i = degr; i > 0; i--)
    {
        if (coefs[i] == 0)
        {
            d--;
        }
        else
        {
            break;
        }
    }
    if (d == 0)
    {
        cout << "Your polynom is incorrect\n";
        correctPoly = false;
        p.SetPoly(degr, coefs);
    }
    else
    {
        correctPoly = true;
        p.SetPoly(d, coefs);
        return p;
    }
    assert(1==0);
}

ComplexPolynom RootSeeker::SetPoly_(int d, complex <double>* coefs)
{
    cp.deletePoly();

    int degr = d;
    for (int i = degr; i > 0; i--)
    {
        if (coefs[i] == 0.)
        {
            d--;
        }
        else
        {
            break;
        }
    }
    if (d == 0)
    {
        cout << "Your polynom is incorrect\n";
        correctPoly = false;
        cp.SetPoly(degr, coefs);
    }
    else
    {
        correctPoly = true;
        cp.SetPoly(d, coefs);
        return cp;
    }
    assert(1==0);
}

void RootSeeker::SetPoly(Polynom poly)
{
    p.deletePoly();

    int d = poly.degree;
    int degr = d;
    for (int i = degr; i > 0; i--)
    {
        if (poly.koef[i] == 0)
        {
            d--;
        }
        else
        {
            break;
        }
    }
    if (d == 0)
    {
        cout << "Your polynom is incorrect\n";
        correctPoly = false;
        p.SetPoly(degr, poly.koef);
    }
    else
    {
        correctPoly = true;
        p = poly;
    }
}

void RootSeeker::printPoly()
{
    p.printPoly();
}

void RootSeeker::printCPoly()
{
    cp.printPoly();
}

void RootSeeker::SetInterval(double a, double b)
{
    this->a = a;
    this->b = b;
}

void RootSeeker::countOfRoots()
{
    if (!correctPoly)
    {
        return;
    }

    // Sturm's theorem

    Polynom* hi;
    hi = new Polynom[p.degree + 1];
    hi[0] = p;
    hi[1] = p.derivative();

    for (int i = 2; i <= p.degree; i++)
    {
        hi[i] = hi[i - 2] % hi[i - 1];
        for (int j = 0; j <= hi[i].degree; j++)
        {
            hi[i].koef[j] *= -1;
        }
    }

    int c1, c2;
    double y1, y2, y3, y4;
    c1 = 0;
    c2 = 0;
    y1 = hi[0].value(a);
    y3 = hi[0].value(b);

    for (int i = 1; i <= p.degree; i++)
    {

        y2 = hi[i].value(a);
        y4 = hi[i].value(b);

        if (!((y1 > 0 && y2 > 0) || (y1 < 0 && y2 < 0)))
        {
            c1++;
        }
        if (!((y3 > 0 && y4 > 0) || (y3 < 0 && y4 < 0)))
        {
            c2++;
        }

        y1 = y2;
        y3 = y4;
    }

    this->cr = c1 - c2;
    cout << "Counts of the real roots on interval [" << a << ", " << b << "]: " << this->cr << "\n";
    roots = new double[cr];
}
/// <summary>
/// False Position method for finding one root of a polynomial.
/// </summary>
/// <param name="poly"></param>
/// <param name="left"></param>
/// <param name="right"></param>
/// <returns>
/// Returns <see cref="double a"/> in case of successful cycle.
/// Returns <see cref="NOROOT"/> and prints "This method doesn't work for this polynom." in case of unsuccessful cycle.
/// </returns>
double RootSeeker::findOneRoot_FalsePosition(Polynom& poly, double left, double right)
{
    double a = left;
    double b = right;
    double c = (a * poly.value(b) - b * poly.value(a)) / (poly.value(b) - poly.value(a));

    int i = 0;
    while (fabs(poly.value(a)) > EPSILON)
    {
        if (poly.value(c) < EPSILON)
        {
            a = c;
        }
        else
        {
            b = c;
        }

        c = (a * poly.value(b) - b * poly.value(a)) / (poly.value(b) - poly.value(a));


        i++;
        if (i > 10000)
        {
            cout << "This method doesn't work for this polynom." << endl;
            return NOROOT;
        }
    }

    return a;
}
/// <summary>
/// Newton method for finding one real root of a polynomial.
/// </summary>
/// <param name="poly"></param>
/// <param name="left"></param>
/// <param name="right"></param>
/// <returns>
/// Returns <see cref="double xn"/> in case of successful cycle.
/// </returns>
double RootSeeker::findOneRoot_Newton(Polynom& poly, double left, double right)
{
    Polynom pd = poly.derivative();

    double x0 = (left + right) / 2;
    if (x0 == 0)
    {
        x0 = (left + right) / 2;
        if (x0 == 0)
        {
            x0 -= 5;
        }
    }

    double xn = x0 - poly.value(x0) / pd.value(x0);

    while (fabs(xn - x0) > EPSILON)
    {
        x0 = xn;
        xn = xn - poly.value(xn) / pd.value(xn);
    }

    return xn;
}
/// <summary>
/// Secant method for finding one real root of a polynomial.
/// </summary>
/// <param name="poly"></param>
/// <param name="x0"></param>
/// <param name="x1"></param>
/// <returns>
/// Returns <see cref="double xn"/> and prints "Method diverges" in case of successful cycle.
/// </returns>
double RootSeeker::findOneRoot_Secant(Polynom& poly, double x0, double x1)
{
    double xn, f0, f1, fn = 1, e = 1;

    while (e > EPSILON && fn != 0)
    {
        f0 = poly.value(x0);
        f1 = poly.value(x1);
        xn = (x1 - (f1 * (x1 - x0) / (f1 - f0)));
        fn = poly.value(xn);
        e = fabs((x1 - x0) / x1);
        x0 = x1;
        x1 = xn;
    }

    return xn;
}
/// <summary>
/// Muller method for finding one real root of a polynomial.
/// </summary>
/// <param name="poly"></param>
/// <param name="x0"></param>
/// <param name="x1"></param>
/// <param name="x2"></param>
/// <returns>
/// Returns <see cref="double res"/> in case of successful cycle.
/// Returns <see cref="-1"/> and prints "Method diverges. More iterations may be needed." in case of unsuccessful cycle.
/// </returns>
double RootSeeker::findOneRoot_Muller(Polynom& poly, double left, double right)
{
    if (left == 0 || right == 0 || left == right) 
    {
        left += 5;
        right += 10;
    }
    double x2 = left + right + 10;
    int i = 1, j = 0;
    double res, t, h4, f1, f2, f3, d1, d2, h1, h2, a0, a1, a2, x, y;

    do
    {
        f1 = poly.value(left);
        f2 = poly.value(right);
        f3 = poly.value(x2);
        d1 = f1 - f3;
        d2 = f2 - f3;
        h1 = left - x2;
        h2 = right - x2;
        a0 = f3;
        a1 = ((d2 * h1 * h1 - d1 * h2 * h2) / (h1 * h2 * (h1 - h2)));
        a2 = ((d1 * h2 - d2 * h1) / (h1 * h2 * (h1 - h2)));
        x = -((2 * a0) / (a1 + sqrt(fabs(a1 * a1 - (4 * a2 * a0)))));
        y = -((2 * a0) / (a1 - sqrt(fabs(a1 * a1 - (4 * a2 * a0)))));

        if (fabs((a1 + sqrt(fabs(a1 * a1 - (4 * a2 * a0))))) > fabs((a1 - sqrt(fabs(a1 * a1 - (4 * a2 * a0))))))
            h4 = x;
        else
            h4 = y;
        res = x2 + h4;
        if (fabs(poly.value(res)) < 0.0001)
        {
            i = 0;
        }
        else
        {
            t = res;
            left = right;
            right = x2;
            x2 = res;
        }
        if (j > 10000) 
        {
            break;
        }
        if (left != left) 
        {
            break;
        }
        j++;
    } while (i != 0);

    return res;
}
/// <summary>
/// Halley method for finding one real root of a polynomial.
/// </summary>
/// <param name="poly"></param>
/// <param name="left"></param>
/// <param name="right"></param>
/// <returns>
/// Returns <see cref="double xn"/> in case of success.
/// </returns>
double RootSeeker::findOneRoot_Halley(Polynom& poly, double left, double right)
{
    Polynom pd = poly.derivative();
    Polynom pdd = pd.derivative();

    double x0 = (left + right) / 2;
    if (x0 == 0)
    {
        x0 = (left + right) / 2;
        if (x0 == 0)
        {
            x0 -= 5;
        }
    }

    double xn = x0 - (2*poly.value(x0)*pd.value(x0)) / (2*(pow(pd.value(x0), 2)) - poly.value(x0) * pdd.value(x0));

    while (fabs(xn - x0) > EPSILON)
    {
        x0 = xn;
        xn = xn - (2 * poly.value(xn) * pd.value(xn)) / (2 * (pow(pd.value(xn), 2)) - poly.value(xn) * pdd.value(xn));
    }

    return xn;
}
/// <summary>
/// Halley method for finding one complex root of a polynomial.
/// </summary>
/// <param name="poly"></param>
/// <param name="left"></param>
/// <param name="right"></param>
/// <returns>
/// Returns <see cref="complex <double> xn"/> in case of success.
/// </returns>
complex <double> RootSeeker::findOneRoot_Halley_Complex(Polynom& poly, complex <double> left, complex <double> right)
{
    Polynom pd = poly.derivative();
    Polynom pdd = pd.derivative();

    complex <double> x0 = (left + right) / 2.;
    if (x0 == complex <double>(0,0))
    {
        x0 = (left + right) / 2.;
        if (x0 == 0.0)
        {
            x0 -= 5;
        }
    }

    complex <double> xn = x0 - (2. * poly.value_complex(x0) * pd.value_complex(x0)) / (2. * (pow(pd.value_complex(x0), 2)) - poly.value_complex(x0) * pdd.value_complex(x0));

    while (abs(xn - x0) > EPSILON)
    {
        x0 = xn;
        xn = xn - (2. * poly.value_complex(xn) * pd.value_complex(xn)) / (2. * (pow(pd.value_complex(xn), 2)) - poly.value_complex(xn) * pdd.value_complex(xn));
    }
    if (fabs(xn.real()) < EPSILON)
    {
        xn.real (0);
    }
    else if (fabs(xn.imag()) < EPSILON)
    {
        xn.imag (0);
    }
    return xn;
}

complex <double> RootSeeker::findOneRoot_Halley_Complex(ComplexPolynom& poly, complex <double> left, complex <double> right)
{
    ComplexPolynom pd = poly.derivative();
    ComplexPolynom pdd = pd.derivative();

    complex <double> x0 = (left + right) / 2.;
    if (x0 == complex <double>(0, 0))
    {
        x0 = (left + right) / 2.;
        if (x0 == 0.0)
        {
            x0 -= 5;
        }
    }

    complex <double> xn = x0 - (2. * poly.value_complex(x0) * pd.value_complex(x0)) / (2. * (pow(pd.value_complex(x0), 2)) - poly.value_complex(x0) * pdd.value_complex(x0));

    while (abs(xn - x0) > EPSILON)
    {
        x0 = xn;
        xn = xn - (2. * poly.value_complex(xn) * pd.value_complex(xn)) / (2. * (pow(pd.value_complex(xn), 2)) - poly.value_complex(xn) * pdd.value_complex(xn));
    }
    if (fabs(xn.real()) < EPSILON)
    {
        xn.real(0);
    }
    else if (fabs(xn.imag()) < EPSILON)
    {
        xn.imag(0);
    }
    return xn;
}
/// <summary>
/// NewtonRaphson method for finding one complex root of polynomial.
/// </summary>
/// <param name="poly"></param>
/// <param name="x0"></param>
/// <returns>
/// Returns <see cref="complex <double> xn"/> in case of successful cycle.
/// Returns <see cref="-1"/> and prints "Method diverges" in case of unsuccessful test.
/// </returns>
complex <double> RootSeeker::findOneRoot_Newton_Complex(Polynom& poly, complex <double> left, complex <double> right)
{
    Polynom pd = poly.derivative();

    complex <double> x0 = (left + right) / 2.0;
    
    if (pd.value_complex(x0) == 0.0) 
    {
        cout << "Method diverges" << endl;
        return -1;
    }

    complex <double> xn = x0 - poly.value_complex(x0) / pd.value_complex(x0);

    while (abs(xn - x0) > EPSILON)
    {
        x0 = xn;
        xn = xn - poly.value_complex(xn) / pd.value_complex(xn);
    }
    if (fabs(xn.real()) < EPSILON)
    {
        xn.real (0);
    }
    else if (fabs(xn.imag()) < EPSILON)
    {
        xn.imag (0);
    }
    return xn;
}

complex <double> RootSeeker::findOneRoot_Newton_Complex(ComplexPolynom& poly, complex <double> left, complex <double> right)
{
    ComplexPolynom pd = poly.derivative();

    complex <double> x0 = (left + right) / 2.0;

    if (pd.value_complex(x0) == 0.0)
    {
        cout << "Method diverges" << endl;
        return -1.;
    }

    complex <double> xn = x0 - poly.value_complex(x0) / pd.value_complex(x0);

    while (abs(xn - x0) > EPSILON)
    {
        x0 = xn;
        xn = xn - poly.value_complex(xn) / pd.value_complex(xn);
    }
    if (fabs(xn.real()) < EPSILON)
    {
        xn.real(0);
    }
    else if (fabs(xn.imag()) < EPSILON)
    {
        xn.imag(0);
    }
    return xn;
}
/// <summary>
/// Secant method for finding one complex root of polynomial.
/// </summary>
/// <param name="poly"></param>
/// <param name="x0"></param>
/// <param name="x1"></param>
/// <returns>
/// Returns <see cref="complex <double> xn"/> in case of successful cycle.
/// Returns <see cref="-1"/> and prints "Method diverges" in case of unsuccessful test.
/// </returns>
complex <double> RootSeeker::findOneRoot_Secant_Complex(Polynom& poly, complex <double> x0, complex <double> x1)
{
    complex <double> xn, f0, f1, fn = 1;
    double e = 1;

    while (e > 0.005 && fn != 0.0)
    {
        f0 = poly.value_complex(x0);
        f1 = poly.value_complex(x1);
        xn = (x1 - (f1 * (x1 - x0) / (f1 - f0)));
        fn = poly.value_complex(xn);
        e = abs((x1 - x0) / x1);
        x0 = x1;
        x1 = xn;
    }
    if (fabs(xn.real()) < EPSILON)
    {
        xn.real(0);
    }
    else if (fabs(xn.imag()) < EPSILON)
    {
        xn.imag(0);
    }
    return xn;
}

complex <double> RootSeeker::findOneRoot_Secant_Complex(ComplexPolynom& poly, complex <double> x0, complex <double> x1)
{
    complex <double> xn, f0, f1, fn = 1;
    double e = 1;

    while (e > 0.005 && fn != 0.0)
    {
        f0 = poly.value_complex(x0);
        f1 = poly.value_complex(x1);
        xn = (x1 - (f1 * (x1 - x0) / (f1 - f0)));
        fn = poly.value_complex(xn);
        e = abs((x1 - x0) / x1);
        x0 = x1;
        x1 = xn;
    }
    if (fabs(xn.real()) < EPSILON)
    {
        xn.real(0);
    }
    else if (fabs(xn.imag()) < EPSILON)
    {
        xn.imag(0);
    }
    return xn;
}
/// <summary>
/// FalsePosition method for finding one complex root of polynomial.
/// </summary>
/// <param name="poly"></param>
/// <param name="left"></param>
/// <param name="right"></param>
/// <returns>
/// Returns <see cref="complex <double> a"/> in case of successful check.
/// Returns <see cref="NOROOT"/> and prints "This method doesn't work for this polynom." in case of unsuccessful check.
/// </returns>
complex <double> RootSeeker::findOneRoot_FalsePosition_Complex(Polynom& poly, complex <double> left, complex <double> right)
{
    complex <double> a = left;
    complex <double> b = right;
    complex <double> c = (a * poly.value_complex(b) - b * poly.value_complex(a)) / (poly.value_complex(b) - poly.value_complex(a));

    int i = 0;
    while (abs(poly.value_complex(a)) > EPSILON)
    {
        if (norm(poly.value_complex(c)) < EPSILON)
        {
            a = c;
        }
        else
        {
            b = c;
        }

        c = (a * poly.value_complex(b) - b * poly.value_complex(a)) / (poly.value_complex(b) - poly.value_complex(a));


        i++;
        if (i > 10000)
        {
            cout << "This method doesn't work for this polynom." << endl;
            return NOROOT;
        }
    }
    if (fabs(a.real()) < EPSILON) 
    {
        a.real (0);
    }
    else if (fabs(a.imag()) < EPSILON)
    {
        a.imag (0);
    }
    return a;
}

complex <double> RootSeeker::findOneRoot_FalsePosition_Complex(ComplexPolynom& poly, complex <double> left, complex <double> right)
{
    complex <double> a = left;
    complex <double> b = right;
    complex <double> c = (a * poly.value_complex(b) - b * poly.value_complex(a)) / (poly.value_complex(b) - poly.value_complex(a));

    int i = 0;
    while (abs(poly.value_complex(a)) > EPSILON)
    {
        if (norm(poly.value_complex(c)) < EPSILON)
        {
            a = c;
        }
        else
        {
            b = c;
        }

        c = (a * poly.value_complex(b) - b * poly.value_complex(a)) / (poly.value_complex(b) - poly.value_complex(a));


        i++;
        if (i > 10000)
        {
            cout << "This method doesn't work for this polynom." << endl;
            return NOROOT;
        }
    }
    if (fabs(a.real()) < EPSILON)
    {
        a.real(0);
    }
    else if (fabs(a.imag()) < EPSILON)
    {
        a.imag(0);
    }
    return a;
}
/// <summary>
/// Muller's method for finding one complex root of polynomial.
/// </summary>
/// <param name="poly"></param>
/// <param name="left"></param>
/// <param name="right"></param>
/// <returns>
/// Returns <see cref="complex <double> res"/>.
/// </returns>
complex <double> RootSeeker::findOneRoot_Muller_Complex(Polynom& poly, complex <double> left, complex <double> right)
{
    complex <double> x0 = left;
    complex <double> x2 = right;
    complex <double> x1 = (left + right) / 2.;
    int i;
    complex <double> res;

    for (i = 0;; ++i)
    {
        complex <double> f1 = poly.value_complex(x0);
        complex <double> f2 = poly.value_complex(x1);
        complex <double> f3 = poly.value_complex(x2);
        complex <double> d1 = f1 - f3;
        complex <double> d2 = f2 - f3;
        complex <double> h1 = x0 - x2;
        complex <double> h2 = x1 - x2;
        complex <double> a0 = f3;
        complex <double> a1 = (((d2 * pow(h1, 2)) - (d1 * pow(h2, 2))) / ((h1 * h2) * (h1 - h2)));
        complex <double> a2 = (((d1 * h2) - (d2 * h1)) / ((h1 * h2) * (h1 - h2)));
        complex <double> x = ((-2. * a0) / (a1 + abs(sqrt(a1 * a1 - 4. * a0 * a2))));
        complex <double> y = ((-2. * a0) / (a1 - abs(sqrt(a1 * a1 - 4. * a0 * a2))));
        double modx = norm(x);
        double mody = norm(y);
        if (modx >= mody)
            res = x + x2;
        else
            res = y + x2;

        complex <double> m = res * 100.;
        complex <double> n = x2 * 100.;
        double mm = floor(norm(m));
        double nn = floor(norm(n));
        if (mm == nn)
            break;
        x0 = x1;
        x1 = x2;
        x2 = res;
        if (i > 10000)
        {
            cout << "Method diverges. More iterations may be needed." << endl;
            break;
        }
    }
    if (fabs(res.real()) < EPSILON)
    {
        res.real(0);
    }
    else if (fabs(res.imag()) < EPSILON)
    {
        res.imag(0);
    }
    return res;
}

complex <double> RootSeeker::findOneRoot_Muller_Complex(ComplexPolynom& poly, complex <double> left, complex <double> right)
{
    complex <double> x0 = left;
    complex <double> x2 = right;
    complex <double> x1 = (left + right) / 2.;
    int i;
    complex <double> res;

    for (i = 0;; ++i)
    {
        complex <double> f1 = poly.value_complex(x0);
        complex <double> f2 = poly.value_complex(x1);
        complex <double> f3 = poly.value_complex(x2);
        complex <double> d1 = f1 - f3;
        complex <double> d2 = f2 - f3;
        complex <double> h1 = x0 - x2;
        complex <double> h2 = x1 - x2;
        complex <double> a0 = f3;
        complex <double> a1 = (((d2 * pow(h1, 2)) - (d1 * pow(h2, 2))) / ((h1 * h2) * (h1 - h2)));
        complex <double> a2 = (((d1 * h2) - (d2 * h1)) / ((h1 * h2) * (h1 - h2)));
        complex <double> x = ((-2. * a0) / (a1 + abs(sqrt(a1 * a1 - 4. * a0 * a2))));
        complex <double> y = ((-2. * a0) / (a1 - abs(sqrt(a1 * a1 - 4. * a0 * a2))));
        double modx = norm(x);
        double mody = norm(y);
        if (modx >= mody)
            res = x + x2;
        else
            res = y + x2;

        complex <double> m = res * 100.;
        complex <double> n = x2 * 100.;
        double mm = floor(norm(m));
        double nn = floor(norm(n));
        if (mm == nn)
            break;
        x0 = x1;
        x1 = x2;
        x2 = res;
        if (i > 10000)
        {
            cout << "Method diverges. More iterations may be needed." << endl;
            break;
        }
    }
    if (fabs(res.real()) < EPSILON)
    {
        res.real(0);
    }
    else if (fabs(res.imag()) < EPSILON)
    {
        res.imag(0);
    }
    return res;
}
/// <summary>
/// One of numerical methods to solve and find roots of polynomials. Finds both real and complex roots.
/// </summary>
/// <param name="poly">Polynom class poly</param>
/// <returns>
/// Returns <see cref="rootApprox"/> list of found roots.
/// </returns>
complex <double>* RootSeeker::DurandKerner(Polynom& poly, int deg)
{
    complex<double> constant(0.4, 0.9), reset(1, 0);
    complex<double>* rootApprox = NULL;
    complex<double>* firstDenom = new complex<double>[deg];
    complex<double>* secondDenom = new complex<double>[deg];
    rootApprox = new complex<double>[deg];
    fill_n(firstDenom, deg, 1);
    fill_n(secondDenom, deg, 1);

    for (int i = 0; i < deg; i++)
    {
        rootApprox[i] = pow(constant, i);
    }

    for (int i = 0; i <= 10000; i++)
    {
        for (int xn = 0; xn < deg; xn++)
        {
            for (int firstHalf = 1; firstHalf <= deg - 1; firstHalf++)
            {
                if (firstHalf == 1)
                {
                    firstDenom[xn] = reset;
                }

                if (xn < firstHalf)
                {
                    firstDenom[xn] *= (rootApprox[xn] - rootApprox[firstHalf]);
                }
            }
            complex<double> y = poly.value_complex(rootApprox[xn]);
            if (xn > 0)
            {
                for (int secondHalf = 0; secondHalf <= deg - 1; secondHalf++)
                {
                    if (secondHalf == 0)
                    {
                        secondDenom[xn] = reset;
                    }

                    if (xn > secondHalf)
                    {
                        secondDenom[xn] *= (rootApprox[xn] - rootApprox[secondHalf]);
                    }
                }
            }
            rootApprox[xn] = (rootApprox[xn] - ((y) / (firstDenom[xn] * secondDenom[xn])));
        }
    }
    return rootApprox;
}

complex <double>* RootSeeker::DurandKerner(ComplexPolynom& poly, int deg)
{
    complex<double> constant(0.4, 0.9), reset(1, 0);
    complex<double>* rootApprox = NULL;
    complex<double>* firstDenom = new complex<double>[deg];
    complex<double>* secondDenom = new complex<double>[deg];
    rootApprox = new complex<double>[deg];
    fill_n(firstDenom, deg, 1);
    fill_n(secondDenom, deg, 1);

    for (int i = 0; i < deg; i++)
    {
        rootApprox[i] = pow(constant, i);
    }

    for (int i = 0; i <= 10000; i++)
    {
        for (int xn = 0; xn < deg; xn++)
        {
            for (int firstHalf = 1; firstHalf <= deg - 1; firstHalf++)
            {
                if (firstHalf == 1)
                {
                    firstDenom[xn] = reset;
                }

                if (xn < firstHalf)
                {
                    firstDenom[xn] *= (rootApprox[xn] - rootApprox[firstHalf]);
                }
            }
            complex<double> y = poly.value_complex(rootApprox[xn]);
            if (xn > 0)
            {
                for (int secondHalf = 0; secondHalf <= deg - 1; secondHalf++)
                {
                    if (secondHalf == 0)
                    {
                        secondDenom[xn] = reset;
                    }

                    if (xn > secondHalf)
                    {
                        secondDenom[xn] *= (rootApprox[xn] - rootApprox[secondHalf]);
                    }
                }
            }
            rootApprox[xn] = (rootApprox[xn] - ((y) / (firstDenom[xn] * secondDenom[xn])));
            //cout << "ATTENTION: " << rootApprox[xn] << endl;
        }
    }
    return rootApprox;
}

void RootSeeker::findRoots(double (*methodName)(Polynom&, double, double))
{
    if (!correctPoly || cr == 0)
    {
        return;
    }

    Polynom dvochlen;
    dvochlen.degree = 1;
    dvochlen.koef = new double[2];
    dvochlen.koef[1] = 1;

    Polynom cop;
    p.copyPoly(cop);
    roots[0] = methodName(cop, a, b);

    if (fabs(roots[0]) < EPSILON)
    {
        roots[0] = 0;
    }

    if (roots[0] == NOROOT)
    {
        return;
    }

    dvochlen.koef[0] = -roots[0];

    for (int i = 1; i < cr; i++)
    {
        cop = cop / dvochlen;

        roots[i] = methodName(cop, a, b);

        if (roots[i] == NOROOT)
        {
            return;
        }

        if (fabs(roots[i]) < EPSILON)
        {
            roots[i] = 0;
        }

        dvochlen.koef[0] = -roots[i];
    }

}

void RootSeeker::printRoots()
{
    if (correctPoly && cr != 0 && roots[0] != NOROOT)
    {
        cout << "Roots: ";
        for (int i = 0; i < cr; i++)
        {
            cout << roots[i] << " ";
        }
        cout << "\n";
    }
    else
    {
        cout << "No roots\n";
    }
}

double* RootSeeker::getRoots()
{
    return roots;
}

complex <double> RootSeeker::findOneRootJenkinsTraub(ComplexPolynom c_poly)
{
    c_poly = c_poly.NomrPoly();
    // 1
    int M = 5;
    complex <double> s(0.0, 0.0), cst, c(1.0, 0.0);
    ComplexPolynom h_poly = c_poly.derivative();
    ComplexPolynom lin_poly, pz_poly, adjust_h_poly, next_h_poly, lin_poly1, h_bar_poly, next_h_bar_poly;
    lin_poly.degree = 1;
    lin_poly1.degree = 1;
    lin_poly.koef[0] = c;
    lin_poly.koef[1] = c;
    lin_poly1.koef[1] = c;
    for (int i = 0; i < M; i++)
    {
        cst = (-h_poly.value_complex(s)) / (c_poly.value_complex(s));
        pz_poly = c_poly * cst;
        adjust_h_poly = h_poly + pz_poly;
        h_poly = adjust_h_poly / lin_poly;
    }
    // 2
    int L = 100;
    complex <double> t_cur, t_next, t_prev, prev_err, cur_err;
    ComplexPolynom initial_h_poly = h_poly.getcopyPoly();
    bool stage_two_success = false, root_found = false;
    while (!root_found)
    {
        while (!stage_two_success)
        {
            h_poly = initial_h_poly.getcopyPoly();
            s = c_poly.get_s();
            lin_poly1.koef[0] = s;
            for (int i = 0; i < L; i++)
            {
                cst = (-h_poly.value_complex(s)) / (c_poly.value_complex(s));
                pz_poly = c_poly * cst;
                adjust_h_poly = h_poly + pz_poly;

                next_h_poly = adjust_h_poly / lin_poly1;

                h_bar_poly = h_poly.NomrPoly();
                next_h_bar_poly = next_h_poly.NomrPoly();

                t_cur = s - c_poly.value_complex(s) / (h_bar_poly.value_complex(s));
                t_next = s - c_poly.value_complex(s) / (next_h_bar_poly.value_complex(s));

                if (i > 0 && abs(t_cur - t_prev) <= 0.5 * abs(t_prev) && abs(t_next - t_cur) <= 0.5 * abs(t_cur))
                {
                    stage_two_success = true;
                    break;
                }
                t_prev = t_cur;
                h_poly = next_h_poly;
            }
        }
        prev_err = c_poly.value_complex(s);
        h_bar_poly = h_poly.NomrPoly();
        s = s - (c_poly.value_complex(s) / (h_bar_poly.value_complex(s)));
        complex <double> prev_s = 0.;
        bool stage_3_success = false;
        for (int i = 0; i < 10000; i++)
        {
            if (abs(c_poly.value_complex(s)) < 0.00001)
            {
                stage_3_success = true;
                break;
            }
            cst = (-h_poly.value_complex(s)) / (c_poly.value_complex(s));
            pz_poly = c_poly * cst;
            adjust_h_poly = h_poly + pz_poly;
            next_h_poly = adjust_h_poly / lin_poly1;
            next_h_bar_poly = next_h_poly.NomrPoly();

            prev_s = s;
            prev_err = c_poly.value_complex(prev_s);

            s = s - (c_poly.value_complex(s) / (h_bar_poly.value_complex(s)));
            cur_err = c_poly.value_complex(s);

            h_poly = next_h_poly;

            if (s.imag() != s.imag() || s.real() != s.real())
            {
                stage_3_success = false;
                break;
            }
        }
    }
    return s;
}

