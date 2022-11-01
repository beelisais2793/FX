FaustGxAmpoutLadspa : UGen
{
  *ar { | in1, amp_on_off(0.0), amp_out_amp(0.0), amp_out_ladspa(0.0) |
      ^this.multiNew('audio', in1, amp_on_off, amp_out_amp, amp_out_ladspa)
  }

  *kr { | in1, amp_on_off(0.0), amp_out_amp(0.0), amp_out_ladspa(0.0) |
      ^this.multiNew('control', in1, amp_on_off, amp_out_amp, amp_out_ladspa)
  } 

  checkInputs {
    if (rate == 'audio', {
      1.do({|i|
        if (inputs.at(i).rate != 'audio', {
          ^(" input at index " + i + "(" + inputs.at(i) + 
            ") is not audio rate");
        });
      });
    });
    ^this.checkValidInputs
  }

  name { ^"FaustGxAmpoutLadspa" }


  info { ^"Generated with Faust" }
}

