/* ------------- textbox.c ------------ */

#include "dfpcomp.h"

static void ComputeWindowTop(WINDOW);
static void ComputeWindowLeft(WINDOW);
static int ComputeVScrollBox(WINDOW);
static int ComputeHScrollBox(WINDOW);
static void MoveScrollBox(WINDOW, int);
static char *GetTextLine(WINDOW, int, char *, int, BOOL *);

BOOL VSliding;
BOOL HSliding;

/* ------------ ADDTEXT Message -------------- */
static BOOL AddTextMsg(WINDOW wnd, char *txt)
{
    /* --- append text to the textbox's buffer --- */
    unsigned adln = strlen(txt);
    if (adln > MAXTEXTLEN)
        return FALSE;
    if (wnd->text != NULL)    {
        /* ---- appending to existing text ---- */
        unsigned txln = strlen(wnd->text);
        if ((unsigned long)txln+adln > MAXTEXTLEN)
            return FALSE;
        if (txln+adln > wnd->textlen)    {
            wnd->text = DFrealloc(wnd->text, txln+adln+3);
            wnd->textlen = txln+adln+1;
        }
    }
    else    {
        /* ------ 1st text appended ------ */
        wnd->text = DFcalloc(1, adln+3);
        wnd->textlen = adln+1;
    }
	wnd->TextChanged = TRUE;
    if (wnd->text != NULL)    {
        /* ---- append the text ---- */
        strcat(wnd->text, txt);
        strcat(wnd->text, "\n");
        BuildTextPointers(wnd);
		return TRUE;
    }
	return FALSE;
}

/* ------------ DELETETEXT Message -------------- */
static void DeleteTextMsg(WINDOW wnd, int lno)
{
	char *cp1 = TextLine(wnd, lno);
	--wnd->wlines;
	if (lno == wnd->wlines)
		*cp1 = '\0';
	else 	{
		char *cp2 = TextLine(wnd, lno+1);
		memmove(cp1, cp2, strlen(cp2)+1);
	}
    BuildTextPointers(wnd);
}

/* ------------ INSERTTEXT Message -------------- */
static void InsertTextMsg(WINDOW wnd, char *txt, int lno)
{
	if (AddTextMsg(wnd, txt))	{
		int len = strlen(txt)+1;
		char *cp2 = TextLine(wnd, lno);
		char *cp1 = cp2+len;
		memmove(cp1, cp2, strlen(cp2)-len);
		strcpy(cp2, txt);
		*(cp2+len-1) = '\n';
	    BuildTextPointers(wnd);
		wnd->TextChanged = TRUE;
	}
}

/* ------------ SETTEXT Message -------------- */
static void SetTextMsg(WINDOW wnd, char *txt)
{
    /* -- assign new text value to textbox buffer -- */
    unsigned int len = strlen(txt)+1;
	SendMessage(wnd, CLEARTEXT, 0, 0);
    wnd->textlen = len;
    wnd->text=DFrealloc(wnd->text, len+1);
    wnd->text[len] = '\0';
    strcpy(wnd->text, txt);
    BuildTextPointers(wnd);
}

/* ------------ CLEARTEXT Message -------------- */
static void ClearTextMsg(WINDOW wnd)
{
    /* ----- clear text from textbox ----- */
    if (wnd->text != NULL)
        free(wnd->text);
    wnd->text = NULL;
    wnd->textlen = 0;
    wnd->wlines = 0;
    wnd->textwidth = 0;
    wnd->wtop = wnd->wleft = 0;
    ClearTextBlock(wnd);
    ClearTextPointers(wnd);
}

/* ------------ KEYBOARD Message -------------- */
static int KeyboardMsg(WINDOW wnd, PARAM p1)
{
    switch ((int) p1)    {
        case UP:
            return SendMessage(wnd,SCROLL,FALSE,0);
        case DN:
            return SendMessage(wnd,SCROLL,TRUE,0);
#ifdef HOOKKEYB
        case FWD: /* right arrow */
#else
	case RARROW: /* formerly called FWD */
#endif
            return SendMessage(wnd,HORIZSCROLL,TRUE,0);
#ifndef HOOKKEYB
	case LARROW: /* hope this makes sense */
#endif
        case BS:
            return SendMessage(wnd,HORIZSCROLL,FALSE,0);
        case PGUP:
            return SendMessage(wnd,SCROLLPAGE,FALSE,0);
        case PGDN:
            return SendMessage(wnd,SCROLLPAGE,TRUE,0);
        case CTRL_PGUP:
            return SendMessage(wnd,HORIZPAGE,FALSE,0);
        case CTRL_PGDN:
            return SendMessage(wnd,HORIZPAGE,TRUE,0);
        case HOME:
            return SendMessage(wnd,SCROLLDOC,TRUE,0);
        case END:
            return SendMessage(wnd,SCROLLDOC,FALSE,0);
        default:
            break;
    }
    return FALSE;
}

