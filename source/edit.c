/*  FreeDOS Editor

*/


/* D E F I N E S ///////////////////////////////////////////////////////// */

#include "sysint.h"
#define CHARSLINE 80
#define LINESPAGE 66

/* I N C L U D E S /////////////////////////////////////////////////////// */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dos.h>
#include <process.h>
#include <conio.h>
#include <bios.h>
#include <ctype.h>
#include <io.h>
#include <time.h>
#include <setjmp.h>

#include "dflatp.h"
#include "dfptools.h"
#include "edit.h"

/* G L O B A L S ///////////////////////////////////////////////////////// */



extern DBOX PrintSetup;
char DFlatApplication[] = "Edit";
static char Untitled[] = "Untitled";
static int wndpos, /* LineStartsAt, StartLine, */ LineCtr, CharCtr;
static BOOL InsertMode = TRUE;
static BOOL WordWrap = TRUE;
BOOL ZenMode = FALSE;
WINDOW EditApplication = NULL;

BOOL HasOpenEditor(void)
{
    WINDOW cwnd;
    if (EditApplication == NULL)
        return FALSE;
    cwnd = FirstWindow(EditApplication);
    while (cwnd != NULL) {
        if (GetClass(cwnd) == EDITBOX && cwnd->condition != ISCLOSING)
            return TRUE;
        cwnd = NextWindow(cwnd);
    }
    return FALSE;
}



/* P R O T O T Y P E S /////////////////////////////////////////////////// */

int classify_args(int, char *[], char *[], char *[]);
static int MemoPadProc(WINDOW, MESSAGE, PARAM, PARAM);
static void NewFile(WINDOW,char *);
static void SelectFile(WINDOW);
static void PadWindow(WINDOW, char *);
static void OpenPadWindow(WINDOW, char *,char *);
static void LoadFile(WINDOW);
static void SaveFile(WINDOW, int);
static int EditorProc(WINDOW, MESSAGE, PARAM, PARAM);
static char *NameComponent(char *);
static void FixTabMenu(void);

/* F U N C T I O N S ///////////////////////////////////////////////////// */

int classify_args(int argc, char *rawargs[], char *fileargs[], char *optargs[])
{
    int index, jndex, kndex;
    char *argptr;

    /* skip index=0, aka argv[0]==edit.exe */
    for (index=1,jndex=0,kndex=0;index<argc;index++)
        {
        argptr = rawargs[index];
        if (*argptr == '/')
            {
            argptr++;
            optargs[kndex++] = argptr;
            } /* end if. */
        else
            {
            fileargs[jndex++] = argptr;
            } /* end else. */

        } /* end for. */

   return kndex;

} /* end classify_args. */

int DfpMain(int argc, char *argv[])
{
    FILE *fp;
    char *fileargs[64], *optargs[64];
    int n_options, n_files, index, help_flag=0;
    char WindowTitle[255];
    
#ifdef NEWLOG
 		StartLogger ("message.log", LL_CRITICAL);
 		Debug_LogMessages = TRUE;
 		CurrentLogLevel = LL_NOTIFY;
#endif

    n_options = classify_args(argc, argv, fileargs, optargs);
    n_files = argc - n_options - 1; /* the (-)1 is argv[0] */

    for (index=0; index<n_options; index++) {
        if (optargs[index][0] == '?') help_flag=1;
        else if (optargs[index][0] == 'B' || optargs[index][0] == 'b')
							SelectColorScheme (bw);
        else if (optargs[index][0] == 'i' || optargs[index][0] == 'I')
							SelectColorScheme (reverse);
        else if (optargs[index][0] == 'r' || optargs[index][0] == 'R')
            SysConfig.EditorGlobalReadOnly = TRUE;
        else if (optargs[index][0] == 'z' || optargs[index][0] == 'Z')
            ZenMode = TRUE;
        else {
            printf("Invalid parameter - /%s\n", strupr(optargs[index]));
            unhidecursor();
            exit(1);
        }
    }

    if (help_flag)
    {
        printf("Syntax: EDIT [/B] [/I] [/R] [/Z] [/?] [file(s)]\n"
               "  /B     Force monochrome mode\n"
               "  /I     Use inverse color scheme\n"
               "  /R     Open all files read-only\n"
               "  /Z     Enable zen mode (full screen editor)\n"
               "  /?     Display this help message\n"
               "  [file] Specify file(s) to load.\n"
               "         Wildcards can be used here.\n");
        unhidecursor();
        exit(1);
    }

    /* (LoadConfig was called at this place in pre-0.7b versions) */
   	SelectLines (TXT25);
	SysConfig.EditorTabSize = 8;
   
    EditApplication = CreateWindow(APPLICATION, "", 0, 0, -1, -1,
            &MainMenu, NULL, MemoPadProc, (int)(HASSTATUSBAR));

    SendMessage(EditApplication, SETFOCUS, TRUE, 0);

    /*  Load the files from args - if the file does not exist, open a new
        window.... */
    for (index=0;index<n_files;index++) {
        /*  Check if the file exists... */
        /* added by Eric: Do NOT try to open files with names */
        /* that contain wildcards, otherwise you may NewFile  */
        /* files with wildcards in their names. Sigh! 11/2002 */
        fp = NULL;
        if (((strchr(fileargs[index],'*') == NULL)) && ((strchr(fileargs[index],'?') == NULL)) && ((fp = fopen(fileargs[index],"rt")) == NULL)) {
            NewFile(EditApplication,fileargs[index]);
        }
        else {
            if (fp != NULL)
                 fclose(fp);  /* don't leave open file handle [from test if exists in above if] */
            PadWindow(EditApplication, fileargs[index]);
        }

    }


	ProcessMessages();

#ifdef NEWLOG
 		StopLogger ();
#endif

		return (0);
}

