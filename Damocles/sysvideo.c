/*
 * sysvideo.c
 *
 *  Created on: Oct 7, 2009
 */

#include "include/sys.h"
#include "include/sched.h"
#include "drivers/video/crtc6845.h"

void syssetCursor(coord_t *t){
	_vtsetcursor(schedAttachedTTY(), *t);
}

void sysgetCursor(coord_t *t){
	*t = _vgetcursor(schedAttachedTTY());
}

