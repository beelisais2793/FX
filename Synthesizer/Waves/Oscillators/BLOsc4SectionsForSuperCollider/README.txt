The mechanism of this UGen is explained in “Additive synthesis with band-limited oscillator sections”, Peter Pabon & So Oishi, ICMC 2016.
http://www.icmc2016.com/proceedings.pdf#page=31

============================

SuperCollider UGen “BLOsc4section”:

Band-Limited Oscillator UGen that produces waveforms whose spectrum can have up to 4 frequency bands.

See demo video at:
http://sooishi.com/engineering/

============================

How to install:

If SuperCollider is open and the localhost is in use, quit the server.

Move the UGen file with extension .scx and the class definition file with extension .sc to the SuperCollider Extensions folder in the users library for Application Support, e.g. /Users/user/Library/Application Support/SuperCollider/ExtensionsIf the Extensions folder does not exist, it should be made.
Move the help file with extension .schelp to the Extensions folder in HelpSource/Classes, e.g. /Users/user/Library/Application Support/SuperCollider/Extensions/HelpSource/Classes(Sorry, help file is not yet written)
Recompile the class library (Language > Recompile Class Library) and reboot the server within SuperCollider IDE.
In case SuperCollider is unhappy dealing with new UGens, quit SuperCollider and load it again.

============================

SuperCollider demo code:

SuperCollider file with extension .scd demonstrates the basic function of this UGen.

============================

Arguments and default values:

freq = 440.0,

loHarmonics1 = 1.0,
hiHarmonics1 = 5.0,
slope1 = 1.0,
evenOddRatio1 = 1.0,
spread1 = 1,
sectionAmp1 = 1.0,

loHarmonics2 = 0.0,
hiHarmonics2 = 0.0,
slope2 = 1.0,
evenOddRatio2 = 1.0,
spread2 = 1,
sectionAmp2 = 1.0,

loHarmonics3 = 0.0,
hiHarmonics3 = 0.0,
slope3 = 1.0,
evenOddRatio3 = 1.0,
spread3 = 1,
sectionAmp3 = 1.0,

loHarmonics4 = 0.0,
hiHarmonics4 = 0.0,
slope4 = 1.0,
evenOddRatio4 = 1.0,
spread4 = 1,
sectionAmp4 = 1.0,

spreadCompensation = 1,

mul = 1.0,
add = 0.0

Activate a section by giving non-zero value to both Lowest or Highest Harmonic Index.
Deactivate a section by giving “0” to either Lowest or Highest Harmonic Index.


The best way to know these arguments is to try demo code (especially the part called “Demonstration with Gui”).


============================

Modifying the software:

The C++ code for the plugin and CMakeLists.txt file are included. You can use them to modify the software.


============================

License:

Copyright (c) 2016 So Oishi and Peter Pabon. All rights reserved.
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or any later version.


============================

Author:

So Oishi<oishiso@gmail.com>
sooishi.com