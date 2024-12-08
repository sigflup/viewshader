CC=cc

CFLAGS=-DSDL_DISABLE_IMMINTRIN_H-I/usr/X11R6/include -I/usr/local/include -I/usr/local/include -I/usr/local/include/SDL2 -D_GNU_SOURCE=1 -D_REENTRANT -I/usr/X11R6/include -DXTHREADS -ggdb
LDFLAGS=-L/usr/X11R6/lib -L/usr/local/lib -L/usr/local/lib -lSDL2 -pthread -L/usr/X11R6/lib -lm -ggdb

OBJS= main.o build.o link.o

fragc: ${OBJS}
	${CC} -o gl ${OBJS} ${LDFLAGS} -lpthread -lGLESv2 -ljpeg -lX11

main.o: main.c
	${CC} -c ${CFLAGS} main.c


clean:
	rm -f gl gl.core ${OBJS}
