#=========================================================================
# GLV test makefile
#=========================================================================

include ../Makefile.config
CFLAGS += $(addprefix -I../, $(INC_DIRS))
LFLAGS += -L../$(BIN_DIR) ../$(BIN_DIR)/$(SLIB_FILE)

SRCS =	test_examples.cpp \
	test_glv.cpp \
	test_icon.cpp \
	test_multi_window.cpp \
	test_notifications.cpp \
	test_units.cpp \
	test_widgets.cpp \
	test_window.cpp

TARGETS = $(addprefix ../$(BIN_DIR)/, $(SRCS:.cpp=))

#--------------------------------------------------------------------------
# Targets
#--------------------------------------------------------------------------
all: $(TARGETS)

../$(BIN_DIR)/%: %.cpp
	@echo CC $<
	@$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)
