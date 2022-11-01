1
0.0
0.9
0.75

Hey Folks.  This is a specification file for
the filter program.  The first row contains an
integer specifying the order (number of delays/
coefficients in the filter).
The second row of numbers are the FIR coefficients.
The third row of numbers are the IIR coefficients
(careful of signs here, an IIR .9 cancels an FIR -.9).
The last number on line 4 controls overall gain.

useage is: filter specFile infile.snd outFile.snd

This only works for 16 bit linear mono files.
Enjoy the filter.  Have fun.
