#
SHELL=/bin/sh
SHAR=/usr/bin/shar
MAKE=/usr/bin/make

#
# Will most likely have to edit the location of the
# X Libraries.
#
Libs_sun= -lpixrect
Libs_x= -L/usr/lib -lX11

WHICHconqrast: WHICHconqrast.o error.o WHICHstuff.o file.o util.o
	cc $(CFLAGS) -o WHICHconqrast WHICHconqrast.o WHICHstuff.o file.o\
			util.o error.o $(Libs_WHICH)

error.o: error.c
file.o: file.c
util.o:util.c

WHICHconqrast.o: conqrast.c WHICHconqrast.h file.h ../header.h
	$(CC) $(CFLAGS) -c conqrast.c -DHEADERFILE=\"WHICHconqrast.h\"
	mv conqrast.o WHICHconqrast.o

WHICHstuff.o: WHICHstuff.c WHICHconqrast.h

lint.WHICH:
	lint -abchx -DHEADERFILE=\"WHICHconqrast.h\" \
		conqrast.c WHICHstuff.c error.c file.c util.c $(Libs_WHICH)

### don't delete this line!

all: sunconqrast xconqrast

lint: lint.sun lint.x

lint.sun: makefile.sun 
	$(MAKE) $(MFLAGS) -f makefile.sun lint.sun

lint.x: makefile.x 
	$(MAKE) $(MFLAGS) -f makefile.x lint.x

sun sunconqrast: makefile.sun conqrast.c sunstuff.c sunconqrast.h file.c\
			file.h util.c
	$(MAKE) $(MFLAGS) -f makefile.sun sunconqrast

makefile.sun : Makefile
	sed -e '/^###/q' -e '/WHICH/s//sun/g' Makefile > makefile.sun

x xconqrast: makefile.x conqrast.c xstuff.c xconqrast.h file.c file.h util.c
	$(MAKE) $(MFLAGS) -f makefile.x xconqrast

makefile.x : Makefile
	sed -e '/^###/q' -e '/WHICH/s//x/g' Makefile > makefile.x

shar.core: conqrast.c 
	shar -c -v conqrast.c > shar.core

shar.sun: sunstuff.c sunconqrast.h
	shar -c -v sunstuff.c sunconqrast.h > shar.sun

shar.x: xstuff.c xconqrast.h
	shar -c -v xstuff.c xconqrast.h > shar.x

shar.general: Makefile README error.c file.c file.h util.c conqrast.man
	shar -c -v Makefile README conqrast.man error.c file.c file.h util.c > shar.general

shar: shar.general shar.sun shar.x shar.core

checkpoint:
	cp Makefile README *.[ch] *.man old
	/bin/rm *.diff newfiles difffiles

clean:
	/bin/rm -f *.o *.diff patch shar.* *~ "#*" makefile.* newfiles difffiles

diffs:
	echo -n >patch
	-for i in old/* ; do \
		j=`basename $$i` ;\
		if diff -c $$i $$j > $$j.diff ;\
			then \
			/bin/rm $$j.diff ;\
		else \
			cat  $$j.diff >> patch ;\
			wc $$j.diff ;\
		fi ;\
	done

newfiles:
	echo -n > newfiles
	-for i in *.[ch1] ; do\
		if test -f old/$$i ; then\
			: ;\
		else \
			echo $$i >> newfiles ;\
		fi ;\
	done

difffiles: newfiles diffs
	echo -n > difffiles
	-for i in *.diff ; do\
		j=`basename $$i .diff` ;\
		echo -n $$j ;\
		is=`wc $$i |colrm 9`;\
		js=`wc $$j |colrm 9`;\
		echo " " $$js $$is ;\
		if test $$is -gt $$js ; then \
			echo is very new! ;\
			echo $$j >> newfiles ;\
		else \
			echo is changed ;\
			echo $$i >> difffiles ;\
		fi ;\
	done

update: newfiles difffiles
	-n=1;for i in `cat difffiles` `cat newfiles` ; do \
		lines=`wc $$them $$i| tail -1 | colrm 9` ;\
		echo $$them $$i $$lines ;\
		if test $$lines -gt 800 ; then\
			echo chop! ;\
			$(SHAR) -c -v $$them > shar.$$n ;\
			n=`expr $$n + 1` ;\
			them="$$i" ;\
		else \
			them="$$them $$i" ;\
		fi ;\
	done ;\
	$(SHAR) -c -v $$them > shar.$$n
	/bin/rm newfiles difffiles

