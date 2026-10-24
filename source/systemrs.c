/* ----------- dialogs.c --------------- */

#include "dfpcomp.h"
#include "resource.h"
#include "system.h"

DEFDFLATP
  MOD_DESCRIPTION("FreeDOS DFlat+")
  MOD_VERSION(1,0,0,0)
  MOD_COPYRIGHT("Santamaria/Auer/Cosentino/Dr.DobbJ")
  MOD_LICENSE("GNU GPL 2.0")
  MOD_ABOUT("FreeDOS DFlat+ application|framework for the FreeDOS Project")
END_DEFMODULE


/* -------------- the File Open dialog box --------------- */
DIALOGBOX(FileOpen)
    DB_TITLE("Open File", -1,-1,19,59)
    CONTROL(TEXT,    "File ~Name:",   3, 1, 1,10, ID_FILENAME)
    CONTROL(EDITBOX, NULL,           14, 1, 1,40, ID_FILENAME)
    CONTROL(TEXT,    NULL,            3, 3, 1,50, ID_PATH) 
    CONTROL(TEXT,    "~Files:",       3, 5, 1, 6, ID_FILES)
    CONTROL(LISTBOX, NULL,            3, 6,10,14, ID_FILES)
    CONTROL(TEXT,    "~Directories:",19, 5, 1,12, ID_DIRECTORY)
    CONTROL(LISTBOX, NULL,           19, 6,10,13, ID_DIRECTORY)
    CONTROL(TEXT,    "Dri~ves:",     34, 5, 1, 7, ID_DRIVE)
    CONTROL(LISTBOX, NULL,           34, 6,10,10, ID_DRIVE)
    CONTROL(BUTTON,  "   ~OK   ",    46, 7, 1, 8, ID_OK)
    CONTROL(BUTTON,  " ~Cancel ",    46,10, 1, 8, ID_CANCEL)
ENDDB

/* -------------- the Save As dialog box --------------- */
DIALOGBOX(SaveAs)
    DB_TITLE("Save As", -1,-1,19,59)
    CONTROL(TEXT,    "File ~Name:",   3, 1, 1, 9, ID_FILENAME)
    CONTROL(EDITBOX, NULL,           13, 1, 1,40, ID_FILENAME)
    CONTROL(TEXT,    NULL,            3, 3, 1,50, ID_PATH) 
    CONTROL(TEXT,    "~Files:",       3, 5, 1, 6, ID_FILES)
    CONTROL(LISTBOX, NULL,            3, 6,10,14, ID_FILES)
    CONTROL(TEXT,    "~Directories:",19, 5, 1,12, ID_DIRECTORY)
    CONTROL(LISTBOX, NULL,           19, 6,10,13, ID_DIRECTORY)
    CONTROL(TEXT,    "Dri~ves:",     34, 5, 1, 7, ID_DRIVE)
    CONTROL(LISTBOX, NULL,           34, 6,10,10, ID_DRIVE)
    CONTROL(BUTTON,  "   ~OK   ",    46, 7, 1, 8, ID_OK)
    CONTROL(BUTTON,  " ~Cancel ",    46,10, 1, 8, ID_CANCEL)
ENDDB


/* -------------- the Search Text dialog box --------------- */
DIALOGBOX(SearchTextDB)
    DB_TITLE("Find",-1,-1,9,48)
    CONTROL(TEXT,    "~Find What:",           2, 1, 1, 11, ID_SEARCHFOR)
    CONTROL(EDITBOX, NULL,                   14, 1, 1, 29, ID_SEARCHFOR)
    CONTROL(TEXT, "~Match Case:"  ,           2, 3, 1, 23, ID_MATCHCASE)
	CONTROL(CHECKBOX,  NULL,                 26, 3, 1,  3, ID_MATCHCASE)
    CONTROL(BUTTON, "   ~OK   ",             13, 5, 1,  8, ID_OK)
    CONTROL(BUTTON, " ~Cancel ",             25, 5, 1,  8, ID_CANCEL)
ENDDB

