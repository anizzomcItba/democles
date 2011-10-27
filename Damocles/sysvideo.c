/*
 * sysvideo.c
 *
 *  Created on: Oct 7, 2009
 */

#include "sys.h"
#include "sched.h"
#include "process.h"
#include "system/drivers/video/crtc6845.h"

void syssetCursor(coord_t *t){
	_vtsetcursor(procAttachedTTY(schedCurrentProcess()), *t);
}

void sysgetCursor(coord_t *t){
	*t = _vgetcursor(procAttachedTTY(schedCurrentProcess()));
}

