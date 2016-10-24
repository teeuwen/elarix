#
# src/kernel/Makefile
#

include src/kernel/arch/$(ARCHT)/Makefile
include src/kernel/dev/Makefile
include src/kernel/fs/Makefile
include src/kernel/lib/Makefile
include src/kernel/mm/Makefile
include src/kernel/init/Makefile

kernel-i += -I src/kernel/dev
kernel-i += -I src/kernel/include
kernel-i += -I src/kernel/lib

%32.o: %32.c
	@echo -e "\033[1;37m> Compiling \033[0;32m$<\033[1m...\033[0m"
	@$(CC32) $(CFLAGS32) -c $< $(kernel-i) -o $@
ifeq ($(ARCH),x86_64)
	@objcopy -O elf64-x86-64 $@ $@.64
	@mv $@.64 $@
endif

%.o: %.c
	@echo -e "\033[1;37m> Compiling \033[0;32m$<\033[1m...\033[0m"
	@$(CC) $(CFLAGS) -c $< $(kernel-i) -o $@

%.o: %.S
	@echo -e "\033[1;37m> Assembling \033[0;32m$<\033[1m...\033[0m"
	@$(CC) $(ASFLAGS) -c $< -o $@


PHONY += kernel
kernel: bin/kernel
bin/kernel: $(kernel-o) $(kernel-o32)
	@echo -e "\033[1m> Linking \033[0;32m$@\033[1m...\033[0m"
	@$(LD) $(LDFLAGS) -T src/kernel/link.ld -o bin/kernel $(kernel-o) $(kernel-o32)