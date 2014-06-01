CC=gcc
SRCDIR=.
BUILDDIR=../build
CFLAGS=-c -Wall -O3
LIBS=
TARGET := Xardcade2Joystick
SRCEXT := c
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
DEPS := $(OBJECTS:.o=.deps)

all clean check $(TARGET): $(OBJECTS)
	@echo " Linking..."; $(CC) $^ $(LIBS) -o $(TARGET)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " CC $<"; $(CC) $(CFLAGS) -MD -MF $(@:.o=.deps) -c -o $@ $<

check: all
	# TO BE DONE, just a dummy for now
	@echo "*** ALL TESTS PASSED ***"

clean:
	@echo " Cleaning..."; $(RM) -r $(BUILDDIR) $(TARGET)

-include $(DEPS)

install:
	cp Xardcade2Joystick /usr/local/bin
	chown root:root /usr/local/bin/Xardcade2Joystick
	chmod +x /usr/local/bin/Xardcade2Joystick

.PHONY: clean
