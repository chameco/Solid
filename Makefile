OBJECTS = core/linenoise/linenoise.o core/solid.o core/node.o core/vm.o core/object.o core/ast.o core/common.o
CFLAGS = -std=c99 -I/usr/local/include -I. -Icore -Wall -g -fPIC
INSTALL = install
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = $(INSTALL) -m 644
prefix = /usr/local
exec_prefix = $(prefix)
bindir = $(prefix)/bin
datarootdir = $(prefix)/share
datadir = $(datarootdir)
libdir = $(exec_prefix)/lib
includedir = $(prefix)/include
all : deps parser.o lexer.o $(OBJECTS)
	$(CC) parser.o lexer.o $(OBJECTS) -lreadline -ldl -lcuttle -g -Wall -Werror -o solid
parser.o: core/parser.y
	bison -d core/parser.y
	$(CC) $(CFLAGS) -Wno-implicit-function-declaration -c parser.c -o parser.o -g
lexer.o: core/parser.l
	flex core/parser.l
	$(CC) $(CFLAGS) -Wno-unneeded-internal-declaration -Wno-implicit-function-declaration -c lexer.c -o lexer.o -g
tarball:
	mkdir solid-$(VERSION)
	cp -r Makefile parser core solid-$(VERSION)
	tar -czf solid-$(VERSION).tar.gz solid-$(VERSION)
install: all shared
	mkdir -p $(DESTDIR)$(bindir)
	$(INSTALL_PROGRAM) solid $(DESTDIR)$(bindir)/solid
	$(INSTALL_DATA) libsolid.so.1.0 $(DESTDIR)$(libdir)
	ln -sf $(DESTDIR)$(libdir)/libsolid.so.1.0 $(DESTDIR)$(libdir)/libsolid.so.1
	ln -sf $(DESTDIR)$(libdir)/libsolid.so.1.0 $(DESTDIR)$(libdir)/libsolid.so
	mkdir -p $(DESTDIR)$(includedir)/solid
	$(INSTALL_DATA) lexer.h $(DESTDIR)$(includedir)/solid
	$(INSTALL_DATA) parser.h $(DESTDIR)$(includedir)/solid
	$(INSTALL_DATA) core/solid.h $(DESTDIR)$(includedir)/solid
	$(INSTALL_DATA) core/common.h $(DESTDIR)$(includedir)/solid
	$(INSTALL_DATA) core/scanner_state.h $(DESTDIR)$(includedir)/solid
	$(INSTALL_DATA) core/node.h $(DESTDIR)$(includedir)/solid
	$(INSTALL_DATA) core/vm.h $(DESTDIR)$(includedir)/solid
	$(INSTALL_DATA) core/ast.h $(DESTDIR)$(includedir)/solid
	$(INSTALL_DATA) core/object.h $(DESTDIR)$(includedir)/solid
static: parser.o lexer.o $(OBJECTS)
	ar -cvq libsolid.a parser.o lexer.o $(OBJECTS)
shared: parser.o lexer.o $(OBJECTS)
	$(CC) -shared -Wl,-soname,libsolid.so.1.0 -o libsolid.so.1.0 parser.o lexer.o $(OBJECTS)
lib: lib/$(TARGET).o
	$(CC) -shared -Wl,-soname,$(TARGET).so -lsolid -o $(TARGET).so lib/$(TARGET).o
deps:
	mkdir -p deps
	cd deps; git clone http://github.com/chameco/Cuttle; cd Cuttle; make && sudo make install
uninstall:
	rm $(DESTDIR)$(bindir)/solid
clean:
	rm -f parser.c parser.h lexer.c lexer.h
	rm -f parser.o lexer.o $(OBJECTS)
	rm -f lib/*.o
