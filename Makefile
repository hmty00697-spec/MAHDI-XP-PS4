TOOLCHAIN := $(OO_PS4_TOOLCHAIN)

CC := clang
CXX := clang++
LD := ld.lld

TARGET := eboot.bin
BUILD := build

SOURCES := graphics/main.cpp _common/graphics.cpp
OBJECTS := $(BUILD)/main.o $(BUILD)/graphics.o

CFLAGS := --target=x86_64-pc-freebsd12-elf \
          -fPIC -funwind-tables -c \
          -isysroot $(TOOLCHAIN) \
          -isystem $(TOOLCHAIN)/include

CXXFLAGS := $(CFLAGS) \
            -isystem $(TOOLCHAIN)/include/c++/v1 \
            -I_common

LDFLAGS := -m elf_x86_64 -pie \
           --script $(TOOLCHAIN)/link.x \
           --eh-frame-hdr \
           -L$(TOOLCHAIN)/lib \
           -lc -lkernel -lc++ \
           $(TOOLCHAIN)/lib/crt1.o

all: $(TARGET)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/main.o: graphics/main.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -o $@ $<

$(BUILD)/graphics.o: _common/graphics.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -o $@ $<

$(TARGET): $(OBJECTS)
	$(LD) $(OBJECTS) -o $(BUILD)/game.elf $(LDFLAGS)
	$(TOOLCHAIN)/bin/linux/create-fself \
	-in=$(BUILD)/game.elf \
	-out=$(BUILD)/game.oelf \
	--eboot "$(TARGET)" \
	--paid 0x3800000000000011

clean:
	rm -rf $(BUILD) $(TARGET)

.PHONY: all clean
