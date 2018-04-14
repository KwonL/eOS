##################################################################
#
# Choose the Target
#
#	* Emulation on IA32/Linux
#	* smdk2440 board (ARM)
#	* ...
#
##################################################################
#export HAL := smdk2440
export HAL := linux

##################################################################
#
# Kernel Build
#
##################################################################
export TOP_DIR := $(shell pwd)
export MAKERULE := $(TOP_DIR)/rules.make
export HPATH := $(TOP_DIR)
include $(TOP_DIR)/hal/$(HAL)/Makefile
export CC := $(GCC_PREFIX)gcc
export LD := $(GCC_PREFIX)ld

all: prepare eos

prepare:
	@echo ----------------------------------------------------
	@echo Building EOS - embedded Operating System
	@echo ----------------------------------------------------
	rm -f hal/current
	ln -sf $(HAL) hal/current

