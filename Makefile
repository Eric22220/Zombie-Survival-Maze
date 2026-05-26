# ============================================================
#  Makefile  –  Zombie Maze (cu sistem de nivele 1-10)
#  Compileaza cu: make
#  Ruleaza cu:    make run
#  Curata cu:     make clean
# ============================================================

CC     = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2 -I.
TARGET = zombie_maze

# --- Fisierele sursa ---
SRC = main.c            \
      save.c            \
      maze/maze.c       \
      world/player.c    \
      world/zombie.c    \
      world/level.c     \
      world/powerup.c   \
      brain/pathfinder.c\
      brain/queue.c     \
      gfx/renderer.c

OBJ = $(SRC:.c=.o)

# --- Detectie platforma ---
UNAME := $(shell uname 2>/dev/null || echo Windows)

ifeq ($(UNAME), Darwin)
    # macOS: Raylib instalat via Homebrew
    LIBS = -lraylib \
           -framework OpenGL \
           -framework Cocoa  \
           -framework IOKit  \
           -framework CoreAudio \
           -framework CoreVideo
else ifeq ($(UNAME), Linux)
    # Linux: sudo apt install libraylib-dev
    LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
else
    # Windows (MinGW): raylib.dll in acelasi folder
    LIBS   = -lraylib -lopengl32 -lgdi32 -lwinmm
    TARGET = zombie_maze.exe
endif

# ============================================================

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $^ -o $@ $(LIBS)
	@echo ""
	@echo "  >>> Build OK! Ruleaza cu: ./$(TARGET)"
	@echo ""

# Regula generica de compilare a unui .c in .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET) $(TARGET).exe

# Sterge si save-ul de progres
clean-save:
	rm -f save.dat

# Curata tot
clean-all: clean clean-save

run: all
	./$(TARGET)

# Verifica memory leaks (necesita Valgrind instalat pe Linux)
valgrind: all
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

.PHONY: all clean clean-save clean-all run valgrind
