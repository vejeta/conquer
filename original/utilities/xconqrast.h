
/* header for X */

#define driver_version "preliminary"
#define conqrast_name "Xconqrast"

#include <X11/Xlib.h>

typedef Drawable bitmap ;
typedef Display *screen_type;
typedef char *font;		/* this needs doing */

#define is_screenname(name) (index(name,':')!=NULL) /* I think */
#define get_default_screen_name() NULL /* I have no idea */

screen_type screen_named();

bitmap get_screen_bitmap();
bitmap create_bitmap();

#define destroy_bitmap(bm)	/* I don't know how to */
#define font_named(name) (name)	/* ditto */
#define bad_font(font) ((font)==NULL) /* ??? */
#define font_height(font) (0)	/* ??? */
#define font_width(font) (0)	/* ??? */
#define font_baseline(font) (0) /* ??? */

#define BLACK (1)
#define WHITE (0)		/* guessing */
#define DRAW_BLACK (1)		/* ?? */
#define DRAW_WHITE (2)

void set_bit();

#define draw_line(bitmap,x1,y1,x2,y2,colour) /* again no idea! */


