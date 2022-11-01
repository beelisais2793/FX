import numpy as np
from numpy.polynomial import Polynomial



def auto_poly(start, end, deg, func, n_points = 50):
    x = np.linspace(start, end, n_points)
    y = [func(x_k) for x_k in x]

    return Polynomial.fit(x,y,deg, domain = (start, end), window = (start, end))

def calc_approx_sincos(deg):

    print("---------------------")
    print(f"Sin/Cos approximation for degree {deg}")
    print()

    # Create fitting polynomial for cos and sin
    approx_cos = auto_poly(-np.pi, np.pi, deg, np.cos)
    approx_sin = auto_poly(-np.pi, np.pi, deg, np.sin)

    # Make near-zero coefficients as zero for both sin and cos.
    approx_cos.coef[1::2] = 0 # zero coefficients for odd powers
    approx_sin.coef[0::2] = 0 # zero coefficients for even powers


    # Display coefficients
    print("Approximate cos coefficients")
    print(approx_cos.coef)
    print()
    print("Approximate sin coefficients")
    print(approx_sin.coef)
    print()

    # Display coefficients as hex
    print("Approximate cos coefficients as hex")
    print([x.hex() for x in approx_cos.coef])
    print()
    print("Approximate sin coefficients as hex")
    print([x.hex() for x in approx_sin.coef])
    print()


    # Measure approximation accuracy.
    x = np.arange(-np.pi, np.pi, 0.001)
    #
    max_abs_err = np.max(np.abs(  approx_cos(x) - np.cos(x)   ))
    print(  "max absolute error for approximate cos:\t%e"%(max_abs_err)   )
    #
    residual = approx_cos(x) - np.cos(x)
    snr = 10 * np.log10(np.sum(np.cos(x) * np.cos(x)) / np.sum(residual * residual))
    print(  "snr (db) for approximate cos:\t%e"%(snr)   )
    #
    max_abs_err = np.max(np.abs(  approx_sin(x) - np.sin(x)   ))
    print(  "max absolute error for approximate sin:\t%e"%(max_abs_err)   )
    #
    residual = approx_sin(x) - np.sin(x)
    snr = 10 * np.log10(np.sum(np.sin(x) * np.sin(x)) / np.sum(residual * residual))
    print(  "snr (db) for approximate sin:\t%e"%(snr)   )

    print("---------------------")

def calc_approx_half_cos(deg):

    print("---------------------")
    print(f"Half Cos approximation for degree {deg}")
    print()

    # Create fitting polynomial for cos and sin
    approx_cos = auto_poly(0, np.pi, deg, np.cos)
    print("Approximate cos coefficients")
    print(approx_cos.coef)

    x = np.arange(0., np.pi, 0.001)
    #
    max_abs_err = np.max(np.abs(  approx_cos(x) - np.cos(x)   ))
    print(  "max absolute error for approximate cos:\t%e"%(max_abs_err)   )
    #
    residual = approx_cos(x) - np.cos(x)
    snr = 10 * np.log10(np.sum(np.cos(x) * np.cos(x)) / np.sum(residual * residual))
    print(  "snr (db) for approximate cos:\t%e"%(snr)   )



if __name__ == "__main__":
    calc_approx_sincos(10)



