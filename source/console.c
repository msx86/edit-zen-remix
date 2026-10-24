/* ----------- console.c ---------- */

//#include "dflat.h"
#include "dfpcomp.h"

/* ----- table of alt keys for finding shortcut keys ----- */
static int altconvert[] = {
    ALT_A,ALT_B,ALT_C,ALT_D,ALT_E,ALT_F,ALT_G,ALT_H,
    ALT_I,ALT_J,ALT_K,ALT_L,ALT_M,ALT_N,ALT_O,ALT_P,
    ALT_Q,ALT_R,ALT_S,ALT_T,ALT_U,ALT_V,ALT_W,ALT_X,
    ALT_Y,ALT_Z,ALT_0,ALT_1,ALT_2,ALT_3,ALT_4,ALT_5,
    ALT_6,ALT_7,ALT_8,ALT_9
};

unsigned video_mode;
unsigned video_page;

static int near cursorpos[MAXSAVES];
static int near cursorshape[MAXSAVES];
static int cs;

static union REGS regs;

/* ------------- clear the screen -------------- */
void clearscreen(void)
{
	int ht = SCREENHEIGHT;
	int wd = SCREENWIDTH;
	cursor(0, 0);
	regs.h.al = ' ';
	regs.h.ah = 9;
	regs.x.bx = 7;
	regs.x.cx = ht * wd;
	int86(VIDEO, &regs, &regs);
}

void SwapCursorStack(void)
{
	if (cs > 1)	{
		swap(cursorpos[cs-2], cursorpos[cs-1]);
		swap(cursorshape[cs-2], cursorshape[cs-1]);
	}
}

/* ---- BIOS keyboard routines with 84 and 102 key keyboard support ---- */
/* (EDIT 0.7 -ea) */

unsigned Xbioskey(int cmd)
{
    union REGPACK r;
    char kbdtype = *(unsigned char __far *)MK_FP(0x40,0x96) & 0x10;

    memset(&r, 0, sizeof(r));
    r.h.ah = (char) (kbdtype + cmd);
    r.h.al = 0;
    intr(0x16, &r);

    if (cmd == 1) {
        /* zero flag set if no key available */
        return !(r.w.flags & 0x40);
    }
    else {
        return r.w.ax;
    }
}


/* ---- Test for keystroke ---- */
BOOL keyhit(void)
{
    return (Xbioskey(1) ? TRUE : FALSE);
}

/* ---- Read a keystroke ---- */
unsigned getkey(void)
{
    unsigned int c;
#ifndef HOOKKEYB
    unsigned int theShift;
    unsigned int theScan;
#endif
#if 0	/* removed pointless polling loop in 0.7c */
!   while (keyhit() == FALSE);	/* wait for a key */
#endif
#ifdef HOOKKEYB
    if (((c = bioskey(0)) & 0xff) == 0)
        c = (c >> 8) | 0x1080;
    else
        c &= 0xff;
    return c & 0x10ff;
#else

    c = Xbioskey(0); /* fetch key */
    theShift = getshift();
    theScan = c >> 8;	/* scan code */
    c = c & 0xff;	/* ASCII code or 0 of 0xe0 */

    if ( theShift & (LEFTSHIFT|RIGHTSHIFT) ) {
        /* BIOS normally calls shift-ins "ins" and shift-del "del" */
        if (theScan == 0x52) /* INS */
            return CTRL_V; /* SHIFT_INS; */ /* shift-ins is paste, ctrl-v */
        if (theScan == 0x53) /* DEL */
            return CTRL_X; /* SHIFT_DEL; */ /* shift-del is cut, ctrl-x */
    } /* special SHIFT cases */

    if ( (theShift & ALTKEY) && (theScan == 0x0e) ) /* Alt-BS  */
        return CTRL_Z;	/* ALT_BS; */ /* alt-backspace is undo, ctrl-z */

    if (theShift & CTRLKEY) {
        if (theScan == 0x92) {	/* ^ins / ^-numpad-Ins */
            return CTRL_C; /* CTRL_INS;	*/ /* ctrl-ins is copy, ctrl-c */
        }
    } /* special CTRL cases */

    if (  (c != 0) && (c != 0xe0) )	/* nonzero / nonnumpad ASCII part? */
        return c;			/* then return only the ASCII part */

    /* Watch out: special case for Russian non-numpad "0xe0 ASCII" */
    if ( (c == 0xe0) && (theScan == 0) )
        return 0xe0;

    return (FKEY | theScan);	/* else return scancode and a flag */

#endif
}

/* ---------- read the keyboard shift status --------- */
unsigned getshift(void)
{
    char kbdtype = *(unsigned char __far *)MK_FP(0x40,0x96) & 0x10;

    if (!kbdtype) { /* old/new by Eric */

        regs.h.ah = 2;
        int86(KEYBRD, &regs, &regs);
        return regs.h.al;
    } else { /* new by Eric 11/2002 */

        regs.h.ah = 0x12; /* extended shift: AL as above... */
        int86(KEYBRD, &regs, &regs);
        /* ignore SysRQ (Alt-PrtScr) and shift lock presses */
        regs.x.ax &= 0x0fff;
        /* treat RALT as NO ALT (but as AltGr): */
        if (regs.x.ax & RALTKEY)
            regs.x.ax &= ~ALTKEY;
        return regs.x.ax;
    }
}

