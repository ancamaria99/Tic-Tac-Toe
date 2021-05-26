/**************************************************************************
  This is a library for several Adafruit displays based on ST77* drivers.

  Works with the Adafruit 1.8" TFT Breakout w/SD card
    ----> http://www.adafruit.com/products/358
  The 1.8" TFT shield
    ----> https://www.adafruit.com/product/802
  The 1.44" TFT breakout
    ----> https://www.adafruit.com/product/2088
  The 1.14" TFT breakout
  ----> https://www.adafruit.com/product/4383
  The 1.3" TFT breakout
  ----> https://www.adafruit.com/product/4313
  The 1.54" TFT breakout
    ----> https://www.adafruit.com/product/3787
  The 2.0" TFT breakout
    ----> https://www.adafruit.com/product/4311
  as well as Adafruit raw 1.8" TFT display
    ----> http://www.adafruit.com/products/618

  Check out the links above for our tutorials and wiring diagrams.
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional).

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 **************************************************************************/

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

#if defined(ARDUINO_FEATHER_ESP32) // Feather Huzzah32
  #define TFT_CS         14
  #define TFT_RST        15
  #define TFT_DC         32

#elif defined(ESP8266)
  #define TFT_CS         4
  #define TFT_RST        16                                            
  #define TFT_DC         5

#else
  // For the breakout board, you can use any 2 or 3 pins.
  // These pins will also work for the 1.8" TFT shield.
  #define TFT_CS        10
  #define TFT_RST        9 // Or set to -1 and connect to Arduino RESET pin
  #define TFT_DC         8
#endif

// OPTION 1 (recommended) is to use the HARDWARE SPI pins, which are unique
// to each board and not reassignable. For Arduino Uno: MOSI = pin 11 and
// SCLK = pin 13. This is the fastest mode of operation and is required if
// using the breakout board's microSD card.

// For 1.44" and 1.8" TFT with ST7735 use:
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// For 1.14", 1.3", 1.54", and 2.0" TFT with ST7789:
//Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);


// OPTION 2 lets you interface the display using ANY TWO or THREE PINS,
// tradeoff being that performance is not as fast as hardware SPI above.
//#define TFT_MOSI 11  // Data out
//#define TFT_SCLK 13  // Clock out

// For ST7735-based displays, we will use this call
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

// OR for the ST7789-based displays, we will use this call
//Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

//pini joystick
#define CIRCLE 1
#define CROSS  2

const int DIM_BOARD = 3;
const int SW_pin = 2;
const int X_pin = 0;
const int Y_pin = 1;
const int buzzer = 5;

int board[DIM_BOARD][DIM_BOARD];

//pozitie joystick curent
//se incepe din centru
int current_X = 1, current_Y = 1;
int game_won = 0;
int game_lost = 0;
int game_tie = 0;

struct Move
{
  int row, col;
};

int player = CIRCLE, opponent = CROSS;

//pozitii pentru cursorul printarii simbolului X in careu
int symbol_X_axis_Y1 = 7, symbol_X_axis_Y2, symbol_X_axis_Y3;
int symbol_X_axis_X1 = 7, symbol_X_axis_X2, symbol_X_axis_X3;
int symbol_O_axis_X1, symbol_O_axis_X2, symbol_O_axis_X3;
int symbol_O_axis_Y1, symbol_O_axis_Y2, symbol_O_axis_Y3;
int middleXPosition = -1;
int delta_pin = -1;

int middleYPosition = -1;


