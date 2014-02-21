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

/* $Id: database.c,v 1.16 1997/04/25 20:08:00 yavuzv Exp $ */

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "glosstex.h"
#include "database.h"
#include "error.h"
#include "list.h"
#include "labels.h"

/* states when reading lines */
enum e_state {HEADER, BODY};

static void process_file(FILE* infile);
static void process_line(char* line);
static int glo_parse_item(char* line, char* label, char* item, int* ptr);

void
read_databases(void)
{
  FILE* dbfile;
  s_node* filename = databases.root;

  while (filename != 0) {
    if ((dbfile = fopen(filename->ptr, "r")) == NULL) {
      error("database %s", (char*)filename->ptr);
      fprintf(logfile, "Couldn't open database %s\n", (char*)filename->ptr);
    }
    else {
      printf("Reading database %s\n", (char*)filename->ptr);
      fprintf(logfile, "Reading database %s\n", (char*)filename->ptr);
      process_file(dbfile);
    }
    filename = filename->next;
  }
}

static void 
process_file(FILE* dbfile)
{
  int status;
  char buf[LINESIZE];
  enum e_state state = HEADER;
  char* line = 0;
  
  while (fgets(buf, LINESIZE, dbfile) != 0) {
    status = strncmp(buf, "@entry{", 7);
    if ((state == HEADER) && (status != 0)) {
      /* ignore heading garbage */;
    } else {
      state = BODY;
      if (status == 0) {	/* begin new entry */
        
        /* process current entry before starting new entry */
        if (line != 0) {
          process_line(line);
          free(line);
        }

        line = (char*)malloc(strlen(buf) + 1);
        assert(line != 0);
	strcpy(line, buf);

        if (line[strlen(line) - 1] == '\n')
          line[strlen(line) - 1] = ' '; 
        
      } else {			/* add lines to current entry */
        size_t len = strlen(line); /* LINT: line is never NULL here */
        line = (char*)realloc(line, len + strlen(buf) + 1);
	assert(line != 0);
        strcpy(&line[len], buf);
        if (line[strlen(line) - 1] == '\n')
          line[strlen(line) - 1] = ' '; 
      }
    }
  }
  /* process last pending line in file */
  if (line != 0) {
    process_line(line);
    free(line);
  }
}

static void
process_line(char* line)
{
  char label[LINESIZE];
  char item[LINESIZE];
  size_t index;
  int ptr = 0;
  s_label* node;

  if (glo_parse_item(line, label, item, &ptr) != 0)
    fprintf(logfile,
	    "warning: couldn't parse item from line: %s, ignoring.\n", line);

  /* remove all trailing spaces */
  index = strlen(&line[ptr]);
  index--;

  while (line[ptr + index] == ' ') {
    line[ptr + index] = '\0';
    index--;
  }

  /* only process entry if a corresponding label is present
     and if it is not already resolved -> first definition wins */
  if (((node = find_label(labels, label)) != 0) && 
      (node->flag != RESOLVED)) {
    fprintf(outfile, 
	    "\\glosstexentry{%s@[%s]%s\\GlossTeXMode{%s}|GlossTeXPage}{%s}\n", 
	    label, item, &line[ptr], node->mode, node->page);
  } /* LINT: label, item are initialized through glo_parse_item()*/

  if (node != 0) {
    switch ((enum e_label_flag)node->flag) {
    case UNRESOLVED:
      node->flag = RESOLVED;
      break;
    case RESOLVED:
      fprintf(logfile,
	      "information: label: %s already defined, "
	      "ignoring definition: %s\n", label, &line[ptr]);
      /* LINT: label ist initialized through glo_parse_item() */
      break;
    }
  }
  /* LINT: no need to release storage referenced through node,
     since that storage is in a linked list */
}

/* #module    GloScan    "2-001"
***********************************************************************
*                                                                     *
* The software was developed at the Monsanto Company and is provided  *
* "as-is".  Monsanto Company and the auther disclaim all warranties   *
* on the software, including without limitation, all implied warran-  *
* ties of merchantabilitiy and fitness.                               *
*                                                                     *
* This software does not contain any technical data or information    *
* that is proprietary in nature.  It may be copied, modified, and     *
* distributed on a non-profit basis and with the inclusion of this    *
* notice.                                                             *
*                                                                     *
***********************************************************************
*/

/* this has been slightly modified by volkan yavuz 1996/12/18 */

static int 
glo_parse_item(char* line, char* label, char* item, int* ptr)
{
  int i, brace;
  char x;

  /* Copy the label to the output string. */
  i = 0;
  brace = 1;    
  *ptr = 7;
  while (1) {
    x = line[(*ptr)++];
    if (x == '\0') return 1;
    if (x == '{') if (line[*ptr - 2] != '\\') brace++;
    if (x == '}') {
      if (line[*ptr - 2] != '\\') brace--;
      if (brace <= 0) break;
    }
    if (x == ',') break;
    label[i++] = x;
  }
  label[i] = '\0';

  /* Find the beginning of the item string. */
  while (isspace(line[*ptr]) != 0) (*ptr)++;
  
  /* Copy the item to the output string. */
  i = 0;
  while (brace > 0) {
    x = line[(*ptr)++];
    if (x == '\0') return 1;
    if (x == '{') if (line[*ptr - 2] != '\\') brace++;
    if (x == '}') {
      if (line[*ptr - 2] != '\\') brace--;
      if (brace <= 0) break;
    }
    item[i++] = x;
  }
  item[i] = '\0';

  /* Check to see if the item is missing. If it is, default to the label. */
  if (i == 0) (void)strcpy(item, label); /* FIXME: lint code error */
  return 0;                     /* it's all ok */
}

