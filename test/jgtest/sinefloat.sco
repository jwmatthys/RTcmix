rtsetparams(44100, 1)
load("WAVETABLE")
set_option("audio_off", "clobber_on")
rtoutput("test.snd", "next", "float")
setline(0,0, 1,1, 19,1, 20,0)
makegen(2,10,10000, 1)
WAVETABLE(0, dur=10, amp=10000, freq=440)
