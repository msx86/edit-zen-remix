/* --------------------- video.c -------------------- */

#include "dfpcomp.h"

BOOL ClipString;

static unsigned video_address;
void movefromscreen(void far *bf, int offset, int len);
void movetoscreen(void far *bf, int offset, int len);

VideoResolution  TXT25 = {80, 25, FALSE, "PC Standard 80x25 text mode"};

/* -- read a rectangle of video memory into a save buffer -- */
void getvideo(RECT rc, void far *bf)
{
    int ht = RectBottom(rc)-RectTop(rc)+1;
    int bytes_row = (RectRight(rc)-RectLeft(rc)+1) * 2;
    unsigned vadr = vad(RectLeft(rc), RectTop(rc));

    hide_mousecursor_in_rect(rc);
    while (ht--)    {
		movefromscreen(bf, vadr, bytes_row);
        vadr += SCREENWIDTH*2;
        bf = (char far *)bf + bytes_row;
    }
    show_mousecursor();
}

/* -- write a rectangle of video memory from a save buffer -- */
void storevideo(RECT rc, void far *bf)
{
    int ht = RectBottom(rc)-RectTop(rc)+1;
    int bytes_row = (RectRight(rc)-RectLeft(rc)+1) * 2;
    unsigned vadr = vad(RectLeft(rc), RectTop(rc));

    hide_mousecursor_in_rect(rc);
    while (ht--)    {
		movetoscreen(bf, vadr, bytes_row);
        vadr += SCREENWIDTH*2;
        bf = (char far *)bf + bytes_row;
    }
    show_mousecursor();
}

/* -------- read a character of video memory ------- */
unsigned int GetVideoChar(int x, int y)
{
    int c;

    if (x < 0 || y < 0 || x >= SCREENWIDTH || y >= SCREENHEIGHT)
        return ' ';

    c = peek(video_address, vad(x,y));

    return c;
}

/* -------- write a character of video memory ------- */
void PutVideoChar(int x, int y, int c)
{
    if (x >= 0 && y >= 0 && x < SCREENWIDTH && y < SCREENHEIGHT)    {
        poke(video_address, vad(x,y), c);
    }
}

BOOL CharInView(WINDOW wnd, int x, int y)
{
	WINDOW nwnd = NextWindow(wnd);
	WINDOW pwnd;
	RECT rc;
    int x1 = GetLeft(wnd)+x;
    int y1 = GetTop(wnd)+y;

	if (!TestAttribute(wnd, VISIBLE))
		return FALSE;
    if (!TestAttribute(wnd, NOCLIP))    {
        WINDOW wnd1 = GetParent(wnd);
        while (wnd1 != NULL)    {
            /* --- clip character to parent's borders -- */
			if (!TestAttribute(wnd1, VISIBLE))
				return FALSE;
			if (!InsideRect(x1, y1, ClientRect(wnd1)))
                return FALSE;
            wnd1 = GetParent(wnd1);
        }
    }
	while (nwnd != NULL)	{
		if (!isHidden(nwnd) /* && !isAncestor(wnd, nwnd) */ )	{
			rc = WindowRect(nwnd);
    		if (TestAttribute(nwnd, SHADOW))    {
        		RectBottom(rc)++;
        		RectRight(rc)++;
    		}
			if (!TestAttribute(nwnd, NOCLIP))	{
				pwnd = nwnd;
				while (GetParent(pwnd))	{
					pwnd = GetParent(pwnd);
					rc = subRectangle(rc, ClientRect(pwnd));
				}
			}
			if (InsideRect(x1,y1,rc))
				return FALSE;
		}
		nwnd = NextWindow(nwnd);
	}
    return (x1 >= 0 && y1 >= 0 && x1 < SCREENWIDTH && y1 < SCREENHEIGHT);
}

/* -------- write a character to a window ------- */
void wputch(WINDOW wnd, int c, int x, int y)
{
	if (CharInView(wnd, x, y))	{
		int ch = (c & 255) | (clr(foreground, background) << 8);
		int xc = GetLeft(wnd)+x;
		int yc = GetTop(wnd)+y;
        
        poke(video_address, vad(xc, yc), ch);
	}
}

