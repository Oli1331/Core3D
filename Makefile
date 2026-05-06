all: Core3D

Core3D: ./src/main.c
	  gcc ./src/main.c -lSDL2 -lm -O2 -o Core3D