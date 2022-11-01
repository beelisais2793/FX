2
-0.291038 1.0
-1.29953 0.9801
0.1

Hey Folks.  This is a specification file
for the filter program.  The first row
contains an integer specifying the order
(number of delays/coefficients in the
filter).  The second row of numbers are the 
FIR coefficients.  The third row of 
numbers are the IIR coefficients. The 
last number on line 4 controls overall gain.

The useage is filter specFile infile.snd outFile.snd

Under LINUX, this only works for 16 bit linear
mono files.  Have fun.
