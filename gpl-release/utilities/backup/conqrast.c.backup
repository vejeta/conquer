#include <stdio.h>
#include <strings.h>
#include <string.h>
#include "../header.h"
#include "file.h"
#include HEADERFILE

/*********************************************************************
*                                                                    *
* This program draws a map of the conquer world on a bitmap system   *
*                                                                    *
* Written by Richard Caley July 1989 and hacked extensively          *
* thereafter.                                                        *
*                                                                    *
* You may copy, distribute, modify or do what you will with this     *
* code so long as this message remains in it and so long as you do   *
* not charge for it, nor distribute the program without the source.  *
*                                                                    *
* There is a horrid hack for getpass - to make this read the         *
* password from standard in one has to disconnect from the control   *
* terminal. GAG! It should be possible to disconnect just the        *
* conquer from the control terminal . . . someday.                   *
*                                                                    *
* See Readme for more details.                                       *
*                                                                    *
*********************************************************************/

#define GAP (2)			/* fudge factor - space above
				 * and below titles */

#define total_height (height*mag+ (do_title?4*GAP+2*font_height(titlefont):0))
				/* height of map and title bars */

#define yoffset(place) ((place)+(do_title?font_height(titlefont)+GAP+GAP:0))
				/* a y position */

#define tracef if(trace) printf

char *getpass();
void error(), ioerror();
char *passwd=NULL;
char *nation=NULL;
int turn;
int trace=0;
int bg=0;
char conquer_version[100];

#define conqrast_version "1.0"

#ifndef conqrast_name
#    define conqrast_name "Some conqrast" /* this is redefined in the header */
#    define driver_version "unknown driver"
#endif

#define HEADER_TAG "Conquer Version" /* just before first line of map */
#define HEADER_TAG_LENGTH (sizeof(HEADER_TAG)/sizeof(char)-1)

extern struct				/* maps from size of map to the font */
				/* to use */
    {				/* if size > mag use font */
    int mag;
    char *romanfont,*boldfont;
    } fonts[];

struct				/* Maps from terrain to brightness */
    {
    char c;			
    int val;			/* must be between 0 and 255 */
    } values[] =
        {
	' ', 256,
	'~', 0,
	'-', 64,
	'%', 128,
	'^', 192,
	'#', 224,
	'\0'
	    };

char *mapnames[] =
    {
    "This should never happen",
    "Altitude",
    "Vegetation",
    "Nation",
    "Designation"
    };

#define num_mapnames (sizeof(mapnames)/sizeof(char *))

/*********************************************************************
*                                                                    *
* Font mapping information. Fonts used by the system have symbolic   *
* names so they can be referred to on the commend line. font_table   *
* records which fonts go with which name.                            *
*                                                                    *
*********************************************************************/

enum mapfont
    {
    f_nation,
    f_designation,
    f_title,
    f_last
	};

struct 
    {
    char *name;
    font romanfont;
    font boldfont;
    } font_table[(unsigned)f_last+1] =
    {
    {"nation"},
    {"designation"},
    {"title"},
    {NULL}
    };

#define MAXDITHER 4		/* don't touch this */

short dit[1<<MAXDITHER][1<<MAXDITHER];

/*********************************************************************
*                                                                    *
* Sets the default fonts for the given size. If the font has already *
* been specified ( ie is non-null ) leaves it alone.                 *
*                                                                    *
*********************************************************************/

void
set_default_fonts(mag,f)

int mag;
enum mapfont f;

{
    int i;

    if (font_table[(unsigned)f].romanfont==NULL) /* if there was no font specified */
	{
	for(i=0;;i++)
	    {			/* find one */
	    if (fonts[i].mag >= mag )
		break;
	    else if (fonts[i].mag < mag )
		{
		if(bad_font(font_table[(unsigned)f].romanfont=
			    font_named(fonts[i].romanfont)))
		    error("Can't get font '%s' as %s font",
			  fonts[i].romanfont,font_table[(unsigned)f].name);
		if ( font_table[(unsigned)f].boldfont==NULL && fonts[i].boldfont !=NULL)
		    if (bad_font(font_table[(unsigned)f].boldfont=
				 font_named(fonts[i].boldfont)))
			error("Can't get font '%s' as bold %s font",
			      fonts[i].romanfont,font_table[(unsigned)f].name);
		}
	    }
	if(font_table[(unsigned)f].romanfont==NULL)
	    error("No font small enough for size %d %s",mag,font_table[(unsigned)f].name);
	}
    
    }