/* ------ open text files and put them into editboxes ----- */
static void PadWindow(WINDOW wnd, char *FileName)
{
    int ax, criterr = 1;
		FBLOCK ff;
		
    char path[66];
    char *cp;

    CreatePath(path, FileName, FALSE, FALSE);
    cp = path+strlen(path);
    CreatePath(path, FileName, TRUE, FALSE);
    while (criterr == 1)
        {
        	ax = FindFirst (path, _A_NORMAL, ff);
        criterr = TestCriticalError();
        }

    while (ax == 0 && !criterr)
        {
        strcpy(cp, NameOf(ff));
        OpenPadWindow(wnd, path,NULL);
        ax = FindNext(ff);
        }

}

/* ------- window processing module for the Edit application window ----- */
static int MemoPadProc(WINDOW wnd,MESSAGE msg,PARAM p1,PARAM p2)
{
    int rtn;

    switch (msg)
        {
        case CREATE_WINDOW:
      
            rtn = DefaultWndProc(wnd, msg, p1, p2);
            if (InsertMode)
                SetCommandToggle(&MainMenu, ID_INSERT);

            if (WordWrap)
                SetCommandToggle(&MainMenu, ID_WRAP);

            FixTabMenu();
            return rtn;
    	    case CLOSE_WINDOW:
    				break;
        case COMMAND:
	    switch ((int)p1)
                {
        case ID_WINDOW:
            /* C.M.S. set by menubar (global hotkey, new 0.7c) */
            /* or by popdown (click, local hotkey)... */
            ChooseWindow(wnd, CurrentMenuSelection-2);
            break;
        case ID_CLOSEALL:
            CloseAll(wnd, FALSE);
            if (ZenMode && !HasOpenEditor()) {
                if (EditApplication->MenuBarWnd != NULL) {
                    SetVisible(EditApplication->MenuBarWnd);
                    ReFocus(EditApplication->MenuBarWnd);
                    SendMessage(EditApplication->MenuBarWnd, PAINT, 0, 0);
                }
                if (EditApplication->StatusBar != NULL) {
                    SetVisible(EditApplication->StatusBar);
                    ReFocus(EditApplication->StatusBar);
                    SendMessage(EditApplication->StatusBar, PAINT, 0, 0);
                }
                SendMessage(EditApplication, SETFOCUS, TRUE, 0);
                SendMessage(EditApplication, PAINT, 0, 0);
            }
            break;
        case ID_MOREWINDOWS:
            MoreWindows(wnd);
            break;
#ifdef INCLUDE_LOGGING
        case ID_LOG:
            MessageLog(wnd);
        		if (CheckBoxSetting(&dbLog, ID_LOGGING))   
            		SetCommandToggle(&MainMenu, ID_LOG);
        		else
            		ClearCommandToggle(&MainMenu, ID_LOG);
            break;
#endif
		case ID_NEW:
		    NewFile(wnd,NULL);
		    return TRUE;
		case ID_OPEN:
		    SelectFile(wnd);
		    return TRUE;
		case ID_SAVE:
		    SaveFile(inFocus, FALSE);
		    return TRUE;
		case ID_SAVEAS:
		    SaveFile(inFocus, TRUE);
		    return TRUE;
                case ID_CLOSE:
                    SendMessage(inFocus, CLOSE_WINDOW, 0, 0);
                    SkipApplicationControls();
                    return TRUE;
                case ID_EXIT: 
            						PostMessage(wnd, CLOSE_WINDOW, 0, 0);
                    return TRUE;
                case ID_WRAP:
                    WordWrap = GetCommandToggle(&MainMenu, ID_WRAP);
                    if (inFocus != NULL)
                        SendMessage(inFocus, COMMAND, ID_WRAP, 0);
		    return TRUE;
                case ID_INSERT:
                    InsertMode = GetCommandToggle(&MainMenu, ID_INSERT);
                    return TRUE;
							 case ID_TAB0:
		    						SysConfig.EditorTabSize = 1; /* type-through TAB char mode -ea */
                    FixTabMenu(); /* show current value in tab menu */
		    return TRUE;
                case ID_TAB2:
                    SysConfig.EditorTabSize = 2;
                    FixTabMenu(); /* show current value in tab menu */
		    return TRUE;
                case ID_TAB4:
                    SysConfig.EditorTabSize = 4;
                    FixTabMenu();
		    return TRUE;
                case ID_TAB6:
                    SysConfig.EditorTabSize = 6; 
                    FixTabMenu();
		    return TRUE;
                case ID_TAB8:
                    SysConfig.EditorTabSize = 8;
                    FixTabMenu();
		    return TRUE;

		default:
		    break;

                }
	    break;
	default:
	    break;
        }

    return DefaultWndProc(wnd, msg, p1, p2);

}

