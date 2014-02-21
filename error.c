/* -*- c-*- */

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

/* $Id: error.c,v 1.8 1997/04/25 20:08:00 yavuzv Exp $ */

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "glosstex.h"

#define BUFSIZE 4096

void
error(const char* fmt, ...)
{
  char buf[4096];
  int errno_save = errno;

  va_list ap;
  va_start(ap, fmt);

  sprintf(buf, "%s: ", progname);
  (void)vsprintf(buf + strlen(buf), fmt, ap);
  sprintf(buf + strlen(buf), ": %s\n", strerror(errno_save));

  (void)fflush(stdout);
  (void)fputs(buf, stderr);
  (void)fflush(0);		/* LINT: 0 is ok here */

  va_end(ap);
}


