# 🧟 Zombie Maze

Un joc de supravietuire in labirint generat procedural, scris in **C pur** cu **Raylib**.

---

## 🎮 Cum se joaca

| Tasta | Actiune |
|-------|---------|
| `WASD` sau Sageti | Misca personajul |
| `ENTER` | Incepe jocul (din meniu) |
| `R` | Restart dupa win/lose |
| `ESC` | Iesi din joc |

**Obiectiv:** Aduna toate cele **3 chei** (`K`) din labirint si ajunge la **EXIT** (patratul verde) inainte sa te prinda zombie-ii.

- Ai **3 puncte de viata** — un zombie te love o data la 1.5 secunde
- Fiecare partida genereaza un **labirint diferit** (seed random)

---

## 🏗️ Structura proiectului

```
ZombiesMaze/
├── Makefile              Compilare + link cu Raylib
├── core.h                Constante globale, Vec2i, GameState
├── main.c                Game loop: init → update → draw
│
├── maze/
│   ├── maze.h            Structura Maze (grid alocat cu malloc)
│   └── maze.c            Generare Recursive Backtracking (DFS)
│
├── world/
│   ├── player.h / .c     Input tastatura, miscare, colectare chei
│   ├── zombie.h / .c     Timer de miscare, apeleaza BFS
│
├── brain/
│   ├── queue.h / .c      Coada dinamica (linked-list, malloc/free)
│   ├── pathfinder.h / .c BFS – calculeaza urmatorul pas al zombie-ului
│
└── gfx/
    ├── renderer.h / .c   Desenare Raylib (maze, entitati, HUD, overlay)
```

---

## 🔧 Instalare Raylib

### Linux (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install libraylib-dev
```

### macOS (Homebrew)
```bash
brew install raylib
```

### Windows (MinGW)
Descarca binarul de pe [raylib.com/download](https://www.raylib.com/) si pune `raylib.dll` langa executabil.

---

## 🚀 Compilare si rulare

```bash
# Compileaza
make

# Ruleaza
make run

# Sau direct
./zombie_maze

# Verifica memory leaks (Linux, cu Valgrind)
make valgrind
```

---

## 🧠 Concepte tehnice implementate

### 1. Generarea labirintului – Recursive Backtracking
- Grila initiala: tot perete (`0`)
- Pornim din `(1,1)` si "sapam" coridoare la fiecare 2 celule
- Directiile sunt amestecate random (Fisher-Yates shuffle)
- Rezultat: labirint perfect, unic, diferit la fiecare pornire

### 2. AI Zombie – BFS (Breadth-First Search)
- La fiecare `ZOMBIE_SPEED` secunde, fiecare zombie ruleaza BFS
- BFS exploreaza celulele layer-by-layer pana il gaseste pe jucator
- Stocheaza array-ul `parent[]` pentru reconstructia drumului
- Returneaza **primul pas** pe drumul cel mai scurt spre jucator

### 3. Coada dinamica (linked-list)
- Folosita intern de BFS pentru explorarea celulelor
- Operatii `enqueue`/`dequeue` in O(1)
- Memoria fiecarui nod e alocata cu `malloc` si eliberata cu `free`

### 4. Alocare dinamica
- Grila labirintului: `int **grid` cu `malloc/free` per rand
- Coada BFS: noduri `QNode*` alocate individual
- Nicio scurgere de memorie (verificabil cu Valgrind)

---

## 🎨 Cum arata jocul

```
██████████████████████████████████████████
█  @  ·  ·  ·  K  ·  ·  ·  ·  ·  ·  ·  █
█  ·  ██████  ·  ██████  ·  ██████  ·  █
█  ·  ·  ·  ·  ·  ·  ·  ·  ·  K  ·  ·  █
█  ████  ·  ████████  ·  ████  ·  ████  █
█  ·  ·  ·  ·  ·  z  ·  ·  ·  ·  ·  ·  █
█  ·  ██  ·  ████  ·  ████  ·  ██  ·   █
█  ·  ·  ·  ·  K  ·  ·  z  ·  ·  OUT  █
██████████████████████████████████████████
HP: <3 <3 <3   Chei: 0 / 3
```

- `@` = Jucator (albastru)
- `z` = Zombie (rosu, ochi verzi)
- `K` = Cheie de colectat (galben)
- `OUT` = Exit (gri = blocat, verde = deschis)