/* The New command. Open an empty editor window */
static void NewFile(WINDOW wnd, char *FileName)
{
    OpenPadWindow(wnd, Untitled,FileName);
}

/* --- The Open... command. Select a file  --- */
static void SelectFile(WINDOW wnd)
{
    char FileName[MAXPATH];

    if (OpenFileDialogBox("*.*", FileName))
        {
        /* See if the document is already in a window */
	WINDOW wnd1 = FirstWindow(wnd);

        while (wnd1 != NULL)
            {
	    if (stricmp(FileName, wnd1->extension) == 0)
                {
		SendMessage(wnd1, SETFOCUS, TRUE, 0);
		SendMessage(wnd1, RESTORE, 0, 0);
		return;
                }

            wnd1 = NextWindow(wnd1);
            }

        OpenPadWindow(wnd, FileName,NULL);
        }

}

/* --- open a document window and load a file --- */
static void OpenPadWindow(WINDOW wnd, char *FileName,char *NewFileName)
{
    static WINDOW wnd1 = NULL;
    struct stat sb;
    char *Fname = FileName, *ermsg;

    if (strcmp(FileName, Untitled))
        {
	if (stat(FileName, &sb))
            {
	    ermsg = DFmalloc(strlen(FileName)+20);
	    strcpy(ermsg, "No such file:\n");
	    strcat(ermsg, FileName);
	    ErrorMessage(ermsg);
	    free(ermsg);
	    return;
            }

	Fname = NameComponent(FileName);

        /* check file size against 32 MB limit */
        if (sb.st_size > MAXTEXTLEN)
        {
            ermsg = DFmalloc(strlen(FileName)+100);
            sprintf(ermsg, "File too large (exceeds 32 MB limit):\n%s", FileName);
            ErrorMessage(ermsg);
            free(ermsg);
            return;
        }

        /* check if the machine has enough free memory to load the file */
        {
            unsigned long free_mem = get_free_dpmi_memory();
            /* Loading needs read buffer + window text + line pointers + safety margin */
            unsigned long needed = (unsigned long)sb.st_size * 2 + ((unsigned long)sb.st_size / 4) + (512 * 1024UL);
            if (free_mem > 0 && free_mem < needed)
            {
                ermsg = DFmalloc(strlen(FileName)+160);
                sprintf(ermsg, "Not enough free memory to load %s:\nNeed %lu KB, machine has %lu KB free",
                        Fname, needed / 1024UL, free_mem / 1024UL);
                ErrorMessage(ermsg);
                free(ermsg);
                return;
            }
        }

        } /* actual filename given */

    wndpos += 2;
    if (NewFileName != NULL)
        Fname = NameComponent(NewFileName);

    if (wndpos == 20)
        wndpos = 2;

    if (ZenMode) {
        if (EditApplication != NULL) {
            if (EditApplication->MenuBarWnd != NULL)
                ClearVisible(EditApplication->MenuBarWnd);
            if (EditApplication->StatusBar != NULL)
                ClearVisible(EditApplication->StatusBar);
        }
        wnd1 = CreateWindow(EDITBOX,
            Fname,
            0, 0, SCREENHEIGHT, SCREENWIDTH,
            NULL, wnd, EditorProc,
            MULTILINE | NOCLIP | LEFTBORDER);
    } else {
        wnd1 = CreateWindow(EDITBOX,
            Fname,
            (wndpos-1)*2, wndpos, 10, 40,
            NULL, wnd, EditorProc,
            SHADOW     |
            MINMAXBOX  |
            CONTROLBOX |
            VSCROLLBAR |
            HSCROLLBAR |
            MOVEABLE   |
            HASBORDER  |
            HASTITLEBAR|
            SIZEABLE   |
            MULTILINE);
    }

    wnd1->WordWrapMode = WordWrap;

    if (SysConfig.EditorGlobalReadOnly)		/* new feature in 0.7b */
        AddAttribute(wnd1, READONLY);
        /* needed because ReadOnlyMode must not make ALL text */
        /* entry fields read only, only EDITBOXes become r/o! */

    /* suggested code change to ix saveas bug -
       contrib: James Sandys-Lumsdaine 

    OLD CODE SEGMENT! 

    if (strcmp(FileName, Untitled))    {
	wnd1->extension = DFmalloc(strlen(FileName)+1);
	strcpy(wnd1->extension, FileName);
	LoadFile(wnd1);
    }

    NEW CODE SEGMENT!
    */

    if (NewFileName != NULL)
        {
	/* Either a command line new file or one that's on the 
	disk to load - Either way, must set the extension
	to the given filename */

	wnd1->extension = DFmalloc(strlen(NewFileName) + 1);
	strcpy(wnd1->extension,NewFileName);
        }
    else
        {
        if (strcmp(FileName,Untitled)) {
            wnd1->extension = DFmalloc(strlen(FileName)+1);
            strcpy(wnd1->extension, FileName);
            LoadFile(wnd1); /* Only load if not a new file */
        }

        }

    SetHighlightType(wnd1, wnd1->extension);

    SendMessage(wnd1, SETFOCUS, TRUE, 0);
    SendMessage(wnd1, MAXIMIZE, 0, 0); 

}

