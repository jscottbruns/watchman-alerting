# This program is free software; you can redistribute
# it and/or modify it under the terms of the GNU
# General Public License ...
# ...
# ...

SHELL = /bin/sh

# If you use gcc, you should either run the
# fixincludes script that comes with it or else use
# gcc with the -traditional option.  Otherwise ioctl
# calls will be compiled incorrectly on some systems.
# CC = gcc -O
# YACC = bison -y


#CROSS_COMPILE  =
ifeq ($(LinPAC), ARM)
        CROSS_COMPILE = arm-linux-
        PJ = libi7k_arm
else
        PJ = libi7k
        CROSS_COMPILE =
endif

ifdef Device

ifeq ($(Device), I7565H1H2)
        MACRO = -DI7565H1H2
else
        MACRO = -DVCOM
endif

else
        MACRO = -DSTD
endif

AS              = $(CROSS_COMPILE)as
LD              = $(CROSS_COMPILE)ld
CC              = $(CROSS_COMPILE)gcc
CPP             = $(CC) -E
AR              = $(CROSS_COMPILE)ar
NM              = $(CROSS_COMPILE)nm
STRIP           = $(CROSS_COMPILE)strip
OBJCOPY         = $(CROSS_COMPILE)objcopy
OBJDUMP         = $(CROSS_COMPILE)objdump

#AR = ar
ARFLAGS = rv

CDEBUG = -g
CFLAGS = $(MACRO) $(CDEBUG) -I. -I../include
CFLAGS_1 = $(MACRO) $(CDEBUG) -I. -I../include -O2 -fPIC
LDFLAGS = -m elf_i386 -shared -soname 

INSTALL = install -c
INSTALLDATA = install -c -m 644

libdir = ../lib/

ifdef Device
SRCS = timer.c sio.c i7k.c msw.c i7000.c
else
SRCS = timer.c sio.c i7k.c msw.c i7000.c i8000.c i87000.c
endif

ifeq ($(LinPAC), ARM)
all:	$(libdir)/$(PJ).a
else
all:	$(libdir)/$(PJ).a $(libdir)/$(PJ).so.1.0
endif

$(libdir)/$(PJ).a: $(SRCS)
	@echo "HHHH MACRO = $(MACRO) SRCS = $(SRCS) HHHHH"
	$(CC) -c $(CFLAGS) $?
	$(AR) $(ARFLAGS) $@ *.o
	rm -f *.o

$(libdir)/$(PJ).so.1.0:$(SRCS)
	$(CC) -c $(CFLAGS_1) $?
	ld $(LDFLAGS) $(libdir)/$(PJ).so.1 -o $(libdir)/$(PJ).so.1.0 *.o
	rm -f *.o
	ln -sf $(libdir)/$(PJ).so.1.0 $(libdir)/$(PJ).so.1
	ln -sf $(libdir)/$(PJ).so.1 $(libdir)/$(PJ).so

install: all
	$(INSTALL) $(PJ) $(bindir)/$(binprefix)$(PJ)

TAGS:   $(SRCS)
	etags $(SRCS)

indent:
	indent -kr -ts4 *.c

clean:
	rm -f *.o *~ *.tmp *.bak \#* .\#* $(libdir)/$(PJ).a $(libdir)/$(PJ).so*
