\mainpage Puredata DSF external

\section intro Discrete summation formula

When synthesizing sounds digitally,
classic waveforms with rich spectra
inevitably create unwanted alias-frequencies.
This results from the fact that the overtone
series goes to infinity while a digital system
can only realize a limited bandwith. The
surplus energy is being reflected back into
the created frequency spectrum as alias-frequencies.

One way around this problem is to create the
overtones piece by piece up to the maximal frequency
(which is the Nyquist frequency or half the sample
rate at which the digital system operates).
Especially with low frequencies, this is not
feasible, because the number of sine waves to
keep track of can grow up to 1000 for low
bass notes.

DSF synthesis creates sine and cosine waves
by rotating phasors. Instead of generating
sine waves up to the Nyquist frequency,
the complex numbers which represent the 
position of the phasors are being put into
a geometric series for which a closed
formula exists. This formula can be computed
in near-linear time, so that computational
costs stay low.

One main factor in this is the efficient
calculation of a complex power function
as implemented here: power_complex().
Using a "divide-and-conquer" approach,
the number of calculations are reduced
to the 2-logarithm of the number of calculations
in the naive implementation power_complex_naiv().
When creating an 8Hz wave with 1400 overtones,
the difference between those two implementations
resulted in a cpu-usage of 5% vs 50%, which is
a ten-fold speedup.


\section imp Implementation

The core functionality is implemented in
dsflib.c. When compiling, two externals
dsf~ and dsffm~ are produced, based on
the code in dsf.c and dsffm.c.
While dsf~ provides a userfriendly message-based
interface to access the functionality of
the dsflib, dsffm~ is optimized for signal-rate
shaping of the dsf-parameters. Please refer
to the pd-helper patches in this repository
for a demonstration of the functionality.


The connection to the puredata interface is
implemented via dsf_pd.c and dsffm_pd.c, both
of which are based on an example by
Johannes Zmoelnig (see <a href="https://github.com/pure-data/externals-howto/blob/master/example4/pan%7E.c">here</a>)
 and include the m_pd.h header with 
essential definitions of the puredata interface.




