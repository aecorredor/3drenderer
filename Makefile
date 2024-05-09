build:
	gcc -Wall -std=c99 ./src/*.c -arch x86_64 -lSDL2 -o renderer

run:
	./renderer

clean:
	rm renderer
