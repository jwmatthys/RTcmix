// HELMHOLTZ - FFT-based Autocorrelation pitch tracker
// Adapted from a design by Katja
// http://www.katjaas.nl/helmholtz/helmholtz.html
//
//   p[0]: outskip
//   p[1]: inskip
//   p[2]: duration
//   p[3]: output amp
//   p[4]: fidelity threshold 0-1, default 0.95
//   p[5]: maximum frequency, default 1000
//   p[6]: inchan, default 0
//   p[7]: sensitivity (minimum RMS) 0-1, default 0.003
//   p[8]: overlap (int), default 1
//   p[9]: bias, default 0.2
//
//   Joel Matthys <joel at matthys music dot com> 20 June 2013

rtsetparams(44100,2,2048)
// the buffer size makes a difference with HELMHOLTZ

load("WAVETABLE")
load("HELMHOLTZ")

rtinput("/home/jwmatthys/Music/detour_rhsl.wav")

outskip = 0
inskip = 0
dur = DUR()
// HELMHOLTZ is a pass-thru instrument; amp=1 will
// pass the input to the output unaltered.
// Of course, if you just want HELMHOLTZ to track
// pitch, set the amp to 0.
amp = maketable("window",1000,1)
// fidelity threshold sets how certain you want HELMHOLTZ
// to be about pitch reporting. 0 means always report a
// pitch. Default is 0.95
fidelity_threshold = 0.9
// maxfreq sets the top frequency HELMHOLTZ will attempt
// to report. An internal lowpass filter is set at this
// frequency. Default is 1000 Hz
maxfreq = 1000
inchan = 0 // input channel to read
// sensitivity is the minimum RMS to track frequency.
// Default is 0.003
sensitivity = 0.005
// overlap is an int value for overlapping frames. Must
// be 1,2,4, or 8. Default is 1.
overlap = 2
// bias is... well, I don't know what bias is. Default
// is 0.2
bias = 0.2

// HELMHOLTZ's frequency analysis is accessed as a PField
freq = makeconnection("helmholtz")

HELMHOLTZ(outskip, inskip, dur, amp, fidelity_threshold, maxfreq, inchan, sensitivity, overlap, bias)

// use the HELMHOLTZ frequency data to drive a WAVETABLE
WAVETABLE(0, dur, 15000, freq, 0.5)