void setup(void) {
  symbol_X_axis_X2 = tft.width()/ 3 + 10;
  symbol_X_axis_X3 = tft.width() / 3 * 2 + 10;
  symbol_X_axis_Y2 = tft.height() / 3 + 10;
  symbol_X_axis_Y3 = tft.height() / 3 * 2 + 10;

  symbol_O_axis_X1 = tft.width() / 3 / 2;
  symbol_O_axis_X2 = tft.width() / 2;
  symbol_O_axis_X3 = tft.width() - tft.width() / 3 / 2;
  symbol_O_axis_Y1 = tft.height() / 3 / 2;
  symbol_O_axis_Y2 = tft.height() / 2;
  symbol_O_axis_Y3 = tft.height() - tft.height() / 3 / 2;

  //Switch pin on joystick
  pinMode(SW_pin, INPUT);
  digitalWrite(SW_pin, HIGH);
  
  Serial.begin(9600);
  

  // Use this initializer if using a 1.8" TFT screen:
    pinMode(buzzer, OUTPUT);
    tone(buzzer, 1000);
    delay(1000);
    noTone(buzzer);
    delay(500);
    tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab



  //desenare careu
  
  middleXPosition = analogRead(X_pin);
  middleYPosition = analogRead(Y_pin); 
  delta_pin = 5;

  game_won = 0;
  game_lost = 0;
  game_tie = 0;

  current_X = 1;
  current_Y = 1;

  for (int i = 0; i < DIM_BOARD; i++) {
    for (int j = 0; j < DIM_BOARD; j++) {
      board[i][j] = 0;
    }
  }
  drawBackground();
  drawSymbolX(current_X, current_Y);

}

void loop() {
start:
    if (game_won == 0 && game_lost == 0 && game_tie == 0) {
    if (analogRead(X_pin) < middleXPosition - delta_pin && current_X > 0) {
      current_X--;
      drawBackground();
      drawSymbolX(current_X, current_Y); 
      delay(400);
    }

    else if (analogRead(X_pin) > middleXPosition + delta_pin && current_X < 2) {
      current_X++;
      drawBackground();
      drawSymbolX(current_X, current_Y);
      delay(400);
    }

    if (analogRead(Y_pin) < middleYPosition - delta_pin && current_Y > 0) {
      current_Y--;
      drawBackground();
      drawSymbolX(current_X, current_Y);
      delay(400);
    }

    else if (analogRead(Y_pin) > middleYPosition + delta_pin && current_Y < 2) {
      current_Y++;
      drawBackground();
      drawSymbolX(current_X, current_Y);
      delay(400);
    }
        
    if (digitalRead(SW_pin) == 0) {
      if (board[current_X][current_Y] != 0) {
        tone(buzzer, 1000);
        delay(300);
        noTone(buzzer);
        goto start;
      }
      tone(buzzer, 600);
      delay(100);
      noTone(buzzer);
      board[current_X][current_Y] = CROSS;
       checkGameStatus();
      if (game_won == 1) {
         tft.fillScreen(ST77XX_BLACK);
         tft.setCursor(0, 30);
         tft.setTextColor(ST77XX_YELLOW);
         tft.setTextSize(4);
         tft.println("YOU\nWON!");
         delay(1024);
      }
       else if (game_lost == 1) {
        tft.fillScreen(ST77XX_BLACK);
         tft.setCursor(0, 30);
         tft.setTextColor(ST77XX_YELLOW);
         tft.setTextSize(4);
         tft.println("GAME OVER!");
      }
      else if (game_tie == 1) {
         tft.fillScreen(ST77XX_BLACK);
         tft.setCursor(0, 30);
         tft.setTextColor(ST77XX_YELLOW);
         tft.setTextSize(4);
         tft.println("IT'S A TIE!");
      }
      if (game_won == 0 && game_tie ==0 && game_lost == 0) {
 
        Move bestMove = findBestMove(board);
        Serial.print("S-a intamplat din apasare buton\n");
        drawSymbolO(bestMove.row, bestMove.col);
        delay(1000);
        noTone(buzzer);
        board[bestMove.row][bestMove.col] = CIRCLE;
      }
    }
    
    }

    checkGameStatus();
    if (game_won == 1) {
       tft.fillScreen(ST77XX_BLACK);
       tft.setCursor(0, 30);
       tft.setTextColor(ST77XX_YELLOW);
       tft.setTextSize(4);
       tft.println("YOU\nWON!");
       delay(1024);
       setup();
    }
     else if (game_lost == 1) {
      tft.fillScreen(ST77XX_BLACK);
       tft.setCursor(0, 30);
       tft.setTextColor(ST77XX_YELLOW);
       tft.setTextSize(4);
       tft.println("GAME OVER!");
       delay(1024);
       setup();
    }
    else if (game_tie == 1) {
       tft.fillScreen(ST77XX_BLACK);
       tft.setCursor(0, 30);
       tft.setTextColor(ST77XX_YELLOW);
       tft.setTextSize(4);
       tft.println("IT'S A TIE!");
       delay(1024);
       setup();
    }

}

