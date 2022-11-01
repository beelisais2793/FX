require('fluidlite')

settings = fluidlite.new_fluid_settings()
synth    = fluidlite.new_fluid_synth(settings)
fluidlite.fluid_synth_sfload(synth,"piano.sf2",1)
