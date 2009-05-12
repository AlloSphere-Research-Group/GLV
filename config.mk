# Uncomment your system
LINUX = 1
#MAC = 1

# Customize to fit your system
PREFIX = /usr/local
CONFPREFIX = ${PREFIX}/etc
MANPREFIX = ${PREFIX}/share/man
LIBS = -L${PREFIX}/lib -L/usr/lib
CFLAGS = -O3 -I. -I./include -I${PREFIX}/include -I/usr/include
LDFLAGS = ${LIBS}
AR = ar cr
CC = g++
RANLIB = ranlib

# OS dependent section
ifdef LINUX
	LDFLAGS += -lglut -lGLEW
	CFLAGS += -DLINUX
endif
ifdef MAC
	LDFLAGS += -framework GLUT -framework AGL -framework OpenGL
endif