void drawSymbolO(int current_X, int current_Y) {
  int drawX, drawY;
   if (current_X == 0){
    drawX = symbol_O_axis_X1;
   }
   if (current_X == 1) {
    drawX = symbol_O_axis_X2;
   }
   if (current_X == 2) {
    drawX = symbol_O_axis_X3;
   }

   if (current_Y == 0) {
    drawY = symbol_O_axis_Y1;
   }
   if (current_Y == 1) {
    drawY = symbol_O_axis_Y2;
   }
   if (current_Y == 2) {
    drawY = symbol_O_axis_Y3;
   }
   
   tft.drawCircle(drawX, drawY, 17, ST77XX_YELLOW);
}

void drawSymbolX(int current_X, int current_Y) {
  int drawX, drawY;
  if (current_X == 0) {
    drawX = symbol_X_axis_X1;
  }
  if (current_X == 1) {
    drawX = symbol_X_axis_X2;
  }
  if (current_X == 2) {
    drawX = symbol_X_axis_X3;
  }

  if (current_Y == 0) {
    drawY = symbol_X_axis_Y1;
  }
  if (current_Y == 1) {
    drawY = symbol_X_axis_Y2;
  }

  if (current_Y == 2) {
    drawY = symbol_X_axis_Y3;
  }

  tft.setCursor(drawX, drawY);
  tft.setTextSize(5);
  tft.setTextColor(ST77XX_RED);
  tft.print("X"); 
}

void drawBackground() {
   tft.fillScreen(ST77XX_BLACK);
   tft.drawLine(tft.width() / 3, 1, tft.width() / 3, tft.height() - 1, ST77XX_WHITE);
   tft.drawLine((tft.width() / 3) * 2, 1, (tft.width() / 3) * 2, tft.height() - 1, ST77XX_WHITE);
   tft.drawLine(1, tft.height() / 3, tft.width() - 1, tft.height() / 3, ST77XX_WHITE);
   tft.drawLine(1, (tft.height() / 3) * 2, tft.width() - 1, (tft.height() / 3) * 2, ST77XX_WHITE);

   //de adaugat simbolurile puse deja
   int i, j;
   for (i = 0; i < DIM_BOARD; i++) {
    for (j = 0; j < DIM_BOARD; j++) {
      if (board[i][j] == CROSS) {
        drawSymbolX(i, j);
      }

      if (board[i][j] == CIRCLE) {
        Serial.print("S-a intamplat din desenare de back\n");
        drawSymbolO(i, j);
      }
    }
   }
}


// This function returns true if there are moves
// remaining on the board. It returns false if
// there are no moves left to play.
int isMovesLeft(int board[3][3])
{
  for (int i = 0; i<3; i++)
    for (int j = 0; j<3; j++)
      if (board[i][j]==0)
        return 1;
  return 0;
}

int evaluate(int b[3][3])
{
  // Checking for Rows for X or O victory.
  for (int row = 0; row<3; row++)
  {
    if (b[row][0]==b[row][1] &&
      b[row][1]==b[row][2])
    {
      if (b[row][0]==player)
        return +10;
      else if (b[row][0]==opponent)
        return -10;
    }
  }

  // Checking for Columns for X or O victory.
  for (int col = 0; col<3; col++)
  {
    if (b[0][col]==b[1][col] &&
      b[1][col]==b[2][col])
    {
      if (b[0][col]==player)
        return +10;

      else if (b[0][col]==opponent)
        return -10;
    }
  }

  // Checking for Diagonals for X or O victory.
  if (b[0][0]==b[1][1] && b[1][1]==b[2][2])
  {
    if (b[0][0]==player)
      return +10;
    else if (b[0][0]==opponent)
      return -10;
  }

  if (b[0][2]==b[1][1] && b[1][1]==b[2][0])
  {
    if (b[0][2]==player)
      return +10;
    else if (b[0][2]==opponent)
      return -10;
  }

  // Else if none of them have won then return 0
  return 0;
}

