
set(bandlimited_sawtooth_synthesis_sources
    source/BLITSaw_processor.h
    source/BLITSaw_controller.cpp
    source/BLITSaw_controller.h
    source/BLITSaw_entry.cpp
    source/BLITSaw_guids.h
    source/BLITSaw_oscillator.cpp
    source/BLITSaw_oscillator.h
    source/BLITSaw_processor.cpp
)

set(target bandlimited_sawtooth_synthesis)
smtg_add_vst3plugin(${target} ${bandlimited_sawtooth_synthesis_sources})
set_target_properties(${target} PROPERTIES ${SDK_IDE_PLUGIN_EXAMPLES_FOLDER})
target_link_libraries(${target} PRIVATE sdk)
