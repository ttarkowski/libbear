SHELL      = /bin/bash
CXX        = g++-10
CXXFLAGS   = -pthread -fPIC -Wall -Wextra -pedantic -O3 -std=c++20 -fconcepts -g -ggdb -I..
LDFLAGS    = -shared
TARGET     = libbear.so
SOURCES    = $(shell echo core/*.cc ea/*.cc)
OBJECTS    = $(SOURCES:.cc=.o)
DEPENDENCY = $(OBJECTS:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

-include $(DEPENDENCY)

%.o : %.cc
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

.PHONY: clean
clean:
	rm -f *~ */*~ $(TARGET) $(OBJECTS) $(DEPENDENCY)