/* ------------ LEFT_BUTTON Message -------------- */
static int LeftButtonMsg(WINDOW wnd, PARAM p1, PARAM p2)
{
    int mx = (int) p1 - GetLeft(wnd);
    int my = (int) p2 - GetTop(wnd);
    if (TestAttribute(wnd, VSCROLLBAR) &&
                        mx == WindowWidth(wnd)-1)    {
        /* -------- in the right border ------- */
        if (my == 0 || my == ClientHeight(wnd)+1)
            /* --- above or below the scroll bar --- */
            return FALSE;
        if (my == 1)
            /* -------- top scroll button --------- */
            return SendMessage(wnd, SCROLL, FALSE, 0);
        if (my == ClientHeight(wnd))
            /* -------- bottom scroll button --------- */
            return SendMessage(wnd, SCROLL, TRUE, 0);
        /* ---------- in the scroll bar ----------- */
        if (!VSliding && my-1 == wnd->VScrollBox)    {
            RECT rc;
            VSliding = TRUE;
            rc.lf = rc.rt = GetRight(wnd);
            rc.tp = GetTop(wnd)+2;
            rc.bt = GetBottom(wnd)-2;
            return SendMessage(NULL, MOUSE_TRAVEL,
                (PARAM) &rc, 0);
        }
        if (my-1 < wnd->VScrollBox)
            return SendMessage(wnd,SCROLLPAGE,FALSE,0);
        if (my-1 > wnd->VScrollBox)
            return SendMessage(wnd,SCROLLPAGE,TRUE,0);
    }
    if (TestAttribute(wnd, HSCROLLBAR) &&
                        my == WindowHeight(wnd)-1) {
        /* -------- in the bottom border ------- */
        if (mx == 0 || my == ClientWidth(wnd)+1)
            /* ------  outside the scroll bar ---- */
            return FALSE;
        if (mx == 1)
            return SendMessage(wnd, HORIZSCROLL,FALSE,0);
        if (mx == WindowWidth(wnd)-2)
            return SendMessage(wnd, HORIZSCROLL,TRUE,0);
        if (!HSliding && mx-1 == wnd->HScrollBox)    {
            /* --- hit the scroll box --- */
            RECT rc;
            rc.lf = GetLeft(wnd)+2;
            rc.rt = GetRight(wnd)-2;
            rc.tp = rc.bt = GetBottom(wnd);
            /* - keep the mouse in the scroll bar - */
            SendMessage(NULL,MOUSE_TRAVEL,(PARAM)&rc,0);
            HSliding = TRUE;
            return TRUE;
        }
        if (mx-1 < wnd->HScrollBox)
            return SendMessage(wnd,HORIZPAGE,FALSE,0);
        if (mx-1 > wnd->HScrollBox)
            return SendMessage(wnd,HORIZPAGE,TRUE,0);
    }
    return FALSE;
}

/* ------------ MOUSE_MOVED Message -------------- */
static BOOL MouseMovedMsg(WINDOW wnd, PARAM p1, PARAM p2)
{
    int mx = (int) p1 - GetLeft(wnd);
    int my = (int) p2 - GetTop(wnd);
    if (VSliding)    {
        /* ---- dragging the vertical scroll box --- */
        if (my-1 != wnd->VScrollBox)    {
            foreground = FrameForeground(wnd);
            background = FrameBackground(wnd);
            hide_mousecursor();
            wputch(wnd, SCROLLBARCHAR, WindowWidth(wnd)-1,
                    wnd->VScrollBox+1);
            wnd->VScrollBox = my-1;
            wputch(wnd, SCROLLBOXCHAR, WindowWidth(wnd)-1,
                    my);
            show_mousecursor();
        }
        return TRUE;
    }
    if (HSliding)    {
        /* --- dragging the horizontal scroll box --- */
        if (mx-1 != wnd->HScrollBox)    {
            foreground = FrameForeground(wnd);
            background = FrameBackground(wnd);
            hide_mousecursor();
            wputch(wnd, SCROLLBARCHAR, wnd->HScrollBox+1,
                    WindowHeight(wnd)-1);
            wnd->HScrollBox = mx-1;
            wputch(wnd, SCROLLBOXCHAR, mx, WindowHeight(wnd)-1);
            show_mousecursor();
        }
        return TRUE;
    }
    return FALSE;
}

/* ------------ BUTTON_RELEASED Message -------------- */
static void ButtonReleasedMsg(WINDOW wnd)
{
    if (HSliding || VSliding)    {
        /* release the mouse ouside the scroll bar */
        SendMessage(NULL, MOUSE_TRAVEL, 0, 0);
        VSliding ? ComputeWindowTop(wnd):ComputeWindowLeft(wnd);
        SendMessage(wnd, PAINT, 0, 0);
        SendMessage(wnd, KEYBOARD_CURSOR, 0, 0);
        VSliding = HSliding = FALSE;
    }
}

