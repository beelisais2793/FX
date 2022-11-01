import sys
import jack

UDUK_APP = str(sys.argv[1])

client = jack.Client('Connection')

PLAYBACK_1 = "system:playback_1"
PLAYBACK_2 = "system:playback_2"

'''
CAPTURE_1 = "system:capture_1"
CAPTURE_2 = "system:capture_2"
PULSE_SOURCE_RIGHT = "PulseAudio JACK Source:front-right"
PULSE_SOURCE_LEFT = "PulseAudio JACK Source:front-left"
PULSE_SINK_RIGHT = "PulseAudio JACK Sink:front-right"
PULSE_SINK_LEFT = "PulseAudio JACK Sink:front-left"
'''

AMSYNTH_R = "amsynth:R out"
AMSYNTH_L = "amsynth:L out"
GUITARIX_0 = "gx_head_fx:out_0"
GUITARIX_1 = "gx_head_fx:out_1"

GUITAR_IN = "guitar_in"
GUITAR_OUT = "guitar_out"
SYNTH_IN =  "synth_in"
SYNTH_OUT = "synth_out"

GUITAR_PORT_IN = UDUK_APP + ":" + GUITAR_IN
GUITAR_PORT_OUT = UDUK_APP + ":" + GUITAR_OUT
SYNTH_PORT_IN = UDUK_APP + ":" + SYNTH_IN
SYNTH_PORT_OUT = UDUK_APP + ":" + SYNTH_OUT

# Reset

client.disconnect(AMSYNTH_R, PLAYBACK_2)
client.disconnect(AMSYNTH_L, PLAYBACK_1)

# Input

client.connect(GUITARIX_0, GUITAR_PORT_IN)
client.connect(GUITARIX_1, GUITAR_PORT_IN)

client.connect(AMSYNTH_R, SYNTH_PORT_IN)
client.connect(AMSYNTH_L, SYNTH_PORT_IN)

# Playback

client.connect(GUITAR_PORT_OUT, PLAYBACK_1)
client.connect(SYNTH_PORT_OUT, PLAYBACK_2)