int minimax(int board[3][3], int depth, int isMax)
{
  int score = evaluate(board);

  // If Maximizer has won the game return his/her
  // evaluated score
  if (score == 10)
    return score;

  // If Minimizer has won the game return his/her
  // evaluated score
  if (score == -10)
    return score;

  // If there are no more moves and no winner then
  // it is a tie
  if (isMovesLeft(board)==0)
    return 0;

  // If this maximizer's move
  if (isMax)
  {
    int best = -1000;

    // Traverse all cells
    for (int i = 0; i<3; i++)
    {
      for (int j = 0; j<3; j++)
      {
        // Check if cell is empty
        if (board[i][j]==0)
        {
          // Make the move
          board[i][j] = player;

          // Call minimax recursively and choose
          // the maximum value
          int minimax_result = minimax(board, depth+1, !isMax);
          if (best < minimax_result) {
            best = minimax_result;
          }

          // Undo the move
          board[i][j] = 0;
        }
      }
    }
    return best;
  }

  // If this minimizer's move
  else
  {
    int best = 1000;

    // Traverse all cells
    for (int i = 0; i<3; i++)
    {
      for (int j = 0; j<3; j++)
      {
        // Check if cell is empty
        if (board[i][j]==0)
        {
          // Make the move
          board[i][j] = opponent;

          // Call minimax recursively and choose
          // the minimum value
          int minimax_result = minimax(board, depth+1, !isMax);
          if (best < minimax_result) {
            best = minimax_result;
          }

          // Undo the move
          board[i][j] = 0;
        }
      }
    }
    return best;
  }
}

Move findBestMove(int board[3][3])
{
  int bestVal = -1000;
  Move bestMove;
  bestMove.row = -1;
  bestMove.col = -1;

  // Traverse all cells, evaluate minimax function for
  // all empty cells. And return the cell with optimal
  // value.
  for (int i = 0; i<3; i++)
  {
    for (int j = 0; j<3; j++)
    {
      // Check if cell is empty
      if (board[i][j]==0)
      {
        // Make the move
        board[i][j] = player;

        // compute evaluation function for this
        // move.
        int moveVal = minimax(board, 0, 0);

        // Undo the move
        board[i][j] = 0;

        // If the value of the current move is
        // more than the best value, then update
        // best/
        if (moveVal > bestVal)
        {
          bestMove.row = i;
          bestMove.col = j;
          bestVal = moveVal;
        }
      }
    }
  }

  return bestMove;
}

void checkGameStatus() {
  // Checking for Rows for X or O victory.
  for (int row = 0; row<3; row++)
  {
    if (board[row][0]==board[row][1] &&
      board[row][1]==board[row][2])
    {
      if (board[row][0]==CIRCLE){
        game_lost = 1;
        return;
      }
      else if (board[row][0]==CROSS) {
        game_won = 1;
        return;
      }
    }
  }

  // Checking for Columns for X or O victory.
  for (int col = 0; col<3; col++)
  {
    if (board[0][col]==board[1][col] &&
      board[1][col]==board[2][col])
    {
      if (board[0][col]==CIRCLE){
        game_lost = 1;
        return;
      }
      else if (board[0][col]==CROSS){
        game_won = 1;
        return;
      }
    }
  }

  // Checking for Diagonals for X or O victory.
  if (board[0][0]==board[1][1] && board[1][1]==board[2][2])
  {
    if (board[0][0]==CIRCLE) {
      game_lost = 1;
      return;
    }
    else if (board[0][0]==CROSS){
      game_won = 1;
      return;
    }
  }

  if (board[0][2]==board[1][1] && board[1][1]==board[2][0])
  {
    if (board[0][2]==CIRCLE){
      game_lost = 1;
      return;
    }
    else if (board[0][2]==CROSS){
      game_won = 1;
      return;
    }
  }

  for (int i = 0; i < DIM_BOARD; i++) {
    for (int j = 0; j < DIM_BOARD; j++) {
      if (board[i][j] == 0)
        return;
    }
  }
  game_tie = 1;
}
