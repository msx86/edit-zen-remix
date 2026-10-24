/*  Status Bar

    Part of the FreeDOS Editor

*/

#include "dfpcomp.h"

int StatusBarProc(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
    char *statusbar;

    switch (msg)
        {
        case KEYBOARD:
            if ((int)p1 == CTRL_F4)
                return TRUE;

            break;
        case PAINT: 
            if (!isVisible(wnd))
                break;

            statusbar = DFcalloc(1, WindowWidth(wnd)+1);
            memset(statusbar, ' ', WindowWidth(wnd));
            *(statusbar+WindowWidth(wnd)) = '\0';
            if (wnd->text)
                {
                int len = min(strlen(wnd->text), WindowWidth(wnd));
                if (len > 0)
                    {
                    int off = WindowWidth(wnd) - len;
                    if (len < strlen(wnd->text))
                        {
                        if (len > 3)
                            {
                            strncpy(statusbar+off, wnd->text, len-3);
                            strncpy(statusbar+off+len-3, "...", 3);
                            }
                        }
                    else
                        strncpy(statusbar+off, wnd->text, len);
                    }
                }

            SetStandardColor(wnd);
            PutWindowLine(wnd, statusbar, 0, 0);
            free(statusbar);
            return TRUE;
        case BORDER:
            return TRUE;
        default:
            break;
        }

    return BaseWndProc(STATUSBAR, wnd, msg, p1, p2);
}
