
CXX     = g++
CFLAGS  = -O3 -s -fomit-frame-pointer -ffunction-sections -ffast-math
CFLAGS += -Wall -W -pedantic -Wextra -Wunused -Wformat=2 -Weffc++ -Wpadded -fexceptions
LDLIBS  = -lpthread
OBJ     = rijndael.o tables.o functions.o stats.o main.o
NAME    = cptcracker-ng

all: $(NAME)

clean:
	rm -f *.o

mrproper: clean
	rm -f cptcracker-ng

$(NAME): $(OBJ)
	$(CXX) $(CFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CXX) $(CFLAGS) -c $*.c

%.o: %.cc
	$(CXX) $(CFLAGS) -c $*.cc

functions.o: ../shared/functions-ng.cc ../shared/functions-ng.h
	$(CXX) $(CFLAGS) -c ../shared/$*-ng.cc -o $@

stats.o: ../shared/stats.cc ../shared/stats.h
	$(CXX) $(CFLAGS) -c ../shared/$*.cc
