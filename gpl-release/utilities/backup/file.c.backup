#include <stdio.h>
#include "file.h"

/*********************************************************************
*                                                                    *
* All the my* routines mimic stdio routines but manipulate struct    *
* file's The point is that we can read from and otherwise manipulate *
* more than one place in a file ( for more than one map ).           *
*                                                                    *
*********************************************************************/

/*********************************************************************
*                                                                    *
* struct file's play the roll of FILE *'s                            *
*                                                                    *
*********************************************************************/

#define MAX_FILES 10

static struct file 
    {
    FILE *fd;			/* which file */
    long offset,		/* where are we reading from now */
         top;			/* where is the notional start */
    } file_table[MAX_FILES];
    

void
myopen(which,file)

int which;
FILE *file;

{
file_table[which].fd=file;
file_table[which].offset=file_table[which].top=ftell(file);
}

myisopen(which)

int which;

{
return file_table[which].fd != NULL;
}

char *
mygets(buffer,num,which)

char *buffer;
int num;
int which;
{
    fseek(file_table[which].fd,file_table[which].offset,0);

    buffer=fgets(buffer,num,file_table[which].fd);

    file_table[which].offset=ftell(file_table[which].fd);

    return buffer;
    }

void
myrewind(which)

int which;

{
    fseek(file_table[which].fd,file_table[which].top,0);
    file_table[which].offset=file_table[which].top;
    }

void
myclose(which)

int which;

{
    int i;

    for(i=0;i<MAX_FILES;i++)
	if ( i != which && file_table[i].fd == file_table[which].fd)
	    {
	    file_table[which].fd=NULL;
	    return;
	    }

    fclose(file_table[which].fd);
    }

