#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <SDL2/SDL.h>
#include <GLES2/gl2.h>

#include "link.h"

#include "build.h"

const GLfloat verts[] = {
 -1,-1,0,
 -1,1,0,
  1,1,0,

 -1,-1,0,
  1,1,0,
  1,-1,0
};

int start;
GLuint res_sym, time_sym;


SDL_Window *window;
SDL_GLContext context;

void draw_shader(int width, int height,float time, prog_t *prog) {

 glUseProgram(prog->prog);

 
 glUniform1f(get_address(prog, "iTime"), time);

 glUniform2f(get_address(prog, "iResolution"), width, height);


 //glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 0, verts);
 //glEnableVertexAttribArray(0);
 glDrawArrays(GL_TRIANGLES, 0, 6);

}

void q_input(void) {
 SDL_Event event;
 while(SDL_PollEvent(&event)) {
  switch(event.type) {
   case SDL_KEYDOWN:
    exit(0);
    break;
  }
 }
}

void main_tick(prog_t *prog) {
 float time;
 SDL_GL_MakeCurrent(window, context);
 q_input();
 time = (float)(SDL_GetTicks() - start) / 1000.0f;
 draw_shader(1024, 768,time, prog);
 SDL_GL_SwapWindow(window);
}

int main(void) { 
 prog_t *main_prog;
 SDL_Init(SDL_INIT_VIDEO);

 window = SDL_CreateWindow("gl", 0,0,1024,768, SDL_WINDOW_OPENGL);
 context = SDL_GL_CreateContext(window);
// SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 0); 
 SDL_GL_MakeCurrent(window, context);

 main_prog = build("frag.glsl", (char *)0,2, "iResolution", "iTime");
 
 SDL_GL_SetSwapInterval(0);
 glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 0, verts);
 glEnableVertexAttribArray(0);
 start = SDL_GetTicks();
 for(;;) {
  SDL_Delay(10);
  main_tick(main_prog);
 }
 return 0;
}
