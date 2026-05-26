# 🧟 Zombie Maze

Joc de supravietuire in labirint generat procedural cu **sistem de 10 nivele**, **5 power-uri** si **progres salvat persistent**. Scris in **C pur** cu **Raylib**.

---

## 🎮 Cum se joaca

| Tasta | Actiune |
|-------|---------|
| `WASD` / Sageti | Misca personajul |
| `ENTER` | Start joc / selecteaza nivel / treci la urmatorul |
| `R` | Restart nivel (dupa ce mori) |
| `ESC` | Inapoi la lobby / iesire din meniu |

**Obiectiv:** Aduna toate cheile (`K`) si ajunge la **EXIT** inainte sa te prinda zombii.

- **3 puncte de viata** — un zombi te love o data la ~1.5 secunde (invincibilitate post-hit)
- Fiecare partida genereaza un **labirint diferit** (seed = `time(NULL)`)
- **Progresul se salveaza** in `save.dat` la fiecare nivel completat

---

## 🆕 Ce e nou (sistem de nivele)

### 10 nivele cu dificultate progresiva
```
 Nv | Maze  | Zombii | Z-speed | Chei | Loops
----|-------|--------|---------|------|------
  1 | 21x21 |   2    |  0.55s  |  3   |  20
  2 | 21x21 |   3    |  0.52s  |  3   |  22
  3 | 23x23 |   3    |  0.50s  |  3   |  26
  4 | 23x23 |   4    |  0.47s  |  4   |  28
  5 | 25x25 |   4    |  0.44s  |  4   |  32
  6 | 25x25 |   5    |  0.42s  |  4   |  34
  7 | 27x27 |   5    |  0.40s  |  5   |  38
  8 | 27x27 |   6    |  0.37s  |  5   |  40
  9 | 29x29 |   7    |  0.34s  |  5   |  46
 10 | 31x31 |   8    |  0.30s  |  6   |  52
```

**Loops** = pereti sparti dupa Recursive Backtracking. Astfel labirintul nu mai e "perfect" (drum unic intre orice 2 puncte) ci are multiple drumuri si bucle, deci jucatorul are pe unde fugi de zombi. Mai multi zombi → mai multe loops.

Curba e tweaked manual ca sa fie balansata — fiecare nivel introduce ceva nou, nu doar viteza la nesfarsit.

### 5 Power-uri random pe jos (3 per nivel)

| Glifa | Nume | Efect | Durata |
|-------|------|-------|--------|
| `S` | **VITEZA** | Misca rapid cu IsKeyDown | 4s |
| `F` | **INGHET** | Zombii stau pe loc | 4s |
| `+` | **VIATA** | +1 HP (cap la 3) | instant |
| `I` | **INVINCIBIL** | Zombii nu te lovesc | 4s |
| `X` | **PURGE** | Toti zombii dispar; revin dupa expirare | 4s |

Timer-ele active se afiseaza in HUD in timp real.

### Lobby de selectie nivel

Grid 5×2 cu butoanele nivelelor:
- **DESCHIS** (albastru) — deblocat, dar nejucat
- **TERMINAT** (verde) — completat
- **BLOCAT** (gri) — inca nu il poti accesa

Navigare cu sageti, `ENTER` pentru start. Nivelele se deblocheaza progresiv.

### Salvare persistenta (`save.dat`)
- Format binar mic cu magic number `ZMZ1` pentru validare
- Salveaza automat la fiecare nivel terminat
- `make clean-save` reseteaza progresul

---

## 🏗️ Structura proiectului

```
ZombiesMaze/
├── Makefile              Compilare + link cu Raylib
├── core.h                Constante globale, Vec2i, GameState, PowerupType
├── main.c                Game loop + state machine (6 stari)
├── save.h / save.c       Persistenta progres in save.dat
│
├── maze/
│   ├── maze.h            Struct Maze (num_keys variabil)
│   └── maze.c            Generare Recursive Backtracking
│
├── world/
│   ├── player.h / .c     Input + miscare normala/SPEED + timer-e powerup
│   ├── zombie.h / .c     Viteza configurabila + suport FREEZE/KILL_ALL
│   ├── level.h / .c      Tabel cu 10 LevelConfig-uri
│   ├── powerup.h / .c    Spawn random + nume/glife pentru cele 5 tipuri
│
├── brain/
│   ├── queue.h / .c      Coada dinamica (linked-list) – nemodificat
│   └── pathfinder.h / .c BFS pentru AI zombie – nemodificat
│
└── gfx/
    ├── renderer.h        Declaratii pentru toate functiile de desen
    └── renderer.c        Render maze (cu offset), HUD, lobby, overlay
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
Descarca de pe [raylib.com](https://www.raylib.com/) si pune `raylib.dll` langa executabil.

---

## 🚀 Compilare si rulare

```bash
make            # Compileaza
make run        # Compileaza + ruleaza
make clean      # Sterge .o si executabilul
make clean-save # Reseteaza progresul (sterge save.dat)
make clean-all  # Sterge tot
make valgrind   # Verifica memory leaks (Linux + Valgrind)
```

---

## 🧠 Concepte tehnice

### Generare labirint – Recursive Backtracking
Grila incepe ca tot perete. Pornim din `(1,1)` si sapam coridoare la fiecare 2 celule, alegand directii random (Fisher-Yates shuffle). Rezultat: labirint perfect (un singur drum intre orice 2 puncte).

### AI Zombie – BFS
La fiecare `zombie_speed` secunde (configurabil per nivel), fiecare zombie ruleaza BFS din pozitia lui spre jucator si face primul pas pe drumul cel mai scurt. Complexitate: O(rows × cols) per apel.

### Coada dinamica
Linked-list cu enqueue/dequeue in O(1). Folosita intern de BFS. Toate nodurile se elibereaza la final → zero memory leaks.

### State machine
6 stari (MENU, LEVEL_SELECT, PLAYING, LEVEL_COMPLETE, LOSE, GAME_COMPLETE) cu tranzitii curate. Input-ul de gameplay si input-ul de tranzitii sunt separate.

### Alocare dinamica
- Grila labirintului: `int **grid` cu malloc/free per rand
- Coada BFS: noduri `QNode*` alocate individual
- Maze realocat la fiecare schimbare de nivel (eliberat inainte)

### Persistenta
Save in binar mic cu magic number (`ZMZ1`) pentru validare. Daca fisierul lipseste sau e corupt, se ruleaza default (nivel 1 deblocat).
