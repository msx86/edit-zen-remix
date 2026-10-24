/* ---------------- edit.h ----------------- */

/*
 * Include file for EDIT 0.9X+
 *
 */


#ifndef EDIT_H
#define EDIT_H


#include "resource.h"
//#define INCLUDE_LOGGING


extern WINDOW EditApplication;
extern DBOX dbLog;



void PrepFileMenu(void *, struct Menu *);
void PrepEditMenu(void *, struct Menu *);
void PrepSearchMenu(void *, struct Menu *);
void PrepWindowMenu(void *, struct Menu *);





BEGIN_USER_COMMANDS
    /* --------------- File menu ---------------- */
    ID_OPEN,
    ID_NEW,
    ID_SAVE,
    ID_SAVEAS,
    ID_CLOSE,
    ID_EXIT,
    /* --------------- Edit menu ---------------- */
//    ID_UNDO,
//    ID_CUT,
//    ID_COPY,
//    ID_PASTE,
//    ID_PARAGRAPH,
//    ID_CLEAR,
//    ID_DELETETEXT,
    /* 0.7d additions for edit menu: */
//    ID_UPCASE,
//    ID_DOWNCASE,
//    ID_WORDCOUNT,
    /* --------------- Search Menu -------------- */
//    ID_SEARCH,
//    ID_REPLACE,
//    ID_SEARCHNEXT,
    /* --------------- Utilities Menu ------------- */

    /* -------------- Options menu -------------- */
    ID_INSERT,
    ID_WRAP,
    ID_LOG,
    ID_TABS,
    /* --------------- Window menu -------------- */
    ID_CLOSEALL,
    ID_WINDOW,
	ID_MOREWINDOWS,
    
    /* -------------- TabStops menu ------------- */
    ID_TAB0, /* tab-as-char mode -ea */
    ID_TAB2,
    ID_TAB4,
    ID_TAB6,
    ID_TAB8
END_USER_COMMANDS









#endif
