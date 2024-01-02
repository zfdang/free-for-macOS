#
# free Makefile
# by:  Zhengfa Dang
# See LICENSE for license and copyright information.
#

PKG         = free-for-macos
VER         = 1.2.0

CWD         = $(shell pwd)
PREFIX      ?= /usr/local

CC          = gcc
CFLAGS      = -O2 -Wall -std=c99 -D_FREE_VERSION=\"$(VER)\"
CDEBUGFLAGS = -g

SRCS        = free.c
OBJS        = free.o

free: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o free

.c.o:
	$(CC) $(CFLAGS) -o $@ $< -c

install: free
	mkdir -p $(PREFIX)/bin
	mkdir -p $(PREFIX)/share/man/man1
	install -p -s -m 0755 free $(PREFIX)/bin
	install -p -m 0644 free.1 $(PREFIX)/share/man/man1

uninstall:
	rm -rf $(PREFIX)/bin/free
	rm -rf $(PREFIX)/share/man/man1/free.1

clean:
	-rm -rf $(OBJS) free core free-$(VER).tar.gz
	sudo rm -rf $(PKG) $(PKG)-$(VER).pkg $(PKG).dmg $(PKG)-$(VER)

tag:
	git tag -m "v$(VER)" v$(VER)

release: tag
	rm -rf free-$(VER)
	mkdir -p free-$(VER)
	cp -p Makefile README.md LICENSE free.1 free.c free.h free-$(VER)
	tar -cvf - free-$(VER) | gzip -9c > free-$(VER).tar.gz
	rm -rf free-$(VER)
