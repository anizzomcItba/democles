/*
 * sysvideo.c
 *
 *  Created on: Oct 7, 2009
 */

#include "include/sys.h"
#include "include/sched.h"
#include "include/process.h"
#include "drivers/video/crtc6845.h"

void syssetCursor(coord_t *t){
	_vtsetcursor(procAttachedTTY(schedCurrentProcess()), *t);
}

void sysgetCursor(coord_t *t){
	*t = _vgetcursor(procAttachedTTY(schedCurrentProcess()));
}

