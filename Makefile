include config.mk

SRCDIR = ./src

SRC = \
	  ${SRCDIR}/glv_abstract_binding.cpp \
	  ${SRCDIR}/glv_binding_glut.cpp \
	  ${SRCDIR}/glv_color.cpp \
	  ${SRCDIR}/glv_core.cpp \
	  ${SRCDIR}/glv_draw.cpp \
	  ${SRCDIR}/glv_glv.cpp \
	  ${SRCDIR}/glv_inputdevice.cpp \
	  ${SRCDIR}/glv_layout.cpp \
	  ${SRCDIR}/glv_sliders.cpp \
	  ${SRCDIR}/glv_texture.cpp \
	  ${SRCDIR}/glv_textview.cpp \
	  ${SRCDIR}/glv_view.cpp \
	  ${SRCDIR}/FontGL/FontGL.cpp

OBJ = ${SRC:.cpp=.o}

.cpp.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} -o $*.o $<

libglv.a: ${OBJ}
	@echo AR $@
	@${AR} $@ ${OBJ}
	@${RANLIB} $@

.PHONY: tests

test: libglv.a
	@cd test && make all

clean:
	rm -f ${OBJ} libglv.a
	@cd test && make clean
#	cd tests && make clean

current:
	libglv.a

all: libglv.a test