/*********************************************************************
*                                                                    *
* Set up a single font. if the name is in the table ( or is a prefix *
* ) then insert the name into the font table. If the name begins     *
* with 'b' then sets the bold font.                                  *
*                                                                    *
*********************************************************************/

void
set_a_font(name,fontname)

char *name;
char *fontname;

{
    int i,bold=0,l;
    font thefont;

    if( name[0]=='b')
	{
	name++;
	bold=1;
	}

    l=strlen(name);

    for (i=0;i< (int)f_last;i++)
	if (!strncmp(name,font_table[i].name,l))
	    break;

    if ( i == (int)f_last)
	error("Unknown font name '%s'",name);

    thefont=font_named(fontname);

    if ( bad_font(thefont) )
	error("Can't get font '%s'",fontname);

    if (bold)
	font_table[i].boldfont=thefont;
    else
	font_table[i].romanfont=thefont;
    }

/*********************************************************************
*                                                                    *
* scan over the header of the map. Returns the number of the map, or *
* 0 if the map is bad. If `pipe' is != 0 then the input is a pipe    *
* and an eof implies a bad password.                                 *
*                                                                    *
* Tries to cope with DEBUG and none-DEBUG conquers                   *
*                                                                    *
*********************************************************************/

scan_header(f,pipe)

FILE *f;
int pipe;

{
    char line[1024];
    char which[100];
    static char who[100];
    int mapid;

    while ( fgets(line,1024,f) != NULL)	/* scan to top of map */
	{
	if (!strncmp(line,HEADER_TAG,HEADER_TAG_LENGTH))
	    break;
	}

    if (feof(f))
	{
	if(pipe)
	    error("Password incorrect");
	else
	    error("Unexpected end of map file");
	}

    if (sscanf(line,"Conquer Version %[0-9.] : %s Map %*s %*s %s on Turn %d",conquer_version,which,who,&turn)!=4)
	error("Bad title line (wrong version?) '%s'",line);

    for(mapid=1;mapid < num_mapnames;mapid++)
	if(!strcmp(mapnames[mapid],which))
	    break;

    if ( mapid==num_mapnames)
	mapid= 0;
    else if ( trace && !pipe)
	printf("Found %s map\n", which);

    if(!strcmp(who,"World"))
	nation="god";
    else
	nation=who;

    tracef("Nation is %s\n",nation);

    return mapid;
    }

/*********************************************************************
*                                                                    *
* Find maps in a file and store away a file pointer and offset for   *
* each                                                               *
*                                                                    *
*********************************************************************/

void
process_map_file(name)

char *name;

{
    int which;
    FILE *f;
    char line[1024];

    tracef("Searching file %s\n",name);

    if ((f=fopen(name,"r"))==NULL)
	ioerror("Can't open map file '%s'",name);

    while(1)
	{
	long top;

	which=scan_header(f,0);
    
	if(which==0)
	    error("Bad map file '%s'",name);

	myopen(which,f);
	top=ftell(f);
	while(fgets(line,1024,f)!=NULL)
	    {
	    if (!strncmp(line,HEADER_TAG,HEADER_TAG_LENGTH))
		break;
	    top=ftell(f);
	    }

	if(feof(f))
	    break;

	fseek(f,top,0);
	}
    }

/*********************************************************************
*                                                                    *
* Get a map. If the map was not in one of the files on the command   *
* line it runs conquer -p to get it.                                 *
*                                                                    *
*     Which selects the map.                                         *
*     Args are passed to conqrun.                                    *
*     Tmpname is the name of a temporary file to use. This is        *
*         deleted in this routine so you can reuse the name.         *
*                                                                    *
*     All except `which' are ignored if the user has given a map     *
*     file of the correct type as an argumant.                       *
*                                                                    *
*********************************************************************/

int
get_map_file(which,args,tmpname)

int which;
char *args;
char *tmpname;

