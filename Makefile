
CC      = g++
CFLAGS  = -march=native -fomit-frame-pointer -ffunction-sections -fwhole-program -ffast-math -DUSE_STATS
LDFLAGS = -s -lpthread 
ZIP_F   = -O2 -lboost_iostreams
#ZIP_F  += -lmpi
CPT_F   = -O3
NAME    = cracker-ng

all: title
	@echo " - Available modules are: cpt, zip."
	@echo " + Try 'make <module>' to build <module>cracker-ng."
	@echo " + Try 'make <module> debug=yes' to build <module>cracker-ng in debug mode."
	@echo " + Try 'make modules' to build all modules."
	@echo " + Try 'make modules debug=yes' to build all modules in debug mode."

clean:
	rm -f bin/*$(NAME)

debug:
ifneq ($(debug),)
# For debug/devel purpose
	@echo "(DEBUG enabled)"
CFLAGS  = -DDEBUG -g -O0
CFLAGS += -pedantic -Wall -Wextra -Wunused -Wformat=2 -Weffc++ -Wpadded -fexceptions
endif

modules: cpt zip

title: debug
	@echo " ~ Cracker-ng, a multiple file password finder."
	@mkdir -p bin

# Modules
cpt: title
	@echo " + Building $@ module ..."
	@$(CC) $(CPT_F) $(CFLAGS) $(LDFLAGS) src/$@/$@$(NAME).cpp -o bin/$@$(NAME)

zip: title
	@echo " + Building $@ module ..."
	@$(CC) $(ZIP_F) $(CFLAGS) $(LDFLAGS) src/$@/$@$(NAME).cpp -o bin/$@$(NAME)
