/*

o   o o-o   o   o o  o 
|   | |  \  |   | | /  
|   | |   O |   | OO   
|   | |  /  |   | | \  
 o-o  o-o    o-o  o  o  Uduk MIDI v1.0

[1] https://www.w3.org/TR/webmidi/
[2] http://www.dvfugit.com/beats-per-minute-millisecond-delay-calculator.php

*/

var UdukMIDI = (function () {

  var _MIDIoutput;

  var _onMIDISuccess = function (midiAccess) {

    _MIDIoutput = midiAccess.outputs.values().next().value;

    midiAccess.outputs.forEach(function(port) {
      var name = port.name;
      var manufacturer = port.manufacturer;
      var id = port.id;
      var state = port.state;
      console.log(name, manufacturer, id, state);
    });
    
    console.log("%c UDUK MIDI v1.0", "background: #000; color: #fff");
    console.log(midiAccess);
  };

  var _onMIDIFailure = function (e) {
    console.log("MIDI device not found: " + e);
  };
 
  var initialize = function () {
    if (navigator.requestMIDIAccess) {
      navigator.requestMIDIAccess({
        sysex: false
      }).then(_onMIDISuccess, _onMIDIFailure);
    } 
    else {
      console.log("No MIDI support in your browser.");
    }
  };

  var playSequence = function (sequence, timing) {
    var c = 0;
    var timer_t = setInterval(intervalSequence, timing);

    function intervalSequence() {
      _MIDIoutput.send([0x90, sequence[c], 0x7f]);
      _MIDIoutput.send([0x80, sequence[c], 0x40], window.performance.now() + timing);
      if (c++ >= sequence.length - 1) {
       clearInterval(timer_t);
      }
    }
  };
  
  var sendMessage = function (message) {
    var timer_t = setInterval(intervalMsg, 300);
    function intervalMsg() {
      _MIDIoutput.send(message);
       clearInterval(timer_t);
    }
  };
  
  var bpmTomillisecond = function (bpm) {
    return (60000/bpm);
  };

  var restTiming = function (bpm) {
    var qtrN        = Math.round(((60/bpm)*1000)*100000)/100000;
    var wholeN      = (qtrN*4);
    var halfN       = (qtrN*2);
    var eighthN     = (qtrN/2);
    var sixteenN    = (qtrN/4);
    var thirtyTwoN  = (qtrN/8);
    var sixtyFourN  = (qtrN/16);
    var sixN        = (wholeN/6);
    var twelveN     = (wholeN/12);
    var twentyFourN = (wholeN/24);

    qtrN        = Math.round(qtrN*100000)/100000; 
    wholeN      = Math.round(wholeN*100000)/100000; 
    halfN       = Math.round(halfN*100000)/100000; 
    eighthN     = Math.round(eighthN*100000)/100000; 
    sixteenN    = Math.round(sixteenN*100000)/100000; 
    thirtyTwoN  = Math.round(thirtyTwoN*100000)/100000; 
    sixtyFourN  = Math.round(sixtyFourN*100000)/100000; 
    sixN        = Math.round(sixN*100000)/100000; 
    twelveN     = Math.round(twelveN*100000)/100000; 
    twentyFourN = Math.round(twentyFourN*100000)/100000; 

    var timing = {
      'qtrN': qtrN, 'wholeN': wholeN, 'halfN': halfN, 'eighthN': eighthN, 'sixteenN': sixteenN, 
      'thirtyTwoN': thirtyTwoN, 'sixtyFourN': sixtyFourN, 'sixN': sixN, 'twelveN': twelveN, 'twentyFourN':twentyFourN 
    };

    return timing
  };

  return {
    initialize        : initialize,
    playSequence      : playSequence,
    sendMessage       : sendMessage,
    bpmTomillisecond  : bpmTomillisecond,
    restTiming        : restTiming
  };

})();
