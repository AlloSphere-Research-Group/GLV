# Customize to fit your system

# paths
PREFIX = /usr/local
CONFPREFIX = ${PREFIX}/etc
MANPREFIX = ${PREFIX}/share/man

# includes and libs
LIBS = -L${PREFIX}/lib -L/usr/lib

# Linux/BSD
CFLAGS = -g -Wall -I. -I../include/ -I${PREFIX}/include -I/usr/include -I../src/FontGL -I/usr/include/freetype2 -DLINUX
LDFLAGS = ${LIBS} -lglut ../libglv.a

AR = ar cr
CC = g++
RANLIB = ranlib