/* ------- write a string to a window ---------- */
void wputs(WINDOW wnd, void *s, int x, int y)
{
    int x1=GetLeft(wnd)+x;
    int x2=x1;
    int y1=GetTop(wnd)+y;
    RECT rc;

    if (x1 < SCREENWIDTH && y1 < SCREENHEIGHT && isVisible(wnd))
        {
        u16 ln[512];
        u16 *cp1=ln;
        int fg=foreground;
        int bg=background;
        int len;
        int off=0;
        unsigned char *str=s;

        while (*str && (cp1 - ln) < (int)(sizeof(ln)/sizeof(ln[0]) - 1))
            {
            if (*str == CHANGECOLOR)
                {
                int fgcode, bgcode;	/* new 0.7c: sanity checks */
                str++;
                fgcode = (*str++);
                bgcode = (*str++);
                if ((fgcode & 0x80) && (bgcode & 0x80) &&
                    !(fgcode & 0x70) && !(bgcode & 0x70)) {
                    foreground = fgcode & 0x7f;
                    background = bgcode & 0x7f;
                    continue;
                } else {	/* this also makes CHANGECOLOR almost normal */
                    str--;	/* and useable as character in your texts... */
                    str--;	/* treat as non-escape sequence */
                    str--;
                }
                }

            if (*str == RESETCOLOR)
                {
                foreground = fg & 0x7f;
                background = bg & 0x7f;
                str++;
                continue;
                }

#ifdef TAB_TOGGLING	/* made consistent with editor.c - 0.7c */
            if (*str == ('\t' | 0x80) || *str == ('\f' | 0x80))
                *cp1 = ' ' | (clr(foreground, background) << 8);
            else 
#endif
                *cp1 = (*str & 255) | (clr(foreground, background) << 8);

            if (ClipString)
                if (!CharInView(wnd, x, y)) {
                    *cp1 = peek(video_address, vad(x2,y1));
                }

            cp1++;
            str++;
            x++;
            x2++;
            }

        foreground = fg;
        background = bg;
        len = (int)(cp1-ln);
        if (x1+len > SCREENWIDTH)
            len = SCREENWIDTH-x1;

        if (!ClipString && !TestAttribute(wnd, NOCLIP))
            {
            /* -- clip the line to within ancestor windows -- */
            WINDOW nwnd = GetParent(wnd);
            rc = WindowRect(wnd);

            while (len > 0 && nwnd != NULL)
                {
                if (!isVisible(nwnd))
                    {
                    len = 0;
                    break;
                    }

                rc = subRectangle(rc, ClientRect(nwnd));
                nwnd = GetParent(nwnd);
                }

            while (len > 0 && !InsideRect(x1+off,y1,rc))
                {
                off++;
                --len;
                }

            if (len > 0)
                {
                x2 = x1+len-1;
                while (len && !InsideRect(x2,y1,rc))
                    {
                    --x2;
                    --len;
                    }

                }

            }

        if (len > 0)
            {
                rc.lf = x1;
                rc.rt = x2;
                rc.tp = rc.bt = y1;
                hide_mousecursor_in_rect(rc);
            movetoscreen(ln+off, vad(x1+off,y1), len*2);
                show_mousecursor();
           }

        }

}

/* --------- get the current video mode -------- */
void get_videomode(void)
{
    videomode();

    /* ---- Monochrome Display Adaptor or text mode ---- */
    if (ismono())
        video_address = 0xb000;
    else
        video_address = 0xb800 + video_page;
}

/* --------- scroll the window. d: 1 = up, 0 = dn ---------- */
void scroll_window(WINDOW wnd, RECT rc, int d)
{
	if (RectTop(rc) != RectBottom(rc))	{
		union REGS regs;
		regs.h.cl = RectLeft(rc);
		regs.h.ch = RectTop(rc);
		regs.h.dl = RectRight(rc);
		regs.h.dh = RectBottom(rc);
		regs.h.bh = clr(WndForeground(wnd),WndBackground(wnd));
		regs.h.ah = 7 - d;
		regs.h.al = 1;
    	hide_mousecursor();
    	int86(VIDEO, &regs, &regs);
    	show_mousecursor();
	}
}


void movetoscreen(void far *bf, int offset, int len)
{
	movedata(FP_SEG(bf), FP_OFF(bf), dpmi_seg_to_sel((unsigned short)video_address), offset, len);
}

void movefromscreen(void far *bf, int offset, int len)
{
	movedata(dpmi_seg_to_sel((unsigned short)video_address), offset,	FP_SEG(bf), FP_OFF(bf),	len);
}



