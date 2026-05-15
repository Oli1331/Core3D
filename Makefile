Warning_flag = -Wall -Wextra -Werror -Wpedantic
all: Core3D

Core3D: ./src/main.o ./src/easy_math.o ./src/elements.o Makefile
	  gcc ./src/main.o ./src/elements.o ./src/easy_math.o  -lSDL2 -lm -lSDL2_ttf -O2 -march=native -o Core3D

./src/elements.o: ./src/elements.c Makefile
	gcc ./src/elements.c -O3 -march=native  -c $(Warning_flag) -o ./src/elements.o

./src/easy_math.o: ./src/easy_math.c Makefile
	gcc ./src/easy_math.c -O3 -march=native  -c $(Warning_flag) -o ./src/easy_math.o

./src/main.o: ./src/main.c Makefile
	gcc ./src/main.c -O3 -march=native -c $(Warning_flag) -o ./src/main.o