/* --- Load the notepad file into the editor text buffer --- */
static void LoadFile(WINDOW wnd)
{
    char *Buf = NULL;
    unsigned int recptr = 0;
    unsigned int buf_cap = 0;
    FILE *fp;
    WINDOW wwnd;
    long fsize = 0;

    if (!strcmp(wnd->extension, Untitled))
    {
      SendMessage(wnd, SETTEXT, (PARAM) "", 0); /* fill with empty string */
      return;
    } /* not a real file load */

    if ((fp = fopen(wnd->extension, "rt")) != NULL)
    {
        int expandTabs = -1;
        int theColumn = 0;
        unsigned int i;
        unsigned int rmax = 1024;
        int coop_ctr = 0;

        fseek(fp, 0, SEEK_END);
        fsize = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        if (fsize > (long)MAXTEXTLEN)
        {
            fclose(fp);
            ErrorMessage("File too large (exceeds 32 MB limit).");
            return;
        }

        buf_cap = (fsize > 0) ? (unsigned int)fsize + 4096 : 4096;
        Buf = malloc(buf_cap);
        if (Buf == NULL)
        {
            buf_cap = 4096;
            Buf = malloc(buf_cap);
        }
        if (Buf == NULL)
        {
            fclose(fp);
            ErrorMessage("Not enough memory to load file.");
            return;
        }
        memset(Buf, 0, buf_cap);

        wwnd = MomentaryMessage("Loading...");

        while (!feof(fp))
        {
            if ((++coop_ctr & 0x3f) == 0)
                Cooperate();			/* let messages flow periodically */
            rmax = 1024;
            if (recptr + rmax + 512 > buf_cap)
            {
                unsigned int new_cap = buf_cap + 65536;
                char *new_buf;
                if (new_cap > MAXTEXTLEN + 65536)
                {
                    fclose(fp);
                    free(Buf);
                    SendMessage(wwnd, CLOSE_WINDOW, 0, 0);
                    ErrorMessage("File exceeded 32 MB limit while reading.");
                    return;
                }
                new_buf = realloc(Buf, new_cap);
                if (new_buf == NULL)
                {
                    fclose(fp);
                    free(Buf);
                    SendMessage(wwnd, CLOSE_WINDOW, 0, 0);
                    ErrorMessage("Ran out of memory while loading file.");
                    return;
                }
                Buf = new_buf;
                memset(Buf + buf_cap, 0, new_cap - buf_cap);
                buf_cap = new_cap;
            }
            if (fgets(Buf+recptr, 512, fp) == NULL)
                break;
            if ( (expandTabs == -1) && (SysConfig.EditorTabSize > 1) &&
                 (strchr(Buf+recptr,'\t') != NULL) )
            {
                char tMsg[200];

                SendMessage(wwnd, CLOSE_WINDOW, 0, 0);

                sprintf(tMsg,"Tabs detected in\n%s\nExpand them at tab width %d?",
                    (strlen(wnd->extension)>120) ? "file" : wnd->extension,
                    SysConfig.EditorTabSize);
                expandTabs = (YesNoBox(tMsg))
                   ? 1 : 0;

                wwnd = MomentaryMessage("Loading...");

            }
            for (i=0; Buf[recptr+i]; i++)
            {
                switch (Buf[recptr+i])
                {
                    case '\r':
                    case '\n':
                        theColumn = 0;
                        break;
                    case '\t':
                        if (expandTabs == 1)
                        {
                            if ((strlen(Buf+recptr)+SysConfig.EditorTabSize+8) >= rmax)
                            {
                                rmax += 512;
                                if (recptr + rmax > buf_cap)
                                {
                                    unsigned int new_cap = buf_cap + 65536;
                                    char *new_buf = realloc(Buf, new_cap);
                                    if (new_buf == NULL)
                                    {
                                        fclose(fp);
                                        free(Buf);
                                        SendMessage(wwnd, CLOSE_WINDOW, 0, 0);
                                        ErrorMessage("Ran out of memory expanding tabs.");
                                        return;
                                    }
                                    Buf = new_buf;
                                    memset(Buf + buf_cap, 0, new_cap - buf_cap);
                                    buf_cap = new_cap;
                                }
                            }
                            {
                                int j = SysConfig.EditorTabSize - (theColumn % SysConfig.EditorTabSize);
                                memmove(Buf+recptr+i+j, Buf+recptr+i+1, rmax-i-j);
                                memset(Buf+recptr+i, ' ', j);
                                theColumn += j;
                                i += j;
                                i--;
                            }
                        } else
                            theColumn++;
                        break;
                    default:
                        theColumn++;
                }
            }
            recptr += strlen(Buf+recptr);
        }

        fclose(fp);
        if (Buf != NULL)
        {
            if (strlen(Buf) > wnd->MaxTextLength)
            {
                ErrorMessage("File contents exceed maximum document size (32 MB).");
            }
            else
            {
                SendMessage(wnd, SETTEXT, (PARAM) Buf, 0);
            }
            free(Buf);
        }

        SendMessage(wwnd, CLOSE_WINDOW, 0, 0);
    }
    else
    {
        char fMsg[200];
        sprintf(fMsg,"Could not load %s",
                    (strlen(wnd->extension)>120) ? "file" : wnd->extension);
        ErrorMessage(fMsg);
    }

}


