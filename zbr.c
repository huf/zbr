#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <curses.h>

#include "frames.h"

//#define FRAME_DELAY 20000
#define FRAME_DELAY 40000


typedef unsigned int (*tickfunction)(int*, int*, unsigned int, unsigned int, int*);
unsigned int tfn_simple(int *x, int *y, unsigned int dir, unsigned int tick, int  *no_touch);
unsigned int tfn_stride(int *x, int *y, unsigned int dir, unsigned int tick, int  *no_touch);
unsigned int tfn_poop(int *x, int *y, unsigned int dir, unsigned int tick, int  *no_touch);

int one_frame(int *x, int *y, unsigned int dir, unsigned int tick, tickfunction tickfn);

int main(int argc, char *argv[])
{
	unsigned int dir = D_LEFT,
				 tick = 0;
	int	x,
		y;

	tickfunction tickfn = tfn_poop;

	signal(SIGINT, SIG_IGN);
	initscr();

	y = LINES-NR_H;
	x = COLS-1;

	noecho();
	leaveok(stdscr, TRUE);
	scrollok(stdscr, FALSE);
	curs_set(0);

	for (tick = 0; ; tick++) {
		if (!one_frame(&x, &y, dir, tick, tickfn)) {
			if (dir == D_LEFT) {
				dir = D_RIGHT;
				x++;
			}
			else {
				break;
			}
		}
		refresh();
		usleep(FRAME_DELAY);
	}

	mvcur(0, COLS - 1, LINES - 1, 0);
	endwin();
	return 0;
}

int one_frame(int *x, int *y, unsigned int dir, unsigned int tick, tickfunction tickfn)
{
	int h, w;
	if ((*x<-NR_W) || *x == COLS+NR_W) {
		tickfn(NULL, NULL, dir, tick, NULL);
		return 0;
	}

	int no_touch = 0;

	unsigned int frameno = tickfn(x, y, dir, tick, &no_touch);

	for (h = 0; h < NR_H; h++) {
		for (w = 0; w < NR_W; w++) {
			mvaddch(*y+h, *x+w, frames[dir][frameno][h][w]);
		}
	}

	if (!no_touch) {
		if (dir == D_LEFT) {
			--*x;
		}
		else {
			++*x;
		}
	}

	return 1;
}

unsigned int tfn_simple(int *x, int *y, unsigned int dir, unsigned int tick, int *no_touch)
{
	if (x == NULL) {
		return 0;
	}
	return tick&1;
}

unsigned int tfn_stride(int *x, int *y, unsigned int dir, unsigned int tick, int *no_touch)
{
	if (x == NULL) {
		return 0;
	}
	return (tick&0x7) < 4;
}

unsigned int tfn_poop(int *x, int *y, unsigned int dir, unsigned int tick, int *no_touch)

{
	static int sitting = 0, counter = 0;
	if (x == NULL) {
		sitting = 0;
		return 0;
	}
	if (*x+(NR_W/2)==COLS/2 && !sitting) {
		sitting = 1;
		counter = 50;
	}

	if (sitting && counter > 0) {
		counter--;
		*no_touch = 1;
		int offs;
		if (counter <= 25) {
			offs = 4;
		}
		else {
			offs = 2;
		}
		return offs+tfn_stride(x, y, dir, tick, no_touch);
	}
	return tfn_stride(x, y, dir, tick, no_touch);
}
