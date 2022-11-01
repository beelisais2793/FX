FaustGxOutputlevelLadspa : MultiOutUGen
{
  *ar { | in1, in2, amp_out_master(0.0), amp_out_master_ladspa(0.0) |
      ^this.multiNew('audio', in1, in2, amp_out_master, amp_out_master_ladspa)
  }

  *kr { | in1, in2, amp_out_master(0.0), amp_out_master_ladspa(0.0) |
      ^this.multiNew('control', in1, in2, amp_out_master, amp_out_master_ladspa)
  } 

  checkInputs {
    if (rate == 'audio', {
      2.do({|i|
        if (inputs.at(i).rate != 'audio', {
          ^(" input at index " + i + "(" + inputs.at(i) + 
            ") is not audio rate");
        });
      });
    });
    ^this.checkValidInputs
  }

  init { | ... theInputs |
      inputs = theInputs
      ^this.initOutputs(2, rate)
  }

  name { ^"FaustGxOutputlevelLadspa" }


  info { ^"Generated with Faust" }
}

