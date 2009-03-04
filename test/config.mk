# Customize to fit your system

# paths
PREFIX = /usr/local
CONFPREFIX = ${PREFIX}/etc
MANPREFIX = ${PREFIX}/share/man

# includes and libs
LIBS = -L${PREFIX}/lib -L/usr/lib

# Linux
CFLAGS = -O3 -I. -I../include/ -I${PREFIX}/include -I/usr/include -DLINUX -DGLV_BINDING_H=\"glv_pimpl_binding.h\"
LDFLAGS = ${LIBS} -lglut ../libglv.a

# OS X
#CFLAGS = -O3 -I. -I../include/ -I${PREFIX}/include -I/usr/include -DGLV_BINDING_H=\"glv_pimpl_binding.h\"
#LDFLAGS = ${LIBS} -framework GLUT -framework AGL -framework OpenGL ../libglv.a

AR = ar cr
CC = g++
RANLIB = ranlib
