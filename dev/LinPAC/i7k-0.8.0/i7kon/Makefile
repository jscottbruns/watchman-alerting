# This program is free software; you can redistribute
# it and/or modify it at your convenience.

SHELL = /bin/sh

# If you use gcc, you should either run the
# fixincludes script that comes with it or else use
# gcc with the -traditional option.  Otherwise ioctl
# calls will be compiled incorrectly on some systems.
# CC = gcc -O
# YACC = bison -y
RM = rm

# define macros here

LIBS = ../lib/libi7k.a

#CDEBUG = -g
CDEBUG =
CFLAGS = -c $(CDEBUG) -I. -I../include
LDFLAGS = $(CDEBUG) -lform -lncurses

INSTALL = /usr/bin/install -c
INSTALLDATA = /usr/bin/install -c -m 644

# i7k library directory
i7k = ../i7k

##############################################
#### End of system configuration section. ####
##############################################
#.SUFFIXES: .c .o


PJ   =  i7kon

INCS =  i7kon.h init.h dev.h scope.h msg.h menu.h forms.h misc.h global.h
SRC1 =  main.c init.c forms.c misc.c global.c
SRC2 =  msg.c menu.c scope.c dev.c
SRC3 =
SRCS =  $(SRC1) $(SRC2) $(SRC3)
OBJ1 =  main.o init.o forms.o misc.o global.o
OBJ2 =  msg.o menu.o scope.o dev.o
OBJS =  $(OBJ1) $(OBJ2)
AUX  =

all:    $(PJ)

$(PJ):  $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

main.o: main.c $(INCS)
	$(CC) $(CFLAGS) main.c

init.o: init.c $(INCS)
	$(CC) $(CFLAGS) init.c

forms.o: forms.c $(INCS)
	$(CC) $(CFLAGS) forms.c

misc.o: misc.c $(INCS)
	$(CC) $(CFLAGS) misc.c

global.o: global.c $(INCS)
	$(CC) $(CFLAGS) global.c

dev.o:  dev.c $(INCS)
	$(CC) $(CFLAGS) dev.c

scope.o: scope.c $(INCS)
	$(CC) $(CFLAGS) scope.c

menu.o:  menu.c $(INCS)
	$(CC) $(CFLAGS) menu.c

msg.o:  msg.c $(INCS)
	$(CC) $(CFLAGS) msg.c

#
install: all
	su root -c "$(INSTALL) $(PJ) $(bindir)/$(binprefix)$(PJ)"

#
uninstall: all
	su root -c "$(RM) $(bindir)/$(binprefix)$(PJ)"

#	
TAGS:   $(SRCS)
	etags $(SRCS)

indent:
	indent -kr -ts4 *.c *.h

#
clean:
	rm -f *.o $(PJ) *~ *.tmp *.bak \#* .\#*
