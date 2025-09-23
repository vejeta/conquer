#include <stdio.h>
#include "xconqrast.h"

/* utilities for X */

Display *display;
Drawable root;
int screen;
GC gc;
short greyscale;

struct				/* maps from size of map to the font */
				/* to use */
    {				/* if size > mag use font */
    int mag;
    char *font,*bold;
    } fonts[] =			/* copied from the sun driver! need replacing */
        {
	9,  "/usr/lib/fonts/fixedwidthfonts/screen.r.7", NULL,
	11, "/usr/lib/fonts/fixedwidthfonts/cour.r.10",
	    "/usr/lib/fonts/fixedwidthfonts/cour.b.10",
	12, "/usr/lib/fonts/fixedwidthfonts/cour.r.12",
	    "/usr/lib/fonts/fixedwidthfonts/cour.b.12",
	14, "/usr/lib/fonts/fixedwidthfonts/cour.r.14",
	    "/usr/lib/fonts/fixedwidthfonts/cour.b.14",
	16, "/usr/lib/fonts/fixedwidthfonts/cour.r.16",
	    "/usr/lib/fonts/fixedwidthfonts/cour.b.16",
	18, "/usr/lib/fonts/fixedwidthfonts/cour.r.18",
	    "/usr/lib/fonts/fixedwidthfonts/cour.b.18",
	22, "/usr/lib/fonts/fixedwidthfonts/cour.r.24",
	    "/usr/lib/fonts/fixedwidthfonts/cour.b.24",
	999999, NULL
	};

int
argument_parse(argv)

char *argv[];

{
return 0;
}

void
initialise_bitmaps()




{
    }

screen_type
screen_named(name)

char *name;

{
int sc;

if ((display=XOpenDisplay(name)) == NULL)
    error("No X server!");

root=XDefaultRootWindow(display);
sc=XDefaultScreen(display);
gc=XDefaultGC(display,sc);

return display;
}

bitmap
get_screen_bitmap(screen)

screen_type screen;

{
int sc;
return root;
}

bitmap
create_bitmap(width,height)

int width,height;

{
int depth=1;

if ( depth != 1 )
	greyscale=1;

return XCreatePixmap(display,root,width,height,depth);
}

void
set_bit(bm,x,y,val)

bitmap bm;
int x,y;
int val;

{

if (!greyscale )
    {
    XSetForeground(display,gc,(unsigned long)val);
    XDrawPoint(display,bm,gc,x,y);
    }
else    
    if (val)
        XDrawPoint(display,bm,gc,x,y);
}

void
display_bitmap( bm,x,y,width,height)

bitmap bm;
int x,y,width,height;

{
XCopyArea(display,bm,root,gc,0,0,width,height,x,y);
    }

int 
write_bitmap(bm,file,width,height)

bitmap bm;
char *file;
int width,height;
{

return XWriteBitmapFile(display,file,bm,width,height,-1,-1);
    }


void
finish_bitmaps()

{
XCloseDisplay(display);
}

bitmap_text(bm,x,y,colour,pf,str)

bitmap bm;
int x,y;
int colour;
font pf;
char *str;

{
				/* ??? */
    }

