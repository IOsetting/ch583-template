# 'make V=1' will show all compiler calls.
V		?= 0
ifeq ($(V),0)
Q		:= @
NULL	:= 2>/dev/null
endif

PREFIX		?= $(GCC_TOOCHAIN)/$(GCC_PREFIX)
CC			= $(PREFIX)gcc
AS			= $(PREFIX)as
LD			= $(PREFIX)ld
OBJCOPY		= $(PREFIX)objcopy
OBJDUMP		= $(PREFIX)objdump
BDIR		= $(TOP)/$(BUILD_DIR)

# For each direcotry, add it to csources
CSOURCES := $(foreach dir, $(CDIRS), $(shell find $(TOP)/$(dir) -maxdepth 1 -name '*.c'))
# Add single c source files to csources
CSOURCES += $(addprefix $(TOP)/, $(CFILES))
# Then assembly source folders and files
ASOURCES := $(foreach dir, $(ADIRS), $(shell find $(TOP)/$(dir) -maxdepth 1 -iname '*.s'))
ASOURCES += $(addprefix $(TOP)/, $(AFILES))

LIBSOURCES := $(addprefix $(TOP)/, $(LIBS))

# Fill object files with c and asm files (keep source directory structure), format: $(var:a=b)
OBJS = $(CSOURCES:$(TOP)/%.c=$(BDIR)/%.o)
OBJS += $(ASOURCES:$(TOP)/%.s=$(BDIR)/%.o)
OBJS += $(LIBSOURCES)
# d files for detecting h file changes
DEPS=$(CSOURCES:$(TOP)/%.c=$(BDIR)/%.d)

# Arch and target specified flags
# - GCC 12: use `-march=rv32imac_zicsr` instead of `-march=rv32imac`
ARCH_FLAGS	:= -march=rv32imac -mabi=ilp32 \
			-mcmodel=medany -msmall-data-limit=8 -mno-save-restore \
			-fmessage-length=0 \
			-fsigned-char

# Debug options, -gdwarf-2 for debug, -g0 for release 
# https://gcc.gnu.org/onlinedocs/gcc-12.2.0/gcc/Debugging-Options.html
#  -g: system’s native format, -g0:off, -g/g1,-g2,-g3 -> more verbosely
#  -ggdb: for gdb, -ggdb0:off, -ggdb/ggdb1,-ggdb2,-ggdb3 -> more verbosely
#  -gdwarf: in DWARF format, -gdwarf-2,-gdwarf-3,-gdwarf-4,-gdwarf-5
DEBUG_FLAGS ?= -gdwarf-3

# c flags
OPT			?= -Og
CSTD		?= -std=gnu99
TGT_CFLAGS 	+= $(ARCH_FLAGS) $(DEBUG_FLAGS) $(OPT) $(CSTD) $(addprefix -D, $(LIB_FLAGS)) -Wall -ffunction-sections -fdata-sections -fno-common

# asm flags
TGT_ASFLAGS += $(ARCH_FLAGS) $(DEBUG_FLAGS) $(OPT) -Wa,--warn

# ld flags
TGT_LDFLAGS += $(ARCH_FLAGS) $(OPT) -Wall -nostartfiles \
				-Xlinker --gc-sections -Wl,-Map,$(BDIR)/$(PROJECT).map -Wl,--print-memory-usage \
				--specs=nano.specs --specs=nosys.specs

# include paths
TGT_INCFLAGS := $(addprefix -I $(TOP)/, $(INCLUDES))


.PHONY: all clean flash echo

all: $(BDIR)/$(PROJECT).elf $(BDIR)/$(PROJECT).bin $(BDIR)/$(PROJECT).hex $(BDIR)/$(PROJECT).lst

# for debug
echo:
	$(info 1. $(AFILES))
	$(info 2. $(ASOURCES))
	$(info 3. $(CSOURCES))
	$(info 4. $(OBJS))
	$(info 5. $(TGT_INCFLAGS))

# include d files without non-exist warning
-include $(DEPS)

# Compile c to obj
$(BDIR)/%.o: $(TOP)/%.c
	@printf "  CC\t$<\n"
	@mkdir -p $(dir $@)
	$(Q)$(CC) $(TGT_CFLAGS) $(TGT_INCFLAGS) -MT $@ -o $@ -c $< -MMD -MP -MF $(BDIR)/$*.d

# Compile asm to obj
$(BDIR)/%.o: $(TOP)/%.s
	@printf "  AS\t$<\n"
	@mkdir -p $(dir $@)
	$(Q)$(CC) $(TGT_ASFLAGS) -o $@ -c $<

# Link object files to elf
$(BDIR)/$(PROJECT).elf: $(OBJS) $(TOP)/$(LDSCRIPT)
	@printf "  LD\t$@\n"
	$(Q)$(CC) $(TGT_LDFLAGS) $(TGT_INCFLAGS) -T$(TOP)/$(LDSCRIPT) $(OBJS) -o $@

# Convert elf to bin
%.bin: %.elf
	@printf "  OBJCP BIN\t$@\n"
	$(Q)$(OBJCOPY) -O binary  $< $@

# Convert elf to hex
%.hex: %.elf
	@printf "  OBJCP HEX\t$@\n"
	$(Q)$(OBJCOPY) -O ihex  $< $@

%.lst: %.elf
	@printf "  OBJDP LST\t$@\n"
	$(Q)$(OBJDUMP) --source --all-headers --demangle --line-numbers --wide $< > $@

clean:
	rm -rf $(BDIR)/*

# wch-riscv.cfg can be found under wch openocd install path
#  - Erase all:     openocd -f wch-riscv.cfg -c init -c halt -c "flash erase_sector wch_riscv 0 last" -c exit
#  - Program:       openocd -f wch-riscv.cfg  -c init -c halt  -c "program xxx.hex\bin\elf" -c exit
#  - Verify:        openocd -f wch-riscv.cfg -c init -c halt -c "verify_image xxx.hex\bin\elf" -c exit
#  - Reset&Resume:  openocd -f wch-riscv.cfg -c init -c halt -c wlink_reset_resume -c exit
flash:
	$(OPENOCD_PATH)/openocd -f $(TOP)/$(OPENOCD_CFG) -c init -c halt -c "flash erase_sector wch_riscv 0 last" -c "program $(BDIR)/$(PROJECT).hex" -c "verify_image $(BDIR)/$(PROJECT).hex" -c wlink_reset_resume -c exit

reset:
	$(OPENOCD_PATH)/openocd -f $(TOP)/$(OPENOCD_CFG) -c init -c halt -c wlink_reset_resume -c exit
