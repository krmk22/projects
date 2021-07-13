#ifndef _KEYPAD_H
#define _KEYPAD_H

#ifndef MC2KEY
#define MC2KEY FIVE_KEYPAD
#endif

#if MC2KEY == FIVE_KEYPAD
#ifndef FiveKeypadMod
#define FiveKeypadMod
#define SET 'A'
#define MOV 'B'
#define INC 'C'
#define DIC 'D'
#define ENT 'E'
#endif

#ifndef FiveKeypadConfig
#define FiveKeypadConfig
#define FiveKeypadPort PORTD
#define FiveKeypadDirection TRISD
#define FiveKeypadKey1 0
#define FiveKeypadKey2 1
#define FiveKeypadKey3 2
#define FiveKeypadKey4 3
#define FiveKeypadKey5 4
#endif
#endif

#if MC2KEY == MATRIX_KEYPAD
#ifndef MatrixColConfig
#define MatrixColConfig
#define MatrixColPort PORTA
#define MatrixColDirection TRISA
#define MatrixColPin1 0
#define MatrixColPin2 1
#define MatrixColPin3 2
#define MatrixColPin4 3
#endif

#ifndef MatrixRowConfig
#define MatrixRowConfig
#define MatrixRowPort PORTC
#define MatrixRowDirection TRISC
#define MatrixRowPin1 0
#define MatrixRowPin2 1
#define MatrixRowPin3 2
#define MatrixRowPin4 3
#endif
#endif

#if MC2KEY == MATRIX_KEYPAD
static bit Row1 @ ((unsigned char) &MatrixRowPort * 8 + MatrixRowPin1);
static bit Row2 @ ((unsigned char) &MatrixRowPort * 8 + MatrixRowPin2);
static bit Row3 @ ((unsigned char) &MatrixRowPort * 8 + MatrixRowPin3);
static bit Row4 @ ((unsigned char) &MatrixRowPort * 8 + MatrixRowPin4);
static bit Col1 @ ((unsigned char) &MatrixColPort * 8 + MatrixColPin1);
static bit Col2 @ ((unsigned char) &MatrixColPort * 8 + MatrixColPin2);
static bit Col3 @ ((unsigned char) &MatrixColPort * 8 + MatrixColPin3);
static bit Col4 @ ((unsigned char) &MatrixColPort * 8 + MatrixColPin4);
#endif

#if MC2KEY == FIVE_KEYPAD
static bit SetKey @ ((unsigned char) &FiveKeypadPort * 8 + FiveKeypadKey1);
static bit MovKey @ ((unsigned char) &FiveKeypadPort * 8 + FiveKeypadKey2);
static bit EntKey @ ((unsigned char) &FiveKeypadPort * 8 + FiveKeypadKey3);
static bit IncKey @ ((unsigned char) &FiveKeypadPort * 8 + FiveKeypadKey4);
static bit DecKey @ ((unsigned char) &FiveKeypadPort * 8 + FiveKeypadKey5);
#endif

void Keypad_Initialize();
char FetchKey(volatile unsigned int*);
void Keypad(unsigned char, unsigned char*, unsigned int);
void RTC_Keypad(unsigned char*);
#endif