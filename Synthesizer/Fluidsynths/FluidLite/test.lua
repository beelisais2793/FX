require('fluidlite')

settings = fluidlite.new_fluid_settings()
synth    = fluidlite.new_fluid_synth(settings)
fluidlite.fluid_synth_sfload(synth,"test.sf2",1)