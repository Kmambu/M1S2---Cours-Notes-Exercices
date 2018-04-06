
#include "stdio.h"

#define NPIXEL 128
#define NLINE  128

///////////////////////////////////////////////////////////////////////////////
//	main function
///////////////////////////////////////////////////////////////////////////////
__attribute__ ((constructor)) void main() 
{
	unsigned char 	BUF1[NPIXEL*NLINE];
	unsigned char 	BUF2[NPIXEL*NLINE];
	unsigned int 	line;
	unsigned int 	pixel;
	unsigned int 	step;

	/***** Prologue *****/
	tty_printf("\n*** damier 1 ***\n\n");
	for(pixel = 0 ; pixel < NPIXEL ; pixel++)
	{ 
		for(line = 0 ; line < NLINE ; line++) 
		{
			if( ( (pixel>>step & 0x1) && !(line>>step & 0x1)) || (!(pixel>>step & 0x1) &&  (line>>step & 0x1)) )
				BUF2[NPIXEL*line + pixel] = 0xFF;
			else
				BUF2[NPIXEL*line + pixel] = 0x0;
		}
	}
	tty_printf(" - build   OK at cycle %d\n", proctime());

	/***** Software pipelining *****/
	for(step = 2 ; step < 6 ; step++)
	{
		if(((step%2) == 0) && (step < 5)) {
			if(fb_write(0, BUF2, NLINE*NPIXEL) != 0)
			{
				tty_printf("\n!!! error in fb_syn_write syscall !!!\n"); 
				exit();
			}
			tty_printf(" - display OK at cycle %d\n", proctime());
			tty_printf("\n*** damier %d ***\n\n",step);
			for(pixel = 0 ; pixel < NPIXEL ; pixel++)
			{ 
				for(line = 0 ; line < NLINE ; line++) 
				{
					if( ( (pixel>>step & 0x1) && !(line>>step & 0x1)) || (!(pixel>>step & 0x1) &&  (line>>step & 0x1)) )
						BUF1[NPIXEL*line + pixel] = 0xFF;
					else
						BUF1[NPIXEL*line + pixel] = 0x0;
				}
			}
			tty_printf(" - build   OK at cycle %d\n", proctime());
		}
		else {
			if(fb_write(0, BUF1, NLINE*NPIXEL) != 0)
			{
				tty_printf("\n!!! error in fb_syn_write syscall !!!\n"); 
				exit();
			}
			tty_printf(" - display OK at cycle %d\n", proctime());
			tty_printf("\n*** damier %d ***\n\n",step);

			for(pixel = 0 ; pixel < NPIXEL ; pixel++)
			{ 
				for(line = 0 ; line < NLINE ; line++) 
				{
					if( ( (pixel>>step & 0x1) && !(line>>step & 0x1)) || (!(pixel>>step & 0x1) &&  (line>>step & 0x1)) )
						BUF2[NPIXEL*line + pixel] = 0xFF;
					else
						BUF2[NPIXEL*line + pixel] = 0x0;
				}
			}

			tty_printf(" - build   OK at cycle %d\n", proctime());
		}
		if(fb_completed() != 0)
		{
			tty_printf("\n!!! error in fb_completed syscall !!!\n"); 
			exit();
		}
	}
	/***** Epilogue *****/
	if(fb_write(0, BUF1, NLINE*NPIXEL) != 0)
	{
		tty_printf("\n!!! error in fb_syn_write syscall !!!\n"); 
		exit();
	}
	tty_printf(" - display OK at cycle %d\n", proctime());
	tty_printf("\nFin du programme au cycle = %d\n\n", proctime());
	exit(); 
} // end main