/* ------------ SCROLL Message -------------- */
static BOOL ScrollMsg(WINDOW wnd, PARAM p1)
{
    /* ---- vertical scroll one line ---- */
    if (p1)    {
        /* ----- scroll one line up ----- */
        if (wnd->wtop+ClientHeight(wnd) >= wnd->wlines)
            return FALSE;
        wnd->wtop++;
    }
    else    {
        /* ----- scroll one line down ----- */
        if (wnd->wtop == 0)
            return FALSE;
        --wnd->wtop;
    }
    if (isVisible(wnd))    {
        RECT rc;
        rc = ClipRectangle(wnd, ClientRect(wnd));
        if (ValidRect(rc))    {
            /* ---- scroll the window ----- */
            if (wnd != inFocus)
                SendMessage(wnd, PAINT, 0, 0);
            else    {
                scroll_window(wnd, rc, (int)p1);
                if (!(int)p1)
                    /* -- write top line (down) -- */
                    WriteTextLine(wnd,NULL,wnd->wtop,FALSE);
                else    {
                    /* -- write bottom line (up) -- */
                    int y=RectBottom(rc)-GetClientTop(wnd);
                    WriteTextLine(wnd, NULL,
                        wnd->wtop+y, FALSE);
                }
            }
        }
        /* ---- reset the scroll box ---- */
        if (TestAttribute(wnd, VSCROLLBAR))    {
            int vscrollbox = ComputeVScrollBox(wnd);
            if (vscrollbox != wnd->VScrollBox)
                MoveScrollBox(wnd, vscrollbox);
        }
    }
    return TRUE;
}

/* ------------ HORIZSCROLL Message -------------- */
static BOOL HorizScrollMsg(WINDOW wnd, PARAM p1)
{
    /* --- horizontal scroll one column --- */
    if (p1)    {
        /* --- scroll left --- */
        if (wnd->wleft + ClientWidth(wnd)-1 >= wnd->textwidth)
			return FALSE;
        wnd->wleft++;
    }
    else	{
        /* --- scroll right --- */
        if (wnd->wleft == 0)
			return FALSE;
        --wnd->wleft;
	}
    SendMessage(wnd, PAINT, 0, 0);
	return TRUE;
}

/* ------------  SCROLLPAGE Message -------------- */
static void ScrollPageMsg(WINDOW wnd, PARAM p1)
{
    /* --- vertical scroll one page --- */
    if ((int) p1 == FALSE)    {
        /* ---- page up ---- */
        if (wnd->wtop)
            wnd->wtop -= ClientHeight(wnd);
    }
    else     {
        /* ---- page down ---- */
        if (wnd->wtop+ClientHeight(wnd) < wnd->wlines) {
            wnd->wtop += ClientHeight(wnd);
            if (wnd->wtop>wnd->wlines-ClientHeight(wnd))
                wnd->wtop=wnd->wlines-ClientHeight(wnd);
        }
    }
    if (wnd->wtop < 0)
        wnd->wtop = 0;
    SendMessage(wnd, PAINT, 0, 0);
}

/* ------------ HORIZSCROLLPAGE Message -------------- */
static void HorizScrollPageMsg(WINDOW wnd, PARAM p1)
{
    /* --- horizontal scroll one page --- */
    if ((int) p1 == FALSE)
        /* ---- page left ----- */
        wnd->wleft -= ClientWidth(wnd);
    else    {
        /* ---- page right ----- */
        wnd->wleft += ClientWidth(wnd);
        if (wnd->wleft > wnd->textwidth-ClientWidth(wnd))
            wnd->wleft = wnd->textwidth-ClientWidth(wnd);
    }
    if (wnd->wleft < 0)
        wnd->wleft = 0;
    SendMessage(wnd, PAINT, 0, 0);
}

/* ------------ SCROLLDOC Message -------------- */
static void ScrollDocMsg(WINDOW wnd, PARAM p1)
{
    /* --- scroll to beginning or end of document --- */
    if ((int) p1)
        wnd->wtop = wnd->wleft = 0;
    else if (wnd->wtop+ClientHeight(wnd) < wnd->wlines){
        wnd->wtop = wnd->wlines-ClientHeight(wnd);
        wnd->wleft = 0;
    }
    SendMessage(wnd, PAINT, 0, 0);
}

