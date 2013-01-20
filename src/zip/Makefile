
CXX     = g++
#CFLAGS  = -O0 -g
CFLAGS  = -march=native -O3 -mmmx -msse -msse2 -msse3 -m3dnow -ffast-math -mno-ieee-fp -s
CFLAGS += -Wfatal-errors -Werror -Wall -W -pedantic -Wextra -Wunused -Wformat=2 -Weffc++ -Wpadded -fexceptions
LDLIBS  = -lboost_iostreams -lpthread
#LDLIBS += -lcurses
OBJ     = functions.o stats.o gui.o read.o puff.o main.o
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

gui.o: ../shared/gui.cc ../shared/gui.h
	$(CXX) $(CFLAGS) -c ../shared/$*.cc

functions.o: ../shared/functions.cc ../shared/functions.h
	$(CXX) $(CFLAGS) -c ../shared/$*.cc -DZIP

stats.o: ../shared/stats.cc ../shared/stats.h
	$(CXX) $(CFLAGS) -c ../shared/$*.cc