/* ---------- save a file to disk ------------ */
static void SaveFile(WINDOW wnd, int Saveas)
{
    FILE *fp;

    if (wnd->extension == NULL || Saveas) /* ask for new name? */
        {
        char FileName[MAXPATH];

	FileName[0] = 0;
	trySaveAgain:	/* moved label up in 0.7c */

        if (SaveAsDialogBox("*.*", NULL, FileName))
            {
            if (wnd->extension != NULL)
                free(wnd->extension);

            wnd->extension = DFmalloc(strlen(FileName)+1);
            strcpy(wnd->extension, FileName);
            AddTitle(wnd, NameComponent(FileName));
            SendMessage(wnd, BORDER, 0, 0);
            SetHighlightType(wnd, wnd->extension);
            }
        else
            {
            ErrorMessage("No name given - not saved.");
            return; /* abort if no name provided by user */
            }
        }

    if (wnd->extension != NULL)	/* if there is a filename for the window */
        {
        WINDOW mwnd;
        /* trySaveAgain: */
        mwnd = MomentaryMessage("Saving...");

        if ((fp = fopen(wnd->extension, "wt")) != NULL)
            {
            /* could use CollapseTabs() here if user wants us to do so!? */
            size_t howmuch = strlen(GetText(wnd));
            size_t written = fwrite(GetText(wnd), 1, howmuch, fp); /* ONE item, SIZE howmuch */
            fclose(fp);
            SendMessage(mwnd, CLOSE_WINDOW, 0, 0);
            if (written != howmuch)
                {
                if (YesNoBox("Write error. Try again?"))
                    {
                    Saveas = 1;	/* 0.7c: ask user for a new place for next try */
                    goto trySaveAgain;
                    }
                }
            else
                wnd->TextChanged = FALSE;	/* give up */
            }
        else
            {
            char fMsg[200];
            SendMessage(mwnd, CLOSE_WINDOW, 0, 0);
            sprintf(fMsg,"Could not save %s, try again?",
                    (strlen(wnd->extension)>120) ? "file" : wnd->extension);
            if (YesNoBox(fMsg))
                {
                Saveas = 1;	/* 0.7c: ask user for a new place for next try */
                goto trySaveAgain;
                }
            }
        } /* if any file loaded */
}

