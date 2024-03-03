# Setup

ifeq ($(OS),Windows_NT)
	ifeq ($(MSYSTEM), MINGW32)
		UNAME := MSYS
	else
		UNAME := Windows
	endif
else
	UNAME := $(shell uname)
endif

default: all

# Directories

src_dir = .
corelib_dir = ../../SDK
thirdparty_libs = ../../3rdparty

# Rules

RISCV_PREFIX ?= riscv64-unknown-elf-
RISCV_GCC ?= $(RISCV_PREFIX)g++

ifeq ($(UNAME), Windows)
RISCV_GCC_OPTS ?= -mcmodel=medany -std=c++14 -Wall -Ofast -march=rv32imc -mabi=ilp32 --param "l1-cache-line-size=64" --param "l1-cache-size=16" -ffunction-sections -fdata-sections -Wl,-gc-sections -fPIC -lgcc -lm
else
RISCV_GCC_OPTS ?= -mcmodel=medany -std=c++14 -Wall -Ofast -march=rv32imc_zicsr_zifencei -mabi=ilp32 --param "min-pagesize=0" --param "l1-cache-line-size=64" --param "l1-cache-size=16" -ffunction-sections -fdata-sections -Wl,-gc-sections -fPIC -lgcc -lm
endif

incs  += -I$(src_dir) -I$(corelib_dir)/ -I$(thirdparty_libs)/ -I$(src_dir)/.
libs += $(wildcard $(corelib_dir)/*.c) $(wildcard $(thirdparty_libs)/**/*.c)
objs  := chris24.elf

$(foreach folder,$(folders),$(eval $(call compile_template,$(folder))))

# Build

chris24.elf: $(wildcard $(src_dir)/*)
	$(RISCV_GCC) $(incs) -o $@ $(wildcard $(src_dir)/*.cpp) $(libs) $(RISCV_GCC_OPTS)

junk += $(folders_riscv_bin)

# Default

all: chris24.elf

# Clean

clean:
ifeq ($(UNAME), Windows)
	del $(objs) $(junk)
else
	rm -rf $(objs) $(junk)
endif