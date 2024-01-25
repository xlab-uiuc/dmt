# Makefile for parallel simulated annealer

PREFIX=${PARSECDIR}/pkgs/kernels/canneal/inst/${PARSECPLAT}

TARGET=canneal
LIBS:=$(LIBS) -lm


# CXXFLAGS=-msse3 -O3 -march=native
CXXFLAGS=-msse3 -O3 -g -Wall -mno-avx

ifdef version
  ifeq "$(version)" "pthreads"
    CXXFLAGS+=-DENABLE_THREADS -pthread
  endif
endif

all:
	$(CXX) $(CXXFLAGS) annealer_thread.cpp -c -o annealer_thread.o
	$(CXX) $(CXXFLAGS) rng.cpp -c -o rng.o
	$(CXX) $(CXXFLAGS) netlist.cpp -c -o netlist.o
	$(CXX) $(CXXFLAGS) main.cpp -c -o main.o
	$(CXX) $(CXXFLAGS) netlist_elem.cpp -c -o netlist_elem.o

	$(CXX) $(CXXFLAGS) -DENABLE_THREADS -pthread annealer_thread.cpp -c -o annealer_thread.o-mt
	$(CXX) $(CXXFLAGS) -DENABLE_THREADS -pthread rng.cpp -c -o rng.o-mt
	$(CXX) $(CXXFLAGS) -DENABLE_THREADS -pthread netlist.cpp -c -o netlist.o-mt
	$(CXX) $(CXXFLAGS) -DENABLE_THREADS -pthread main.cpp -c -o main.o-mt
	$(CXX) $(CXXFLAGS) -DENABLE_THREADS -pthread netlist_elem.cpp -c -o netlist_elem.o-mt

	$(CXX) $(CXXFLAGS) $(LDFLAGS) *.o $(LIBS) -o $(TARGET)
	
	ar rcs libcanneal.a *.o
	ar rcs libcannealmt.a *.o-mt

clean:
	rm -f *.o $(TARGET)
	rm -f *.a
	rm -f *.o-mt

install:
	mkdir -p $(PREFIX)/bin
	cp -f $(TARGET) $(PREFIX)/bin/$(TARGET)

