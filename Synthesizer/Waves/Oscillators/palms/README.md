## palms

This is a six-voice sawtooth drone.

Each voice (`SawVoice`) is composed of of three detuned saw waveforms (`SawDetuned`). Each detuned saw waveform is mmade of two sawtooth oscillators (`Saw`) which are tuned close to the requested frequency.

## ideas

target supercollider oscilators:

```
(
{
	var hz=[55.midicps];
	var amp=0.5;
	var snd,fx;
	// convert hertz into midi notes
	snd=Mix.ar({
		var snd;
		snd=Saw.ar(hz*Rand(0.997,1.003));
		snd=LPF.ar(snd,LinExp.kr(SinOsc.kr(Rand(1/30,1/10),Rand(0,2*pi)),-1,1,hz/2,12000));
		//snd=DelayN.ar(snd, mdelay, LFNoise1.kr(Rand(1,10)).range(0.001,mdelay/5));
		snd=Pan2.ar(Mix.new(snd),SinOsc.kr(Rand(0.01,0.1),Rand(0,2*pi)))/4;
		snd/2
	}!6);
	snd=LPF.ar(snd.tanh,SinOsc.kr(Rand(1/30,1/10),Rand(0,2*pi)).range(hz*4,hz*8));
	//snd=MoogLadder.ar(snd.tanh,LinExp.kr(VarLag.kr(LFNoise0.kr(1/6),6,warp:\sine),-1,1,hz*8,hz*60));
	snd=HPF.ar(snd,40);
	snd=snd*EnvGen.ar(Env.perc(5,5),doneAction:2);
	//snd=Greyhole.ar(snd*MouseX.kr());
	// snd=BLowPass.ar(snd,hz*4,0.707);
	Out.ar(0,snd*amp);
}.play;
)

(
{
	LFNoise1.kr(100)
}.plot(0.1)
)
```

##vim editing

```
filetype plugin indent on

function FormatBuffer()
   if &modified && !empty(findfile('.clang-format', expand('%:p:h') . ';'))
    let cursor_pos = getpos('.')
    :%!clang-format
    call setpos('.', cursor_pos)
  endif
endfunction

autocmd BufWritePre *.h,*.hpp,*.c,*.cpp,*.vert,*.frag :call FormatBuffer()
```

~/.config/serialosc/m1001055.conf

```
server {
  port = 14656
}
application {
  osc_prefix = "/monome"
  host = "127.0.0.1"
  port = 8000
}
device {
  rotation = 0
}
```
