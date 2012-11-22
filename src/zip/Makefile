
CXX     = g++
CFLAGS  = -march=native -O3 -mmmx -msse -msse2 -msse3 -m3dnow -ffast-math -mno-ieee-fp -s
CFLAGS += -Wfatal-errors -Werror -Wall -W -pedantic -Wextra -Wunused -Wformat=2 -Weffc++ -Wpadded -fexceptions
LDLIBS  = -lpthread -lboost_iostreams
OBJ     = functions.o read.o stats.o main.o
NAME    = zipcracker-ng

all: $(NAME)

clean:
	rm -f *.o

mrproper: clean
	rm -f cptcracker-ng

$(NAME): $(OBJ)
	$(CXX) $(CFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.cc
	$(CXX) $(CFLAGS) -c $*.cc

functions.o: ../shared/functions.cc ../shared/functions.h
	$(CXX) $(CFLAGS) -c ../shared/$*.cc -DZIP

stats.o: ../shared/stats.cc ../shared/stats.h
	$(CXX) $(CFLAGS) -c ../shared/$*.cc
