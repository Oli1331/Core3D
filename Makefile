Warning_flag = -Wall -Wextra -Werror -Wpedantic
all: Core3D

Core3D: ./src/main.o ./src/easy_math.o ./src/elements.o Makefile
	  gcc ./src/main.o ./src/elements.o ./src/easy_math.o  -lSDL2 -lm -O2 -o Core3D

./src/elements.o: ./src/elements.c 
	gcc ./src/elements.c -c $(Warning_flag) -o ./src/elements.o

./src/easy_math.o: ./src/easy_math.c 
	gcc ./src/easy_math.c -c $(Warning_flag) -o ./src/easy_math.o

./src/main.o: ./src/main.c 
	gcc ./src/main.c -c $(Warning_flag) -o ./src/main.o


