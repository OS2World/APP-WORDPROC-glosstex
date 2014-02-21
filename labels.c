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

/* $Id: labels.c,v 1.12 1997/04/25 20:08:03 yavuzv Exp $ */

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "glosstex.h"
#include "labels.h"
#include "list.h"

static int get_label(char buf[], char** label, char** mode, char** page);

void
read_labels(void)
{
  char buf[LINESIZE];

  printf("Reading glossary-file %s\n", inname);
  fprintf(logfile, "Reading glossary-file %s\n", inname);

  while (fgets(buf, LINESIZE, infile) != 0) {
    if ((strncmp(buf, "\\glosstexentry{", 15)) == 0) {

      s_label* label = (s_label*)malloc(sizeof(s_label));
      assert(label != 0);
      label->label = 0;
      label->mode = 0;
      label->page = 0;
      label->flag = UNRESOLVED;

      if (get_label(&buf[15], &label->label, 
		    &label->mode, &label->page) != 0) { /* LINT: 0 is ok */
	fprintf(logfile, "warning: parsing failed for line: %s\n", buf);
	count_label_parsing++;
	continue;
      }

      if (find_label(labels, label->label) == 0) { /* LINT: 0 is ok here */
	if (list_add(&labels, label) == 0) {
	  fprintf(logfile, 
		  "warning: couldn't add label: %s to list, ignoring.\n",
		  label->label); /* LINT: 0 is ok here */
	  count_label_add++;
	} else {
	  fprintf(logfile, "information: label: %s, page: %s\n", label->label, label->page); /* LINT: 0 is ok here */
	  count_label_success++;
	}
      } else {
	fprintf(logfile, 
		"information: label: %s already defined, ignoring line: %s\n", 
		label->label, buf); /* LINT: 0 is OK here */
	count_label_defined++;
      }
      /* LINT: label needn't be freed, it is held in a list */
    } else {
      count_label_ignored++;
    }
  }
}

s_label*
find_label(s_list list, char* string)
{
  s_node* p = list.root;
  while (p != 0) {
    if (strcmp(((s_label*)(p->ptr))->label, string) == 0)
      return ((s_label*)(p->ptr)); /* FIXME: lint */
    p = p->next;
  }
  return 0;			/* FIXME: lint */
}

unsigned int 
show_unresolved_labels(s_list list, FILE* file)
{
  unsigned int i = 0;
  s_node* p = list.root;
  while (p != 0) {
    if ((((s_label*)(p->ptr))->flag) == UNRESOLVED) {
      fprintf(file, "warning: label: %s unresolved\n", 
	      ((s_label*)(p->ptr))->label);
      ++i;
    }
    p = p->next;
  }
  return i;
}

static int
get_label(char buf[], char** label, char** mode, char** page)
{
  /* FIXME: this has to be improved! */
  size_t len;
  size_t len2;
  size_t len3;
  size_t len4;
  size_t len5;

  /* parse label */
  if ((len = strcspn(buf, "}")) == 0)
    return 1;

  *label = (char*)malloc(len + 1);
  assert(*label != 0);
  strncpy(*label, buf, len);
  (*label)[len] = '\0';

  /* parse mode */
  if ((len2 = strcspn(&buf[len], "{")) == 0)
      return 2;
  
  if ((len3 = strcspn(&buf[len + len2 + 1], "}")) == 0)
    return 3;

  *mode = (char*)malloc(len3 + 1);
  assert(*mode != 0);
  strncpy(*mode, &buf[len + len2 + 1 ], len3);
  (*mode)[len3] = '\0';

  /* parse page */
  if ((len4 = strcspn(&buf[len + len2 + 1 + len3], "{")) == 0)
      return 4;
  
  if ((len5 = strcspn(&buf[len + len2 + 1 + len3 + len4 + 1], "}")) == 0)
    return 5;

  *page = (char*)malloc(len5 + 1);
  assert(*page != 0);
  strncpy(*page, &buf[len + len2 + 1 + len3 + len4 + 1 ], len5);
  (*page)[len5] = '\0';

  return 0;
}

/* FIXME: lint gives 2 code errors */
