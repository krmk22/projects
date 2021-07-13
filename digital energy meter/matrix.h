#ifndef _MATRIX_H
#define _MATRIX_H

//#include "Arduino.h"

#ifndef _HEADER_H
#include "header.h"
#endif

#ifndef _LCD_H
#include "lcd.h"
#endif


#ifndef MatrixRowConfig
#define MatrixRowConfig
#define Row1 10
#define Row2 9
#define Row3 8
#define Row4 7
#endif

#ifndef MatrixColConfig
#define MatrixColConfig
#define Col1 13
#define Col2 12
#define Col3 11
//#define Col4 4
#endif

#ifndef MatrixRoutines
#define MatrixRoutines
void Matrix_Initialize();
char Matrix_FetchKey(unsigned int*);
#endif

const char *Symbols = "\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2A\x2B\x2C\x2D\x2E\x2F\x3A\x3B\x3C\x3D\x3E\x3F\x40\x5B\x5C\x5D\x5E\x5F\x60\x7B\x7C\x7D\x7E\x7F";
const char *LowerCast = "abcdefghijklmnopqrstuvwxyz";
const char *UpperCast = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char *Numbers = "0123456789";
//const char *LUT = {1, 0, 0, 3, 6, 9, 12, 15, 19, 22, 25};
//unsigned char LUT[] = {0, 0, 0, 3, 6, 9, 12, 15, 19, 22, 25};
unsigned char LUT[] = {14, 0, 0, 3, 6, 9, 12, 15, 19, 22, 26};

void Matrix_Initialize()
{
  pinMode(Col1, OUTPUT);
  pinMode(Col2, OUTPUT);
  pinMode(Col3, OUTPUT);
  //pinMode(Col4, OUTPUT);

  pinMode(Row1, INPUT);
  pinMode(Row2, INPUT);
  pinMode(Row3, INPUT);
  pinMode(Row4, INPUT);

  digitalWrite(Row1, HIGH);
  digitalWrite(Row2, HIGH);
  digitalWrite(Row3, HIGH);
  digitalWrite(Row4, HIGH);

  digitalWrite(Col1, HIGH);
  digitalWrite(Col2, HIGH);
  digitalWrite(Col3, HIGH);
  //digitalWrite(Col4, HIGH);
}

char Matrix_FetchKey(unsigned int *Var1)
{
  unsigned int Timer = 0;
  while(!digitalRead(Row1) && !digitalRead(Row2) && !digitalRead(Row3) && !digitalRead(Row4));
  digitalWrite(Col1, LOW); digitalWrite(Col2, HIGH); digitalWrite(Col3, HIGH); //digitalWrite(Col4, HIGH);
  if(!digitalRead(Row1))
  {
    Timer = millis();
    while(!digitalRead(Row1));
    *Var1 = millis() - Timer;
    return '1';
  }
  if(!digitalRead(Row2))
  {
    Timer = millis();
    while(!digitalRead(Row2));
    *Var1 = millis() - Timer;
    return '4';
  }
  if(!digitalRead(Row3))
  {
    Timer = millis();
    while(!digitalRead(Row3));
    *Var1 = millis() - Timer;
    return '7';
  }
  if(!digitalRead(Row4))
  {
    Timer = millis();
    while(!digitalRead(Row4));
    *Var1 = millis() - Timer;
    return '*';
  }

  while(!digitalRead(Row1) && !digitalRead(Row2) && !digitalRead(Row3) && !digitalRead(Row4));
  digitalWrite(Col1, HIGH); digitalWrite(Col2, LOW); digitalWrite(Col3, HIGH); //digitalWrite(Col4, HIGH);
  if(!digitalRead(Row1))
  {
    Timer = millis();
    while(!digitalRead(Row1));
    *Var1 = millis() - Timer;
    return '2';
  }
  if(!digitalRead(Row2))
  {
    Timer = millis();
    while(!digitalRead(Row2));
    *Var1 = millis() - Timer;
    return '5';
  }
  if(!digitalRead(Row3))
  {
    Timer = millis();
    while(!digitalRead(Row3));
    *Var1 = millis() - Timer;
    return '8';
  }
  if(!digitalRead(Row4))
  {
    Timer = millis();
    while(!digitalRead(Row4));
    *Var1 = millis() - Timer;
    return '0';
  }

  while(!digitalRead(Row1) && !digitalRead(Row2) && !digitalRead(Row3) && !digitalRead(Row4));
  digitalWrite(Col1, HIGH); digitalWrite(Col2, HIGH); digitalWrite(Col3, LOW); //digitalWrite(Col4, HIGH);
  if(!digitalRead(Row1))
  {
    Timer = millis();
    while(!digitalRead(Row1));
    *Var1 = millis() - Timer;
    return '3';
  }
  if(!digitalRead(Row2))
  {
    Timer = millis();
    while(!digitalRead(Row2));
    *Var1 = millis() - Timer;
    return '6';
  }
  if(!digitalRead(Row3))
  {
    Timer = millis();
    while(!digitalRead(Row3));
    *Var1 = millis() - Timer;
    return '9';
  }
  if(!digitalRead(Row4))
  {
    Timer = millis();
    while(!digitalRead(Row4));
    *Var1 = millis() - Timer;
    return '#';
  }

  /*
  while(!digitalRead(Row1) && !digitalRead(Row2) && !digitalRead(Row3) && !digitalRead(Row4));
  digitalWrite(Col1, HIGH); digitalWrite(Col2, HIGH); digitalWrite(Col3, HIGH); digitalWrite(Col4, LOW);
  if(!digitalRead(Row1))
  {
    Timer = millis();
    while(!digitalRead(Row1));
    *Var1 = millis() - Timer;
    return 'A';
  }
  if(!digitalRead(Row2))
  {
    Timer = millis();
    while(!digitalRead(Row2));
    *Var1 = millis() - Timer;
    return 'B';
  }
  if(!digitalRead(Row3))
  {
    Timer = millis();
    while(!digitalRead(Row3));
    *Var1 = millis() - Timer;
    return 'C';
  }
  if(!digitalRead(Row4))
  {
    Timer = millis();
    while(!digitalRead(Row4));
    *Var1 = millis() - Timer;
    return 'D';
  }
  */
  return '\0';
}
#endif
