OBJECTS = core/solid.o core/node.o core/vm.o core/object.o core/ast.o
CC = clang
CFLAGS = -I/usr/local/include -I. -Icore -Wall -g -fPIC
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
all : parser $(OBJECTS)
	$(CC) parser.o lexer.o $(OBJECTS) -ldl -lcuttle -g -Wall -Werror -o solid
parser: core/parser.y core/parser.l
	bison -d core/parser.y
	flex core/parser.l
	$(CC) -Wno-implicit-function-declaration -c lexer.c -o lexer.o -g -Icore
	$(CC) -Wno-implicit-function-declaration -c parser.c -o parser.o -g -Icore
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
	$(INSTALL_DATA) core/node.h $(DESTDIR)$(includedir)/solid
	$(INSTALL_DATA) core/vm.h $(DESTDIR)$(includedir)/solid
	$(INSTALL_DATA) core/ast.h $(DESTDIR)$(includedir)/solid
	$(INSTALL_DATA) core/object.h $(DESTDIR)$(includedir)/solid
static: parser $(OBJECTS)
	ar -cvq libsolid.a parser.o lexer.o $(OBJECTS)
shared: parser $(OBJECTS)
	$(CC) -shared -Wl,-soname,libsolid.so.1.0 -o libsolid.so.1.0 parser.o lexer.o $(OBJECTS)
lib: $(TARGET)
	$(CC) -shared -Wl,-soname,$(LIBNAME).so -lsolid -o $(LIBNAME).so $(TARGET)
get-deps:
	mkdir -p deps
	cd deps; git clone http://github.com/chameco/Cuttle; cd Cuttle; make && sudo make install
uninstall:
	rm $(DESTDIR)$(bindir)/solid
clean:
	rm parser.c parser.h lexer.c lexer.h
	rm $(OBJECTS)
	rm lib/*.o
