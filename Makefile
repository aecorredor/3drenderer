CPPFLAGS := -I$(shell brew --prefix)/include
LDFLAGS := -L$(shell brew --prefix)/lib

COMPILER = clang
CFILES = src/*.c
SOURCE_LIBS = $(CPPFLAGS) $(LDFLAGS) -lSDL2 -lm
WARNINGS = -Wall
BUILD = -g -v -std=c99 -o "binary"

build:
	$(COMPILER) $(CFILES) $(SOURCE_LIBS) $(WARNINGS) $(BUILD)

run:
	./binary

clean:
	rm -rf binary.dSYM
	rm binary
