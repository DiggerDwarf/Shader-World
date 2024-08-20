EXE 		= Shader_world

SRC 		= $(wildcard src/*.cpp)
OBJ 		= $(subst src, build, $(patsubst %.cpp, %.o, $(SRC))) build/image_bin.o

DBG 		= 

INCLUDE 	= -Iinclude
LIB 		= -L lib -lsfml-graphics -lsfml-window -lsfml-system -lopengl32
# LIB 		= -L lib -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lopengl32 -lfreetype -lwinmm -lgdi32
EXTRA		= -lcomdlg32
STATIC  	= -static-libgcc -static-libstdc++

all: image link

remake: clean all

image:
	objcopy -I binary -O elf64-x86-64 resources/earth.jpg build/image_bin.o

run:
	$(EXE)

clean:
	erase $(subst build/, build\, $(OBJ))

mkbuild:
	mkdir build

build/%.o: src/%.cpp
	g++ $(INCLUDE) -c src/$*.cpp -o build/$*.o $(DBG)

link: $(OBJ)
	g++ $(OBJ) -o $(EXE) $(LIB) $(EXTRA) $(STATIC) $(DBG)
