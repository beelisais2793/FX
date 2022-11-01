
# Pine Synthesizer

**A Physical Modeling Synth based on the Daisy pod and Daisy platform**

*3/7/2022 Brian Gunnison*

[Music](https://soundcloud.com/briang-1-1)

[Virtual Robot](https://virtualrobot.net/)

## Overview

An experimental software platform based on the Daisy Pod HW and software libraries:

[Daisy Pod](https://www.electro-smith.com/daisy/pod)

[Daisy Forum](https://forum.electro-smith.com/)

[Daisy DSP Library](https://electro-smith.github.io/DaisySP/index.html)

Trying to boil down the complexity of a sound module to something I can perform with live. 
Imagine a MIDI drum pad with a MIDI foot controller. No screen, knobs or embedded menus.
The Pine module can be (for example) configured to play only a D major pentatonic scale, then a E minor pentatonic scale etc. 

**Software Features:**
1. Extensible voice selection and control
2. Extensible filter selection and control
3. MIDI mapping includes CC and note mapping. Soon to come uploadable MIDI maps to map your favorite controller. 
4. CPU usage bounded polyphony

## Development

C++ with Visual Studio 2019 and Visual GDB. [STLINK-V3MINI](https://www.st.com/resource/en/user_manual/dm00555046-stlink-v3mods-and-stlink-v3mini-mini-debuggers-programmers-for-stm32-stmicroelectronics.pdf) debugger/programmer.

**Debugging**

The Python file "serial_monitor.py" connects to the USB serial output of the software for log messages. 

**Directories**

spring <- this project adjacent to the below from Daisy

[daisyexamples](https://github.com/electro-smith/DaisyExamples) 

## Contributing
Pull requests are welcome. 

## License
[MIT](https://choosealicense.com/licenses/mit/)

