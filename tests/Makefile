NAME    = cracker-ng
MODULES = cpt zip
DICO    = -w list/10k-most-common.txt

# Colors
GREEN   = \033[32;02m
YELLOW  = \033[33m
NORMAL  = \033[0m
PAUSE   = @echo "$(YELLOW)Press any key to continue ...$(NORMAL)" && read PAUSE

# Debug/test tools
VAL     = /usr/bin/valgrind
VALOPT  = --leak-check=full --show-possibly-lost=yes --show-reachable=yes --track-origins=yes --track-fds=yes --malloc-fill=0x80 --free-fill=0x70
VALOPT2 = --tool=callgrind --simulate-cache=yes --dump-instr=yes
PERF    = /usr/bin/perf
PERFOPT = -B -e cycles,instructions,cpu-clock,branch-misses,cache-misses,faults,alignment-faults


all:
	@echo " - Available modules are: $(GREEN)$(MODULES)$(NORMAL)"
	@echo " + Try 'make <module>' to build <module>cracker-ng."
	@echo " + Try 'make modules' to build all modules."

modules: $(MODULES)

$(MODULES):
	@echo "$(GREEN) === Module $@ ===$(NORMAL)"

	#@echo "$(GREEN) + Address Sanitizer first ...$(NORMAL)"
	#make -C ../src/ -f Makefile $@-ASAN mrproper DEBUG=1
	#$(PAUSE)

	@echo "$(GREEN) + Building ...$(NORMAL)"
	make -C ../src/ -f Makefile $@ clean DEBUG=1
	mv ../src/$@$(NAME) $@$(NAME)
	$(PAUSE)

	@echo "$(GREEN) + Testing/debugging ...$(NORMAL)"
	@echo "$(GREEN)     === Valgrind$(NORMAL)"
	$(VAL) $(VALOPT) ./$@$(NAME) -f files/$@.test $(DICO)
	$(PAUSE)

	@echo "$(GREEN) + CPU cycles ...$(NORMAL)"
	@echo "$(GREEN)     === Valgrind$(NORMAL)"
	$(VAL) $(VALOPT2) ./$@$(NAME) -f files/$@.test $(DICO)
	$(PAUSE)

	@echo "$(GREEN) + Performances ...$(NORMAL)"
	$(PERF) stat $(PERFOPT) ./$@$(NAME) -f files/$@.test $(DICO)
	$(PAUSE)
	$(PERF) record ./$@$(NAME) -f files/$@.test $(DICO)
	$(PERF) report

	@echo "$(YELLOW)<<!>> $(GREEN)END$(YELLOW) <<!>>$(NORMAL)"
