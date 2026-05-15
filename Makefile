Warning_flag = -Wall -Wextra -Werror -Wpedantic
Optimize_flag = -O3 -march=native
Test_with_vectorize = -fopt-info-vec-optimized -DTESTING=1
Test_without_vectorize = -fno-tree-vectorize 
all: Core3D

Core3D: ./src/main.o ./src/easy_math.o ./src/elements.o Makefile
	  gcc ./src/main.o ./src/elements.o ./src/easy_math.o  -lSDL2 -lm -lSDL2_ttf $(Optimize_flag) -o Core3D

./src/elements.o: ./src/elements.c Makefile
	gcc ./src/elements.c $(Optimize_flag)  -c $(Warning_flag) -o ./src/elements.o

./src/easy_math.o: ./src/easy_math.c Makefile
	gcc ./src/easy_math.c $(Optimize_flag)  -c $(Warning_flag) -o ./src/easy_math.o

./src/main.o: ./src/main.c Makefile
	gcc ./src/main.c $(Optimize_flag)  -c $(Warning_flag) -Wno-unused-result -o ./src/main.o 



Core3D_test_with_vectorize: ./src/maintv.o ./src/easy_mathtv.o ./src/elementstv.o Makefile
	  gcc ./src/maintv.o ./src/elementstv.o ./src/easy_mathtv.o  -lSDL2 -lm -lSDL2_ttf $(Optimize_flag) $(Test_with_vectorize) -o Core3D_test_with_vectorize

./src/elementstv.o: ./src/elements.c Makefile
	gcc ./src/elements.c $(Optimize_flag) $(Test_with_vectorize) -c $(Warning_flag) -o ./src/elementstv.o

./src/easy_mathtv.o: ./src/easy_math.c Makefile
	gcc ./src/easy_math.c $(Optimize_flag) $(Test_with_vectorize)  -c $(Warning_flag) -o ./src/easy_mathtv.o

./src/maintv.o: ./src/main.c Makefile
	gcc ./src/main.c $(Optimize_flag) $(Test_with_vectorize) -c $(Warning_flag) -Wno-unused-result -o ./src/maintv.o 


Core3D_test_without_vectorize: ./src/maintnv.o ./src/easy_mathtnv.o ./src/elementstnv.o Makefile
	  gcc ./src/maintnv.o ./src/elementstnv.o ./src/easy_mathtnv.o  -lSDL2 -lm -lSDL2_ttf $(Optimize_flag) $(Test_without_vectorize) -o Core3D_test_without_vectorize

./src/elementstnv.o: ./src/elements.c Makefile
	gcc ./src/elements.c $(Optimize_flag) $(Test_without_vectorize) -c $(Warning_flag) -o ./src/elementstnv.o

./src/easy_mathtnv.o: ./src/easy_math.c Makefile
	gcc ./src/easy_math.c $(Optimize_flag) $(Test_without_vectorize)  -c $(Warning_flag) -o ./src/easy_mathtnv.o

./src/maintnv.o: ./src/main.c Makefile
	gcc ./src/main.c $(Optimize_flag) $(Test_without_vectorize) -c $(Warning_flag) -Wno-unused-result -o ./src/maintnv.o 