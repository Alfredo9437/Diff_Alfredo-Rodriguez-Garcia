//
//  para.c
//  diff
//
//  Created by William McCarthy on 5/9/19.
//  Copyright © 2019 William McCarthy. All rights reserved.
//

#include <stdlib.h>
#include <string.h>
#include "para.h"
#include "util.h"

#define BUFLEN 256


para* para_make(char* base[], int filesize, int start, int stop) {
  para* p = (para*) malloc(sizeof(para));
  p->base = base;
  p->filesize = filesize;
  p->start = start;
  p->stop = stop;
  p->firstline = (p == NULL || start < 0) ? NULL : p->base[start];
  p->secondline = (p == NULL || start < 0 || filesize < 2) ? NULL : p->base[start + 1];
  
  return p;
}

para* para_first(char* base[], int size) {
  para* p = para_make(base, size, 0, -1);
  return para_next(p);
}

void para_destroy(para* p) { free(p); }

para* para_next(para* p) {
  if (p == NULL || p->stop == p->filesize) { return NULL; }
  
  int i;
  para* pnew = para_make(p->base, p->filesize, p->stop + 1, p->stop + 1);
  for (i = pnew->start; i < p->filesize && strcmp(p->base[i], "\n") != 0; ++i) { }
  pnew->stop = i;
  
  if (pnew->start >= p->filesize) {
    free(pnew);
    pnew = NULL;
  }
  return pnew;
}
size_t para_filesize(para* p) { return p == NULL ? 0 : p->filesize; }

size_t para_size(para* p) { return p == NULL || p->stop < p->start ? 0 : p->stop - p->start + 1; }

char** para_base(para* p) { return p->base; }

char* para_info(para* p) {
  static char buf[BUFLEN];   // static for a reason
  snprintf(buf, sizeof(buf), "base: %p, filesize: %d, start: %d, stop: %d\n",
           p->base, p->filesize, p->start, p->stop);
  return buf;  // buf MUST be static
}

int para_equal(para* p, para* q) {
  if (p == NULL || q == NULL) { return 0; }
  if (para_size(p) != para_size(q)) { return 0; }
  if (p->start >= p->filesize || q->start >= q->filesize) { return 0; }
  int i = p->start, j = q->start, equal = 0;
  while ((equal = strcmp(p->base[i], q->base[i])) == 0) { ++i; ++j; }
  return 1;
}

void para_print(para* p, void (*fp)(const char*)) {
  if (p == NULL) { return; }
  for (int i = p->start; i <= p->stop && i != p->filesize; ++i) { fp(p->base[i]); }
}

void para_printfile(char* base[], int count, void (*fp)(const char*)) {
  para* p = para_first(base, count);
  while (p != NULL) {
    para_print(p, fp);
    p = para_next(p);
  }
  printline();
}

void norm(para * left, para * right){
  int equal = 0;
  char * change;
  int i = left->start, j = right->start;
  para * nr = para_next(right);
  para * nl = para_next(left);
  
  int a = nl->start, b = nr->start;
  if((equal = strcmp(left->base[i], right->base[j])) != 0){ //this will check to see if lines are different

    if(strcmp(nr->base[b], left->base[i]) == 0){ //these statements are checking to see if the next paragrapgh in the opposite file is the same
      change = "a"; 
      printf("%s%d,%d\n", change, right->start+1, right->stop+1);
    }

    if( (equal = strcmp(nl->base[a], right->base[j])) == 0){
      change = "d";       
      printf("%d,%d%s\n", left->start+1, left->stop+1, change);
      return;
    }
  }
}

void linebyline(para * left, para * right){
  int nequal = 0;
  int i, j, leftstop = left->stop, rightstop = right->stop;
  for(i=0, j=0; i<leftstop && j < rightstop; i++, j++){
    if( (nequal = strcmp(left->base[i], right->base[j])) != 0 ){
        printf("%dc%d\n",left->start+i, right->start+i);
        printf("%s\n", left->base[i]);
        printf("---\n");
        printf("%s", right->base[j]);
    }
  }
}
void normlast(para* right){
  if(right->start != right->stop){
      printf("%d,%da\n", right->start+1, right->stop+1);
  }
}