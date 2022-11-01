%{
#include "SKINIMsg.h"
%}

// you can edit these if you don't like the way it is laid out but it has to be renamed to something
%constant int skini_nope = NOPE;
%constant int skini_yep  = YEP;
%constant int skini_dbl  = SK_DBL;
%constant int skini_int  = SK_INT;
%constant int skini_str  = SK_STR;
%constant int skini_exit  = __SK_Exit;

%constant int skini_noteoff = __SK_NoteOff_;
%constant int skini_noteon  = __SK_NoteOn_;
%constant int skini_polypressure = __SK_PolyPresssure_;
%constant int skini_controlchange = __SK_ControlChange_;
%constant int skini_programchange = __SK_ProgramChange_;
%constant int skini_aftertouch = __SK_AfterTouch_;
%constant int skini_channelpressure = __SK_ChannelPressure_;
%constant int skini_pitchwheel = __SK_PitchWheel_;
%constant int skini_pitchbend = __SK_PitchBend_;
%constant int skini_pitchchange = __SK_PitchChange_;

%constant int skini_clock = __SK_Clock_;
%constant int skini_songstart = __SK_SongStart_;
%constant int skini_continue = __SK_Continue_;
%constant int skini_songstop = __SK_SongStop_;
%constant int skini_activesensing = __SK_ActiveSensing_;
%constant int skini_systemreset = __SK_SystemReset_;

%constant int skini_volume = __SK_Volume_;
%constant int skini_modwheel = __SK_ModWheel_;
%constant int skini_modulation = __SK_Modulation_;
%constant int skini_breath = __SK_Breath_;
%constant int skini_footcontrol = __SK_FootControl_;
%constant int skini_portamento = __SK_Portamento_;
%constant int skini_balance = __SK_Balance_;\
%constant int skini_pan = __SK_Pan_;
%constant int skini_sustain = __SK_Sustain_;
%constant int skini_damper = __SK_Damper_;
%constant int skini_expression = __SK_Expression_;

%constant int skini_aftertouch_cont = __SK_AfterTouch_Cont_;
%constant int skini_modfrequency    = __SK_ModFrequency_;

// probably not useful unless you have one
%constant int skini_prophesyribbon = __SK_ProphecyRibbon_;
%constant int skini_prophesywheelup = __SK_ProphecyWheelUp_;
%constant int skini_prophesywheeldown = __SK_ProphecyWheelDown_;
%constant int skini_prophesypedal = __SK_ProphecyPedal_;
%constant int skini_prophesyknob1 = __SK_ProphecyKnob1_;
%constant int skini_prophesyknob2 = __SK_ProphecyKnob2_;

%constant int skini_noiselevel = __SK_NoiseLevel_;
%constant int skini_pickposition = __SK_PickPosition_;
%constant int skini_stringdamping = __SK_StringDamping_;
%constant int skini_stringdetune = __SK_StringDetune_;
%constant int skini_bodysize = __SK_BodySize_;
%constant int skini_bowpressure = __SK_BowPressure_;
%constant int skini_bowposition = __SK_BowPosition_;
%constant int skini_bowbeta = __SK_BowBeta_;


%constant int skini_reedstifness = __SK_ReedStiffness_;
%constant int skini_reedrestpos = __SK_ReedRestPos_;

%constant int skini_fluteembouchre = __SK_FluteEmbouchure_;
%constant int skini_jetdelay = __SK_JetDelay_;

%constant int skini_liptension = __SK_LipTension_;
%constant int skini_slidelength = __SK_SlideLength_;

%constant int skini_strikeposition = __SK_StrikePosition_;
%constant int skini_strikehardness = __SK_StrikeHardness_;

%constant int skini_trilldepth = __SK_TrillDepth_;
%constant int skini_trillspeed = __SK_TrillSpeed_;
%constant int skini_strumdepth = __SK_StrumDepth_;
%constant int skini_rollspeed = __SK_RollSpeed_;


%constant int skini_filterq = __SK_FilterQ_;
%constant int skini_filterfreq = __SK_FilterFreq_;
%constant int skini_filtersweeprate = __SK_FilterSweepRate_;


%constant int skini_shakerinst = __SK_ShakerInst_;
%constant int skini_shakerenergy = __SK_ShakerEnergy_;
%constant int skini_shakerdamping = __SK_ShakerDamping_;
%constant int skini_shakernumobjects = __SK_NumObjects_;

%constant int skini_strumming = __SK_Strumming_;
%constant int skini_notstrumming = __SK_NotStrumming_;
%constant int skini_trilling = __SK_Trilling_;
%constant int skini_nottrilling = __SK_NotTrilling_;
%constant int skini_rolling = __SK_Rolling_;
%constant int skini_notrolling = __SK_NotRolling_;

%constant int skini_playerskill = __SK_PlayerSkill_;
%constant int skini_chord = __SK_Chord_;
%constant int skini_chordoff = __SK_ChordOff_;

%constant int skini_singer_filepath = __SK_SINGER_FilePath_;
%constant int skini_singer_frequency = __SK_SINGER_Frequency_;
%constant int skini_singer_notename = __SK_SINGER_NoteName_;
%constant int skini_singer_shape = __SK_SINGER_Shape_;
%constant int skini_singer_glot = __SK_SINGER_Glot_;
%constant int skini_singer_voicedunvoiced = __SK_SINGER_VoicedUnVoiced_;
%constant int skini_singer_synthesize = __SK_SINGER_Synthesize_;
%constant int skini_singer_silence = __SK_SINGER_Silence_;
%constant int skini_singer_vibratoamt = __SK_SINGER_VibratoAmt_;
%constant int skini_singer_rndvibamt = __SK_SINGER_RndVibAmt_;
%constant int skini_singer_vibfreq = __SK_SINGER_VibFreq_;


struct SkiniSpec { char messageString[32];
                   long  type;
                   long data2;
                   long data3;
                 };

    
struct SkiniSpec skini_msgs[80];