/* ------ display the row and column in the statusbar ------ */
static void ShowPosition(WINDOW wnd)
{
    /* This is where we place the "INS" display */
    char status[40], *InsModeText;
    int cur_line = (wnd->CurrLine < 0) ? 0 : wnd->CurrLine;
    int cur_col = (wnd->CurrCol < 0) ? 0 : wnd->CurrCol;
    if (wnd->InsertMode)
        {
        InsModeText = "INS ";           /* Not on */
        }
    else
        {
        InsModeText = "OVER";           /* "Insert" (Overtype!?) is on */
        }

    if (WindowWidth(wnd) < 50) /* auto-condense new in EDIT 0.7 */
        {
        sprintf(status, "%c %c Li:%d Co:%d", 
            (SysConfig.EditorGlobalReadOnly) ? 'R' : (wnd->TextChanged ? '*' : ' '),
            InsModeText[0], cur_line+1, cur_col+1);
            /* 1-based column / row are nicer for humans (EDIT 0.7b) */
            /* new flag char R for readonly added 0.7b */
        }
    else
        sprintf(status, "%c %4s  Line: %4d  Col: %3d ",
            (SysConfig.EditorGlobalReadOnly) ? 'R' : (wnd->TextChanged ? '*' : ' '),
            InsModeText, cur_line+1, cur_col+1);
            /* 1-based column / row are nicer for humans (EDIT 0.7b) */
            /* new flag char R for readonly added 0.7b */
    SendMessage(GetParent(wnd), ADDSTATUS, (PARAM) status, 0);

}

/* ----- window processing module for the editboxes ----- */
static int EditorProc(WINDOW wnd,MESSAGE msg,PARAM p1,PARAM p2)
{
    int rtn;
    switch (msg)    {
	case SETFOCUS:
			if ((int)p1)    {
				wnd->InsertMode = GetCommandToggle(&MainMenu, ID_INSERT);
				wnd->WordWrapMode = GetCommandToggle(&MainMenu, ID_WRAP);
			}
	    rtn = DefaultWndProc(wnd, msg, p1, p2);
	    if ((int)p1 == FALSE)
		SendMessage(GetParent(wnd), ADDSTATUS, 0, 0);
	    else 
		ShowPosition(wnd);
	    return rtn;
	case KEYBOARD_CURSOR:
	    rtn = DefaultWndProc(wnd, msg, p1, p2);
	    ShowPosition(wnd);
	    return rtn;
	case COMMAND:
	    if (SysConfig.EditorGlobalReadOnly && TestAttribute(wnd, READONLY)) {
    	    	/* read only mode added 0.7b */
	        switch ((int)p1) {
	            case ID_REPLACE:
	            case ID_CUT:
	            case ID_PASTE:
	            case ID_DELETETEXT:
	                beep();
	                return TRUE;	/* consume event */
	        }
	    }
		switch ((int) p1)       {
			case ID_SEARCH:
				SearchText(wnd);
				return TRUE;
			case ID_REPLACE:
				ReplaceText(wnd);
				return TRUE;
			case ID_SEARCHNEXT:
				SearchNext(wnd);
				return TRUE;
			case ID_CUT:
				CopyToClipboard(wnd);
				SendMessage(wnd, COMMAND, ID_DELETETEXT, 0);
				SendMessage(wnd, PAINT, 0, 0);
				return TRUE;
			case ID_COPY:
				CopyToClipboard(wnd);
				ClearTextBlock(wnd);
				SendMessage(wnd, PAINT, 0, 0);
				return TRUE;
			case ID_PASTE:
				PasteFromClipboard(wnd);
				SendMessage(wnd, PAINT, 0, 0);
				return TRUE;
			case ID_DELETETEXT:
				rtn = DefaultWndProc(wnd, msg, p1, p2);
				SendMessage(wnd, PAINT, 0, 0);
				return rtn;
			case ID_WRAP:
				wnd->WordWrapMode = WordWrap;
				if (wnd->text != NULL) {
					char *cp = CurrChar;
					unsigned int off = (unsigned int)(cp - wnd->text);
					wnd->wleft = 0;
					BuildTextPointers(wnd);
					if (wnd->wlines == 0) {
						wnd->CurrLine = 0;
						wnd->CurrCol = 0;
						wnd->wtop = 0;
						wnd->WndRow = 0;
					} else {
						wnd->CurrLine = TextLineNumber(wnd, wnd->text + off);
						wnd->CurrCol = (int)((wnd->text + off) - TextLine(wnd, wnd->CurrLine));
						if (wnd->CurrLine < wnd->wtop)
							wnd->wtop = wnd->CurrLine;
						else if (wnd->CurrLine >= wnd->wtop + ClientHeight(wnd))
							wnd->wtop = max(0, wnd->CurrLine - ClientHeight(wnd) + 1);
						wnd->WndRow = wnd->CurrLine - wnd->wtop;
					}
					SendMessage(wnd, PAINT, 0, 0);
					SendMessage(wnd, KEYBOARD_CURSOR, WndCol, wnd->WndRow);
				}
				return TRUE;
			case ID_INSERT:
				SendMessage(GetParent(wnd), COMMAND, ID_INSERT, 0);
				wnd->InsertMode = InsertMode;
				SendMessage(NULL, SHOW_CURSOR, wnd->InsertMode, 0);
				return TRUE;
			default:
				break;
	    	} 	/* end of switch int p1 */
	    break;	/* end of case COMMAND  */
	case CLOSE_WINDOW:
	    if (wnd->TextChanged)
            {
            char *cp;
            BOOL saveAsFlag = 0;
            cp = DFmalloc(75+strlen(GetTitle(wnd)));
            strcpy(cp, "             The file\n            '");
            strcat(cp, GetTitle(wnd));
            strcat(cp, "'\nhas not been saved yet.  Save it now?");

            saveAsOnClose:
            SendMessage(wnd, SETFOCUS, TRUE, 0);
            if (YesNoBox(cp)) {            
                SendMessage(GetParent(wnd), COMMAND,
                    (saveAsFlag ? ID_SAVEAS : ID_SAVE), 0);
                if (wnd->TextChanged) { /* still unsaved changes? */
                  ErrorMessage("File could not be saved! Try to save elsewhere.");
                  saveAsFlag = 1;
                  goto saveAsOnClose; /* do not let user leave yet */
                } /* still unsaved */
            } /* user selected "yes", save before closing window */
            free(cp);
            } /* modified file - suggested to save */

        wndpos = 0;
	    if (wnd->extension != NULL)
            {
            free(wnd->extension);
            wnd->extension = NULL;
            }
	    break;
	default:
	    break;
    }
    rtn = DefaultWndProc(wnd, msg, p1, p2);
    if (msg == CLOSE_WINDOW && ZenMode && !HasOpenEditor()) {
        if (EditApplication != NULL) {
            if (EditApplication->MenuBarWnd != NULL) {
                SetVisible(EditApplication->MenuBarWnd);
                ReFocus(EditApplication->MenuBarWnd);
                SendMessage(EditApplication->MenuBarWnd, PAINT, 0, 0);
            }
            if (EditApplication->StatusBar != NULL) {
                SetVisible(EditApplication->StatusBar);
                ReFocus(EditApplication->StatusBar);
                SendMessage(EditApplication->StatusBar, PAINT, 0, 0);
            }
            SendMessage(EditApplication, SETFOCUS, TRUE, 0);
            SendMessage(EditApplication, PAINT, 0, 0);
        }
    }
    return rtn;
}

