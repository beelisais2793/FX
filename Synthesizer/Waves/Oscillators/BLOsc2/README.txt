How to install
-------
If SuperCollider is open and the localhost is in use, quit the server.

Move the UGen file with extension .scx and the class definition file with extension .sc to the SuperCollider Extensions folder in the users library for Application Support, e.g. /Users/user/Library/Application Support/SuperCollider/ExtensionsIf the Extensions folder does not exist, it should be made.
Move the help file with extension .schelp to the Extensions folder in HelpSource/Classes, e.g. /Users/user/Library/Application Support/SuperCollider/Extensions/HelpSource/ClassesRecompile the class library (Language > Recompile Class Library) and reboot the server within SuperCollider IDE.
In case SuperCollider is unhappy dealing with new UGens, quit SuperCollider and load it again.


SuperCollider demo code
-------
SuperCollider file with extension .scd demonstrates the basic function of this UGen.


Modifying the software
-------
The C++ code for the plugin and CMakeLists.txt file are included. You can use them to modify the software.


License
-------
Copyright (c) 2014 So Oishi. All rights reserved.
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or any later version.


Author
-------
So Oishi<oishiso@gmail.com>