CC ?= 
CFLAGS ?=
TARGET ?=

ENTRY ?=

OUTPUT = simple-games
EXTENSION ?=

BUILD = $(CC) -std=c99 $(CFLAGS) -target $(TARGET) -Iinclude
EO = build/$(TARGET)/$(ENTRY).o
OO = build/$(TARGET)/$(OUTPUT).o

build/$(TARGET)/$(OUTPUT)$(EXTENSION): $(EO) $(OO)
	mkdir -p build/$(TARGET)
	$(BUILD) $^ -o $@

$(EO): entry/$(ENTRY).c
	mkdir -p build/$(TARGET)
	$(BUILD) -c $^ -o $@

$(OO): src/*.c
	mkdir -p build/$(TARGET)
	$(BUILD) -c $^ -o $@

.PHONY: clean
clean:
	rm -rf build