/* -- point to the name component of a file specification -- */
static char *NameComponent(char *FileName)
{
    char *Fname;
    if ((Fname = strrchr(FileName, '\\')) == NULL)
	if ((Fname = strrchr(FileName, ':')) == NULL)
	    Fname = FileName-1;
    return Fname + 1;
}

static char *ports[] = {
	"Lpt1", "Lpt2", "Lpt3",
	"Com1", "Com2", "Com3", "Com4",
	 NULL
};


static void FixTabMenu(void)
{
	char *cp = GetCommandText(&MainMenu, ID_TABS);
	if (cp != NULL) {
		cp = strchr(cp, '(');
		if (cp != NULL) {
			*(cp+1) = (SysConfig.EditorTabSize>1) ? (SysConfig.EditorTabSize + '0') : '-';
			if (GetClass(inFocus) == POPDOWNMENU)
				SendMessage(inFocus, PAINT, 0, 0);
		}
	}
}

/* Prep....Menu are called to activate drop-downs in the main menu bar */
void PrepFileMenu(void *w, struct Menu *mnu)
{
    WINDOW wnd = w;

    if (mnu != NULL) {}; /* unused parameter! */

    DeactivateCommand(&MainMenu, ID_SAVE);
    DeactivateCommand(&MainMenu, ID_SAVEAS);
    DeactivateCommand(&MainMenu, ID_CLOSE);

    if (SysConfig.EditorGlobalReadOnly) {		/* new in 0.7b */
        DeactivateCommand(&MainMenu, ID_NEW);
    }

    if (wnd != NULL && GetClass(wnd) == EDITBOX)
        {
        if (isMultiLine(wnd))
            {
            if (!SysConfig.EditorGlobalReadOnly) {	/* new in 0.7b */
                ActivateCommand(&MainMenu, ID_SAVE);
                ActivateCommand(&MainMenu, ID_SAVEAS);
            }
            ActivateCommand(&MainMenu, ID_CLOSE);
            }

	}

}

void PrepSearchMenu(void *w, struct Menu *mnu)
{
    WINDOW wnd = w;

    if (mnu != NULL) {}; /* unused parameter! */

    DeactivateCommand(&MainMenu, ID_SEARCH);
    DeactivateCommand(&MainMenu, ID_REPLACE);
    DeactivateCommand(&MainMenu, ID_SEARCHNEXT);

    if (wnd != NULL && GetClass(wnd) == EDITBOX) {
	if (isMultiLine(wnd))   {
	    ActivateCommand(&MainMenu, ID_SEARCH);
            if (!SysConfig.EditorGlobalReadOnly) {	/* new in 0.7b */
	        ActivateCommand(&MainMenu, ID_REPLACE);
	    }
	    ActivateCommand(&MainMenu, ID_SEARCHNEXT);
	}
    }
}

