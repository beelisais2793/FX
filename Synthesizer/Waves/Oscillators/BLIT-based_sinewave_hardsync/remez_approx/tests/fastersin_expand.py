# -*- coding: utf-8 -*-
import sympy

def fastersin_expand():
    f = sympy.sympify('(s0*x - s1*x*x)*(s2 + s3*(s0*x - s1*x*x))').expand()
    print(f.collect('x'))

    param = {'s0':1.2732395447351627,
             's1':0.40528473456935109,
             's2':0.77633023248007499,
             's3':0.22308510060189463 }
    print(f.subs(param))

if __name__ == '__main__':
    fastersin_expand()

#
# -2*s0*s1*s3*x**3 + s0*s2*x + s1**2*s3*x**4 + x**2*(s0**2*s3 - s1*s2)
# 0.0366430029450163*x**4 - 0.230234777715065*x**3 + 0.0470171509264136*x**2 + 0.988454351767074*x
#
