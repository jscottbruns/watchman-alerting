ifdef Device
ifeq ($(Device), I7565H1H2)
        MACRO = -DI7565H1H2
else
        MACRO = -DVCOM
endif

else
        MACRO =
endif

indent:
	indent -kr -ts4 *.h

clean:
	rm -f *.o *~ *.tmp *.bak \#* .\#*
