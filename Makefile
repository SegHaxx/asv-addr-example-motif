#
#	Makefile for Addr
#	Atari GEM/TOS -> Motif/UNIX demo
#

CFLAGS =-DDEBUG=1 -DSYSV
OBJ = 	addr.o addrdb.o
LIB = 	-lFm -lmalloc -lgls -lMrm -lXm -lXt -lX11\
	-lsocket -lsockhost -lnsl -lgen -lAtari

all:	addr addr.cat

addr: $(OBJ)
	cc -o addr $(OBJ) $(LIB)

addr.cat: addr.msg addrapp.msg
	rm -f addr.cat
	gencat addr.cat addr.msg addrapp.msg

install:
	cp addr.fm /usr/lib/X11/Atari/Fm
	cp addr.cat addr.cat.m /usr/lib/X11/Atari/gls

