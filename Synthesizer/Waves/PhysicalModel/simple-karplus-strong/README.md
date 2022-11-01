## Simple Karplus-Strong synth

This is a polyphonic synthesizer based on the Karplus-Strong algorithm using [JACK](http://www.jackaudio.org/) for audio and [AlsaMidi](http://alsa.opensrc.org/AlsaMidi) for MIDI. At the moment the DSP side of the synthesizer is really simple.

The synthesizer creates simple plucked string sounds.

## Usage

The synth can be started with the command

    $ simple_karplus_strong <midi port>

where \<midi port\> is optional. The default value for it is: hw:1,0,0. The available midi ports can be listed for example using [this](https://ccrma.stanford.edu/~craig/articles/linuxmidi/alsa-1.0/alsarawportlist.c).

The synth is exited by typing anything in the command line and pressing enter.

Please note that you need to have JACK running with for example the [QjackCtl](https://qjackctl.sourceforge.io/) and to connect the output of the synth to your sound card output.

## Motivation

This synthesizer was created for a learning diary for the Aalto University course Audio Signal Processing.

## Installation

The compilation of the project requires the [JACK API](http://www.jackaudio.org/api/) and the [ALSA API](http://www.alsa-project.org/alsa-doc/alsa-lib/index.html).

Compile by running the Makefile with command
    
    $ make

## Possibilities for further development

* Use better strategy for creation of the string pluck
* Use more realistic filtering in the delay line
* Use fractional delays to fine tune the notes of the instrument
* Use better strategy for the termination of notes (now ends the note abruptly after note off)
* Better usage of the APIs used
* Better code
* Maybe some user interface
