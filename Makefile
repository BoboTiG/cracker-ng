NAME    = cracker-ng
MODULES = cpt zip

all: title
	@echo " - Available modules are: cpt, zip."
	@echo " + Try 'make <module>' to build <module>cracker-ng."
	@echo " + Try 'make modules' to build all modules."

clean:
	rm -f bin/*$(NAME)

modules: $(MODULES)

title:
	@echo " ~ Cracker-ng, a multiple file password finder."
	@mkdir -p bin

$(MODULES): title
	@echo " + Building $@ module ..."
	make -C src/$@/ $@$(NAME) clean
	mv src/$@/$@$(NAME) bin/
