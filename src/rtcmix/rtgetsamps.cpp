/* RTcmix  - Copyright (C) 2000  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
/* rev'd for v2.3, JGG, 20-Feb-00 */
/* rev'd for v3.7 (converted to C++ source), DS, April-04 */

#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <globals.h>
#include <prototypes.h>
#include <buffers.h>
#include "../rtstuff/rtdefs.h"
#include "AudioDevice.h"
#include "Option.h"

/* ----------------------------------------------------------- rtgetsamps --- */
void
rtgetsamps(AudioDevice *inputDevice)
{
	assert(options.record() == true);
	if (inputDevice->getFrames(audioin_buffer, RTBUFSAMPS) < 0)
	{
		fprintf(stderr, "rtgetsamps error: %s\n", inputDevice->getLastError());
	}
}