/* ------------ PAINT Message -------------- */
static void PaintMsg(WINDOW wnd, PARAM p1, PARAM p2)
{
    /* ------ paint the client area ----- */
    RECT rc, rcc;
    int y;
    char blankline[512];

    /* ----- build the rectangle to paint ----- */
    if ((RECT *)p1 == NULL)
        rc=RelativeWindowRect(wnd, WindowRect(wnd));
    else
        rc= *(RECT *)p1;
    if (TestAttribute(wnd, HASBORDER | LEFTBORDER) &&
            RectRight(rc) >= WindowWidth(wnd)-1) {
        if (RectLeft(rc) >= WindowWidth(wnd)-1)
            return;
        RectRight(rc) = WindowWidth(wnd)-2;
    }
    rcc = AdjustRectangle(wnd, rc);

	if (!p2 && wnd != inFocus)
		ClipString++;

    /* ----- blank line for padding ----- */
    memset(blankline, ' ', SCREENWIDTH);
    blankline[RectRight(rcc)+1] = '\0';

    /* ------- each line within rectangle ------ */
    for (y = RectTop(rc); y <= RectBottom(rc); y++){
        int yy;
        /* ---- test outside of Client area ---- */
        if (TestAttribute(wnd,
                    HASBORDER | HASTITLEBAR))    {
            if (y < TopBorderAdj(wnd))
                continue;
            if (y > WindowHeight(wnd)-2)
                continue;
        }
        yy = y-TopBorderAdj(wnd);
        if (yy < wnd->wlines-wnd->wtop)
            /* ---- paint a text line ---- */
            WriteTextLine(wnd, &rc,
                        yy+wnd->wtop, FALSE);
        else    {
            /* ---- paint a blank line ---- */
            SetStandardColor(wnd);
            if (TestAttribute(wnd, LEFTBORDER))
                wputch(wnd, ' ', 0, y);
            writeline(wnd, blankline+RectLeft(rcc),
                    RectLeft(rcc)+BorderAdj(wnd), y, FALSE);
        }
    }
    /* ------- position the scroll box ------- */
    if (TestAttribute(wnd, VSCROLLBAR|HSCROLLBAR)) {
        int hscrollbox = ComputeHScrollBox(wnd);
        int vscrollbox = ComputeVScrollBox(wnd);
        if (hscrollbox != wnd->HScrollBox ||
                vscrollbox != wnd->VScrollBox)    {
            wnd->HScrollBox = hscrollbox;
            wnd->VScrollBox = vscrollbox;
            SendMessage(wnd, BORDER, p1, 0);
        }
    }
	if (!p2 && wnd != inFocus)
		--ClipString;
}

/* ------------ CLOSE_WINDOW Message -------------- */
static void CloseWindowMsg(WINDOW wnd)
{
    SendMessage(wnd, CLEARTEXT, 0, 0);
    if (wnd->TextPointers != NULL)    {
        free(wnd->TextPointers);
        wnd->TextPointers = NULL;
    }
}

/* ----------- TEXTBOX Message-processing Module ----------- */
int TextBoxProc(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
    switch (msg)    {
        case CREATE_WINDOW:
            wnd->HScrollBox = wnd->VScrollBox = 1;
            ClearTextPointers(wnd);
            break;
        case ADDTEXT:
            return AddTextMsg(wnd, (char *) p1);
		case DELETETEXT:
            DeleteTextMsg(wnd, (int) p1);
            return TRUE;
		case INSERTTEXT:
            InsertTextMsg(wnd, (char *) p1, (int) p2);
            return TRUE;
        case SETTEXT:
            SetTextMsg(wnd, (char *) p1);
			return TRUE;
        case CLEARTEXT:
            ClearTextMsg(wnd);
            break;
        case KEYBOARD:
            if (WindowMoving || WindowSizing)
                break;
            if (KeyboardMsg(wnd, p1))
                return TRUE;
            break;
        case LEFT_BUTTON:
            if (WindowSizing || WindowMoving)
                return FALSE;
            if (LeftButtonMsg(wnd, p1, p2))
                return TRUE;
            break;
        case MOUSE_MOVED:
            if (MouseMovedMsg(wnd, p1, p2))
                return TRUE;
            break;
        case BUTTON_RELEASED:
            ButtonReleasedMsg(wnd);
            break;
        case SCROLL:
            return ScrollMsg(wnd, p1);
        case HORIZSCROLL:
            return HorizScrollMsg(wnd, p1);
        case SCROLLPAGE:
            ScrollPageMsg(wnd, p1);
            return TRUE;
        case HORIZPAGE:
            HorizScrollPageMsg(wnd, p1);
            return TRUE;
        case SCROLLDOC:
            ScrollDocMsg(wnd, p1);
            return TRUE;
        case PAINT:
            if (isVisible(wnd))    {
                PaintMsg(wnd, p1, p2);
                return FALSE;
            }
            break;
        case CLOSE_WINDOW:
            CloseWindowMsg(wnd);
            break;
        default:
            break;
    }
    return BaseWndProc(TEXTBOX, wnd, msg, p1, p2);
}

/* ------ compute the vertical scroll box position from
                   the text pointers --------- */
