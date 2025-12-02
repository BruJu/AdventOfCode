CXX = g++
CXXFLAGS = -std=c++23 -Wall -Wextra -Wpedantic -O0 -flto -fno-lto
SRCDIR = .
COMMONDIR = ../common
UTILDIR = ../util
SRCEXT = cpp
OBJDIR = build
SOURCES = $(shell find $(SRCDIR) $(COMMONDIR) $(UTILDIR) -type f -name '*.$(SRCEXT)')
OBJECTS = $(patsubst $(SRCDIR)/%.$(SRCEXT),$(OBJDIR)/%.o,$(SOURCES))
EXECUTABLE = main

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJDIR) $(EXECUTABLE)

.PHONY: all clean