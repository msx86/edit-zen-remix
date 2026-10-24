/* -------------- menus.c ------------- */

#include <stdlib.h>
#include "resource.h"
#include "edit.h"


DEFPROGRAM
  MOD_DESCRIPTION("FreeDOS Edit Zen Remix")
  MOD_VERSION(1,0,0,0)
  MOD_COPYRIGHT("")
  MOD_LICENSE("GNU GPL 2.0")
  MOD_ABOUT("A low distraction writing environment for DOS")
END_DEFMODULE



/* --------------------- the main menu --------------------- */
DEFMENU(MainMenu)
    /* --------------- the File popdown menu ----------------*/
    POPDOWN( "~File",  PrepFileMenu, "Commands for manipulating files" )
        SELECTION( "~New",        ID_NEW,     CTRL_N, 0) /* 0.7a */
        SELECTION( "~Open...",    ID_OPEN,    CTRL_O, 0) /* 0.7a */
        SEPARATOR
        SELECTION( "~Save",       ID_SAVE,    CTRL_S, INACTIVE) /* 0.7a */
        SELECTION( "Save ~as...", ID_SAVEAS,       0, INACTIVE)
        SELECTION( "~Close",      ID_CLOSE,        0, INACTIVE)
        SEPARATOR
        SELECTION( "E~xit",       ID_EXIT,     ALT_X, 0)
    ENDPOPDOWN

    /* --------------- the Edit popdown menu ----------------*/
    POPDOWN( "~Edit", PrepEditMenu, "Commands for editing files" )
#ifdef HOOKKEYB
        SELECTION( "~Undo",      ID_UNDO,  ALT_BS,    INACTIVE)
#else
        SELECTION( "~Undo",      ID_UNDO,  CTRL_Z,    INACTIVE)
#endif
        SEPARATOR
        SELECTION( "Cu~t",       ID_CUT,   CTRL_X, INACTIVE)
        SELECTION( "~Copy",      ID_COPY,  CTRL_C,  INACTIVE)
        /* ^-- must handle ^C / ^Break as "ignore" to use this */
        SELECTION( "~Paste",     ID_PASTE, CTRL_V, INACTIVE)
        SELECTION( "~Delete",    ID_DELETETEXT, DEL,  INACTIVE)
    ENDPOPDOWN

    /* --------------- the Search popdown menu ----------------*/
    POPDOWN( "~Search", PrepSearchMenu, "Search and replace text" )
        SELECTION( "~Find", ID_SEARCH,      CTRL_F,    INACTIVE)
                        /* *** CTRL_F added 0.7c, see also editbox.c *** */
        SELECTION( "Repeat ~Last Find",      ID_SEARCHNEXT,  F3,   INACTIVE)
        SELECTION( "~Replace",ID_REPLACE,     0,    INACTIVE)
    ENDPOPDOWN



    /* ------------- the Options popdown menu ---------------*/
    POPDOWN( "~Options", NULL, "Commands for setting editor options" )
#ifdef INCLUDE_LOGGING
        SELECTION( "~Log messages", ID_LOG,     ALT_L,      0 )
        SEPARATOR
#endif
        SELECTION( "~Insert",       ID_INSERT,     INS, TOGGLE)
        SELECTION( "~Word wrap",    ID_WRAP,        0,  TOGGLE)
        SELECTION( "~Tabs ( )",     ID_TABS,        0,  CASCADED)
    ENDPOPDOWN

    /* --------------- the Window popdown menu --------------*/
    POPDOWN( "~Window", PrepWindowMenu, "Select/close document windows" )
        SELECTION(  "~Close all",  ID_CLOSEALL, 0, 0)
        SEPARATOR
        SELECTION(  NULL,  ID_WINDOW, 0, 0 )
        SELECTION(  NULL,  ID_WINDOW, 0, 0 )
        SELECTION(  NULL,  ID_WINDOW, 0, 0 )
        SELECTION(  NULL,  ID_WINDOW, 0, 0 )
        SELECTION(  NULL,  ID_WINDOW, 0, 0 )
        SELECTION(  NULL,  ID_WINDOW, 0, 0 )
        SELECTION(  NULL,  ID_WINDOW, 0, 0 )
        SELECTION(  NULL,  ID_WINDOW, 0, 0 )
        SELECTION(  NULL,  ID_WINDOW, 0, 0 )
        SELECTION(  NULL,  ID_WINDOW, 0, 0 )
        SELECTION(  NULL,  ID_WINDOW, 0, 0 )
        SELECTION(  "~More Windows...", ID_MOREWINDOWS, 0, 0)
        SELECTION(  NULL,  ID_WINDOW, 0, 0 )
    ENDPOPDOWN



        /* ----- cascaded pulldown from Tabs... above ----- */
        CASCADED_POPDOWN( ID_TABS, NULL )
                SELECTION( "raw tabs ~0", ID_TAB0, 0, 0) /* -ea */
                SELECTION( "tab size ~2", ID_TAB2, 0, 0)
                SELECTION( "tab size ~4", ID_TAB4, 0, 0)
                SELECTION( "tab size ~6", ID_TAB6, 0, 0)
                SELECTION( "tab size ~8", ID_TAB8, 0, 0)
    ENDPOPDOWN

ENDMENU


