OBJECTS = core/solid.o core/node.o core/vm.o core/object.o core/ast.o core/utils.o
CFLAGS = -I. -Icore -Wall -g
INSTALL = install
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = $(INSTALL) -m 644
prefix = /usr/local
exec_prefix = $(prefix)
bindir = $(prefix)/bin
datarootdir = $(prefix)/share
datadir = $(datarootdir)
libdir = $(exec_prefix)/lib
all : parser $(OBJECTS)
	$(CC) parser.o lexer.o $(OBJECTS) -g -Wall -o solid
parser: core/parser.y core/parser.l
	bison -d core/parser.y
	flex core/parser.l
	$(CC) -c lexer.c -o lexer.o -g -Icore
	$(CC) -c parser.c -o parser.o -g -Icore
tarball:
	mkdir solid-$(VERSION)
	cp -r Makefile parser src include solid-$(VERSION)
	tar -czf solid-$(VERSION).tar.gz solid-$(VERSION)
install:
	mkdir -p $(DESTDIR)$(bindir)
	$(INSTALL_PROGRAM) solid $(DESTDIR)$(bindir)/solid
uninstall:
	rm $(DESTDIR)$(bindir)/solid
clean:
	rm parser.c parser.h lexer.c lexer.h
	rm $(OBJECTS)