void PrepEditMenu(void *w, struct Menu *mnu)
{
    WINDOW wnd = w;

    if (mnu != NULL) {}; /* unused parameter! */

    DeactivateCommand(&MainMenu, ID_CUT);
    DeactivateCommand(&MainMenu, ID_COPY);
    DeactivateCommand(&MainMenu, ID_DELETETEXT);
    DeactivateCommand(&MainMenu, ID_PASTE);
    DeactivateCommand(&MainMenu, ID_UNDO);
    if (wnd != NULL && GetClass(wnd) == EDITBOX) {
	if (isMultiLine(wnd) &&
	    (!SysConfig.EditorGlobalReadOnly)) {	/* new mode in 0.7b */

	    if (TextBlockMarked(wnd))       {
		ActivateCommand(&MainMenu, ID_CUT);
		ActivateCommand(&MainMenu, ID_COPY);
		ActivateCommand(&MainMenu, ID_DELETETEXT);
	    }
	    if (!TestAttribute(wnd, READONLY) &&
		Clipboard != NULL)
		ActivateCommand(&MainMenu, ID_PASTE);
	    if (wnd->DeletedText != NULL)
		ActivateCommand(&MainMenu, ID_UNDO);
	} /* editable non-empty EDITBOX */
    }
}


static char *Menus[9] = {
    "~1.                      ",
    "~2.                      ",
    "~3.                      ",
    "~4.                      ",
    "~5.                      ",
    "~6.                      ",
    "~7.                      ",
    "~8.                      ",
    "~9.                      "
};

/* ----------- Prepare the Window menu (window list) ------------ */
void PrepWindowMenu(void *w, struct Menu *mnu)
{
    WINDOW wnd = w;
    struct PopDown *p0 = mnu->Selections;
    struct PopDown *pd = mnu->Selections + 2;	/* first 9 items */
    struct PopDown *ca = mnu->Selections + 13;
    int MenuNo = 0;
    WINDOW cwnd;
    mnu->Selection = 0;
    oldFocus = NULL;

    if (GetClass(wnd) != APPLICATION)    {
        oldFocus = wnd;
        /* ----- point to the APPLICATION window ----- */
	if (EditApplication == NULL)
		return;
	/* Application window is the "outer" / top level window. */
	/* It has FirstWindow and LastWindow child windows, and  */
	/* is Parent for them. The child windows are in a linked */
	/* circular list of NextWindow and PrevWindow items...   */
	/* Painting is done first to last, last being topmost... */
	/* normal.c and lists.c take care of that. As we want a  */
	/* STABLE / stacking-independent list, 0.7c has EXTRA    */
	/* links in window number order, maintained by normal.c! */
//#if CLASSIC_WINDOW_NUMBERING
//	cwnd = FirstWindow(ApplicationWindow);
//#else	/* new 0.7c: stacking-independent window numbering */
	cwnd = NumberOneChildWindow(EditApplication);
//#endif
        /* ----- get the first 9 document windows ----- */
        while (cwnd != NULL && MenuNo < 9)    {
	    /* printf("PREPWINMENU...\n"); */
            if (isVisible(cwnd) && GetClass(cwnd) != MENUBAR &&
                    GetClass(cwnd) != STATUSBAR) {
                /* --- add the document window to the menu --- */
                strncpy(Menus[MenuNo]+4, WindowName(cwnd), 20);
                /* fields are listed in menu.h... */
                pd->SelectionTitle = Menus[MenuNo];
                pd->Accelerator = ALT_1 + MenuNo;	/* new 0.7c */
                /* pd->ActionId left as is, pd->help not set at all... */
                if (cwnd == oldFocus)    {
                    /* -- mark the current document -- */
                    pd->Attrib |= CHECKED;
                    mnu->Selection = MenuNo+2;
                }
                else
                    pd->Attrib &= ~CHECKED;
                pd++;
                MenuNo++;
            }	/* if listable window */
	    cwnd = NextNumberedWindow(cwnd);
        } /* while in linked list of enumerateable windows */
    } /* build window list popdown menu */

    p0->SelectionTitle = "~Close all";
    if (MenuNo) {
        p0->Attrib &= ~INACTIVE;
        (mnu->Selections + 1)->SelectionTitle = (unsigned char *)SEPCHAR;
    }
    else {
        p0->Attrib |= INACTIVE;
        (mnu->Selections + 1)->SelectionTitle = NULL;
    }
    if (MenuNo >= 9)    {
        *pd++ = *ca;
        if (mnu->Selection == 0)
            mnu->Selection = 11;
    }
    if (MenuNo > 0)
        pd->SelectionTitle = NULL;
}

