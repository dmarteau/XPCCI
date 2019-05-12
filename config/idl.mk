#=========================================================
# Make rules for building idl files
#
# This file is intended to be included in the main makefile
#=========================================================

.SUFFIXES: .idl


CPP_IDL_FILES = $(patsubst %.idl,$(EXPORT_DIR)/%.h,$(IDLSRC))
XPT_IDL_FILES = $(patsubst %.idl,$(STAGE_DIR)/%.xpt,$(IDLSRC))
IDL_REQUIRES  = cci $(MODULE_EXPORT_NAME) $(REQUIRES)
IDL_INCLUDES  = $(patsubst %,-I"$(IDLDIR)/%",$(IDL_REQUIRES))
IDL_PYCACHE   = "$(BUILD_DIR)/.parts/xpidl" 
IDL_FLAGS     = --cachedir $(IDL_PYCACHE)

ifdef XPIDL_MODULE
XPT_LINKLIB = $(CCIDIR)/$(XPIDL_MODULE).xpt
endif

xpidl:: $(CPP_IDL_FILES) $(XPT_LINKLIB)

$(XPT_LINKLIB): $(XPT_IDL_FILES)
	@echo "$@"
	@$(PYTHON) -m xpidl.xpt link $@ $?

# Rule for building C++ header
$(EXPORT_DIR)/%.h : %.idl
	@echo $<
	@mkdir -p $(IDL_PYCACHE)
	@$(PYTHON) -m xpidl.header $(IDL_FLAGS) $(IDL_INCLUDES) -xpt $(STAGE_DIR)/$(basename $<).xpt  $< > "$@"

# Rule for building xpt files
# Won't be rebuild if built from previous rule
$(STAGE_DIR)/%.xpt : %.idl
	@echo $<
	@mkdir -p $(IDL_PYCACHE)
	@$(PYTHON) -m xpidl.typelib $(IDL_FLAGS) $(IDL_INCLUDES) $< -o "$@"

clean::
	@for f in $(CPP_IDL_FILES); do \
	  echo "$$f" ; \
	  rm -f "$$f"; \
	done
	@for f in $(XPT_IDL_FILES); do \
	  echo "$$f"; \
	  rm -f "$$f"; \
	done
	rm -f $(XPT_LINKLIB)

