Vg 1 0 Vg
Rs 1 2 Rs
Rf 2 4 Rf
R1 4 0 R1
    .SUBCKT OUTER 1 2 4
    X2 2 1 INNER
        .SUBCKT INNER A B
        R1 A C R1
        R2 C B R2
        .ENDS
    E1 3 5 2 1 K
    Ro 3 4 Ro
    .ENDS
X1 2 0 4 OUTER
.TF V(4) V(1)
.TF V(2) V(1)
.END

*  V(4)
*  ----- =
*  V(1)
*                  R1*R2+R1^2
*  -------------------------------------------
*  Rf*R2+R1*R2+Rf*R1+R1^2+2*R1*Rs+Rf*Rs+Rs*R2

*  V(2)
*  ----- =
*  V(1)
*            Rf*R2+R1*R2+Rf*R1+R1^2
*  -------------------------------------------
*  Rf*R2+R1*R2+Rf*R1+R1^2+2*R1*Rs+Rf*Rs+Rs*R2