/* -------------- the Replace Text dialog box --------------- */
DIALOGBOX(ReplaceTextDB)
    DB_TITLE("Replace",-1,-1,12,50)
    CONTROL(TEXT,    "~Search for:",          2, 1, 1, 11, ID_SEARCHFOR)
    CONTROL(EDITBOX, NULL,                   16, 1, 1, 29, ID_SEARCHFOR)
    CONTROL(TEXT,    "~Replace with:",        2, 3, 1, 13, ID_REPLACEWITH)
    CONTROL(EDITBOX, NULL,                   16, 3, 1, 29, ID_REPLACEWITH)
    CONTROL(TEXT, "~Match Case:",             2, 5, 1, 23, ID_MATCHCASE)
	CONTROL(CHECKBOX,  NULL,                 26, 5, 1,  3, ID_MATCHCASE)
    CONTROL(TEXT, "Replace ~All:",            2, 6, 1, 23, ID_REPLACEALL)
	CONTROL(CHECKBOX,  NULL,                 26, 6, 1,  3, ID_REPLACEALL)
    CONTROL(BUTTON, "   ~OK   ",              13, 8, 1,  8, ID_OK)
    CONTROL(BUTTON, " ~Cancel ",             25, 8, 1,  8, ID_CANCEL)
ENDDB

/* -------------- generic message dialog box --------------- */
DIALOGBOX(MsgBox)
    DB_TITLE(NULL,-1,-1, 0, 0)
    CONTROL(TEXT,   NULL,   1, 1, 0, 0, 0)
    CONTROL(BUTTON, NULL,   0, 0, 1, 8, ID_OK)
    CONTROL(0,      NULL,   0, 0, 1, 8, ID_CANCEL)
ENDDB

/* ----------- InputBox Dialog Box ------------ */
DIALOGBOX(InputBoxDB)
    DB_TITLE(NULL,-1,-1, 9, 0)
    CONTROL(TEXT,    NULL,       1, 1, 1, 0, 0)
	CONTROL(EDITBOX, NULL,       1, 3, 1, 0, ID_INPUTTEXT)
    CONTROL(BUTTON, "   ~OK   ", 0, 5, 1, 8, ID_OK)
    CONTROL(BUTTON, " ~Cancel ", 0, 5, 1, 8, ID_CANCEL)
ENDDB

/* ----------- SliderBox Dialog Box ------------- */
DIALOGBOX(SliderBoxDB)
    DB_TITLE(NULL,-1,-1, 9, 0)
    CONTROL(TEXT,   NULL,       0, 1, 1, 0, 0)
    CONTROL(TEXT,   NULL,       0, 3, 1, 0, 0)
    CONTROL(BUTTON, " Cancel ", 0, 5, 1, 8, ID_CANCEL)
ENDDB


/* ------------ Windows dialog box -------------- */
DIALOGBOX(Windows)
    DB_TITLE("Windows", -1, -1, 19, 24)
    CONTROL(LISTBOX, NULL,         1,  1,11,20, ID_WINDOWLIST)
    CONTROL(BUTTON,  "   ~OK   ",  2, 13, 1, 8, ID_OK)
    CONTROL(BUTTON,  " ~Cancel ", 12, 13, 1, 8, ID_CANCEL)
ENDDB

#ifdef INCLUDE_LOGGING
/* ------------ Message Log dialog box -------------- */
DIALOGBOX(dbLog)
    DB_TITLE("Edit Message Log", -1, -1, 18, 41)
    CONTROL(TEXT,  "~Messages",   10,   1,  1,  8, ID_LOGLIST)
    CONTROL(LISTBOX,    NULL,     1,    2, 14, 26, ID_LOGLIST)
    CONTROL(TEXT,    "~Logging:", 29,   4,  1, 10, ID_LOGGING)
    CONTROL(CHECKBOX,    NULL,    31,   5,  1,  3, ID_LOGGING)
    CONTROL(BUTTON,  "   ~OK   ", 29,   7,  1,  8, ID_OK)
    CONTROL(BUTTON,  " ~Cancel ", 29,  10,  1,  8, ID_CANCEL)
ENDDB
#endif


/* ------------- the System Menu --------------------- */
DEFMENU(SystemMenu)
    POPDOWN("System Menu", NULL, NULL)
#ifdef INCLUDE_RESTORE
        SELECTION("~Restore",  ID_SYSRESTORE,  0,         0 )
