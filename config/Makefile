#
# Global makefile for xpcci
#

# Include user/local configuration

DEPTH=..
include config.mk

BUILD_DIRS= \
  $(DEPTH)/include \
  $(DEPTH)/utilities \
  $(DEPTH)/cci \
  $(DEPTH)/runtime \
  $(NULL)

.PHONY: all clean create_repository package config $(BUILD_DIRS)

all: create_repository $(BUILD_DIRS)

contrib:
	@$(MAKE) -C $(DEPTH)/contrib/sources

create_repository:
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIRS):
	@$(MAKE) -C $@ module

$(DEPTH)/utilities: $(DEPTH)/include
$(DEPTH)/cci: $(DEPTH)/utilities
$(DEPTH)/runtime: $(DEPTH)/cci

package:
	@$(MAKE) -C $(DEPTH)/config -f packager.mk

clean:
	@for d in $(BUILD_DIRS); do \
		$(MAKE) -C $$d clean; \
	done

echo-config:
	@echo BUILD_CONFIG            = $(BUILD_CONFIG)
	@echo MAKE_TARGET             = $(MAKE_TARGET)
	@echo DM_DEBUG                = $(DM_DEBUG)
	@echo BUILD_DIR               = $(BUILD_DIR)
	@$(MAKE) -f $(MAKE_TARGET) config DEPTH=.. BUILD_CONFIG=$(BUILD_CONFIG)
