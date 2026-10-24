# Watcom Makefile for FreeDOS EDIT
.erase

CFLAGS=-q -bt=dos -l=dos4g
!ifdef DEBUG
CFLAGS += -d2
!else
CFLAGS += -ox
!endif

objs = &
	main.obj &
	edit.obj &
	applicat.obj &
	box.obj &
	button.obj &
	checkbox.obj &
	clipbord.obj &
	combobox.obj &
	commdlg.obj &
	console.obj &
	dfalloc.obj &
	dialbox.obj &
	direct.obj &
	editbox.obj &
	keys.obj &
	listbox.obj &
	lists.obj &
	menu.obj &
	menubar.obj &
	menus.obj &
	message.obj &
	mouse.obj &
	msgbox.obj &
	normal.obj &
	pictbox.obj &
	popdown.obj &
	radio.obj &
	rect.obj &
	search.obj &
	slidebox.obj &
	spinbutt.obj &
	statbar.obj &
	sysmenu.obj &
	systemrs.obj &
	text.obj &
	textbox.obj &
	video.obj &
	window.obj


!ifdef LOG
objs +=	logger.obj
#objs += log.obj &
!else
objs += nolog.obj
!endif
	
all: edit.exe

edit.exe: $(objs) edit.lnk Makefile
	wlink @edit.lnk
!ifdef UPX
	upx --best edit.exe
!endif

.c.obj: .autodepend
	wcl386 -c $(CFLAGS) -Fo=$*.obj $*.c

edit.lnk : Makefile .always
	@echo NAME $^& >$^@
	@echo SYSTEM DOS4G >>$^@
	@echo OPTION QUIET >>$^@
	@echo OPTION MAP >>$^@
	@echo OPTION STACK=32768 >>$^@
!ifdef DEBUG
	@echo DEBUG ALL >>$^@
!endif
	@for %i in ($(objs)) do @echo FILE %i >>$^@

clean: .symbolic
	rm -f *.obj
	rm -f *.exe
	rm -f *.OBJ
	rm -f *.EXE
	rm -f *.err
	rm -f *.ERR
