cmake_minimum_required (VERSION 3.0)
project (Synthesizer)

add_subdirectory (libs)

file(GLOB SOURCE
 "src/main.cpp"

 "src/utils/gpio/gpio.hpp"
 "src/utils/gpio/gpio.cpp"

 "src/utils/audio/Audio.hpp"

 "src/utils/midi/Midi.hpp"
 "src/utils/midi/Midi.cpp"

 "src/utils/gui/guilib.hpp"

 "src/components/Effect.hpp"
 "src/components/Effect.cpp"
 "src/components/Generator.hpp" 

 "src/components/effects/Delay.hpp"
 "src/components/effects/Delay.cpp"
 "src/components/effects/Flanger.hpp"
 "src/components/effects/Flanger.cpp"
 "src/components/effects/Phaser.hpp"
 "src/components/effects/Phaser.cpp"
 "src/components/effects/Chorus.hpp"
 "src/components/effects/Chorus.cpp"
 "src/components/effects/Reverb.hpp"
 "src/components/effects/Reverb.cpp"

 "src/components/envelopes/Envelope.hpp"
 "src/components/envelopes/Envelope.cpp"
 "src/components/envelopes/ADSR.hpp"
 "src/components/envelopes/ADSR.cpp"

 "src/components/filters/Filter.hpp"
 "src/components/filters/Filter.cpp"
 "src/components/filters/LPF.hpp"
 "src/components/filters/LPF.cpp"
 "src/components/filters/HPF.hpp"
 "src/components/filters/HPF.cpp"

 "src/components/generators/Oscillator.hpp"
 "src/components/generators/Oscillator.cpp"
 "src/components/generators/Wavetables.hpp"
 "src/components/generators/Wavetables.cpp"
)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")


	set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
	set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if (UNIX)
	find_package(ALSA REQUIRED)
	
	include_directories(
		libs/rtmidi/
	)

	add_executable(Synthesizer
		${SOURCE}
	)

	target_link_libraries(Synthesizer
		${ALSA_LIBRARY}
		rtmidi
	)
endif (UNIX)


if (WIN32)

	add_executable(Synthesizer WIN32
		${SOURCE}
	)

	target_include_directories(Synthesizer PUBLIC
		libs/rtmidi/
		libs/portaudio/include/
	)
	
	target_link_libraries(Synthesizer
		portaudio
		rtmidi
		dwrite
	)
endif (WIN32)