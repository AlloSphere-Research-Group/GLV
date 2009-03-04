include config.mk

SRC = \
	test_examples.cpp \
	test_glv.cpp \
	test_icon.cpp \
	test_multi_window.cpp \
	test_notifications.cpp \
	test_widgets.cpp \
	test_window.cpp

TARGETS = ${SRC:.cpp=}

all: ${TARGETS}

%: %.cpp ../libglv.a
	@echo CC $<
	@${CC} ${CFLAGS} -o $@ $< ${LDFLAGS}

clean:
	@rm -f *.o ${TARGETS}
