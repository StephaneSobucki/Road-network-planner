CXX = g++
SRCDIR = src
OBJDIR = obj
TARGETDIR = bin
TARGET = $(TARGETDIR)/road_network_planner

SRCEXT=cpp
SOURCES=$(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS=$(patsubst $(SRCDIR)/%, $(OBJDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CXXFLAGS = -Wall -Wextra -Werror -pedantic -pedantic-errors -O3 -std=c++11

$(TARGET) : $(OBJECTS)
	@mkdir -p $(TARGETDIR)
	$(CXX) -o $(TARGET) $^

$(OBJDIR)/%.o : $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $^

clean:
	rm -r $(OBJDIR) 
	rm -r $(TARGETDIR)
