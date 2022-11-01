V1 1 0 V1
V2 5 0 V2
R1 1 2 R1
R2 5 3 R2
R3 2 4 R3
C1 2 4 C1
X1 2 3 4 OTRA
.LIB OTRA
.TF V(4) V(1)
.END

*  V(4)
*  ----- =
*  V(1)
*      V1*R2*R3-V2*R1*R3
*  ---------------------------
*  -R1*V1*R2*C1*s*R3-R1*V1*R2
