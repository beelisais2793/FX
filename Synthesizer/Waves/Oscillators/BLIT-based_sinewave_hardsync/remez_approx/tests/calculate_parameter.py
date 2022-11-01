# -*- coding: utf-8 -*-
import numpy
import sympy.mpmath as sm

sm.mp = 1024

def polynomial(x, list_a):
    return sum(a*x**k for k,a in enumerate(list_a))

def d_polynomial(x, list_a):
    return sum(a*k*x**(k-1) for k,a in enumerate(list_a) if k >= 1)

def dd_polynomial(x, list_a):
    return sum(a*k*(k-1)*x**(k-2) for k,a in enumerate(list_a) if k >= 2)

def search_extreme_points(list_a):
    def f(x):
        return d_polynomial(x, list_a) - sm.cos(x);

    def df(x):
        return dd_polynomial(x, list_a) + sm.sin(x);

    check_points = numpy.linspace(0.0, sm.pi, 100)

    sign_reverse_section = \
        [p for p in zip(check_points, check_points[1:]) if f(p[0])*f(p[1]) <= 0.0]

    return [sm.findroot(f, x, df=df, tol=1.0e-20) for x,_ in sign_reverse_section]

###########################
# Remez algorithm --step2--
###########################
def update_polynomial_coefficients(list_x):
    matrix_A = sm.matrix(
        [[x**k for k in range(len(list_x)-1)] + [ 0 if j==0 else (-1)**j ]  \
               for j,x in enumerate(list_x)])

    vector_b = sm.matrix([sm.sin(x) for x in list_x])

    u = sm.lu_solve(matrix_A, vector_b)

    # a[0],...,a[n], d
    return u[:-1], u[u.rows-1]

###########################
# Remez algorithm --step3--
###########################
def update_maximum_error_points(list_a):
    n = len(list_a)-1

    extreme_points = search_extreme_points(list_a)

    if len(extreme_points) == n+1:
        return [sm.mpf(0.0)] + extreme_points
    else:
        raise Exception('[ERROR]number of extreme point ' + \
            str(n+2) + '->' + str(len(extreme_points)))

###########################
# Remez algorithm --step4--
###########################
def check_convergence(
    list_a,
    list_x):

    def ef(x):
        return polynomial(x, list_a) - sm.sin(x)

    err = numpy.var([ef(x)*(-1)**k for k,x in enumerate(list_x) if k>=1])
    return err < 1.0e-32

##########################
# Remez algorithm --main--
##########################
def remez():

    # Remez algorithm --step3--
    list_a = [0.0, 0.988454351767074, 0.0470171509264136, -0.230234777715065, 0.0366430029450163]
    list_x = update_maximum_error_points(list_a)

    for count in range(1, 1000):
        # Remez algorithm --step2--
        list_a, d = update_polynomial_coefficients(list_x)

        # Remez algorithm --step3--
        list_x = update_maximum_error_points(list_a)

        # Remez algorithm --step4--
        if check_convergence(list_a, list_x):
            return list_a, d, list_x, count

    else:
        raise Exception('[ERROR]Remez algorithm failed')

if __name__ == '__main__':
    print('Remez algorithm calculating...', end='')
    list_a, d, list_x, count = remez()

    print(' OK')
    for k,a in enumerate(list_a):
        print('a[' + str(k) + ']=', sm.nstr(a, 17))

    print('d=', sm.nstr(d, 17))

    # -2*s0*s1*s3*x**3 + s0*s2*x + s1**2*s3*x**4 + x**2*(s0**2*s3 - s1*s2)
    def f(s0,s1,s2,s3):
        return s0*s2            - list_a[1], \
               s0**2*s3 - s1*s2 - list_a[2], \
               -2*s0*s1*s3      - list_a[3], \
               s1**2*s3         - list_a[4]

    print()
    print('Newton method calculating...', end='')
    initilal = (1.2732395447351627, 0.40528473456935109, 0.77633023248007499, 0.22308510060189463);
    list_s = sm.findroot(
            f,
            initilal,
            method='newton',
            maxsteps=10000,
            tol=1.0e-25)
    print(' OK')

    for k,s in enumerate(list_s):
        print('s[' + str(k) + ']=', sm.nstr(s, 17))

#
#Remez algorithm calculating... OK
#a[0]= 0.0
#a[1]= 0.9897151132173856
#a[2]= 0.044771099390202579
#a[3]= -0.22906038058222875
#a[4]= 0.036456091836172492
#d= -0.00073239476651250248
#
#Newton method calculating... OK
#s[0]= 1.2728577660723033
#s[1]= 0.40516321064662885
#s[2]= 0.7775535802962265
#s[3]= 0.22208033386249199
#