#endif
        SELECTION("~Move",     ID_SYSMOVE,     0,         0 )
        SELECTION("~Size",     ID_SYSSIZE,     0,         0 )
#ifdef INCLUDE_MINIMIZE
        SELECTION("Mi~nimize", ID_SYSMINIMIZE, 0,         0 )
#endif
#ifdef INCLUDE_MAXIMIZE
        SELECTION("Ma~ximize", ID_SYSMAXIMIZE, 0,         0 )
#endif
        SEPARATOR
        SELECTION("~Close",    ID_SYSCLOSE,    CTRL_F4,   0 )
    ENDPOPDOWN
ENDMENU

/* ----- default colors for color video system ----- */
//unsigned char color[CLASSCOUNT] [4] [2] = {
ColorScheme color = {
		FALSE, 0,{
    /* ------------ NORMAL ------------ */
   {{LIGHTGRAY, BLACK}, /* STD_COLOR    */
    {LIGHTGRAY, BLACK}, /* SELECT_COLOR */
    {WHITE, BLACK},     /* FRAME_COLOR  */
    {LIGHTGRAY, BLACK}},/* HILITE_COLOR */

    /* ---------- APPLICATION --------- */
   {{LIGHTGRAY, BLUE},  /* STD_COLOR    */
    {LIGHTGRAY, BLUE},  /* SELECT_COLOR */
    {LIGHTGRAY, BLUE},  /* FRAME_COLOR  */
    {LIGHTGRAY, BLUE}}, /* HILITE_COLOR */

    /* ------------ TEXTBOX ----------- */
   {{BLACK, LIGHTGRAY}, /* STD_COLOR    */
    {LIGHTGRAY, BLACK}, /* SELECT_COLOR */
    {BLACK, LIGHTGRAY}, /* FRAME_COLOR  */
    {BLACK, LIGHTGRAY}},/* HILITE_COLOR */

    /* ------------ LISTBOX ----------- */
   {{BLACK, LIGHTGRAY}, /* STD_COLOR    */
    {LIGHTGRAY, BLACK}, /* SELECT_COLOR */
    {BLACK, LIGHTGRAY}, /* FRAME_COLOR  */
    {BLACK, LIGHTGRAY}},/* HILITE_COLOR */

    /* ----------- EDITBOX ------------ */
   {{LIGHTGRAY, BLUE}, /* STD_COLOR    */
    {BLACK, LIGHTGRAY}, /* SELECT_COLOR */
    {LIGHTGRAY, BLACK}, /* FRAME_COLOR  */
    {BLACK, LIGHTGRAY}},/* HILITE_COLOR */

    /* ---------- MENUBAR ------------- */
   {{BLACK, LIGHTGRAY}, /* STD_COLOR    */
    {LIGHTGRAY, BLACK},      /* SELECT_COLOR */
    {BLACK, LIGHTGRAY}, /* FRAME_COLOR  */
    {DARKGRAY, WHITE}},   /* HILITE_COLOR
                          Inactive, Shortcut (both FG) */

    /* ---------- POPDOWNMENU --------- */
   {{BLACK, LIGHTGRAY},      /* STD_COLOR    */
    {LIGHTGRAY, BLACK}, /* SELECT_COLOR */
    {BLACK, LIGHTGRAY},      /* FRAME_COLOR  */
    {DARKGRAY, WHITE}},   /* HILITE_COLOR
                           Inactive ,Shortcut (both FG) */

#ifdef INCLUDE_PICTUREBOX
    /* ------------ PICTUREBOX ----------- */
   {{BLACK, LIGHTGRAY}, /* STD_COLOR    */
    {LIGHTGRAY, BLACK}, /* SELECT_COLOR */
    {BLACK, LIGHTGRAY}, /* FRAME_COLOR  */
    {BLACK, LIGHTGRAY}},/* HILITE_COLOR */
#endif

    /* ------------- DIALOG ----------- */
   {{BLACK, LIGHTGRAY}, /* STD_COLOR    */
    {BLACK, LIGHTGRAY}, /* SELECT_COLOR */
    {BLACK, LIGHTGRAY}, /* FRAME_COLOR  */
    {LIGHTGRAY, BLUE}}, /* HILITE_COLOR */

	/* ------------ BOX --------------- */
   {{LIGHTGRAY, BLUE},  /* STD_COLOR    */
    {LIGHTGRAY, BLUE},  /* SELECT_COLOR */
    {LIGHTGRAY, BLUE},  /* FRAME_COLOR  */
    {LIGHTGRAY, BLUE}}, /* HILITE_COLOR */

    /* ------------ BUTTON ------------ */
   {{BLACK, CYAN},      /* STD_COLOR    */
    {WHITE, CYAN},      /* SELECT_COLOR */
    {BLACK, CYAN},      /* FRAME_COLOR  */
    {DARKGRAY, RED}},   /* HILITE_COLOR
                           Inactive ,Shortcut (both FG) */
    /* ------------ COMBOBOX ----------- */
   {{BLACK, LIGHTGRAY}, /* STD_COLOR    */
    {LIGHTGRAY, BLACK}, /* SELECT_COLOR */
    {LIGHTGRAY, BLACK}, /* FRAME_COLOR  */
    {BLACK, LIGHTGRAY}},/* HILITE_COLOR */

    /* ------------- TEXT ----------- */
   {{0xff, 0xff},  /* STD_COLOR    */
    {0xff, 0xff},  /* SELECT_COLOR */
    {0xff, 0xff},  /* FRAME_COLOR  */
    {0xff, 0xff}}, /* HILITE_COLOR */

    /* ------------- RADIOBUTTON ----------- */
   {{LIGHTGRAY, BLUE},  /* STD_COLOR    */
    {BLACK, LIGHTGRAY}, /* SELECT_COLOR */
    {LIGHTGRAY, BLUE},  /* FRAME_COLOR  */
    {LIGHTGRAY, BLUE}}, /* HILITE_COLOR */

    /* ------------- CHECKBOX ----------- */
   {{LIGHTGRAY, BLUE},  /* STD_COLOR    */
    {BLACK, LIGHTGRAY}, /* SELECT_COLOR */
    {LIGHTGRAY, BLUE},  /* FRAME_COLOR  */
    {LIGHTGRAY, BLUE}}, /* HILITE_COLOR */

    /* ------------ SPINBUTTON ----------- */
   {{BLACK, LIGHTGRAY}, /* STD_COLOR    */
    {BLACK, LIGHTGRAY}, /* SELECT_COLOR */
    {LIGHTGRAY, BLACK}, /* FRAME_COLOR  */
    {BLACK, LIGHTGRAY}},/* HILITE_COLOR */

    /* ----------- ERRORBOX ----------- */
   {{YELLOW, RED},      /* STD_COLOR    */
    {YELLOW, RED},      /* SELECT_COLOR */
    {YELLOW, RED},      /* FRAME_COLOR  */
    {YELLOW, RED}},     /* HILITE_COLOR */

    /* ----------- MESSAGEBOX --------- */
   {{BLACK, LIGHTGRAY}, /* STD_COLOR    */
    {BLACK, LIGHTGRAY}, /* SELECT_COLOR */
    {BLACK, LIGHTGRAY}, /* FRAME_COLOR  */
    {BLACK, LIGHTGRAY}},/* HILITE_COLOR */

    /* ---------- STATUSBAR ------------- */
   {{BLACK, LIGHTGRAY},      /* STD_COLOR    */
    {BLACK, LIGHTGRAY},      /* SELECT_COLOR */
    {BLACK, LIGHTGRAY},      /* FRAME_COLOR  */
    {BLACK, LIGHTGRAY}},     /* HILITE_COLOR */

//    /* ----------- EDITOR ------------ */
//   {{LIGHTGRAY, BLUE},  /* STD_COLOR    */
//    {BLACK, LIGHTGRAY}, /* SELECT_COLOR */
//    {BLACK, LIGHTGRAY}, /* FRAME_COLOR  */
//    {BLACK, LIGHTGRAY}},/* HILITE_COLOR */

    /* ---------- TITLEBAR ------------ */
   {{BLACK, CYAN},      /* STD_COLOR    */
    {BLACK, CYAN},      /* SELECT_COLOR */
    {CYAN, BLACK},      /* FRAME_COLOR  */
    {WHITE, CYAN}},     /* HILITE_COLOR */

    /* ------------ DUMMY ------------- */
   {{GREEN, LIGHTGRAY}, /* STD_COLOR    */
    {GREEN, LIGHTGRAY}, /* SELECT_COLOR */
    {GREEN, LIGHTGRAY}, /* FRAME_COLOR  */
    {GREEN, LIGHTGRAY}} /* HILITE_COLOR */
}};

