#
#	Makefile for Addr
#	Atari GEM/TOS -> Motif/UNIX demo
#

CFLAGS =-DDEBUG=1 -DSYSV
OBJ = 	addr.o addrdb.o
LIB = 	-lFm -lmalloc -lXm -lXt -lX11\
	-lsocket -lsockhost -lnsl -lgen -lAtari

all:	addr addr.cat

addr: $(OBJ)
	cc -o addr $(OBJ) $(LIB)

addr.cat: addr.msg addrapp.msg
	rm -f addr.cat
	gencat addr.cat addr.msg addrapp.msg
#
#	Note: If you do a real software product, you have
#	to package it (see System V.4 documentation). This
#	is just a quick and dirty way. Do NOT do it like this.
#
#	You probably have to be root for this action!
#
install:
	sh install.sh