{
    char command[100];
    FILE *tmp;

    if ( ! myisopen(which))
	{
	if (nation==NULL)
	    {
	    tracef("Assuming god\n");

	    nation="god";
	    }

	tracef("Asking conquer for %s map\n",mapnames[which]);

	if (passwd==NULL)
	    {
	    passwd=getpass("Enter conquer password: ");
	    }

	tracef("Disconnecting ( HACK! ) bye bye\n");

	disconnect();		/* disconnect from controling tty */
	                        /* ( hack for getpass ) */

	if (bg)			/* go into background if asked */
	    {
	    tracef("Going into background\n");

	    background();
	    }

	sprintf(command,"conquer -p %s > %s 2>/dev/null",args,tmpname);
	
	if((tmp=popen(command,"w"))==NULL)
	    ioerror("can't run conquer");
	
	if (fprintf(tmp,"%s\n%s\n%c\n",nation,passwd, which+'0')==EOF)
	    error("Couldn't get map. Conquer may be updating.\n");
	
	pclose(tmp);
	
	if((tmp=fopen(tmpname,"r"))==NULL)
	    ioerror("can't open %s",tmpname);
	
	unlink(tmpname);		/* delete it */

	(void)scan_header(tmp,1);
	myopen(which,tmp);
	}

    return which;
    }

/*ARGSUSED*/
main(argc,argv)

int argc;
char *argv[];