static int ComputeVScrollBox(WINDOW wnd)
{
    int pagelen = wnd->wlines - ClientHeight(wnd);
    int barlen = ClientHeight(wnd)-2;
    int lines_tick;
    int vscrollbox;

    if (pagelen < 1 || barlen < 1)
        vscrollbox = 1;
    else    {
        if (pagelen > barlen)
            lines_tick = pagelen / barlen;
        else
            lines_tick = barlen / pagelen;
        vscrollbox = 1 + (wnd->wtop / lines_tick);
        if (vscrollbox > ClientHeight(wnd)-2 ||
                wnd->wtop + ClientHeight(wnd) >= wnd->wlines)
            vscrollbox = ClientHeight(wnd)-2;
    }
    return vscrollbox;
}

/* ---- compute top text line from scroll box position ---- */
static void ComputeWindowTop(WINDOW wnd)
{
    int pagelen = wnd->wlines - ClientHeight(wnd);
    if (wnd->VScrollBox == 0)
        wnd->wtop = 0;
    else if (wnd->VScrollBox == ClientHeight(wnd)-2)
        wnd->wtop = pagelen;
    else    {
        int barlen = ClientHeight(wnd)-2;
        int lines_tick;

        if (pagelen > barlen)
            lines_tick = barlen ? (pagelen / barlen) : 0;
        else
            lines_tick = pagelen ? (barlen / pagelen) : 0;
        wnd->wtop = (wnd->VScrollBox-1) * lines_tick;
        if (wnd->wtop + ClientHeight(wnd) > wnd->wlines)
            wnd->wtop = pagelen;
    }
    if (wnd->wtop < 0)
        wnd->wtop = 0;
}

/* ------ compute the horizontal scroll box position from
                   the text pointers --------- */
static int ComputeHScrollBox(WINDOW wnd)
{
    int pagewidth = wnd->textwidth - ClientWidth(wnd);
    int barlen = ClientWidth(wnd)-2;
    int chars_tick;
    int hscrollbox;

    if (pagewidth < 1 || barlen < 1)
        hscrollbox = 1;
    else     {
        if (pagewidth > barlen)
            chars_tick = barlen ? (pagewidth / barlen) : 0;
        else
            chars_tick = pagewidth ? (barlen / pagewidth) : 0;
        hscrollbox = 1 + (chars_tick ? (wnd->wleft / chars_tick) : 0);
        if (hscrollbox > ClientWidth(wnd)-2 ||
                wnd->wleft + ClientWidth(wnd) >= wnd->textwidth)
            hscrollbox = ClientWidth(wnd)-2;
    }
    return hscrollbox;
}

/* ---- compute left column from scroll box position ---- */
static void ComputeWindowLeft(WINDOW wnd)
{
    int pagewidth = wnd->textwidth - ClientWidth(wnd);

    if (wnd->HScrollBox == 0)
        wnd->wleft = 0;
    else if (wnd->HScrollBox == ClientWidth(wnd)-2)
        wnd->wleft = pagewidth;
    else    {
        int barlen = ClientWidth(wnd)-2;
        int chars_tick;

        if (pagewidth > barlen)
            chars_tick = pagewidth / barlen;
        else
            chars_tick = barlen / pagewidth;
        wnd->wleft = (wnd->HScrollBox-1) * chars_tick;
        if (wnd->wleft + ClientWidth(wnd) > wnd->textwidth)
            wnd->wleft = pagewidth;
    }
    if (wnd->wleft < 0)
        wnd->wleft = 0;
}

/* ----- get the text to a specified line ----- */
static char *GetTextLine(WINDOW wnd, int selection, char *stack_buf, int stack_buf_size, BOOL *needs_free)
{
    char *line;
    int len = 0;
    char *cp, *cp1;
    char *end_line = NULL;

    *needs_free = FALSE;
    if (wnd == NULL || wnd->text == NULL)
        return NULL;

    if (selection + 1 < wnd->wlines)
        end_line = TextLine(wnd, selection + 1);

    cp = cp1 = TextLine(wnd, selection);
    while (*cp && *cp != '\n' && (end_line == NULL || cp < end_line))    {
        len++;
        cp++;
    }
    if (len + 7 <= stack_buf_size) {
        line = stack_buf;
    } else {
        line = DFmalloc(len + 7);
        if (line == NULL)
            return NULL;
        *needs_free = TRUE;
    }
    memmove(line, cp1, len);
    line[len] = '\0';
    return line;
}

/* Set syntax highlight type for a window based on filename */
void SetHighlightType(WINDOW wnd, const char *fn)
{
    int len;

    if (wnd == NULL)
        return;

    if (GetClass(wnd) != EDITBOX) {
        wnd->HighlightType = HL_NONE;
        return;
    }

    if (fn == NULL)
        fn = (const char *)wnd->extension;
    if (fn == NULL)
        fn = GetTitle(wnd);
    if (fn == NULL) {
        wnd->HighlightType = HL_NONE;
        return;
    }

    /* Find basename */
    {
        const char *p1 = strrchr(fn, 0x5c);
        const char *p2 = strrchr(fn, '/');
        if (p2 > p1) p1 = p2;
        if (p1 != NULL) fn = p1 + 1;
    }

    len = strlen(fn);
    if (len >= 3 && stricmp(fn + len - 3, ".md") == 0)
        wnd->HighlightType = HL_MARKDOWN;
    else if (stricmp(fn, "holidia.txt") == 0)
        wnd->HighlightType = HL_HOLIDIA;
    else
        wnd->HighlightType = HL_NONE;
}

