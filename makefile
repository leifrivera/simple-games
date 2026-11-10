CC ?= 
CFLAGS ?=
LDFLAGS ?=
TARGET ?=

ENTRY ?=

OUTPUT = simple-games
EXTENSION ?=

BUILD = $(CC) -std=c99 $(CFLAGS) -target $(TARGET) -Iinclude
SOURCES = entry/$(ENTRY).c $(wildcard src/*.c)

build/$(TARGET)/$(OUTPUT)$(EXTENSION): $(SOURCES)
	mkdir -p build/$(TARGET)
	$(BUILD) $(LDFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -rf build
