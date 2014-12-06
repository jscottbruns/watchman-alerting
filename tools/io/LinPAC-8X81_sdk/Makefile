PJ = 'i8k'
VERSION  = '0.1.a'
HTML_TITLE  = 'ICPDAS Libi8k for LinCon8K Source Code Tour v$(VERSION)'

# add $(LinPAC) value for iX86 LinPAC LX800
#PROCESSOR = $(shell uname -p)
#CPU_TYPE = i$(shell echo $(PROCESSOR) | sed -ne 's/^i[3456].*/X/p')86
#ifeq ($(CPU_TYPE), iX86)
#	LinPAC = LX800
#endif

all:
ifneq ($(LinPAC), LX800)
	cd x305b && make
endif
	cd examples && make

indent:
ifneq ($(LinPAC), LX800)
	cd x305b && make indent
endif
	cd include && make indent
	cd examples && make indent
package:
	rm -f ../$(PJ)-$(VERSION)*
	make clean
	cd .. && ln -s $(PJ) $(PJ)-$(VERSION) && tar --exclude-from EXCLUDE -cf - $(PJ)-$(VERSION)/. | gzip -c > $(PJ)-$(VERSION).tar.gz
#	mv ../$(PJ)-$(VERSION).tar.gz .

#
clean:
	rm -f *.o *~ *.tmp \#* .\#*
ifneq ($(LinPAC), LX800)
	cd x305b && make clean
endif
	cd examples && make clean
	cd include && make clean

#
html:
	make clean.html
	gtags
	htags -fnvat $(HTML_TITLE)
	lynx HTML/index.html

clean.html:
	rm -f -r GPATH GRTAGS GSYMS GTAGS HTML






