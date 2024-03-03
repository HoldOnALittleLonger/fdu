all: fdu
vpath %.h inc/
vpath %.cc src/

CXXFLAGS := -Wall -g -std=c++20 -Iinc/

.SUFFIXES: .o
%.o: %.c %.h
	g++ $(CXXFLAGS) -o $@ -c $<

fdu: fdu.o fdu_server.o fdu_client.o f_server.o f_client.o
	g++ $(CXXFLAGS) -o $@ $^ -lpthread
	mv $@ bin/

fdu.o: fdu.cc fdu_cs.h fdu_options.h
fdu_server.o: fdu_server.cc fdu_cs.h f_server.h
fdu_client.o: fdu_client.cc fdu_cs.h f_client.h
f_server.o: f_server.cc f_server.h misc.h
f_client.o: f_client.cc f_client.h misc.h

.PHONY: clean
clean:
	@rm *.o
