COMPILER = clang
CFILES = src/**.c
SOURCE_LIBS = -I/opt/homebrew/include -L/opt/homebrew/lib -lSDL2
WARNINGS = -Wall
BUILD = -g -v -o "binary" -std=c99

# for debug -fsanitize=address
export ASAN_OPTIONS := allocator_may_return_null=1

build:
	$(COMPILER) $(CFILES) $(SOURCE_LIBS) $(WARNINGS) $(BUILD)
	./binary

clean:
	rm -rf binary.dSYM
	rm binary
