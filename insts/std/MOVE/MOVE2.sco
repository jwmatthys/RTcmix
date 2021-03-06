/* MOVE -- Doug Scott's room simulator with moving sound sources
*
* space (dist_to_front, dist_to_right, -dist_to_back, -dist_to_left, height, abs_fact, rvbtime)
*
* mikes(mike_angle, pattern_factor)
* mikes_off()
*
* path(time0,distance0,angle0, time1,distance1,angle1, ..., timeN,distN,angleN)
* or:
* cpath(time0,xcoord0,ycoord0, time1,xcoord1,ycoord1, ..., timeN,xcoordN,ycoordN)
* or:
* makegen(-1, 9, 1024, 1, 30, 0)
* makegen(-2, 9, 1024, 1, 20, 30)
* cparam(1, 2)
*
* threshold(reset_distance)
*
* MOVE (outskip, inskip, dur, amp, dist_between_mikes, rvb_amp)
*
*/

rtsetparams(44100, 2, 4096)
load("MOVE")
rtinput("../../../snd/input.wav");

space(60, 50, -75, -80, 125, 8, 2)

mikes(45.0, 0.5)

path(0, 5, -45, 1, 5, 90)

MOVE(0, 0, DUR(0), 4, 3, 1)

