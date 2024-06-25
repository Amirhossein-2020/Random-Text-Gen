project		:= project

# Variables for pathes of source, header
inc_dir		:= ./include
src_dir		:= ./src
sources		:= $(wildcard $(src_dir)/*.c)
build_dir	:= ./build
obj_dir		:= $(build_dir)/obj
bin_dir		:= $(build_dir)/bin
executable	:= $(bin_dir)/$(project)
build_dirs	:= $(obj_dir) $(bin_dir)
objects		:= $(subst .c,.o,$(subst $(src_dir),$(obj_dir),$(sources)))
# C Compiler configuration
CC := gcc
CFLAGS := -I$(inc_dir) -Wall -g
# -Wall			Warnings: all - display all warinings
# -I$(inc_dir)	Look in the include directory for include files

# Phony rules
.PHONY: all run clean

all: $(executable)

# help: Display useful goals in this Makefile
help:
	@echo "try one of the following make goals"
	@echo " * all(default) - build the project"
	@echo " * run - execute the project"
	@echo " * clean - delete build files in project"

# Execute the project's binary file
run: $(executable)
	@$(^)

# Build the project by compiling all object files
$(executable): $(objects) | $(bin_dir)
	$(CC) $(CFLAGS) -o $(@) $(^)

# Build object files from sources in source directory
$(obj_dir)/%.o: $(src_dir)/%.c | $(obj_dir)
	$(CC) $(CFLAGS) -c -o $(@) $(<)

# Build directory should be recreated in case of first compilation or after clean command
$(build_dirs):
	mkdir -p $(dir $@/)

# clean will force remove all of the build's directory contents recursively
clean:
	rm -rf $(build_dir)