#include "../../include/timer.h"
#include "../../include/defs.h"
#include "../../include/stdio.h"
#include "../../include/video.h"
#include "../../include/string.h"

#define LAYOUT_LENGTH 30

static long screen_saver=1;
static long tick_waiter=0;
static int is_waiting=0;
static char * saver_pattern = DEFAULT_LAYOUT;
static int from_preview = 0;
static char newLayout[LAYOUT_LENGTH];

void timerHandler(){
/*
	int tick = tick_waiter;
	int save = screen_saver;

		if( tick > save)
			screenSaver();
	tick_waiter ++;
	kprint("tick");*/

	return;
}

void saverInit()
{
	setScreensaver(DEFAULT_TIME);
	return;

}

void setScreensaver (int seconds)
{
	screen_saver=seconds * (TICK_RATIO);
	resetWaiter();
	return;
}

int getScreensaver()
{
	return screen_saver;
}

void resetWaiter ()
{
	tick_waiter=0;
	is_waiting=0;
	return;
}

void screenSaver()
{
	int flag = is_waiting;
//	int tick = tick_waiter;
	from_preview = 0;
	int i;

	if(!flag)
	{
		setPage(SSVER_PAGE);
		clearScreen();

	}

	for ( i = 0; i< 4; i++)
		kprint(saver_pattern);

	is_waiting=1;
	return;
}

void resetScreenSaver()
{
	int flag = is_waiting;

	if(flag)
	{
		setPage(WORK_PAGE);
		is_waiting=0;
	}
	resetWaiter();
	return;
}


void previewSaver()
{
	int i;
	setPage(SSVER_PAGE);
	clearScreen();
	while ( i < 400 )
	{
		kprint(saver_pattern);
		i++;
	}
	is_waiting = 1;
	resetScreenSaver();
	return;
}


void setCounter( int count)
{
	tick_waiter = count * (TICK_RATIO);
	return;
}

void saverLayout ( char * layout )
{

	int i;
	for(i=0;i<LAYOUT_LENGTH;i++)
		newLayout[i]=0x00;

	memcpy(newLayout, layout, strlen(layout));
	saver_pattern = newLayout ;
	return;
}
