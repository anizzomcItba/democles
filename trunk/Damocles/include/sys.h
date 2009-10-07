#include "defs.h"
#include "../drivers/video/crtc6845.h"

#ifndef _DEFS_H_
#define _DEFS_H_

void timer_tick_handler();

void syssetCursor(coord_t *t);

void sysgetCursor(coord_t *t);

#endif