/* ------- macro to wait one clock tick -------- */
#define wait()          \
{                       \
    volatile int now = peek(0x40, 0x6c); \
    while (now == peek(0x40, 0x6c))      \
        ;                                \
}

/* -------- sound a buzz tone, using hardware directly ---------- */
void beep(void)
{
    wait();
    outp(0x43, 0xb6);               /* program the frequency */
    outp(0x42, (int) (COUNT % 256));
    outp(0x42, (int) (COUNT / 256));
    outp(0x61, inp(0x61) | 3);      /* start the sound */
    wait();
    outp(0x61, inp(0x61) & ~3);     /* stop the sound  */
}

static BOOL video_cached = FALSE;

/* -------- get the video mode and page from BIOS -------- */
void videomode(void)
{
    regs.h.ah = 15;
    int86(VIDEO, &regs, &regs);
    video_mode = regs.h.al;
    video_page = regs.x.bx;
    video_page &= 0xff00;
    video_mode &= 0x7f;
    video_cached = TRUE;
}

/* ------ position the cursor ------ */
void cursor(int x, int y)
{
    if (!video_cached)
        videomode();
    if (y >= SCREENHEIGHT) y = SCREENHEIGHT - 1; /* 0.7c */
    regs.x.dx = ((y << 8) & 0xff00) + x;
    regs.h.ah = SETCURSOR;
    regs.x.bx = video_page;
    int86(VIDEO, &regs, &regs);
}

/* ------ get cursor shape and position ------ */
static void near getcursor(void)
{
    if (!video_cached)
        videomode();
    regs.h.ah = READCURSOR;
    regs.x.bx = video_page;
    int86(VIDEO, &regs, &regs);
}

/* ------- get the current cursor position ------- */
void curr_cursor(int *x, int *y)
{
    getcursor();
    *x = regs.h.dl;
    *y = regs.h.dh;
}

/* ------ save the current cursor configuration ------ */
void savecursor(void)
{
    if (cs < MAXSAVES)    {
        getcursor();
        cursorshape[cs] = regs.x.cx;
        cursorpos[cs] = regs.x.dx;
        cs++;
    }
}

/* ---- restore the saved cursor configuration ---- */
void restorecursor(void)
{
    if (cs)    {
        --cs;
        if (!video_cached)
            videomode();
        regs.x.dx = cursorpos[cs];
        if (regs.h.dh >= SCREENHEIGHT)
            regs.h.dh = SCREENHEIGHT - 1;	/* 0.7c */
        regs.h.ah = SETCURSOR;
        regs.x.bx = video_page;
        int86(VIDEO, &regs, &regs);
        set_cursor_type(cursorshape[cs]);
    }
}

/* ------ make a normal cursor ------ */
void normalcursor(void)
{
    set_cursor_type(0x0106);
}

/* ------ hide the cursor ------ */
void hidecursor(void)
{
    getcursor();
    regs.h.ch |= HIDECURSOR;
    regs.h.ah = SETCURSORTYPE;
    int86(VIDEO, &regs, &regs);
}

/* ------ unhide the cursor ------ */
void unhidecursor(void)
{
    getcursor();
    regs.h.ch &= ~HIDECURSOR;
    regs.h.ah = SETCURSORTYPE;
    int86(VIDEO, &regs, &regs);
}

/* ---- use BIOS to set the cursor type ---- */
void set_cursor_type(unsigned t)
{
    if (!video_cached)
        videomode();
    regs.h.ah = SETCURSORTYPE;
    regs.x.bx = video_page;
    regs.x.cx = t;
    int86(VIDEO, &regs, &regs);
}

/* ---- test for VGA -------- */
BOOL isVGA(void)
{
    regs.x.ax = 0x1a00;
    int86(VIDEO, &regs, &regs);
    return regs.h.al == 0x1a && regs.h.bl > 6;
}

static void Scan400(void)
{
    regs.x.ax = 0x1202;
    regs.h.bl = 0x30;
    int86(VIDEO, &regs, &regs);
	regs.h.ah = 0x0f;
    int86(VIDEO, &regs, &regs);
    regs.h.al |= 0x80;
	regs.h.ah = 0x00;
    int86(VIDEO, &regs, &regs);
}

/* ---------- set 25 line mode ------- */
void Set25(void)
{
    unsigned char rows = *(unsigned char __far *)MK_FP(0x40, 0x84);
    if (rows == 24 || rows == 0)
        return;
    if (isVGA())	{
        Scan400();
		regs.x.ax = 0x1114;
	}
	else
		regs.x.ax = 0x1111;
    regs.h.bl = 0;
    int86(VIDEO, &regs, &regs);
}

/* ------ convert an Alt+ key to its letter equivalent ----- */
int AltConvert(int c)
{
	int i, a = 0;
	for (i = 0; i < 36; i++)
		if (c == altconvert[i])
			break;
	if (i < 26)
		a = 'a' + i;
	else if (i < 36)
		a = '0' + i - 26;
	return a;
}

