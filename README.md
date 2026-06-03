# Chess_proiect_TP

Aplicație de șah scrisă în C folosind CSFML.

Proiectul este realizat pentru materia TP și conține în prezent modelul de bază pentru șah, regulile jocului, gestionarea unei sesiuni de joc, meniuri de configurare și interfață grafică.

## Starea curentă

Proiectul suportă în prezent:

- sistem grafic de meniuri
- ecran de configurare pentru Local PvP
- ecran de configurare pentru Engine PvE
- afișarea tablei de șah
- afișarea pieselor
- etichete pentru coordonatele tablei
- evidențierea mutărilor legale
- schimbarea perspectivei tablei
- sesiune de joc Local PvP
- ceas de joc
- istoric de mutări
- generarea comenzilor compatibile cu Stockfish
- validarea regulilor de șah

Modul Engine PvE are momentan partea de configurare și structura de sesiune, dar comunicarea efectivă cu procesul Stockfish nu este complet implementată.

## Funcționalități

### Reguli de șah

Regulile implementate includ:

- poziția tablei
- partea care este la mutare
- mutări legale
- detectarea șahului
- șah-mat
- pat
- rocada
- en passant
- promovarea pionilor
- material insuficient
- starea de final a jocului

Promovarea este momentan simplificată:

Fiecare pion promovat devine damă.
### Sesiune de joc

Layer-ul game_session gestionează starea unei sesiuni de joc.

Acesta conține:

- jocul curent
- ceasul de joc curent
- modul de joc
- culoarea jucătorului uman
- istoricul mutărilor
- detectarea repetiției de trei ori
- rezultatul sesiunii
- generarea comenzii Stockfish de forma position startpos moves ...

### Controlul timpului

Proiectul are o structură separată pentru ceasul de joc: GameClock.

Aceasta suportă:

- timp inițial
- increment
- ceas activ pentru jucătorul aflat la mutare
- pauză și reluare
- detectarea expirării timpului
- formatarea timpului pentru afișare

Configurarea Local PvP folosește:

timp inițial în minute
increment în secunde
### Interfață grafică

Frontend-ul conține:

- meniu principal
- meniu de setări
- meniu de controale
- meniu de credite
- meniu de configurare joc
- meniu de configurare Local PvP
- meniu de configurare Engine PvE
- ecran de joc Local PvP
- ecran de joc Engine PvE
- afișarea tablei de șah
- elemente UI custom

Afișarea tablei gestionează:

- desenarea tablei
- desenarea pieselor
- evidențierea mutărilor legale
- păstrarea pătratului selectat
- perspectiva tablei
- etichetele coordonatelor

## Module principale

### chess

Logică pură de șah.

Acest layer nu depinde de SFML.

Responsabilități:

- reprezentarea pieselor
- reprezentarea tablei
- generarea mutărilor legale
- executarea mutărilor
- șah, șah-mat și pat
- drepturi de rocadă
- en passant
- promovare
- material insuficient
- chei de poziție pentru detectarea repetiției

Fișiere importante:

include/chess/piece.h
include/chess/board.h
include/chess/move.h
include/chess/game.h
### `game_session`

Logică de sesiune/meci.

Responsabilități:
- deține jocul curent
- deține ceasul curent
- păstrează istoricul mutărilor
- detectează repetiția de trei ori
- păstrează rezultatul sesiunii
- oferă perspectiva tablei
- generează comenzi de poziție pentru Stockfish

Fișiere importante:

include/game_session/game_session.h
include/game_session/game_clock.h
### `frontend

Interfață grafică.

Responsabilități:

- meniuri
- ecrane de configurare
- ecrane de joc
- afișarea tablei
- elemente UI
- schimbarea ecranelor

Fișiere importante:

``
include/frontend/screen_manager.h
include/frontend/game/chess_board_view.h
include/frontend/screens/
include/frontend/ui/

### `resources`

Încărcare centralizată a resurselor.

Încarcă:

- uri
- iconițe
- fonturi

Fișiere importante:


include/resources/resources.h
src/resources/resources.c

### `input`

Abstractizare pentru input.

Momentan conține:


Mouse

Fișiere importante:


include/input/mouse.h
src/input/mouse.c

## Dependențe

Proiectul folosește:

- C
- GCC
- CSFML
- Make

Biblioteci CSFML necesare:


csfml-graphics
csfml-window
csfml-system

Makefile-ul face link cu:

make
-lcsfml-graphics -lcsfml-system -lcsfml-window -lm

## Compilare

Din folderul principal al proiectului:

bash
make

## Rulare

bash
make run

sau:

bash
./Chess

## Curățarea fișierelor generate

bash
make clean

## Recompilare completă

bash
make rebuild

## Output

Executabilul generat este:


Chess

Fișierele obiect și fișierele de dependențe sunt generate în:


build/

## Controale

Controale de bază:


Click stânga mouse  - interacțiune cu meniuri și tabla de șah
Tastatură           - editarea câmpurilor  din configurare
Escape              - închiderea ferestrei

## Moduri de joc

### Local PvP

Implementat ca sesiune locală de joc.

Suportă:

- ecran de configurare
- timp inițial custom
- increment custom
- schimbarea perspectivei tablei
- validarea mutărilor legale
- ceas de joc
- rezultat final

### Engine PvE

Structura de configurare și sesiune există.

Momentan suportă:

- alegerea culorii jucătorului
- alegerea nivelului motorului
- crearea unei sesiuni Engine PvE
- generarea comenzilor compatibile cu Stockfish

Încă lipsește:

- pornirea procesului Stockfish
- comunicarea UCI
- primirea și aplicarea mutărilor motorului

## Starea integrării Stockfish

Proiectul poate genera o comandă UCI de poziție din istoricul mutărilor:


position startpos moves e2e4 e7e5 g1f3

Această parte este pregătită pentru comunicarea ulterioară cu Stockfish.

Comunicarea efectivă cu motorul nu este completă încă.

## Resurse

Resursele sunt stocate în:


media/

Categorii de resurse folosite:


media/fonts/
media/icons/
media/ures/

Folderul `media/Extras/` conține resurse sursă/de referință și nu este necesar pentru încărcarea resurselor la rulare.

## Note de proiectare

Proiectul este împărțit intenționat pe layere:


chess        -> reguli pure de șah
game_session -> stare de sesiune/meci
frontend     -> interfață grafică
resources    -> încărcare resurse
input        -> abstractizare input

Datele reale ale tablei sunt independente de perspectiva vizuală.

Tabla folosește întotdeauna coordonate reale de șah:


a1 = file 0, rank 0
h8 = file 7, rank 7
`

Layer-ul vizual convertește între pătratele reale ale tablei și pătratele afișate, în funcție de perspectivă.

## Limitări curente

Nu sunt complet implementate încă:

- comunicarea efectivă cu procesul Stockfish
- PvP prin rețea
- export PGN
- undo
- ofertă/remiză acceptată de ambii jucători
- comportamentul butonului de resign
- comportamentul setărilor de sunet
- comportamentul setărilor fullscreen

Unele sisteme legate de reguli sunt implementate într-o formă de bază și au nevoie de testare suplimentară înainte de predarea finală.

## Note de dezvoltare

Stilul codului este intenționat compact:
- headerele folosesc prototipuri fără nume de parametri
- corpurile if, for, while folosesc acolade
- comentariile sunt scurte și pe secțiuni
- împărțirea excesivă a liniilor este evitată
- logica jocului este separată de randarea SFML

## Note despre licențe

Proiectul folosește SFML/CSFML.