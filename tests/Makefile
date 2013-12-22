NAME    = cracker-ng.d
MODULES = cpt zip
DICO    = -w list/10k-most-common.txt

# Colors
GREEN   = \033[32;02m
YELLOW  = \033[33m
NORMAL  = \033[0m
PAUSE   = @echo "$(YELLOW)Press any key to continue ...$(NORMAL)" && read PAUSE

# Debug/test tools
VAL     = /usr/bin/valgrind
VALOPT  = --leak-check=full --show-possibly-lost=yes --show-reachable=yes --track-origins=yes --malloc-fill=ff
PERF    = /usr/bin/perf
PERFOPT = -B -e cycles,instructions,cpu-clock,branch-misses,cache-misses,faults,alignment-faults


all:
	@echo " - Available modules are: $(GREEN)$(MODULES)$(NORMAL)"
	@echo " + Try 'make <module>' to build <module>cracker-ng."
	@echo " + Try 'make modules' to build all modules."

modules: $(MODULES)

$(MODULES):
	@echo "$(GREEN) + Building the $@ module ...$(NORMAL)"
	make -C ../src/ -f Makefile.debug $@ clean
	mv ../src/$@$(NAME) $@$(NAME)
	$(PAUSE)

	@echo "$(GREEN) + Testing/debugging the $@ module ...$(NORMAL)"
	@echo "$(GREEN)     === Valgrind$(NORMAL)"
	$(VAL) $(VALOPT) ./$@$(NAME) -f files/$@.test $(DICO)
	$(PAUSE)

	@echo "$(GREEN)     === Perf$(NORMAL)"
	$(PERF) stat $(PERFOPT) ./$@$(NAME) -f files/$@.test $(DICO)
	$(PAUSE)
	$(PERF) record ./$@$(NAME) -f files/$@.test $(DICO)
	$(PERF) report

	@echo "$(YELLOW)<<!>> $(GREEN)END$(YELLOW) <<!>>$(NORMAL)"
