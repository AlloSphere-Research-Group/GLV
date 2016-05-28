#=========================================================================
# GLV main makefile
#=========================================================================

include Makefile.config

SRCS = 	glv_buttons.cpp \
	glv_color.cpp \
	glv_color_controls.cpp \
	glv_core.cpp \
	glv_draw.cpp \
	glv_font.cpp \
	glv_glv.cpp \
	glv_grid.cpp \
	glv_inputdevice.cpp \
	glv_layout.cpp \
	glv_model.cpp \
	glv_notification.cpp \
 	glv_plots.cpp \
	glv_preset_controls.cpp \
	glv_sliders.cpp \
	glv_sono.cpp \
	glv_texture.cpp \
	glv_textview.cpp \
	glv_view.cpp \
	glv_view3D.cpp \
	glv_widget.cpp

ifdef WINDOW_BINDING
	SRCS += glv_binding.cpp $(BINDING_SRC)
endif

OBJS		= $(addsuffix .o, $(basename $(notdir $(SRCS))))
CFLAGS		+= $(addprefix -I, $(INC_DIR))
LDFLAGS		:= $(addprefix -L, $(LIB_DIRS)) $(LDFLAGS)


#--------------------------------------------------------------------------
# Rules
#--------------------------------------------------------------------------
include Makefile.rules

.PHONY: clean test


# Install library into path specified by DESTDIR
# Include files are copied into DESTDIR/include/LIB_NAME and
# library files are copied to DESTDIR/lib
install: $(LIB_PATH)
#	@echo 'INSTALL $(DESTDIR)'
	@$(INSTALL) -d $(DESTDIR)/lib
	@$(INSTALL) -d $(DESTDIR)/include/$(LIB_NAME)
	@$(INSTALL) -m 644 $(LIB_PATH) $(DESTDIR)/lib
	@$(INSTALL) -m 644 $(INC_DIR)/*.h $(DESTDIR)/include/$(LIB_NAME)
#	@$(RANLIB) $(DESTDIR)/lib/$(LIB_FILE)


test: $(LIB_PATH) FORCE
#	@$(MAKE) -C $(TEST_DIR)
	@$(MAKE) --no-print-directory test/test_units.cpp

buildtest: test
	@$(MAKE) --no-print-directory examples/*.cpp AUTORUN=0


# Compile and run source files in examples/ or test/ folder
EXEC_TARGETS = examples/%.cpp test/%.cpp
.PRECIOUS: $(EXEC_TARGETS)
$(EXEC_TARGETS): $(LIB_PATH) FORCE
	$(CXX) $(CFLAGS) $(CXXFLAGS) -o $(BIN_DIR)$(*F) $@ $(LIB_PATH) $(LDFLAGS)
ifneq ($(AUTORUN), 0)
	@cd $(BIN_DIR) && ./$(*F) &
endif


# Remove build files
clean:
# Clean only removes object files for now; avoids unintentional removal of user files
	$(call RemoveDir, $(OBJ_DIR))
