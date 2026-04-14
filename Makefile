# ============================================================
#  Makefile  –  Zombie Maze v2
#  Compileaza cu: make
#  Ruleaza cu:    make run
#  Curata cu:     make clean
#  Leaks check:   make valgrind
# ============================================================

CC     = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2 -I.
TARGET = zombie_maze

SRC = main.c            \
      maze/maze.c       \
      world/player.c    \
      world/zombie.c    \
      brain/pathfinder.c\
      brain/queue.c     \
      gfx/renderer.c

OBJ = $(SRC:.c=.o)

UNAME := $(shell uname 2>/dev/null || echo Windows)

ifeq ($(UNAME), Darwin)
    LIBS = -lraylib \
           -framework OpenGL \
           -framework Cocoa  \
           -framework IOKit  \
           -framework CoreAudio \
           -framework CoreVideo
else ifeq ($(UNAME), Linux)
    LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
else
    LIBS   = -lraylib -lopengl32 -lgdi32 -lwinmm
    TARGET = zombie_maze.exe
endif

# ============================================================

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $^ -o $@ $(LIBS)
	@echo ""
	@echo "  >>> Build OK!  ./$(TARGET)"
	@echo ""

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET) $(TARGET).exe

run: all
	./$(TARGET)

# Valgrind cu supresii complete pentru Raylib/GLFW/X11/Mesa GPU
# Rezultat asteptat: "0 errors from 0 contexts" sau
# "ERROR SUMMARY: 0 errors" — tot ce ramane e din driverul GPU
# sistem si e supresat corect.
valgrind: all
	valgrind \
	  --leak-check=full \
	  --show-leak-kinds=definite,indirect \
	  --suppressions=raylib.supp \
	  ./$(TARGET)

.PHONY: all clean run valgrind
