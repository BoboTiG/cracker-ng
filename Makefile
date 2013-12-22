NAME    = cracker-ng
MODULES = cpt zip

all: title
	@echo " - Available modules are: $(MODULES)"
	@echo " + Try 'make <module>' to build <module>cracker-ng."
	@echo " + Try 'make modules' to build all modules."

clean:
	rm -f bin/*$(NAME)

modules: title $(MODULES)

title:
	@echo " ~ Cracker-ng, a multiple file password finder."

$(MODULES):
	@echo " + Building $@ module ..."
	@[ -d bin ] || mkdir bin
	make -C src/ $@ clean
	mv src/$@$(NAME) bin/

love:
	@echo "I'm free, you could find my email ID into source files ;)"

me a sandwich.:
	@if [ "$@" = "sandwich." ]; then echo $(shell if [ $(shell id -ru) = 0 ]; then echo "Okay."; else echo "What? Make it yourself."; fi); fi
