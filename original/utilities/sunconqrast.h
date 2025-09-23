/*********************************************************************
*                                                                    *
* Header file for sun version of conqrast                            *
*                                                                    *
*********************************************************************/

#define driver_version "1"
#define conqrast_name "Sunconqrast"

#include <pixrect/pixrect_hs.h>

typedef struct pixrect * bitmap;
typedef char * screen_type;
typedef struct pixfont * font;

#define initialise_bitmaps() /* EMPTY */
#define finish_bitmaps() /* EMPTY */

#define is_screenname(mapname) (!strncmp(mapname,"/dev/",5))
#define get_default_screen_name() "/dev/fb"
#define screen_named(name) (name)
#define get_screen_bitmap(screen) (pr_open(screen))
#define create_bitmap(width,height) (mem_create(width,height,1))
#define destroy_bitmap(bitmap) pr_close(bitmap)
#define font_named(name) pf_open(name)
#define bad_font(font) ((font)==NULL)
#define font_height(font) ((font->pf_defaultsize).y)
#define font_width(font) ((font->pf_defaultsize).x)
#define font_baseline(font) (0-(font->pf_char)['A'].pc_home.y+1)

#define BLACK PIX_SET
#define WHITE PIX_CLR
#define DRAW_BLACK (PIX_DST|PIX_SRC)
#define DRAW_WHITE (PIX_DST&PIX_NOT(PIX_SRC))

#define set_bit(bitmap,x,y,val) pr_put(bitmap,x,y,val)
#define draw_line(bitmap,x1,y1,x2,y2,colour) pr_vector(bitmap,x1,y1,x2,y2,colour,0)
