/* -*- c -*- */

/*
  GlossTeX, a tool for the automatic preparation of glossaries.
  Copyright (C) 1997 Volkan Yavuz
  
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

  Volkan Yavuz, yavuzv@rumms.uni-mannheim.de
  */

/* $Id: glosstex.h,v 1.7 1997/04/25 20:08:02 yavuzv Exp $ */

#ifndef __GLOSSTEX_H
#define __GLOSSTEX_H

#include <stdio.h>
#include "list.h"

#define LINESIZE 256

extern s_list labels;
extern s_list databases;

extern char* inname;
extern char* outname;
extern char* logname;

extern FILE* infile;
extern FILE* outfile;
extern FILE* logfile;

extern char* progname;

extern unsigned int count_label_ignored;
extern unsigned int count_label_parsing;
extern unsigned int count_label_add;
extern unsigned int count_label_defined;
extern unsigned int count_label_success;

#endif /* not __GLOSSTEX_H */
