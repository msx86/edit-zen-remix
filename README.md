# FreeDOS Edit Zen Remix

A low-distraction writing environment, like a writerdeck, for DOS.

## Usage

Add the /z parameter to open in full screen editing mode, e.g.

edit /z file.txt

From the editor, press ALT to access the menu.

## Differences from FreeDOS EDIT 

The project is derived from FreeDOS EDIT 0.9c.  I did this as it was a nice codebase to start from.

The changes made are:

* 32 bit to allow for working on larger files
* Added fullscreen editing mode
* Added dynamic visual word wrap (it doesn't save the line breaks into the file)
* Added very basic syntax highlighting for markdown files
* Slightly modified behaviour of next word/previous word shortcuts
* Changed the menus to look more like QBasic
* Added some optimisations and bounds checks
* Removed things that I didn't personally need:
    * The clock
    * ASCII table
    * Calendar
    * Watch icon
    * Clear, Uppercase and Lowercase functions
    * Original word wrap implementation
    * High res text modes
    * CGA
    * Loading and saving config settings
    * Help

The code changes are AI generated.

To build, make sure the Open Watcom C v1.9+ tool chain is available,and invoke `wmake` inside the `source` directory. Generated executable is `edit.exe`.
