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

/* $Id: list.c,v 1.11 1997/04/25 20:08:05 yavuzv Exp $ */

#include <assert.h>
#include <stdlib.h>
#include "list.h"

void*
list_add(s_list* list, void* ptr)
{
  s_node* new = 0;

  new = (s_node*)malloc(sizeof(s_node));
  assert(new != 0);

  if (list->root == 0) {
    list->root = new;
    list->top = new;		/* LINT: top == 0 */
    list->top->ptr = ptr;
    list->top->next = 0;
  } else {
    list->top->next = new;	/* list->top->next == 0 */
    list->top = new;
    list->top->ptr = ptr;
    list->top->next = 0;
  }
  
  return new;
}


/* FIXME: finish this one day */
/* void */
/* list_delete(s_list* list, char* string) */
/* { */
/*   if (list->root == 0) */
/*     return; */
/*   else if (list->root == list->top) { */
/*     free(list->root->ptr); */
/*     free(list->root->ptr2); */
/*     free(list->root->ptr2); */
/*     free(list->root); */
/*     list->root = list->top = 0; */
/*     return; */
/*   } else { */
/*     s_node* prev = list->root; */
/*     s_node* p = list->root;  */
/*     for (;;) { */
/*       if (strcmp((char*)p->ptr, string) == 0) { */
	
/*       } else { */
	
/*       } */
/*     } */
/*     return; */
/*   } */
/* } */

/* FIXME: lint gives 23 code errors, most of which are not justified */
