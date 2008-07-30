include config.mk

SRC = \
		test_examples.cpp \
		test_signals.cpp \
		test_glv.cpp \
		test_icon.cpp \
		test_widgets.cpp

TARGETS = ${SRC:.cpp=}

all: ${TARGETS}

%: %.cpp ../libglv.a
	@echo CC $<
	${CC} ${CFLAGS} -o $@ $< ${LDFLAGS}

clean:
	@rm -f *.o ${TARGETS}
