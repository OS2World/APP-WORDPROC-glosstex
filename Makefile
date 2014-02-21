# -*- Makefile -*-
# $Id: Makefile,v 1.22 1997/05/04 12:23:42 yavuzv Exp $

#  GlossTeX, a tool for the automatic preparation of glossaries.
#  Copyright (C) 1997 Volkan Yavuz
  
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
  
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
  
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#  Volkan Yavuz, yavuzv@rumms.uni-mannheim.de

# ======================================================================
# you may need to set some of these
# ======================================================================

include Release

CFLAGS += -Wall -ansi -pedantic

ifdef DEBUG
 CFLAGS += -g
 LDFLAGS += -g
endif

ifeq ($(OSTYPE), OS2)
 CC = gcc
# Minor makefile adaptions by Stefan A. Deutscher (sad@utk.edu)
# On OS/2 under certain UNIX shell ports (like tcsh) seems to want
# sh.exe, so uncomment this:
 SHELL = sh
 EXE = .exe
 EMXBIND = emxbind
endif

LATEX = latex
MAKEINDEX = makeindex
GLOSSTEX = ./glosstex$(EXE)
MV = mv
RM = rm -f

# ======================================================================
# you shouldn't need to touch anything below
# ======================================================================

TEXAUX = *.aux *.lof *.lot *.log *.toc *.glo 
GLOSSTEXAUX = *.gxs *.gxg
MAKEINDEXAUX = *.glg *.glx

O=\
	database.o\
	error.o\
	labels.o\
	list.o\
	main.o\
	version.o\

C=$(O:%.o=%.c)

all: glosstex$(EXE)

glosstex: $(O)
	$(CC) $(LDFLAGS) $(O) $(LOADLIBS) -o $@

ifeq ($(OSTYPE), OS2)
glosstex$(EXE): glosstex
	$(EMXBIND) $<
	$(EMXBIND) -s $<
	$(RM) glosstex
endif

lint:
	lint $(C) 

doc: glosstex$(EXE) glosstex.dvi

glosstex.dvi: glosstex.tex glosstex.sty glosstex.gdf

%.dvi: %.tex
	$(LATEX) $<
	$(GLOSSTEX) $*.glo $*.gdf -o $*.gxs -t $*.gxg
	$(MAKEINDEX) $*.gxs -o $*.glx -t $*.glg -s glosstex.ist
	$(LATEX) $<
	$(GLOSSTEX) $*.glo $*.gdf -o $*.gxs -t $*.gxg
	$(MAKEINDEX) $*.gxs -o $*.glx -t $*.glg -s glosstex.ist
	$(LATEX) $<

# put the correct version-string (from this file) in some other files
version.c : Release
	$(MV) $@ $@.in
	sed < $@.in > $@ -e 's/version .*\\n/version $(RELEASE)\\n/'
	$(RM) $@.in

glosstex.sty : Release
	$(MV) $@ $@.in
	sed < $@.in > $@\
	-e 's/\\def\\fileversion{.*}/\\def\\fileversion{$(RELEASE)}/'\
	-e 's/\\def\\filedate{.*}/\\def\\filedate{'`date '+%Y\/%m\/%d'`'}/'
	$(RM) $@.in

glosstex.tex : Release
	$(MV) $@ $@.in
	sed < $@.in > $@\
	-e 's/\\def\\fileversion{.*}/\\def\\fileversion{$(RELEASE)}/'\
	-e 's/\\def\\filedate{.*}/\\def\\filedate{'`date '+%Y\/%m\/%d'`'}/'
	$(RM) $@.in


dist: clean
	cd ..;\
	cp -r glosstex glosstex-$(RELEASE);\
	tar cvzf glosstex-$(RELEASE).tar.gz glosstex-$(RELEASE);\
	rm -rf glosstex-$(RELEASE);\
	cd glosstex	
dep:
	$(CPP) -MM $(INCDIR) $(C) > .depend

clean:
	$(RM) $(O) $(TEXAUX) $(MAKEINDEXAUX) $(GLOSSTEXAUX) glosstex *~

proper: clean
	$(RM) .depend glosstex.dvi

ifeq (.depend, $(wildcard .depend))
include .depend
endif

# end-of-file
