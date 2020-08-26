SHELL    = /bin/bash
CXX      = g++-10
CXXFLAGS = -fPIC -Wall -Wextra -pedantic -O3 -std=c++20 -fconcepts -g -ggdb
LDFLAGS  = -shared
TARGET   = libbear.so
SOURCES  = $(shell echo core/*.cc ea/*.cc)
HEADERS  = $(shell echo core/*.h  ea/*.h)
OBJECTS  = $(SOURCES:.cc=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

.PHONY: clean
clean:
	rm -f *~ core/*~ ea/*~ core/*.o ea/*.o $(TARGET)
