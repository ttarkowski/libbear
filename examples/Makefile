SHELL      = /bin/bash
CXX        = g++-10
CXXFLAGS   = -Wall -Wextra -pedantic -O3 -std=c++20 -fconcepts -g -ggdb -I../../../
LDFLAGS    = -L../../ -lbear
TARGET     = example
SOURCES    = $(shell echo *.cc)
OBJECTS    = $(SOURCES:.cc=.o)
DEPENDENCY = $(OBJECTS:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

-include $(DEPENDENCY)

%.o : %.cc
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

run:
	LD_LIBRARY_PATH=../../:$LD_LIBRARY_PATH ./$(TARGET)

.PHONY: clean
clean:
	rm -f *~ */*~ $(TARGET) $(OBJECTS) $(DEPENDENCY)
