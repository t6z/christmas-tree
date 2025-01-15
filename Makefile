# Makefile for christmas-tree project

# Toolchain locations
AVR_BIN_DIR=~/avr/avr-gcc-14.1.0-x64-linux/bin
AVR_DUDE_DIR=~/avr/avrdude_Linux_64bit/bin

# Project options
MCU=attiny402
# The internal clock frequency is 20MHz with a divider of 6 (CLKCTRL.PDIV = 0x8 at POR)
CLOCK_FREQ=3333333UL
# The `build` directory is hard-coded throughout this Makefile
PROJ_BLD=build/christmas-tree
# use SerialUPDI programmer (USB to Serial adapter)
# PROG_STR=serialupdi
PROG_STR=pickit5_updi
PROG_MCU=t402

# Compiler flags
# -std=c11 : use C11 standard (published in 2011). Who knows if this actually matters
# -Wall : You use -W<somewarning> to enable certain warnings. This option enables most warnings, like 80 of them.
# -Wextra : Enables additional warnings not enabled by -Wall
# -Werror : Makes all Warnings into Errors
# -mmcu : defines what architecture/microcontroller to compile to. The compiler will use this option to choose startup
#         and run-time libraries that get linked together. If this option isn't specified, the compiler defaults to the
#         8515 processor environment, which you do not want.
# -DF_CPU : -D creates a pre-processor define. This defines `DF_CPU` to the clock frequency for <avr/io.h> and other files
CFLAGS=-std=c11 -Wall -Wextra -Werror -mmcu=$(MCU) -DF_CPU=$(CLOCK_FREQ)
# -Os : Optimize for size. Enables all -O2 optimizations except those that increase code size. Recommended optimization level for AVR.
# -g : Generate debugging information
# -Wl,-Map : -Wl passes options to Linker. -Map option generates a map output file from Linker
# -DDEBUG : defines `DEBUG` for whatever reason
OPT_FLAGS=-O1 -g -gdwarf-2 -Wl,-Map,$(PROJ_BLD).map -DDEBUG

# wildcard expansion will get a list of all the C source files in the src/ directory
SRCS=$(wildcard src/*.c)
# pattern rule (%) will create list of Object file names from C source file names
OBJS=$(SRCS:%.c=build/%.o)
# an includes directory that doesn't exist in this project so is commented out
INC=#-I ./inc

# Compiler and utility tools
OBJCOPY=$(AVR_BIN_DIR)/avr-objcopy
OBJDUMP=$(AVR_BIN_DIR)/avr-objdump
OBJSIZE=$(AVR_BIN_DIR)/avr-size
CC=$(AVR_BIN_DIR)/avr-gcc
AVRDUDE=$(AVR_DUDE_DIR)/avrdude

# Rules

# Rule syntax
# targets : prerequisites
#       recipe
#       ...

# dummy 'all' target, who's prerequisite is the projects ELF file.
# when you run 'make', the first target is 'all' which builds the projects ELF file.
# 'all' could be named anything, but its standard convention to use 'all'
all: $(PROJ_BLD).elf

# Builds project ELF file from prerequisite Object files list
$(PROJ_BLD).elf: $(OBJS)
# Links together Object files into an ELF file
# $@ : file name of the target (ELF file)
# $^ : name of all the prerequisites (Object files) with spaces between them
	$(CC) -o $@ $^ $(INC) $(CFLAGS) $(OPT_FLAGS) $(MCU_FLAGS)
	$(OBJDUMP) -h -S $@ > $(PROJ_BLD).lst
# Display size information of the ELF file
	$(OBJSIZE) $@
# Converts ELF file to Intel HEX file format, only copying the .text and .data sections
# -j <name> : only copy section <name> into the output
# -O : output format
	$(OBJCOPY) -j .text -j .data -O ihex $@ $(PROJ_BLD).hex
# Converts ELF file into Binary file format
	$(OBJCOPY) -j .text -j .data -O binary $@ $(PROJ_BLD).bin

# builds Object files from prerequisite C Source files list.
# I don't really get why we can do $(OBJS) : $(SRCS) here but whatever
build/%.o: %.c
# Create the build and src directory if they don't exist
# -p : generate no error if directory already exists, make parent directories as needed
	mkdir -p build/src
# Compile the C files into Object files
# -c : just compile and stop, do not link (do this because there may be multiple source files)
	$(CC) -c -o $@ $(INC) $(CFLAGS) $(OPT_FLAGS) $(MCU_FLAGS) $<

# I guess this is legal. The OPT_FLAGS variable will be changed before the prerequisite is defined?
# -O2 : Level 2 optimizations, including those that may increase code size
# -DNDEBUG : Defines `NDEBUG`. Likely to disable any sort of debug information?
release: OPT_FLAGS=-O2 -DNDEBUG
release: $(PROJ_BLD).elf

flash:
	$(AVRDUDE) -v -v -v -c $(PROG_STR) -p $(PROG_MCU) -U flash:w:$(PROJ_BLD).hex:i

# flash-debug:
# 	$(AVRDUDE) -v -c $(PROG_STR) -p $(PROG_MCU) -U flash:w:$(PROJ_BLD).elf:e

clean:
# Removes build directory
# -rf : recursive (everything below) forced (ignores nonexistent files, never prompts) removal
	rm -rf build

# We place the Phony Targets (Targets that aren't actually files) down here.
# Two reasons we do this:
# 1. avoid conflict with a file of the same name
# 2. improve performance (make doesn't search for the file)
.PHONY = all, clean, release, flash, flash-debug
