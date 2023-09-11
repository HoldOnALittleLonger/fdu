all: fdu
vpath %.h inc/
vpath %.cc src/

CXXFLAGS := -Wall -std=c++20 -Iinc/

fdu: fdu.o fdu_server.o fdu_client.o f_server.o f_client.o
	g++ $(CXXFLAGS) -o $@ $^
	mv $@ bin/

fdu.o: fdu.cc fdu_cs.h
	g++ $(CXXFLAGS) -o $@ -c $<

fdu_server.o: fdu_server.cc fdu_cs.h f_server.h
	g++ $(CXXFLAGS) -o $@ -c $<

fdu_client.o: fdu_client.cc fdu_cs.h f_client.h
	g++ $(CXXFLAGS) -o $@ -c $<

f_server.o: f_server.cc f_server.h misc.h
	g++ $(CXXFLAGS) -o $@ -c $<

f_client.o: f_client.cc f_client.h misc .h
	g++ $(CXXFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	@rm *.o