/* ------- write a line of text to a textbox window ------- */
void WriteTextLine(WINDOW wnd, RECT *rcc, int y, BOOL reverse)
{
    int len = 0;
    int dif = 0;
    unsigned char line[512];
    unsigned char local_line_buf[512];
    BOOL needs_free = FALSE;
    RECT rc;
    unsigned char *lp, *svlp;
    int lnlen;
    int i;
    BOOL trunc = FALSE;
    enum HighlightType hl;
    BOOL is_heading = FALSE;
    int sym_col = -1;

    /* ------ make sure y is inside the window ----- */
    if (y < wnd->wtop || y >= wnd->wtop+ClientHeight(wnd))
        return;

    /* ---- build the retangle within which can write ---- */
    if (rcc == NULL)    {
        rc = RelativeWindowRect(wnd, WindowRect(wnd));
        if (TestAttribute(wnd, HASBORDER | LEFTBORDER) &&
                RectRight(rc) >= WindowWidth(wnd)-1)
            RectRight(rc) = WindowWidth(wnd)-2;
    }
    else
        rc = *rcc;

    /* ----- make sure rectangle is within window ------ */
    if (RectLeft(rc) >= WindowWidth(wnd)-1)
        return;
    if (RectRight(rc) == 0)
        return;
    rc = AdjustRectangle(wnd, rc);
    if (y-wnd->wtop<RectTop(rc) || y-wnd->wtop>RectBottom(rc))
        return;

    /* --- get the text and length of the text line --- */
    lp = svlp = (unsigned char *)GetTextLine(wnd, y, (char *)local_line_buf, sizeof(local_line_buf), &needs_free);
    if (svlp == NULL)
        return;
    lnlen = LineLength(lp);

    hl = wnd->HighlightType;
    if (hl == HL_MARKDOWN) {
        if (svlp[0] == '#') {
            is_heading = TRUE;
        } else {
            int k = 0;
            while (svlp[k] == ' ' || svlp[k] == '	')
                k++;
            if (svlp[k] == '-' || svlp[k] == '*')
                sym_col = k;
        }
    } else if (hl == HL_HOLIDIA) {
        if (svlp[0] == '*') {
            is_heading = TRUE;
        } else {
            int k = 0;
            while (svlp[k] == ' ' || svlp[k] == '	')
                k++;
            if (svlp[k] == '-')
                sym_col = k;
        }
    }

	if (wnd->protect)	{
		char *pp = lp;
		while (*pp)	{
			if (isprint(*pp))
				*pp = '*'; /* why that??? */
			pp++;
		}
	}

    /* -------- insert block color change controls ------- */
    if (TextBlockMarked(wnd))    {
        int bbl = wnd->BlkBegLine;
        int bel = wnd->BlkEndLine;
        int bbc = wnd->BlkBegCol;
        int bec = wnd->BlkEndCol;
        int by = y;

        /* ----- put lowest marker first ----- */
        if (bbl > bel)    {
            swap(bbl, bel);
            swap(bbc, bec);
        }
        if (bbl == bel && bbc > bec)
            swap(bbc, bec);

        if (by >= bbl && by <= bel)    {
            /* ------ the block includes this line ----- */
            int blkbeg = 0;
            int blkend = lnlen;
            if (!(by > bbl && by < bel))    {
                /* --- the entire line is not in the block -- */
                if (by == bbl)
                    /* ---- the block begins on this line --- */
                    blkbeg = bbc;
                if (by == bel)
                    /* ---- the block ends on this line ---- */
                    blkend = bec;
            }
			if (blkend == 0 && lnlen == 0)	{
				strcpy(lp, " ");
				blkend++;
			}
            /* ----- insert the reset color token ----- */
            memmove(lp+blkend+1,lp+blkend,strlen(lp+blkend)+1);
            lp[blkend] = RESETCOLOR;
            /* ----- insert the change color token ----- */
            memmove(lp+blkbeg+3,lp+blkbeg,strlen(lp+blkbeg)+1);
            lp[blkbeg] = CHANGECOLOR;
            /* ----- insert the color tokens ----- */
            SetReverseColor(wnd);
            lp[blkbeg+1] = foreground | 0x80;
            lp[blkbeg+2] = background | 0x80;
            lnlen += 4;
        }
    }
    /* - make sure left margin doesn't overlap color change - */
    for (i = 0; i < wnd->wleft+3; i++)    {
        if (*(lp+i) == '\0')
            break;
        if (*(unsigned char *)(lp + i) == RESETCOLOR)
            break;
    }
    if (*(lp+i) && i < wnd->wleft+3)    {
        if (wnd->wleft+4 > lnlen)
            trunc = TRUE;
        else 
            lp += 4;
    }
    else     {
        /* --- it does, shift the color change over --- */
        for (i = 0; i < wnd->wleft; i++)    {
            if (*(lp+i) == '\0')
                break;
            if (*(unsigned char *)(lp + i) == CHANGECOLOR)    {
                *(lp+wnd->wleft+2) = *(lp+i+2);
                *(lp+wnd->wleft+1) = *(lp+i+1);
                *(lp+wnd->wleft) = *(lp+i);
                break;
            }
        }
    }
    /* ------ build the line to display -------- */
    if (!trunc)    {
        if (lnlen < wnd->wleft)
            lnlen = 0;
        else
            lp += wnd->wleft;
        if (lnlen > RectLeft(rc))    {
            /* ---- the line exceeds the rectangle ---- */
            int ct = RectLeft(rc);
            char *initlp = lp;
            /* --- point to end of clipped line --- */
            while (ct)    {
                if (*(unsigned char *)lp == CHANGECOLOR)
                    lp += 3;
                else if (*(unsigned char *)lp == RESETCOLOR)
                    lp++;
                else
                    lp++, --ct;
            }
            if (RectLeft(rc))    {
                char *lpp = lp;
                while (*lpp)    {
                    if (*(unsigned char*)lpp==CHANGECOLOR)
                        break;
                    if (*(unsigned char*)lpp==RESETCOLOR) {
                        lpp = lp;
                        while (lpp >= initlp)    {
                            if (*(unsigned char *)lpp ==
                                            CHANGECOLOR) {
                                lp -= 3;
                                memmove(lp,lpp,3);
                                break;
                            }
                            --lpp;
                        }
                        break;
                    }
                    lpp++;
                }
            }
            lnlen = LineLength(lp);
            len = min(lnlen, RectWidth(rc));
            dif = strlen(lp) - lnlen;
            len += dif;
            if (len > 0)
                strncpy(line, lp, len);
        }
    }
    /* -------- pad the line --------- */
    while (len < RectWidth(rc)+dif && len < (int)sizeof(line) - 1)
        line[len++] = ' ';
    line[len] = '\0';
    dif = 0;
    /* ------ establish the line's main color ----- */
    if (reverse)    {
        char *cp = line;
        SetReverseColor(wnd);
        while ((cp = strchr(cp, CHANGECOLOR)) != NULL)    {
            cp += 2;
            *cp++ = background | 0x80;
        }
        if (*(unsigned char *)line == CHANGECOLOR)
            dif = 3;
    }
    else {
        if (is_heading) {
            foreground = YELLOW;
            background = WndBackground(wnd);
        } else {
            SetStandardColor(wnd);
        }

        if (sym_col >= 0) {
            int vis_col = 0;
            for (i = 0; i < sym_col; i++) {
                if (svlp[i] == '	')
                    vis_col = (vis_col / SysConfig.EditorTabSize + 1) * SysConfig.EditorTabSize;
                else
                    vis_col++;
            }
            {
                int scr_col = vis_col - wnd->wleft;
                if (scr_col >= 0 && scr_col < RectWidth(rc)) {
                    BOOL is_selected = FALSE;
                    if (TextBlockMarked(wnd)) {
                        int bbl = wnd->BlkBegLine;
                        int bel = wnd->BlkEndLine;
                        int bbc = wnd->BlkBegCol;
                        int bec = wnd->BlkEndCol;
                        if (bbl > bel) {
                            swap(bbl, bel);
                            swap(bbc, bec);
                        }
                        if (bbl == bel && bbc > bec)
                            swap(bbc, bec);

                        if (y >= bbl && y <= bel) {
                            if (y > bbl && y < bel)
                                is_selected = TRUE;
                            else if (y == bbl && y == bel)
                                is_selected = (vis_col >= bbc && vis_col < bec);
                            else if (y == bbl)
                                is_selected = (vis_col >= bbc);
                            else if (y == bel)
                                is_selected = (vis_col < bec);
                        }
                    }
                    if (!is_selected) {
                        unsigned char *p = line + dif;
                        int curr_vis = 0;
                        while (*p && curr_vis < scr_col) {
                            if (*p == CHANGECOLOR)
                                p += 3;
                            else if (*p == RESETCOLOR)
                                p++;
                            else {
                                p++;
                                curr_vis++;
                            }
                        }
                        if (*p == svlp[sym_col]) {
                            unsigned char sym_char = *p;
                            memmove(p + 5, p + 1, strlen((char *)p + 1) + 1);
                            p[0] = CHANGECOLOR;
                            p[1] = LIGHTCYAN | 0x80;
                            p[2] = background | 0x80;
                            p[3] = sym_char;
                            p[4] = RESETCOLOR;
                        }
                    }
                }
            }
        }
    }
    if (TestAttribute(wnd, LEFTBORDER)) {
        //int save_fg = foreground;
        //int save_bg = background;
        //SetStandardColor(wnd);
        wputch(wnd, ' ', 0, y-wnd->wtop+TopBorderAdj(wnd));
        //foreground = save_fg;
        //background = save_bg;
    }
    /* ------- display the line -------- */
    writeline(wnd, line+dif,
                RectLeft(rc)+BorderAdj(wnd),
                    y-wnd->wtop+TopBorderAdj(wnd), FALSE);
    if (needs_free)
        free(svlp);
}

