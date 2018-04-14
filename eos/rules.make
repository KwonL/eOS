TARGET := $(shell pwd)/module.a

# subdirs that contains Makefile
subdir_makefiles := $(shell find -L ./ -name Makefile )
un := $(filter ./hal%,$(subdir_makefiles))
un := $(filter-out ./hal/$(HAL)%,$(un))
subdir_makefiles := $(filter-out $(un),$(subdir_makefiles))
subdirs := $(patsubst ./%,%,$(dir $(subdir_makefiles)))
subdir_libs := $(patsubst %,%module.a,$(subdirs))

# target to make each subdirs
subdir_targets := $(patsubst %,___%,$(subdirs))

$(patsubst %,___%,$(subdirs)):
	@$(MAKE) -C $(patsubst ___%,%,$@) $(PWD)/$(patsubst ___%,%,$(patsubst %/,%,$@))/module.a

# target to clean each subdirs
subdir_cleans := $(patsubst %,_clean_%,$(subdirs))

$(patsubst %,_clean_%,$(subdirs)):
	@$(MAKE) -C $(patsubst _clean_%,%,$@) clean_

# cleaning this directory
clean_: $(subdir_cleans)
	rm -rf $(TARGET) $(OBJS)

C_SRCS := $(wildcard *.c)
S_SRCS := $(wildcard *.S)
OBJS := $(patsubst %.c,%.o,$(C_SRCS)) $(patsubst %.S,%.o,$(S_SRCS))

# compling and linking this directory
$(TARGET): banner $(OBJS)
ifneq ($(OBJS),)
	ar r $@ $(filter-out banner, $^)
endif

banner:
	@echo ----------------------------------------------------
	@echo Compiling in $(shell pwd)
	@echo ----------------------------------------------------

%.o: %.c
ifeq ($(shell pwd), $(PWD)/user)
	$(CC) -c -Os -Wall -I$(HPATH) -o $@ $< 
else
	$(CC) -c -Os -Wall -D_KERNEL_ -I$(HPATH) -o $@ $< 
endif

%.o: %.S
	$(CC) -c -D_KERNEL_ -I$(HPATH) -o $@ $<
