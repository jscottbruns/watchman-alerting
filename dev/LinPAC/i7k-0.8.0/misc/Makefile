SHELL = /bin/sh

LIBS = ../lib/libi7k.a

CFLAGS =  -I. -I../include
LDFLAGS =

.SUFFIXES: .c .o


PJ   =  met ms rate test

INCS =
OBJS =

all:    $(PJ)

met: met.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ met.c $(LIBS)

ms: ms.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ ms.c $(LIBS)

rate: rate.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ rate.c $(LIBS)

test: test.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ test.c $(LIBS)

indent:
	indent -kr -ts4 *.c

clean:
	rm -f *.o $(PJ) *~ *.tmp *.bak \#* .\#*
