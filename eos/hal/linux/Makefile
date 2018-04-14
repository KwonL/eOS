include $(MAKERULE)

export GCC_PREFIX :=

clean: clean_
	rm -f $(TOP_DIR)/eos

eos: $(subdir_targets)
	$(CC) -o $(TOP_DIR)/eos -Wl,--start-group $(subdir_libs) -Wl,--end-group -lrt
	@echo
	@echo Building EOS is complete. Type ./eos to run EOS.
