#include <GyverMAX7219.h>
#include <Thread.h>
//MAX7219 < W, H, CS > mtrx;    // подключение к аппаратному SPI
// пример: UNO / Nano (CLK - D13, DI - D11, CS - любой пин)

//MAX7219 < W, H, CS, DATA, CLK > mtrx; // подключение к любым пинам
// W и H - количество МАТРИЦ по горизонтали и вертикали
// CS, DATA, CLK - номера пинов

MAX7219 <1, 2, 12, 14, 13> mtrx;   // одна матрица (1х1), пин CS на D5
bool even = false;
// PL pin 1
int load = 33;
// CE pin 15
int clockEnablePin = 25;
// Q7 pin 7
int dataIn = 26;
// CP pin 2
int clockIn = 27;

int DELAY_TIME = 250;

int* xd = new int[16];

int x = -1;
int y = -1;
int newX = -1;
int newY = -1;

// Define the number of bits and create an array to store button states
const int numBits = 16; // Change this to match your specific number of bits
int buttonStates[numBits] = {1}; // Initialize all button states as not pressed (0)
int lightningButtonStates[numBits] = {1};

const int dataPin = 26;   /* Q7 */
const int clockPin = 27;  /* CP */
const int latchPin = 33;  /* PL */

Thread ledThread = Thread(); // создаём поток управления светодиодом
Thread shiftRegistersThread = Thread(); // создаём поток управления сиреной

void matrixTileLightningGesture();
void shiftRegisters();

void setup() {

  mtrx.begin();       // запускаем
  mtrx.setBright(15);  // яркость 0..15
    for(int i = 0; i < 32; i++) {
    xd[i] = 0;
  }
  mtrx.clear();
  //mtrx.dot(7, 7);
  mtrx.dot(0, 0);
  mtrx.dot(15, 7);
  mtrx.dot(7, 15);
  mtrx.update();
   // показать
  //mtrx.setRotation(1);   // можно повернуть 0..3, по 90 град по часовой стрелке

  Serial.begin(9600);
 
  // Setup 74HC165 connections
  pinMode(dataPin, INPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);

  //ledThread.onRun(matrixTileLightningGesture);
  //ledThread.setInterval(0);

  //shiftRegistersThread.onRun(shiftRegisters);
  //shiftRegistersThread.setInterval(5);

}

void matrixTileLightningGesture() {
  if ((x != newX || y != newY) && (newX != -1 && newY != -1)){
    x = newX;
    y = newY;
  }
  if(x != - 1 && y != -1){
    mtrx.clear();
    mtrx.dot(x, y);
    mtrx.update();
    delay(DELAY_TIME);

    mtrx.clear();
    mtrx.dot(x, y + 1);
    mtrx.update();
    delay(DELAY_TIME);

    mtrx.clear();
    mtrx.dot(x + 1, y + 1);
    mtrx.update();
    delay(DELAY_TIME);

    mtrx.clear();
    mtrx.dot(x + 1, y);
    mtrx.update();
    delay(DELAY_TIME);
  }
}

int getNumOfGerkon(int num) {
  switch (num) {
    case 7: 
      newX = 6;
      newY = 6;
      return 0;
    
    case 6:
      newX = 4;
      newY = 6;
      return 1;

    case 4:
      newX = 0;
      newY = 6;
      return 2;

    case 5:
      newX = 2;
      newY = 6;
      return 3;

    case 3:
      newX = 6;
      newY = 4;
      return 4;

    case 2:
      newX = 4;
      newY = 4;
      return 5;

    case 1:
      newX = 2;
      newY = 4;
      return 6;

    case 0:
      newX = 0;
      newY = 4;
      return 7;

    case 15:
      newX = 6;
      newY = 2;
      return 8;

    case 14:
      newX = 4;
      newY = 2;
      return 9;

    case 12:
      newX = 0;
      newY = 2;
      return 10;

    case 13:
      newX = 2;
      newY = 2;
      return 11;

    case 11:
      newX = 6;
      newY = 0;
      return 12;

    case 10:
      newX = 4;
      newY = 0;
      return 13;

    case 9:
      newX = 2;
      newY = 0;
      return 14;

    case 8:
      newX = 0;
      newY = 0;
      return 15;
  }
}

void shiftRegisters() {
  digitalWrite(latchPin, LOW);
  digitalWrite(latchPin, HIGH);
  
  bool dataChanged = false; // Flag to indicate if there are any changes in data
  
  for (int i = 0; i < numBits; i++) {
    int bit = digitalRead(dataPin);
    if (bit == HIGH) {
      Serial.print("1");
    } else {
      Serial.print("0");
    }
    
    if (buttonStates[i] != bit) {
      dataChanged = true;
      buttonStates[i] = bit;
      int gerkonNumber = getNumOfGerkon(i);
    }
    
    digitalWrite(clockPin, HIGH);
    digitalWrite(clockPin, LOW);
  }

  Serial.println();
  if (dataChanged) {
    Serial.println("Data changed!");
  }
}


void loop() {

  if (ledThread.shouldRun())
        ledThread.run(); // запускаем поток
    
    if (shiftRegistersThread.shouldRun())
        shiftRegistersThread.run(); // запускаем поток
}




void matrixMemesBlocks() {
    mtrx.clear();
    if(even) {
      for(int i = 0; i < 8; i += 4) {
        for(int j = 0; j < 8; j += 4) {
          mtrx.rect(i, j, i + 1, j + 1);
        }
      }
    for(int i = 2; i < 8; i += 4) {
      for(int j = 2; j < 8; j += 4) {
        mtrx.rect(i, j, i + 1, j + 1);
      }
    }
      mtrx.update();     // пиксель на координатах 0,0
    }
    else {
      for(int i = 2; i < 8; i += 4) {
        for(int j = 0; j < 8; j += 4) {
          mtrx.rect(i, j, i + 1, j + 1);
        }
      }
      for(int i = 0; i < 8; i += 4) {
        for(int j = 2; j < 8; j += 4) {
          mtrx.rect(i, j, i + 1, j + 1);
        }
      }
          mtrx.update();     // пиксель на координатах 0,0

    }
    // mtrx.update();  
    delay(1000);   // показать
    mtrx.clear();
    mtrx.update();
    even = !even;
    delay(1000); 


    delay(1000);
}