{
    char line[1024];
    char desline[1024];
    char tmpname[20];
    char args[100];
    char *screen_name=get_default_screen_name();
    screen_type screen=NULL;
    int  mapfile;
    int destmapfile;
    int mag=11;
    bitmap pr;
    int width,height;
    extern char *myname;
    int x,y,i,j,val;
    char c;
    char *mapname=NULL;
    char *world=NULL;
    int do_nations=0,barbarians=0,markbarbs=0;
    int all=0,live=0,altitude=1;
    int desig=0;
    int do_title=1;
    font titlefont;
    int boldtitle=0;		/* indicates that the title font should be
				   emboldened - ie `titlefont' is roman and
				   so _we_ must do the bold */


    myname=argv[0];

    strcpy(args,"");

    while(*(++argv)!=NULL)
	if (!strncmp(*argv,"-o",2))
	    {
	    mapname= *(++argv);
	    if ( is_screenname(mapname))
		{
		screen_name=mapname;
		mapname=NULL;
		}
	    }
	else if (!strcmp(*argv,"-l"))
	    live++;
	else if (!strcmp(*argv,"-D"))
	    sprintf(args,"%s -d %s",args,world= *(++argv));
	else if (!strcmp(*argv,"-N"))
	    nation= *(++argv);
	else if (!strcmp(*argv,"-bg"))
	    bg++;
	else if (!strcmp(*argv,"-a"))
	    altitude=0;
	else if (!strcmp(*argv,"-f"))
	    {
	    char *which= *(++argv), *fontname= *(++argv);
	    set_a_font(which,fontname);
	    }
	else if (!strncmp(*argv,"-b",2))
	    {
	    barbarians++;
	    if ( (*argv)[2]=='a')
		markbarbs++;
	    }
	else if (!strcmp(*argv,"-m"))
	    mag=atoi(*(++argv));
	else if (!strncmp(*argv,"-c",2))
	    {
	    c=(*argv)[2];
	    for (i=0;;i++)
		{
		if (values[i].c =='\0')
		    error("Unknown contour '%c'",c);
		else if ( values[i].c == c)
		    {
		    values[i].val=atoi(*(++argv));
		    break;
		    }
		}
	    }
	else if (!strncmp(*argv,"-n",2))
	    {
	    do_nations++;
	    if ( (*argv)[2]=='a')
		++all;
	    }
	else if (!strncmp(*argv,"-d",2))
	    {
	    desig++;
	    }
	else if (!strcmp(*argv,"-h"))
	    help_message(myname);
	else if (!strcmp(*argv,"-t"))
	    do_title=0;
	else if (!strcmp(*argv,"-v"))
	    trace++;
	else if ( (*argv)[0]=='-')
	    {
	    printf("Unknown option '%s'\n",*argv);
	    help_message(myname);
	    }
        else
	    process_map_file(*argv);

    if (do_nations)		/* if we have to put in nations */
	set_default_fonts(mag,f_nation);

    if (desig)
	{
	if (all)
	    error("Only one of -d and -na allowed!");

	set_default_fonts(mag,f_designation);
	}

    if (do_title)
	{
	set_default_fonts(mag+2,f_title);
	if ( (titlefont=font_table[(unsigned)f_title].boldfont)==NULL)
	    {
	    boldtitle=1;
	    titlefont=font_table[(unsigned)f_title].romanfont;
	    }

	}

    screen=screen_named(screen_name);

    sprintf(tmpname,"/tmp/conq%d",getpid());

    mapfile=get_map_file(altitude?1:3, /* get a map */
			 args,tmpname);

    if(mygets(line,1024,mapfile)==NULL)
	error("empty map!");

    width=strlen(line)-1;	/* count height and width */

    for(height=1;mygets(line,1024,mapfile)!=NULL;height++)
	if(!strncmp(line,HEADER_TAG,HEADER_TAG_LENGTH)||
	   !strncmp(line,"reading",7))
	    break;

    tracef("Size %d X %d, image %d X %d\n",width,height,width*mag,height*mag);

    initialise_bitmaps();

    if(live)			/* if live do it on the screen */
	{
	if ((pr=get_screen_bitmap(screen))==(bitmap)NULL)
	    ioerror("Can't open screen %s",screen_name);
	}
    else			/* otherwise in memory */
	pr=create_bitmap(width*mag,total_height);

    myrewind(mapfile);

    if (altitude)		/* if needed do altitude */
	{
	int xx,yy;
	tracef("Drawing topography\n");

	build_dither_matrix(MAXDITHER);

	yy=yoffset(0);
	for(y=0;y<height;y++,yy+=mag)
	    {
	    if (mygets(line,1024,mapfile)==NULL)
		error("unexpected end of file!");
	
	    for(xx=x=0;x<width;x++,xx+=mag)
		{
		val= -1;
		for(i=0;values[i].c!='\0';i++)
		    if ( values[i].c==line[x] )
			val=values[i].val;
	    
		if ( val<0 )
		    {		/* non fatal, I like it that way */
		    printf("unknown character %c\n",line[x]);
		    continue;
		    }
	    
		for(i=0;i<mag;i++)
		    for(j=0;j<mag;j++)
			{
			set_bit(pr,xx+i,yy+j,dit[(xx+i)%16][(yy+j)%16] >=val?1:0);
			}
		}
	    }
    
	myclose(mapfile);
	}

    if(do_nations||desig)	/* nations */
	{
	font nf, bnf;
	font df;
	int xx,yy;
	char des,
	current='\0';		/* keeps track of which nation we are in */
	char last[1024];	/* ditto vertically for each column */

	tracef("Drawing nations\n");

	if (do_nations)
	    {
	    nf = font_table[(unsigned)f_nation].romanfont;
	    bnf = font_table[(unsigned)f_nation].boldfont;
	    }

	if (desig)
	    {
	    df = font_table[(unsigned)f_designation].romanfont;
	    }

	if (altitude)		/* if no altitude this was gotten to */
	    /* count the size */
	    mapfile=get_map_file(3,args,tmpname);

	destmapfile=get_map_file(4,args,tmpname);

	yy=yoffset(mag);
	for(y=0;y<height;y++,yy+=mag)
	    {
	    if(mygets(line,1024,mapfile)==NULL)
		error("unexpected end of file in nation map");
	    if(mygets(desline,1024,destmapfile)==NULL)
		error("unexpected end of file in designation map");

	    current='\0';	       
	    for(x=0;x<width;x++)
		{
		c=line[x];
		des=desline[x];

		if (index("~-%#^",c)!=NULL) /* if no-one owns */
		    c=' ';
		else if ( !barbarians && c == '*' ) /* if barbarian or lizard */
		    c=' ';

		xx=x*mag;
	    
		if (do_nations && c !=current )	/* crossing a vertical border */
		    {
		    draw_line(pr,xx-1,yy,xx-1,yy-mag,BLACK);
		    draw_line(pr,xx,yy,xx,yy-mag,WHITE);
		    draw_line(pr,xx+1,yy,xx+1,yy-mag,BLACK);
		    current=c;
		    }

		if ( do_nations & last[x] != c)	/* horizontal border above */
		    {
		    draw_line(pr,xx,yy-mag,xx+mag,yy-mag,WHITE);
		    draw_line(pr,xx,yy-mag+1,xx+mag,yy-mag+1,BLACK);
		    draw_line(pr,xx,yy-mag-1,xx+mag,yy-mag-1,BLACK);
		    last[x]=c;
		    }
		
		/*
		 * The following is convoluted to get the right things bold
		 * I think it copes with all cases 
		 */

		if ( do_nations && des == 'C' )
		    {
		    if (all||desig)
			{
			if (bnf)
			    put_txt(pr,xx,yy,bnf,c,0);
			else
			    put_txt(pr,xx,yy,nf,c,1);
			}
		    else
			put_txt(pr,xx,yy,nf,c,0);
		    }
		else if (do_nations && all && c != ' ' && ( c != '*' || markbarbs ) )
		    put_txt(pr,xx,yy,nf,c,0);
		else if (desig && des != '-')
		    put_txt(pr,xx,yy,df,des,0);
		}
	    }
	    
	    
	myclose(mapfile);
	myclose(destmapfile);
	}

				/* line down right */
    draw_line(pr,mag*width-1,yoffset(0),mag*width-1,yoffset(mag*height),BLACK);
    draw_line(pr,mag*width-2,yoffset(0),mag*width-2,yoffset(mag*height),WHITE);
    draw_line(pr,mag*width-3,yoffset(0),mag*width-3,yoffset(mag*height),BLACK);
				/* left */
    draw_line(pr,0,yoffset(0),0,yoffset(mag*height),BLACK);
    draw_line(pr,1,yoffset(0),1,yoffset(mag*height),WHITE);
    draw_line(pr,2,yoffset(0),2,yoffset(mag*height),BLACK);
				/* top */
    draw_line(pr,0,yoffset(0),mag*width,yoffset(0),BLACK);
    draw_line(pr,0,yoffset(1),mag*width,yoffset(1),WHITE);
    draw_line(pr,0,yoffset(2),mag*width,yoffset(2),BLACK);
				/* bottom */
    draw_line(pr,0,yoffset(mag*height-1),mag*width,yoffset(mag*height-1),BLACK);
    draw_line(pr,0,yoffset(mag*height-2),mag*width,yoffset(mag*height-2),WHITE);
    draw_line(pr,0,yoffset(mag*height-3),mag*width,yoffset(mag*height-3),BLACK);

    if (do_title)
	{
	sprintf(line,"Conquer Version %s",conquer_version);
	put_txt(pr,mag,
		font_baseline(titlefont)+GAP+1,titlefont,'\0',boldtitle,line);

	sprintf(line,"%s Version %s/%s",conqrast_name,conqrast_version,driver_version);
	put_txt(pr,width*mag-strlen(line)*font_width(titlefont)-GAP-GAP,
		font_baseline(titlefont)+GAP+1,titlefont,'\0',boldtitle,line);

	if ( nation==NULL || !strcmp(nation,"god"))
	    sprintf(line,"Conquer World Map For Turn %d",turn);
	else
	    sprintf(line,"Conquer Map For Nation %s On Turn %d",nation,turn);

	put_txt(pr,(width*mag-strlen(line)*font_width(titlefont))/2,
		height*mag+GAP+font_height(titlefont)+GAP+
		GAP+font_baseline(titlefont),titlefont,'\0',boldtitle,line);
	}

    if (mapname !=NULL)		/* told to save in file */
	{
	write_bitmap(pr,mapname,width*mag,total_height);
	destroy_bitmap(pr);
	}
    else if (!live)		/* not drawn already */
	{
	display_bitmap(pr,0,0,width*mag,total_height);
	destroy_bitmap(pr);
	}
    else
	destroy_bitmap(pr);

    finish_bitmaps();

    tracef("done\n");

    }

