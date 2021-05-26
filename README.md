# Tic-tac-toe Game

## Introducere
Proiectul constă într-un joc simplu de X și 0, realizat pe Arduino UNO,
folosind ca display un LCD SPI de 1.8'', iar interacțiunea cu jocul se
va permite printr-un joystick.

Rolul acestui proiect este de familiarizare cu Arduino prin implementarea
unui joc simplu și cunoscut tuturor.

## Descriere generală
Jucătorul va vedea pe ecran un careu și semnul X cu care se va putea deplasa între pătrățelele
careului cu ajutorul mișcărilor realizate prin joystick. Odată ce se decide
unde vrea să își plaseze semnul (X), el va confirma alegerea prin
apăsarea joystick-ului. Dacă alegerea este pe o poziție goală, buzzerul va emite un sunet simplu, 
dar dacă încearcă să pună pe o poziție deja ocupată, buzzerul va scoate un sunet mai intens, iar jucătorul va
trebui să facă altă mutare. Acesta va avea ca adversar "computerul"(O), care folosește un algoritm Minimax pentru a decide următoarea mutare a sa.

## Software Design
Descrierea codului aplicației:
  * mediu de dezvoltare: Arduino IDE
  * librării şi surse 3rd-party (pentru LCD): 
    - Adafruit GFX
    - Adafruit ST7735
  * algoritmi și structuri de date:
    - logică simplă (if, while, for, etc.)
    - structuri de bază C: variabile, constante, etc.
    - algoritm Minimax pentru mutările calculatorului(O)
  * surse și funcții implementate:
    - funcția **setup()** inițializează pozițiile pe axe ale liniilor careului, pozițiile pentru X și O în fiecare careu, diferite variabile care marchează statusul jocului și poziția curentă a jucătorului, pinii pentru joystick, buzzer, de asemenea desenează careul inițial și poziția inițială la care se afla jucătorul X. Mai mult, buzzerul va produce un zgomot care marchează începutul jocului. Este inițializată și o matrice 3x3 pentru a ști ce poziții sunt ocupate și de către cine.
    - funcția **loop()** afișează de fiecare dată noile informații și citește pinii pentru axele X și Y de la joystick și mută X-ul conform datelor primite de la joystick. Aici, citind pinul SW de la joystick, se confirmă mutarea dacă este validă și se dă prin funcția tone() semnal la buzzer pentru a scoate sunet.
    - funcțiile **drawSymbolX(currentX, currentY) drawSymbolO(currentX, currentY)** desenează simbolul X/O la poziția din matrice primită ca parametrii la funcție.
    - funcția **drawBackground()** o folosesc deseori de-a lungul programului pentru a redesena careul și simbolurile X/O deja prezente
    - funcția **isMovesLeft(matrix[3][3])** verifică dacă mai sunt locuri libere in matrice pentru a ști dacă s-a finalizat jocul.
    - funcția **evaluate(matrix[3][3])** este folosită în algoritmul Minimax pentru a verifica victoria unuia dintre jucători.
    - funcția **minimax(matrix[3][3], depth, isMax)** implementează efectiv algoritmul, folosindu-se și de funcția findBestMove.
    - funcția **checkgGameStatus()** o folosesc în funcția loop() pentru a verifica dacă a câștigat cineva, dacă e remiză sau încă nu s-a terminat jocul.

