#=========================================================================
# GLV main makefile
#=========================================================================

include ./Makefile.config

SRCS = 	glv_buttons.cpp \
	glv_color.cpp \
	glv_core.cpp \
	glv_draw.cpp \
	glv_font.cpp \
	glv_glv.cpp \
	glv_inputdevice.cpp \
	glv_layout.cpp \
 	glv_plots.cpp \
	glv_sliders.cpp \
	glv_texture.cpp \
	glv_textview.cpp \
	glv_util.cpp \
	glv_view.cpp \
	glv_view3D.cpp

ifdef WINDOW_BINDING
	SRCS += glv_binding.cpp $(BINDING_SRC)
endif

OBJS		= $(addsuffix .o, $(basename $(notdir $(SRCS))))
CFLAGS		+= $(addprefix -I, $(INC_DIRS))
LFLAGS		:= $(addprefix -L, $(LIB_DIRS)) $(LFLAGS)
DLIB_FILE 	:= $(addprefix $(BIN_DIR)/, $(DLIB_FILE))
SLIB_FILE 	:= $(addprefix $(BIN_DIR)/, $(SLIB_FILE))

#--------------------------------------------------------------------------
# Targets
#--------------------------------------------------------------------------
# Build object file from C++ source
$(OBJ_DIR)/%.o: %.cpp
	@echo CC $< $@
	@$(CC) -c $(CFLAGS) $< -o $@

# Build static library
$(SLIB_FILE): createFolders $(addprefix $(OBJ_DIR)/, $(OBJS))
	@echo AR $@
	@rm -f $@
	@$(AR) $@ $(filter %.o, $^)
	@$(RANLIB) $@

# Dummy target to force rebuilds
#FORCE:

.PHONY: test
test: $(SLIB_FILE)
	@make --directory $(TEST_DIR)

.PHONY: clean
clean:
	@rm -rf $(BIN_DIR)/*
#	@cd test && make clean

cleanall:
	@rm -rf $(BUILD_DIR)/*

all: $(SLIB_FILE) test


# Compile and run source files in test/ folder
test/%.cpp: $(SLIB_FILE)
	@$(CC) $(CFLAGS) -o $(BIN_DIR)/$(*F) $@ $(LFLAGS) $(SLIB_FILE)
	@./$(BIN_DIR)/$(*F) &

# Compile and run source files in example/ folder
example/%.cpp: $(SLIB_FILE)
	@$(CC) $(CFLAGS) -o $(BIN_DIR)/$(*F) $@ $(LFLAGS) $(SLIB_FILE)
	@./$(BIN_DIR)/$(*F) &

createFolders:
	@mkdir -p $(OBJ_DIR)
