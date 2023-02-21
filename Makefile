### *Get a more readable version of this Makefile* by `make html` (requires python-markdown)
html:
	cat Makefile | sed 's/^\([^#]\)/    \1/g' | markdown_py > Makefile.html
.PHONY: html

ifeq ($(MAKECMDGOALS),)
  MAKECMDGOALS  = image
  .DEFAULT_GOAL = image
endif

## architecture is unisys by default

ARCH ?= unisys-riscv32

## please specify this when you want to run this
ISKAM ?= /home/litrehinn/Documents/Programs/iskam-unisys

### Override checks when `make clean/clean-all/html`
ifeq ($(findstring $(MAKECMDGOALS),clean|clean-all|html|count),)

### Print build info message
$(info # Building $(NAME)-$(MAKECMDGOALS) [$(ARCH)])

### Check: environment variable `$ISKAM` looks sane
ifeq ($(wildcard $(ISKAM)/am/include/am.h),)
  $(error $$ISKAM must be an AbstractMachine repo)
endif

### Check if there is something to build
ifeq ($(flavor SRCS), undefined)
  $(error Nothing to build)
endif

endif

## General Compilation Targets

### Create the destination directory (`build/$ARCH`)
WORK_DIR  = $(shell pwd)
DST_DIR   = $(WORK_DIR)/build/$(ARCH)
$(shell mkdir -p $(DST_DIR))
MEM_DIR 	= $(WORK_DIR)/build/memory
$(shell mkdir -p $(MEM_DIR))

### Compilation targets (a binary image or archive)
IMAGE_REL = build/$(NAME)-$(ARCH)
IMAGE     = $(abspath $(IMAGE_REL))
ARCHIVE   = $(WORK_DIR)/build/$(NAME)-$(ARCH).a

### Collect the files to be linked: object files (`.o`) and libraries (`.a`)
OBJS      = $(addprefix $(DST_DIR)/, $(addsuffix .o, $(basename $(SRCS))))
# LIBS      = am klib
LIBS     := $(sort $(LIBS) am klib) # lazy evaluation ("=") causes infinite recursions
LINKAGE   = $(OBJS) \
  $(addsuffix -$(ARCH).a, $(join \
    $(addsuffix /build/, $(addprefix $(ISKAM)/, $(LIBS))), \
    $(LIBS) ))


## Compilation Tools
## Use gcc-riscv64-linux-gnu

CROSS_COMPILE ?= riscv64-linux-gnu-

PYTHON    = python3
AS        = $(CROSS_COMPILE)gcc-11
CC        = $(CROSS_COMPILE)gcc-11
CXX       = $(CROSS_COMPILE)g++-11
LD        = $(CROSS_COMPILE)ld
OBJDUMP   = $(CROSS_COMPILE)objdump
OBJCOPY   = $(CROSS_COMPILE)objcopy
READELF   = $(CROSS_COMPILE)readelf

ISA ?= riscv32
PLATFORM ?= unisys

## Add Libs include
INC_PATH += $(WORK_DIR)/include $(addsuffix /include/, $(addprefix $(ISKAM)/, $(LIBS)))
INCFLAGS += $(addprefix -I, $(INC_PATH))

CFLAGS   += -O2 -MMD -Wall $(INCFLAGS) \
            -DARCH_H=\"arch/$(ARCH).h\" \
            -fno-asynchronous-unwind-tables -fno-builtin -fno-stack-protector \
            -Wno-main -U_FORTIFY_SOURCE

## Arch-specified Rules

## riscv32
COMMON_FLAGS  := -fno-pic -march=rv32i -mabi=ilp32
CFLAGS        += $(COMMON_FLAGS) -static
ASFLAGS       += $(COMMON_FLAGS) -O0
LDFLAGS       += -melf32lriscv

## unisys
AM_SRCS := trm.c \
           ioe/ioe.c \
           ioe/timer.c \
           ioe/input.c \
           ioe/gpu.c \
           ioe/audio.c \
           ioe/disk.c \
					 ioe/uart.c \
					 ioe/ascii.S\
           mpe.c

AM_SRCS += start.S \
           cte.c \
           trap.S \
           vme.c

CFLAGS    += -fdata-sections -ffunction-sections

LDFLAGS   += -T $(ISKAM)/scripts/linker.ld \
             --defsym=_pmem_start=0x80000000 --defsym=_entry_offset=0x0
LDFLAGS   += --gc-sections -e _start

CFLAGS += -DMAINARGS=\"$(mainargs)\"
CFLAGS += -I$(ISKAM)/am/src/include

## sic erat scriptum
## I dont think this is a good idea
# .PHONY: $(ISKAM)/am/src/trm.c

image: $(IMAGE).elf
	@$(OBJDUMP) -d $(IMAGE).elf > $(IMAGE).txt
	@echo + OBJCOPY "->" $(IMAGE_REL).bin
	@$(OBJCOPY) -S --set-section-flags .bss=alloc,contents -O binary $(IMAGE).elf $(IMAGE).bin
	@$(PYTHON) $(ISKAM)/scripts/image.py $(IMAGE).bin $(MEM_DIR)/data
	@mkdir -p $(ISKAM)/build
	@cp $(IMAGE).bin $(ISKAM)/build
	@cp -r $(MEM_DIR) $(ISKAM)/build

## Compilation Rules
## From NJU-ProjectN AM Makefile

### Rule (compile): a single `.c` -> `.o` (gcc)
$(DST_DIR)/%.o: %.c
	@mkdir -p $(dir $@) && echo + CC $<
	@$(CC) -std=gnu11 $(CFLAGS) -c -o $@ $(realpath $<)

### Rule (compile): a single `.cc` -> `.o` (g++)
$(DST_DIR)/%.o: %.cc
	@mkdir -p $(dir $@) && echo + CXX $<
	@$(CXX) -std=c++17 $(CXXFLAGS) -c -o $@ $(realpath $<)

### Rule (compile): a single `.cpp` -> `.o` (g++)
$(DST_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@) && echo + CXX $<
	@$(CXX) -std=c++17 $(CXXFLAGS) -c -o $@ $(realpath $<)

### Rule (compile): a single `.S` -> `.o` (gcc, which preprocesses and calls as)
$(DST_DIR)/%.o: %.S
	@mkdir -p $(dir $@) && echo + AS $<
	@$(AS) $(ASFLAGS) -c -o $@ $(realpath $<)

### Rule (recursive make): build a dependent library (am, klib, ...)
$(LIBS): %:
	@$(MAKE) -s -C $(ISKAM)/$* archive

### Rule (link): objects (`*.o`) and libraries (`*.a`) -> `IMAGE.elf`, the final ELF binary to be packed into image (ld)
$(IMAGE).elf: $(OBJS) am $(LIBS)
	@echo + LD "->" $(IMAGE_REL).elf
	@$(LD) $(LDFLAGS) -o $(IMAGE).elf --start-group $(LINKAGE) --end-group

### Rule (archive): objects (`*.o`) -> `ARCHIVE.a` (ar)
$(ARCHIVE): $(OBJS)
	@echo + AR "->" $(shell realpath $@ --relative-to .)
	@ar rcs $(ARCHIVE) $(OBJS)

### Rule (`#include` dependencies): paste in `.d` files generated by gcc on `-MMD`
-include $(addprefix $(DST_DIR)/, $(addsuffix .d, $(basename $(SRCS))))


## Miscellaneous

### Build order control
image: image-dep
archive: $(ARCHIVE)
image-dep: $(OBJS) am $(LIBS)
	@echo \# Creating image [$(ARCH)]
.PHONY: image image-dep archive run $(LIBS)

### Clean a single project (remove `build/`)
clean:
	rm -rf Makefile.html $(WORK_DIR)/build/
.PHONY: clean

### Clean all sub-projects within depth 2 (and ignore errors)
CLEAN_ALL = $(dir $(shell find . -mindepth 2 -name Makefile))
clean-all: $(CLEAN_ALL) clean
$(CLEAN_ALL):
	-@$(MAKE) -s -C $@ clean
.PHONY: clean-all $(CLEAN_ALL)