/* ----- default colors for mono video system ----- */
//unsigned char bw[CLASSCOUNT] [4] [2] = {
ColorScheme bw = {
		TRUE, 1,{
    /* ------------ NORMAL ------------ */
   {{LIGHTGRAY, BLACK}, /* STD_COLOR    */
    {LIGHTGRAY, BLACK}, /* SELECT_COLOR */
    {LIGHTGRAY, BLACK}, /* FRAME_COLOR  */
    {LIGHTGRAY, BLACK}},/* HILITE_COLOR */

    /* ---------- APPLICATION --------- */
   {{LIGHTGRAY, BLACK}, /* STD_COLOR    */
    {LIGHTGRAY, BLACK}, /* SELECT_COLOR */
    {LIGHTGRAY, BLACK}, /* FRAME_COLOR  */
    {LIGHTGRAY, BLACK}},/* HILITE_COLOR */

    /* ------------ TEXTBOX ----------- */
   {{BLACK, LIGHTGRAY}, /* STD_COLOR    */
    {LIGHTGRAY, BLACK}, /* SELECT_COLOR */
    {BLACK, LIGHTGRAY}, /* FRAME_COLOR  */
    {BLACK, LIGHTGRAY}},/* HILITE_COLOR */

    /* ------------ LISTBOX ----------- */
   {{LIGHTGRAY, BLACK}, /* STD_COLOR    */
    {BLACK, LIGHTGRAY}, /* SELECT_COLOR */
    {LIGHTGRAY, BLACK}, /* FRAME_COLOR  */
    {BLACK, LIGHTGRAY}},/* HILITE_COLOR */

    /* ----------- EDITBOX ------------ */
   {{LIGHTGRAY, BLACK}, /* STD_COLOR    */
    {BLACK, LIGHTGRAY}, /* SELECT_COLOR */
    {LIGHTGRAY, BLACK}, /* FRAME_COLOR  */
    {BLACK, LIGHTGRAY}},/* HILITE_COLOR */

    /* ---------- MENUBAR ------------- */
   {{LIGHTGRAY, BLACK}, /* STD_COLOR    */
    {BLACK, LIGHTGRAY}, /* SELECT_COLOR */
    {BLACK, LIGHTGRAY}, /* FRAME_COLOR  */
    {DARKGRAY, WHITE}}, /* HILITE_COLOR
                           Inactive, Shortcut (both FG) */

    /* ---------- POPDOWNMENU --------- */
   {{LIGHTGRAY, BLACK}, /* STD_COLOR    */
    {BLACK, LIGHTGRAY}, /* SELECT_COLOR */
    {LIGHTGRAY, BLACK}, /* FRAME_COLOR  */
    {DARKGRAY, WHITE}}, /* HILITE_COLOR
                           Inactive ,Shortcut (both FG) */

#ifdef INCLUDE_PICTUREBOX
    /* ------------ PICTUREBOX ----------- */
   {{BLACK, LIGHTGRAY}, /* STD_COLOR    */
    {LIGHTGRAY, BLACK}, /* SELECT_COLOR */
    {BLACK, LIGHTGRAY}, /* FRAME_COLOR  */
    {BLACK, LIGHTGRAY}},/* HILITE_COLOR */
#endif

    /* ------------- DIALOG ----------- */
   {{LIGHTGRAY, BLACK},  /* STD_COLOR    */
    {BLACK, LIGHTGRAY},  /* SELECT_COLOR */
    {LIGHTGRAY, BLACK},  /* FRAME_COLOR  */
    {LIGHTGRAY, BLACK}}, /* HILITE_COLOR */

	/* ------------ BOX --------------- */
   {{LIGHTGRAY, BLACK},  /* STD_COLOR    */
    {LIGHTGRAY, BLACK},  /* SELECT_COLOR */
    {LIGHTGRAY, BLACK},  /* FRAME_COLOR  */
    {LIGHTGRAY, BLACK}}, /* HILITE_COLOR */

    /* ------------ BUTTON ------------ */
   {{BLACK, LIGHTGRAY}, /* STD_COLOR    */
    {WHITE, LIGHTGRAY}, /* SELECT_COLOR */
    {BLACK, LIGHTGRAY}, /* FRAME_COLOR  */
    {DARKGRAY, WHITE}}, /* HILITE_COLOR
                           Inactive ,Shortcut (both FG) */
    /* ------------ COMBOBOX ----------- */
   {{BLACK, LIGHTGRAY}, /* STD_COLOR    */
    {LIGHTGRAY, BLACK}, /* SELECT_COLOR */
    {BLACK, LIGHTGRAY}, /* FRAME_COLOR  */
    {BLACK, LIGHTGRAY}},/* HILITE_COLOR */

    /* ------------- TEXT ----------- */
   {{0xff, 0xff},  /* STD_COLOR    */
    {0xff, 0xff},  /* SELECT_COLOR */
    {0xff, 0xff},  /* FRAME_COLOR  */
    {0xff, 0xff}}, /* HILITE_COLOR */

    /* ------------- RADIOBUTTON ----------- */
   {{LIGHTGRAY, BLACK},  /* STD_COLOR    */
    {BLACK, LIGHTGRAY},  /* SELECT_COLOR */
    {LIGHTGRAY, BLACK},  /* FRAME_COLOR  */
    {LIGHTGRAY, BLACK}}, /* HILITE_COLOR */

    /* ------------- CHECKBOX ----------- */
   {{LIGHTGRAY, BLACK},  /* STD_COLOR    */
    {BLACK, LIGHTGRAY},  /* SELECT_COLOR */
    {LIGHTGRAY, BLACK},  /* FRAME_COLOR  */
    {LIGHTGRAY, BLACK}}, /* HILITE_COLOR */

    /* ------------ SPINBUTTON ----------- */
   {{BLACK, LIGHTGRAY}, /* STD_COLOR    */
    {BLACK, LIGHTGRAY}, /* SELECT_COLOR */
    {BLACK, LIGHTGRAY}, /* FRAME_COLOR  */
    {BLACK, LIGHTGRAY}},/* HILITE_COLOR */

    /* ----------- ERRORBOX ----------- */
   {{LIGHTGRAY, BLACK}, /* STD_COLOR    */
    {LIGHTGRAY, BLACK}, /* SELECT_COLOR */
    {LIGHTGRAY, BLACK}, /* FRAME_COLOR  */
    {LIGHTGRAY, BLACK}},/* HILITE_COLOR */

    /* ----------- MESSAGEBOX --------- */
   {{LIGHTGRAY, BLACK}, /* STD_COLOR    */
    {LIGHTGRAY, BLACK}, /* SELECT_COLOR */
    {LIGHTGRAY, BLACK}, /* FRAME_COLOR  */
    {LIGHTGRAY, BLACK}},/* HILITE_COLOR */

    /* ---------- STATUSBAR ------------- */
   {{BLACK, LIGHTGRAY}, /* STD_COLOR    */
    {BLACK, LIGHTGRAY}, /* SELECT_COLOR */
    {BLACK, LIGHTGRAY}, /* FRAME_COLOR  */
    {BLACK, LIGHTGRAY}},/* HILITE_COLOR */

//    /* ----------- EDITOR ------------ */
//   {{BLACK, LIGHTGRAY}, /* STD_COLOR    */
//    {LIGHTGRAY, BLACK}, /* SELECT_COLOR */
//    {BLACK, LIGHTGRAY}, /* FRAME_COLOR  */
//    {LIGHTGRAY, BLACK}},/* HILITE_COLOR */

    /* ---------- TITLEBAR ------------ */
   {{BLACK, LIGHTGRAY}, /* STD_COLOR    */
    {BLACK, LIGHTGRAY}, /* SELECT_COLOR */
    {BLACK, LIGHTGRAY}, /* FRAME_COLOR  */
    {BLACK, LIGHTGRAY}},/* HILITE_COLOR */

    /* ------------ DUMMY ------------- */
   {{BLACK, LIGHTGRAY}, /* STD_COLOR    */
    {BLACK, LIGHTGRAY}, /* SELECT_COLOR */
    {BLACK, LIGHTGRAY}, /* FRAME_COLOR  */
    {BLACK, LIGHTGRAY}} /* HILITE_COLOR */
}};
/* ----- default colors for reverse mono video ----- */
//unsigned char reverse[CLASSCOUNT] [4] [2] = {
ColorScheme reverse = {
		TRUE, 2,{
    /* ------------ NORMAL ------------ */
   {{BLACK, LIGHTGRAY}, /* STD_COLOR    */
    {BLACK, LIGHTGRAY}, /* SELECT_COLOR */
    {BLACK, LIGHTGRAY}, /* FRAME_COLOR  */
    {BLACK, LIGHTGRAY}},/* HILITE_COLOR */

    /* ---------- APPLICATION --------- */
   {{BLACK, LIGHTGRAY}, /* STD_COLOR    */
    {BLACK, LIGHTGRAY}, /* SELECT_COLOR */
    {BLACK, LIGHTGRAY}, /* FRAME_COLOR  */
    {BLACK, LIGHTGRAY}},/* HILITE_COLOR */

    /* ------------ TEXTBOX ----------- */
   {{BLACK, LIGHTGRAY}, /* STD_COLOR    */
    {LIGHTGRAY, BLACK}, /* SELECT_COLOR */
    {BLACK, LIGHTGRAY}, /* FRAME_COLOR  */
    {BLACK, LIGHTGRAY}},/* HILITE_COLOR */

    /* ------------ LISTBOX ----------- */
   {{BLACK, LIGHTGRAY}, /* STD_COLOR    */
    {LIGHTGRAY, BLACK}, /* SELECT_COLOR */
    {BLACK, LIGHTGRAY}, /* FRAME_COLOR  */
    {BLACK, LIGHTGRAY}},/* HILITE_COLOR */

    /* ----------- EDITBOX ------------ */
   {{BLACK, LIGHTGRAY}, /* STD_COLOR    */
    {LIGHTGRAY, BLACK}, /* SELECT_COLOR */
    {BLACK, LIGHTGRAY}, /* FRAME_COLOR  */
    {BLACK, LIGHTGRAY}},/* HILITE_COLOR */

    /* ---------- MENUBAR ------------- */
   {{BLACK, LIGHTGRAY}, /* STD_COLOR    */
    {LIGHTGRAY, BLACK}, /* SELECT_COLOR */
    {LIGHTGRAY, BLACK}, /* FRAME_COLOR  */
    {DARKGRAY, WHITE}}, /* HILITE_COLOR
                           Inactive, Shortcut (both FG) */

    /* ---------- POPDOWNMENU --------- */
   {{LIGHTGRAY, BLACK}, /* STD_COLOR    */
    {BLACK, LIGHTGRAY}, /* SELECT_COLOR */
    {LIGHTGRAY, BLACK}, /* FRAME_COLOR  */
    {DARKGRAY, WHITE}}, /* HILITE_COLOR
                           Inactive ,Shortcut (both FG) */

#ifdef INCLUDE_PICTUREBOX
    /* ------------ PICTUREBOX ----------- */
   {{BLACK, LIGHTGRAY}, /* STD_COLOR    */
    {LIGHTGRAY, BLACK}, /* SELECT_COLOR */
    {BLACK, LIGHTGRAY}, /* FRAME_COLOR  */
    {BLACK, LIGHTGRAY}},/* HILITE_COLOR */
#endif

    /* ------------- DIALOG ----------- */
   {{BLACK, LIGHTGRAY},  /* STD_COLOR    */
    {LIGHTGRAY, BLACK},  /* SELECT_COLOR */
    {BLACK, LIGHTGRAY},  /* FRAME_COLOR  */
    {BLACK, LIGHTGRAY}}, /* HILITE_COLOR */

	/* ------------ BOX --------------- */
   {{BLACK, LIGHTGRAY},  /* STD_COLOR    */
    {BLACK, LIGHTGRAY},  /* SELECT_COLOR */
    {BLACK, LIGHTGRAY},  /* FRAME_COLOR  */
    {BLACK, LIGHTGRAY}}, /* HILITE_COLOR */

    /* ------------ BUTTON ------------ */
   {{LIGHTGRAY, BLACK}, /* STD_COLOR    */
    {WHITE, BLACK},     /* SELECT_COLOR */
    {LIGHTGRAY, BLACK}, /* FRAME_COLOR  */
    {DARKGRAY, WHITE}}, /* HILITE_COLOR
                           Inactive ,Shortcut (both FG) */
    /* ------------ COMBOBOX ----------- */
   {{BLACK, LIGHTGRAY}, /* STD_COLOR    */
    {LIGHTGRAY, BLACK}, /* SELECT_COLOR */
    {LIGHTGRAY, BLACK}, /* FRAME_COLOR  */
    {BLACK, LIGHTGRAY}},/* HILITE_COLOR */

    /* ------------- TEXT ----------- */
   {{0xff, 0xff},  /* STD_COLOR    */
    {0xff, 0xff},  /* SELECT_COLOR */
    {0xff, 0xff},  /* FRAME_COLOR  */
    {0xff, 0xff}}, /* HILITE_COLOR */

    /* ------------- RADIOBUTTON ----------- */
   {{BLACK, LIGHTGRAY},  /* STD_COLOR    */
    {LIGHTGRAY, BLACK},  /* SELECT_COLOR */
    {BLACK, LIGHTGRAY},  /* FRAME_COLOR  */
    {BLACK, LIGHTGRAY}}, /* HILITE_COLOR */

    /* ------------- CHECKBOX ----------- */
   {{BLACK, LIGHTGRAY},  /* STD_COLOR    */
    {LIGHTGRAY, BLACK},  /* SELECT_COLOR */
    {BLACK, LIGHTGRAY},  /* FRAME_COLOR  */
    {BLACK, LIGHTGRAY}}, /* HILITE_COLOR */

    /* ------------ SPINBUTTON ----------- */
   {{LIGHTGRAY, BLACK}, /* STD_COLOR    */
    {LIGHTGRAY, BLACK}, /* SELECT_COLOR */
    {LIGHTGRAY, BLACK}, /* FRAME_COLOR  */
    {BLACK, LIGHTGRAY}},/* HILITE_COLOR */

    /* ----------- ERRORBOX ----------- */
   {{BLACK, LIGHTGRAY},      /* STD_COLOR    */
    {BLACK, LIGHTGRAY},      /* SELECT_COLOR */
    {BLACK, LIGHTGRAY},      /* FRAME_COLOR  */
    {BLACK, LIGHTGRAY}},     /* HILITE_COLOR */

    /* ----------- MESSAGEBOX --------- */
   {{BLACK, LIGHTGRAY}, /* STD_COLOR    */
    {BLACK, LIGHTGRAY}, /* SELECT_COLOR */
    {BLACK, LIGHTGRAY}, /* FRAME_COLOR  */
    {BLACK, LIGHTGRAY}},/* HILITE_COLOR */

    /* ---------- STATUSBAR ------------- */
   {{LIGHTGRAY, BLACK},      /* STD_COLOR    */
    {LIGHTGRAY, BLACK},      /* SELECT_COLOR */
    {LIGHTGRAY, BLACK},      /* FRAME_COLOR  */
    {LIGHTGRAY, BLACK}},     /* HILITE_COLOR */

//    /* ----------- EDITOR ------------ */
//   {{LIGHTGRAY, BLACK}, /* STD_COLOR    */
//    {BLACK, LIGHTGRAY}, /* SELECT_COLOR */
//    {LIGHTGRAY, BLACK}, /* FRAME_COLOR  */
//    {BLACK, LIGHTGRAY}},/* HILITE_COLOR */
//
	/* ---------- TITLEBAR ------------ */
   {{LIGHTGRAY, BLACK},      /* STD_COLOR    */
    {LIGHTGRAY, BLACK},      /* SELECT_COLOR */
    {LIGHTGRAY, BLACK},      /* FRAME_COLOR  */
    {LIGHTGRAY, BLACK}},     /* HILITE_COLOR */

    /* ------------ DUMMY ------------- */
   {{LIGHTGRAY, BLACK}, /* STD_COLOR    */
    {LIGHTGRAY, BLACK}, /* SELECT_COLOR */
    {LIGHTGRAY, BLACK}, /* FRAME_COLOR  */
    {LIGHTGRAY, BLACK}} /* HILITE_COLOR */
}};
