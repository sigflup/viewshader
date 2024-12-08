#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdarg.h>

#include <SDL/SDL.h>
#include <GLES2/gl2.h>

#include "link.h"

#include "build.h"

GLint get_address(prog_t *in, GLchar *name) {
 symbol_t *walker;

 walker = in->symbols;
 if(walker == (symbol_t *)0) return 0;
 for(;;) {
  if(strcmp(walker->name, name)==0) { 
   return walker->address;
  }
  walker = (symbol_t *)walker->node.next;
  if(walker == in->symbols) break;
 }
 return 0;
}


void compile(prog_t *in,  GLenum type) {
 int infologlen = 0;
 int charswritten = 0;
 char *infolog;


 if(type == GL_FRAGMENT_SHADER) {
  in->frag = glCreateShader(type);
  glShaderSource(in->frag, 1, &in->frag_buf,&in->frag_len); 
  glCompileShader(in->frag);


  glGetShaderiv(in->frag, GL_INFO_LOG_LENGTH,
		           &infologlen);

 if(infologlen >0) {
  infolog = (char *)malloc(infologlen);
  glGetShaderInfoLog(in->frag, infologlen, &charswritten, infolog);
  printf("error!\n%s", infolog);
  exit(0);
 }


 } else {
  in->vect = glCreateShader(type);
  glShaderSource(in->vect,1, &in->vect_buf, &in->vect_len);
  glCompileShader(in->vect);

  glGetShaderiv(in->vect, GL_INFO_LOG_LENGTH,
		           &infologlen);

  if(infologlen >0) {
   infolog = (char *)malloc(infologlen);
   glGetShaderInfoLog(in->vect, infologlen, &charswritten, infolog);
   printf("error!\n%s", infolog);
   exit(0);
  }

 }
 
 
}

void add_symbol(prog_t *in, GLchar *name) {
 symbol_t *new;

 new = (symbol_t *)malloc(sizeof(symbol_t));
 new->name = strdup(name);
 new->address = glGetUniformLocation(in->prog, name);
 if(in->symbols == (symbol_t *)0) {
  in->symbols = new;
  INIT_LIST_HEAD(&in->symbols->node); 
 } else
  list_add(&new->node, &in->symbols->node);
}

prog_t *build(unsigned char *frag_name, unsigned char *vect_name,
	     int symbol_count, 	...) {
 prog_t *code;
 int frag_comp = 0;
 int vect_comp = 0; 
 FILE *fp;
 int i;
 struct stat qstat;
 va_list args;
 unsigned char *name;

 code = (prog_t *)malloc(sizeof(prog_t));
 
 code->prog = glCreateProgram();

 printf("prog_name = {\n");

 if(frag_name != (unsigned char *)0) {
  frag_comp = 1; 
  if((fp = fopen(frag_name, "rb"))<=0) {
   perror(frag_name);
   exit(0);
  }
 
  fstat(fileno(fp), &qstat);
  code->frag_buf = (unsigned char *)malloc(qstat.st_size);
  code->frag_len = qstat.st_size;

  fread(code->frag_buf,1, code->frag_len, fp);

  fclose(fp);

  printf(" compiling fragment %s %d bytes...", frag_name, code->frag_len);
  fflush(stdout);
  compile(code, GL_FRAGMENT_SHADER);
  printf("done\n");
  free(code->frag_buf);
 }
 if(vect_name != (unsigned char *)0) {
  vect_comp = 1;
  if((fp = fopen(vect_name, "rb"))<=0) {
   perror(vect_name);
   exit(0);
  }
 
  fstat(fileno(fp), &qstat);
  code->vect_buf = (unsigned char *)malloc(qstat.st_size);
  code->vect_len = qstat.st_size;

  fread(code->vect_buf,1, code->vect_len, fp);

  fclose(fp);

  printf(" compiling vertex %s %d bytes...", vect_name, code->vect_len);
  fflush(stdout);
  compile(code, GL_VERTEX_SHADER);
  printf("done\n");
  free(code->vect_buf);


 }

 if(vect_comp == 1)
  glAttachShader(code->prog, code->vect);
 if(frag_comp == 1)
  glAttachShader(code->prog, code->frag);
 printf(" linking...");
 fflush(stdout);
 glLinkProgram(code->prog);
 printf("done\n");

 glUseProgram(code->prog);

 printf(" building symbol table... ");
 fflush(stdout);

 code->symbols = (symbol_t *)0;
 if(symbol_count != 0) 
  va_start(args, symbol_count);
 for(i = 0;i<symbol_count; i++) {
  name = va_arg(args, GLchar *);
  add_symbol(code, name);
  printf("%s ", name);
  fflush(stdout);
 }
 if(symbol_count!=0) {
  putchar('\n');
  va_end(args);
 }
 puts("}");

 return code;
}	


