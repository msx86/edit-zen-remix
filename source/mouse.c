/* ------------- mouse.c ------------- */

#include "dfpcomp.h"

static union REGS regs;

int ui_update = 0;

static void near mouse(int m1,int m2,int m3,int m4)
{
    regs.x.dx = m4;
    regs.x.cx = m3;
    regs.x.bx = m2;
    regs.x.ax = m1;
    int86(MOUSE, &regs, &regs);
}

static BOOL mouse_present = FALSE;

/* ---------- reset the mouse ---------- */
void resetmouse(void)
{
    mouse(0,0,0,0);
    mouse_present = (regs.x.ax != 0);
}

/* ----- test to see if the mouse driver is installed ----- */
BOOL mouse_installed(void)
{
    return mouse_present;
}

/* ------ return true if mouse buttons are pressed ------- */
int mousebuttons(void)
{
    if (mouse_installed())	{
        mouse(3,0,0,0);
	    return regs.x.bx & 3;
	}
	return 0;
}

/* ---------- return mouse coordinates ---------- */
void get_mouseposition(int *x, int *y)
{
	*x = *y = -1;
    if (mouse_installed())    {
        mouse(3,0,0,0);
        *x = regs.x.cx/8;
        *y = regs.x.dx/8;
    }
}

/* -------- position the mouse cursor -------- */
void set_mouseposition(int x, int y)
{
    if (mouse_installed() && !ui_update)	{
        mouse(4,0,x*8,y*8);
	}
}

/* --------- display the mouse cursor -------- */
void show_mousecursor(void)
{
    if (mouse_installed())	{
    	if (!ui_update) {
    	    mouse(1,0,0,0);
    	}
	}
}

/* --------- hide the mouse cursor ------- */
void hide_mousecursor(void)
{
    if (mouse_installed() && !ui_update)	{
    	mouse(2,0,0,0);
	}
}

void hide_mousecursor_in_rect(RECT rc)
{
	if (mouse_installed() && !ui_update) {
		regs.x.ax = 0x10;
    	regs.x.cx = rc.lf*8;
    	regs.x.dx = rc.tp*8;
    	regs.x.si = rc.rt*8+7;
    	regs.x.di = rc.bt*8+7;
    	int86(MOUSE, &regs, &regs);
	}
}

/* --- return true if a mouse button has been released --- */
int button_releases(void)
{
    if (mouse_installed())	{
        mouse(6,0,0,0);
	    return regs.x.bx;
	}
	return 0;
}

/* ----- set mouse travel limits ------- */
void set_mousetravel(int minx, int maxx, int miny, int maxy)
{
    if (mouse_installed())	{
        mouse(7, 0, minx*8, maxx*8);
		mouse(8, 0, miny*8, maxy*8);
	}
}
