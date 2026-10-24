/* ------------ main .c ------------ */

#include "dfpcomp.h"

SysConfigType SysConfig = {
	  {FALSE,255},             /* Current Color Scheme */
	  {80,25,FALSE,"No set"},  /* Resolution */
	  8,                       /* Editor Tab Size */
	  FALSE                    /* Editor Global Read-only */
};


void get_mouseposition(int *x, int *y);

int main(int argc, char *argv[])
{	
    int ExitCode=0;
    int mx, my;
    
    printf("\nFreeDOS Edit Zen Remix (version %s)\n\n", ProgramVersion);

    /* Prepare initial configuration */
   	SelectColorScheme (color);			/* Color Scheme */
    
    /* -- All tests of all things required to run the DFLAT+ Application --*/
    if (!init_messages())
    {
    		puts ("DFLAT+: Unable to start the messaging system. Please contact the FreeDOS team");
        return (-1);
    }
  
    /* initialize mouse */
    get_mouseposition(&mx, &my);

    /* -- Here runs the main application --*/
    
    ExitCode = DfpMain (argc, argv);
    
    /* -- Finalization routines --*/
    
    return (ExitCode);

}

