CROSS_COMPILE=
CC=$(CROSS_COMPILE)gcc
SRCDIR=.
BUILDDIR=../build
CFLAGS=-c -Wall -O3
LIBS=
TARGET := xarcade2jstick
SRCEXT := c
SOURCES := $(shell find $(SRCDIR) -type f -name "*.$(SRCEXT)")
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
DEPS := $(OBJECTS:.o=.deps)

all $(TARGET): $(OBJECTS)
	@echo " Linking..."; $(CC) $^ $(LIBS) -o $(TARGET)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " CC $<"; $(CC) $(CFLAGS) -MD -MF $(@:.o=.deps) -c -o $@ $<

check:
	# TO BE DONE! JUST A DUMMY FOR NOW
	@echo "*** NO TESTS IMPLEMENTED ***"

clean:
	@echo " Cleaning..."; $(RM) -r $(BUILDDIR) $(TARGET)

-include $(DEPS)

install: all
	install -d $(DESTDIR)$(bindir)
	install -m 0755 $(TARGET) $(DESTDIR)$(bindir)
	install -d $(DESTDIR)/lib/udev/rules.d/
	install -m 0644 80-xarcade.rules $(DESTDIR)/lib/udev/rules.d/
	install -d $(DESTDIR)/lib/systemd/system/
	install -m 0644 xarcade2jstick.service $(DESTDIR)/lib/systemd/system/

uninstall:
	-rm $(DESTDIR)$(bindir)/$(TARGET)

.PHONY: clean install uninstall