void MarkTextBlock(WINDOW wnd, int BegLine, int BegCol,
                               int EndLine, int EndCol)
{
    wnd->BlkBegLine = BegLine;
    wnd->BlkEndLine = EndLine;
    wnd->BlkBegCol = BegCol;
    wnd->BlkEndCol = EndCol;
}

/* ----- clear and initialize text line pointer array ----- */
void ClearTextPointers(WINDOW wnd)
{
    wnd->TextPointers = DFrealloc(wnd->TextPointers, 2 * sizeof(int));
    *(wnd->TextPointers) = 0;
    *((wnd->TextPointers) + 1) = 0;
}

#define INITLINES 1024

/* ---- build array of pointers to text lines ---- */
void BuildTextPointers(WINDOW wnd)
{
    char *cp;
    int incrs = INITLINES;
    unsigned int off;
    BOOL do_wrap = FALSE;
    int wrap_width = 0;

    if (wnd == NULL || wnd->text == NULL)
        return;

    if (wnd->WordWrapMode && isMultiLine(wnd) && ClientWidth(wnd) > 0) {
        do_wrap = TRUE;
        wrap_width = max(10, ClientWidth(wnd) - 1);
    }

    cp = wnd->text;
    wnd->textwidth = wnd->wlines = 0;
    while (*cp)    {
        char *cp1;
        if (incrs == INITLINES)    {
            incrs = 0;
            wnd->TextPointers = DFrealloc(wnd->TextPointers,
                    (wnd->wlines + INITLINES) * sizeof(int));
        }
        off = (unsigned int) (cp - wnd->text);
        *((wnd->TextPointers) + wnd->wlines) = off;
        wnd->wlines++;
        incrs++;
        cp1 = cp;

        if (do_wrap) {
            char *last_space = NULL;
            int col = 0;
            while (*cp && *cp != '\n' && col < wrap_width) {
                if (*cp == ' ' || *cp == '\t')
                    last_space = cp;
                cp++;
                col++;
            }
            if (*cp && *cp != '\n') {
                if (last_space != NULL && last_space > cp1)
                    cp = last_space + 1;
            } else if (*cp == '\n') {
                cp++;
            }
        } else {
            while (*cp && *cp != '\n')
                cp++;
            if (*cp)
                cp++;
        }
        wnd->textwidth = max(wnd->textwidth,
                        (unsigned int) (cp - cp1));
    }
    if (wnd->wlines == 0) {
        wnd->TextPointers = DFrealloc(wnd->TextPointers, 2 * sizeof(int));
        *(wnd->TextPointers) = 0;
        *((wnd->TextPointers) + 1) = 0;
    } else {
        /* Set sentinel at wlines pointing to terminating null */
        *((wnd->TextPointers) + wnd->wlines) = (unsigned int) (cp - wnd->text);
    }
}

static void MoveScrollBox(WINDOW wnd, int vscrollbox)
{
    foreground = FrameForeground(wnd);
    background = FrameBackground(wnd);

    hide_mousecursor();
    wputch(wnd, SCROLLBARCHAR, WindowWidth(wnd)-1,
            wnd->VScrollBox+1);
    wputch(wnd, SCROLLBOXCHAR, WindowWidth(wnd)-1,
            vscrollbox+1);
    show_mousecursor();
    
    wnd->VScrollBox = vscrollbox;
}

int TextLineNumber(WINDOW wnd, char *lp)
{
    int low, high, mid;
    unsigned int target_off;

    if (wnd == NULL || wnd->wlines <= 0 || lp == NULL || wnd->text == NULL || lp <= wnd->text)
        return 0;

    target_off = (unsigned int)(lp - wnd->text);
    low = 0;
    high = wnd->wlines - 1;

    while (low <= high) {
        mid = low + (high - low) / 2;
        if (*(wnd->TextPointers + mid) == target_off)
            return mid;
        else if (*(wnd->TextPointers + mid) < target_off)
            low = mid + 1;
        else
            high = mid - 1;
    }
    return max(0, high);
}
