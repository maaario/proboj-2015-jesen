
CXXFLAGS = -g -MMD -MP -Wall -Wextra -std=gnu++11

all: server

server: main.o klient.o marshal.o common.o proces.o update.o util.o mapa.o
	$(LINK.cpp) $^ $(LDLIBS) -o $@

clean:
	-rm -f *.d *.o server

-include *.d

