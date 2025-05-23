raylib:
	gcc -o build/raycaster raycaster-raylib.c -l raylib -lm

sdl3:
	gcc -o build/raycaster raycaster-sdl3.c -l SDL3