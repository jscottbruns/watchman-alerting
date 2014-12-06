PJ = 'i7k'
VERSION  = '0.8.0'
HTML_TITLE  = 'ICPDAS Libi7k for Linux Source Code Tour v$(VERSION)'

all:
	cd i7k && make
#	cd i7kon && make
	cd examples && make

indent:
	cd i7k && make indent
	cd i7kon && make indent
	cd include && make indent
	cd iw && make indent
	cd iwc/src && indent -orig *.c *.h
	cd misc && make indent
	cd examples && make indent
package:
	rm -f ../$(PJ)-$(VERSION)*
	make clean
	cd .. && ln -s $(PJ) $(PJ)-$(VERSION) && tar --exclude-from EXCLUDE -cf - $(PJ)-$(VERSION)/. | gzip -c > $(PJ)-$(VERSION).tar.gz
#	mv ../$(PJ)-$(VERSION).tar.gz .

#
clean:
	rm -f *.o *~ *.tmp \#* .\#*
	cd i7k && make clean
	cd i7kon && make clean
	cd iw && make clean
	cd iwc && make clean
	cd iwc && rm -f *.cache *.log *~ \#* .\#*
	cd iwc/src && rm -f *~ *.bak \#* .\#*
	cd misc && make clean
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






