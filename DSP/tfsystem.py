import sympy
import numpy as np
import matplotlib.pyplot as plt
import scipy.signal as sig
from sympy.integrals import inverse_laplace_transform


sympy.init_printing()
(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z)=sympy.symbols('a:z')
pi=sympy.pi



class tf(object):
    def __init__(self, *args, **kwargs):
        s=sympy.Symbol('s')
        if kwargs.get('discrete'):
            s=sympy.Symbol('z')
        N = len(args)
        if N==1:
            self.expr=args[0]
        elif N==2:
            poly=args
            self.expr=sympy.polys.Poly(poly[0], s).as_expr()/sympy.polys.Poly(poly[1], s).as_expr()
        elif N==3:
            poly=sig.zpk2tf(args[0], args[1], args[2])
            self.expr=sympy.polys.Poly(poly[0], s).as_expr()/sympy.polys.Poly(poly[1], s).as_expr()
        elif N==4:
            poly=sig.ss2tf(args[0], args[1], args[2], args[3])
            self.expr=sympy.polys.Poly(poly[0], s).as_expr()/sympy.polys.Poly(poly[1], s).as_expr()
        else:
            raise ValueError("Too many arguments!")

    def __add__(self, other):
        expr=other.expr if isinstance(other, tf) else other
        return tf(self.polyform((self.expr+expr).together()))

    def __mul__(self, other):
        expr=other.expr if isinstance(other, tf) else other
        return tf(self.polyform(self.expr*expr))

    def __sub__(self, other):
        expr=other.expr if isinstance(other, tf) else other
        return tf(self.polyform(self.expr-expr.together()))

    def __div__(self, other):
        expr=other.expr if isinstance(other, tf) else other
        return tf(self.polyform(self.expr/expr))

    def __str__(self):
        return sympy.pretty(self.expr)

    def polyform(self, expr=None):
        expr=self.expr if expr is None else expr
        num, den=sympy.fraction(expr)
        return num.apart()/den.apart()

    def pzform(self, *args):
        def expand(root_dict):
            keys=root_dict.keys()
            vals=root_dict.values()
            roots=1
            z=sympy.Symbol('z')
            for i in range(len(vals)):
                roots*=(z-keys[i])**(vals[i])
            return roots

        num, den=sympy.fraction(self.expr)
        num_roots=expand(sympy.roots(num))
        den_roots=expand(sympy.roots(den))
        return num_roots/den_roots

    def poles(self):
        return sympy.roots(self.expr.as_numer_denom()[1])

    def zeros(self):
        return sympy.roots(self.expr.as_numer_denom()[0])

    def feedback(G, K):
        return G/(G*K+1)

##############CONTINUOUS##############
    def laplace_transform(self):
        pass

    def inverse_laplace_transform(self):
        poles=self.poles().keys()
        s=sympy.Symbol('s')
        t=sympy.Symbol('t', real=True)
        ans=0
        for p in poles:
            ans+=sympy.residue( self.expr * sympy.E**(s*t), s, p).expand(complex=True).simplify()
        print(ans)
        return ans
        pass#residue theorem -> rewrite(sin) -> simplify

    def impulse(self):
        impulse_response=inverse_laplace_transform(self.expr, sympy.Symbol('s'), sympy.var('t'))
        sympy.plot(impulse_response, xlim=[0, 10])

    def step(self):
        step_response=inverse_laplace_transform(self.expr/sympy.Symbol('s'), sympy.var('s'), sympy.var('t'))
        sympy.plot(step_response, xlim=[0, 10])

    def freq_resp(self):#USE PZFORM
        pass

    def pzmap(self):
        poles=self.poles().keys()
        zeros=self.zeros().keys()
        for p in poles:
            plt.plot(p.re, p.im)
        for z in zeros:
            plt.plot(z.re, z.im)



    def bode(self):
        sympy.plot(sympy.functions.Abs(sympy.re(self.expr.subs(s, sympy.Symbol('x')*sympy.I))), xlim=[0, 10])

    def rlocus(self):
        pass

####################DISCRETE#############################
    def z_transform(self, seq):
        z=sympy.Symbol('z')
        n=sympy.Symbol('n')
        sympy.pprint(seq)
        temp=(-1)**n
        print(temp.atoms(sympy.Symbol).pop().assumptions0)
        print(seq.atoms(sympy.Symbol).pop().assumptions0)
        Z=sympy.summation((temp * z**-n), (n, 0, sympy.oo)).args[0]
        return sympy.Piecewise((Z.args[0].simplify(), Z.args[1])).args[0]
        

    def inverse_z_transform(self):
        poles=self.poles().keys()
        z=sympy.Symbol('z')
        n=sympy.Symbol('n', integer=True)
        ans=0
        for p in poles:
            ans+=sympy.residue( self.expr * z**(n-1), z, p).expand(complex=True).simplify()
            sympy.pprint(ans)
        return(ans)
        

'''
def rlocus(system):
 48         expr=sympy.polys.Poly(system.num, s).as_expr()/sympy.polys.Poly(system.den, s).as_expr()
 49         Symboliable=expr.atoms(sympy.Symbol)
 50         if len(Symboliable>1):
 51                 raise ValueError("Too many Symboliables!")
 52         if len(Symboliable=0):
 53                 raise ValueError("Missing Symboliable!")
 54         Symboliable=variable.pop()
 55         #Find pole-zeros while sweeping Symboliable #start at 0 and move out in both directions until at desired window size
 56         np.linspace(-10, 10, 20)
 57 
 58 def Bode(system, ret=[]):
 59         w, mag, phase=sig.bode(system)
 60         plt.title('Bode Plot')
 61         plt.subplot(211)
 62         plt.semilogx(w, mag)
 63         plt.ylabel('Gain (dB)')
 64         plt.xlabel('Angular Frequency (rad/sec)')
 65         plt.subplot(212)
 66         plt.semilogx(w, phase)
 67         plt.ylabel('Phase (degrees)')
 68         plt.xlabel('Angular Frequency (rad/sec)')
 69         plt.show()
 70         ret.append([[w], [mag], [phase]])
 71 
 72 
 73 
 74 def nyquist(system):
 75         pass
 76 
 77 def pzmap(system):
 78         plt.plot(np.real(system.poles), np.imag(system.poles), 'x')
 79         plt.plot(np.real(system.zeros), np.imag(system.zeros), 'o')
 80         plt.title('Pole-Zero Map')
 81         plt.xlabel('Real')
 82         plt.ylabel('Imaginary')
 83         plt.show()
'''

def impulse(system):
    system.impulse()

def step(system):
    system.step()



if __name__=='__main__':
    n=sympy.Symbol('n')
    G1=tf([1], [-1, 1])
    G2=tf([1], [1, 3, 2])
    I=tf([1], [1, 0])
    G1*I
    G1.inverse_laplace_transform()
    #sympy.pprint(G3.expr)