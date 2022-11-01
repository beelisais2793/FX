
# Discrete summation formula

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


# Installation


Clone this repository to your computer,
open the directory and start the compilation
with `make`:

```
# clone repository: 
git clone https://github.com/Seba-birds/dsf.git
# change to project root directory:
cd dsf
# compile sources:
make
# see results:
ls
```

After compilation, you will find two new binary
files whose name depends on your operating
system. On MacOS those files are
`dsf.pd_darwin` and `dsffm.pd_darwin`.

Mind that those names are missing the tilde
that is prescribed by puredata naming conventions.
So we need to rename the binaries to meet the
naming convention:

```
# replace file extension (".pd_darwin")
# according to your OS
mv dsf.pd_darwin dsf~.pd_darwin
mv dsffm.pd_darwin dsffm~.pd_darwin
```


Now we need to copy the binaries and the help
files to the correct folder on your system, so
that they are available in puredata.
Refer to [this site](https://puredata.info/docs/faq/how-do-i-install-externals-and-help-files/)
for more detailed information. On a standard
installation on MacOS, you can do:

```
for i in dsf~.pd_darwin dsffm~.pd_darwin dsf~-help.pd dsffm~-help.pd ; \
do cp $i /Applications/Pd.app/Contents/Resources/extra/$i ; \
done;
```

After restarting puredata, you should now be
able to create the objects `dsf~` and `dsffm~`
on the canvas and access the helper patches via
the right-click menu.


# Module overview

## dsf~

`dsf~` is a puredata implementation of the discrete summation formula.
This module is controlled by messages and thus more transparent for
user interaction, yet not convenient to use in a context where parameters
are to be varied at signal speed. Refer to the `doxygen`-documentation
for more detailed information about the implementation and to the
`dsf~-help.pd`-file for more information about usage.

## dsffm~

`dsffm~` provides signal-rate inlets for fm-style modulation of several
parameters of the DSF. The three inlets provide signal-rate control over
pitch, overtone ratio and overtone weight. Opposed to the `dsf~`-module,
pitch is controlled in MIDI-notes, not frequency, thus providing the same
aural characteristics of a given modulation over the whole frequency
spectrum. Refer to the `doxygen`-documentation
for more detailed information about the implementation and to the
`dsffm~-help.pd`-file for more information about usage.



# Documentation

For detailed description of the code, generate the
documentation via `doxygen` like so:

```
doxygen Doxyfile
```

The documentation is created in a new 
folder `./documentation/`.


# Sources

The following files where used and altered for this project:

* Makefile: [https://github.com/pure-data/pd-lib-builder/](https://github.com/pure-data/pd-lib-builder/)
* *_pd.c-files: [https://github.com/pure-data/externals-howto/blob/master/example4/pan%7E.c](https://github.com/pure-data/externals-howto/blob/master/example4/pan%7E.c)
* m_pd.h: [https://github.com/pure-data/externals-howto#the-interface-to-pd](https://github.com/pure-data/externals-howto#the-interface-to-pd)

The following sources where used to learn about DSF synthesis:

* [James A. Moorer, "The Synthesis of Complex Audio Spectra by Means of Discrete Summation Formulas", 1976](https://ccrma.stanford.edu/files/papers/stanm5.pdf)
* [Tim Stilson,  Julius Smith, "Alias-Free Digital Synthesis of Classic Analog Waveforms", 1996](https://ccrma.stanford.edu/~stilti/papers/blit.pdf)
* [The Math behind DSF Synthesis](https://www.verklagekasper.de/synths/dsfsynthesis/dsfsynthesis.html)

