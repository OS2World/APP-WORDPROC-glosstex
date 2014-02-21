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

/* $Id: main.c,v 1.15 1997/04/25 20:08:06 yavuzv Exp $ */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "database.h"
#include "error.h"
#include "labels.h"
#include "list.h"
#include "version.h"

s_list labels;			/* list of all labels found */
s_list databases;		/* list of all databases */

char* progname = 0;		/* name this prog was called with */

char* inname = 0;		/* name of input (.glo) file */
char* outname = 0;		/* name of output file */
char* logname = 0;		/* name of logfile */

FILE* infile;
FILE* outfile;
FILE* logfile;

unsigned int count_label_ignored = 0; /* line ignored */
unsigned int count_label_parsing = 0; /* parsing faild */
unsigned int count_label_add = 0; /* adding to list failed */
unsigned int count_label_defined = 0; /* label already defined */
unsigned int count_label_success = 0; /* success */
unsigned int count_label_unresolved = 0; /* unresolved label */

char usage[] = "Usage: %s glo gdf0 [gdf1 ...] [-o gxs] [-t gxg]\n";

static void parse_commandline(int argc, char** argv);
static void open_files(void);
static char* check_extension(const char* string, const char* ext);
static char* build_filename(const char* string, const char* ext);
static void print_statistics(FILE* file);

int
main(int argc, char* argv[])
{
  labels.root = labels.top = 0;
  databases.root = databases.top = 0;

  assert(argv[0] != 0);
  progname = (char*)malloc(strlen(argv[0]) + 1);

  assert(progname != 0);
  strcpy(progname, argv[0]);

  printf("%s\n", version);

  parse_commandline(argc, argv);

  open_files();

  read_labels();
  printf("Writing output-file %s\n", outname);
  fprintf(logfile, "Writing output-file %s\n", outname);
  
  read_databases();

  printf("Writing log-file %s\n", logname);
  fprintf(logfile, "Writing log-file %s\n", logname);

  if ((count_label_unresolved = show_unresolved_labels(labels, logfile)) != 0)
    printf("\nThere were %u unresolved labels. "
	   "See log-file %s for details.\n", count_label_unresolved, logname);
  
  print_statistics(logfile);
  print_statistics(stdout);

  return 0;
}

/*
  FUNCTION: parse_commandline(int argc, char** argv)
  PURPOSE: parses the commandline from argc and argv
  CALLED:
    main
  CALLS:
    check_extension
    list_add
  GLOBALS:
    char usage[] : contains information about calling conventions
    char* progname : the name this binary was called with
    char* inname : name of input-file
    char* outname : name of output-file
    char* logname : name of logfile
    s_list databases : list of all databases
  */

static void
parse_commandline(int argc, char** argv)
{
  int index;

  /* check argc */
  if (argc < 3) {
    fprintf(stderr, usage, progname);
    exit(EXIT_FAILURE);
  }

  /* parse command-line */
  inname = check_extension(argv[1], ".glo");

  for (index = 2; index < argc; index++) {
    if (strncmp(argv[index], "-o", 2) == 0)
      outname = check_extension(argv[++index], ".gxs");
    else if (strncmp(argv[index], "-t", 2) == 0)
      logname = check_extension(argv[++index], ".gxg");
    else 
      (void)list_add(&databases, check_extension(argv[index], ".gdf"));
  }
}

static void
open_files(void)
{
  if ((infile = fopen(inname, "r")) == NULL) {
    error("input-file %s", inname);
    exit(EXIT_FAILURE);	
  }

  if (outname == 0)
    outname = build_filename(inname, ".gxs");

  if ((outfile = fopen(outname, "w")) == NULL) {
    error("output-file %s", outname);
    exit(EXIT_FAILURE);
  }

  if (logname == 0)
    logname = build_filename(inname, ".gxg");

  if ((logfile = fopen(logname, "w")) == NULL) {
    error("log-file %s", logname);
    exit(EXIT_FAILURE);	 
  }

}

static char*
check_extension(const char* string, const char* ext)
{
  char* new = 0;

  if (strchr(string, '.') == 0) {
    new = (char*)malloc(strlen(string) + strlen(ext) + 1);
    assert(new != 0);
    strcpy(new, string);
    strcat(new, ext);
  } else {
    new = (char*)malloc(strlen(string) + 1);
    assert(new != 0);
    strcpy(new, string);
  }
  return new;
}

static char* 
build_filename(const char* string, const char* ext)
{
  char* filename = 0;
  size_t len;

  len = strcspn(string, ".");
  assert(len != 0);

  filename = (char*)malloc(len + strlen(ext) + 1);
  assert(filename != 0);

  strncpy(filename, string, len);
  filename[len] = '\0';		/* mark end of string */
  strcat(filename, ext);
  return filename;

}

static void 
print_statistics(FILE* file)
{
  fprintf(file, "\n");
  fprintf(file, "%s statistics:\n", inname);
  fprintf(file, "---------------------------------------\n");
  fprintf(file, "unresolved labels:     %9u\n", count_label_unresolved);
  fprintf(file, "\n");
  fprintf(file, "lines ignored:         %9u\n", count_label_ignored);
  fprintf(file, "parsing failed:        %9u\n", count_label_parsing);
  fprintf(file, "add to list failed:    %9u\n", count_label_add);
  fprintf(file, "label already defined: %9u\n", count_label_defined);
  fprintf(file, "success:               %9u\n", count_label_success);
  fprintf(file, "                      =================\n");
  fprintf(file, "total lines read:      %9u\n", 
	  count_label_ignored + count_label_parsing + count_label_add + 
	  count_label_defined + count_label_success);
  fprintf(file, "\n");
}

