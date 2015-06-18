/*
 * Zhuonan Sun, CSE333, 7/24/2014
 * 1130849
 * szn1992@cs.washington.edu
 */

/*
 * Copyright 2011 Steven Gribble
 *
 *  This file is part of the UW CSE 333 course project sequence
 *  (333proj).
 *
 *  333proj is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  333proj is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with 333proj.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "libhw1/CSE333.h"
#include "memindex.h"
#include "filecrawler.h"

static void Usage(void);

int main(int argc, char **argv) {
  if (argc != 2)
    Usage();

// Implement searchshell!  We're giving you very few hints
// on how to do it, so you'll need to figure out an appropriate
// decomposition into functions as well as implementing the
// functions.  There are several major tasks you need to build:
//
//  - crawl from a directory provided by argv[1] to produce and index
//  - prompt the user for a query and read the query from stdin, in a loop
//  - split a query into words (check out strtok_r)
//  - process a query against the index and print out the results
//
// When searchshell detects end-of-file on stdin (cntrl-D from the
// keyboard), searchshell should free all dynamically allocated
// memory and any other allocated resources and then exit.

  DocTable doctable;
  MemIndex index;
  int qlen, res;
  char *query[128];
  char *input = (char*) malloc(128);
  char *token;
  char *check;
  char *saveptr;
  LinkedList retlist;
  LLIter llit;
  SearchResult *sres;
  char *name;


  // crawls the directory
  printf("Indexing '%s'\n", argv[1]);
  // gets the doctable and inverted index table
  res = CrawlFileTree(argv[1], &doctable, &index);
  if (res == 0) {  // encounters error
    Usage();
  }

  while (1) {
    printf("enter query:\n");
    // when the user types control-D
    if (fgets(input, 128, stdin) == NULL) {
      printf("shutting down...\n");
      break;
    }
    qlen = 0;

    // gets the first token
    token = strtok_r(input, " ", &saveptr);
    // stores the token in the array
    while (token != NULL) {
      query[qlen] = token;
      qlen++;
      token = strtok_r(NULL, " ", &saveptr);
    }
    // changes \n to \0
    check = strchr(query[qlen - 1], '\n');
    if (check != NULL) {
      *check = '\0';
    }

    // gets the result linkedlist
    retlist = MIProcessQuery(index, query, qlen);
    if (retlist != NULL && NumElementsInLinkedList(retlist) != 0) {
      // iterates through the list and print
      llit = LLMakeIterator(retlist, 0);
      Verify333(llit != NULL);
      do {
        // prints the result from the iterator
        LLIteratorGetPayload(llit, (void **)&sres);
        name = DTLookupDocID(doctable, sres->docid);
        printf("  %s  (%d)\n", name, sres->rank);
      } while (LLIteratorNext(llit));

      LLIteratorFree(llit);  // free the iterator
      FreeLinkedList(retlist, free);  // free the linked list
    }
  }

  free(input);
  FreeDocTable(doctable);
  FreeMemIndex(index);
  return EXIT_SUCCESS;
}

static void Usage(void) {
  fprintf(stderr, "Usage: ./searchshell <docroot>\n");
  fprintf(stderr,
  "where <docroot> is an absolute or relative " \
  "path to a directory to build an index under.\n");
  exit(-1);
}

#endif  // _XOPEN_SOURCE