/*********************************************************************
*                                                                    *
* put the character c at (xx, yy) in pr, using font pf. Highlight    *
* means bold ( not very good at the moment, anyone want to improve   *
* it? ).                                                             *
*                                                                    *
* This works by first blacking out an area around the character and  *
* then printing the character.                                       *
*                                                                    *
* Bolding is done by smearing ( yeuch ).                             *
*                                                                    *
* If c is the null character then str is a string to display in the  *
* same way.                                                          *
*                                                                    *
*********************************************************************/
/*VARARGS6*/
put_txt(pr,xx,yy,pf,c,highlight,str)

bitmap pr;
int xx,yy;
font pf;
char c;
int highlight;
char *str;

{
    int i,j,size;
    static char st[2];
    int x,y;

    if ( c !='\0' )
	{
	st[0]=c;
	st[1]='\0';
	str=st;
	}

    if (highlight)
	size=1;
    else
	size=0;

    xx+= 2;
    yy-= 2;

    for(i=0,x=xx-1; i<3+size; i++,x++)
	for(j=0,y=yy+1; j<3+size; j++,y--)
	    bitmap_text(pr,x,y,DRAW_BLACK,pf,str);


    for(i=0,x=xx;i<1+size;i++,x++)
	for(j=0,y=yy;j<1+size;j++,y--)
	    bitmap_text(pr,x,y,DRAW_WHITE,pf,str);

    }

