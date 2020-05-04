//***************************************************************************************************************************************
/* Librería para el uso de la pantalla ILI9341 en modo 8 bits
   Basado en el código de martinayotte - https://www.stm32duino.com/viewtopic.php?t=637
   Adaptación, migración y creación de nuevas funciones: Pablo Mazariegos y José Morales
   Con ayuda de: José Guerra
   IE3027: Electrónica Digital 2 - 2019
*/
//***************************************************************************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include <TM4C123GH6PM.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

#include "bitmaps.h"
#include "font.h"
#include "lcd_registers.h"

#define note_cc 261
#define note_dd 294
#define note_ee 329
#define note_ff 349
#define note_g 391
#define note_gS 415
#define note_a 440
#define note_aS 455
#define note_b 466
#define note_cH 523
#define note_cSH 554
#define note_dH 587
#define note_dSH 622
#define note_eH 659
#define note_fH 698
#define note_fSH 740
#define note_gH 784
#define note_gSH 830
#define note_aH 880

int buzzerPin = PF_1;
void beep(int note, int duration)
{
  tone(buzzerPin, note, duration/2);
  delay(duration/2);
  noTone(buzzerPin);
  delay(duration/2 + 20);  
}

#define LCD_RST PD_0
#define LCD_CS PD_1
#define LCD_RS PD_2
#define LCD_WR PD_3
#define LCD_RD PE_1
int DPINS[] = {PB_0, PB_1, PB_2, PB_3, PB_4, PB_5, PB_6, PB_7};

const int buttonPin1 = PUSH1;   //ataque
const int buttonPin2 = PUSH2;   //ataque
const int buttonPin3 = PA_2;
const int buttonPin4 = PA_3;
const int buttonPin5 = PA_4;
const int buttonPin6 = PA_5;
const int buttonPin7 = PA_6;
const int buttonPin8 = PA_7;
const int buttonPin9 = PC_5;
const int buttonPin10 = PC_6;


int contmov1 = 50;
int contmov2 = 250;
bool defensa = true;
bool defensa2 = true;
bool def1 = false;
bool def2 = false;
int golpes1 = 0;
int golpes2 = 0;
//***************************************************************************************************************************************
// Functions Prototypes
//***************************************************************************************************************************************
void LCD_Init(void);
void LCD_CMD(uint8_t cmd);
void LCD_DATA(uint8_t data);
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
void LCD_Clear(unsigned int c);
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void LCD_Print(String text, int x, int y, int fontSize, int color, int background);

void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]);
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[], int columns, int index, char flip, char offset);

extern uint8_t mov[];
extern uint8_t attack[];
extern uint8_t defense[];
extern uint8_t mov2[];
extern uint8_t attack2[];
extern uint8_t defense2[];

extern uint8_t fondo[];
//***************************************************************************************************************************************
// Inicialización
//***************************************************************************************************************************************
void setup() {
  SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
  Serial.begin(9600);
  GPIOPadConfigSet(GPIO_PORTB_BASE, 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
  //Serial.println("Inicio");

  LCD_Init();
  LCD_Clear(0x3E19);

  LCD_Bitmap(0, 0, 320, 240, fondo);
  beep(note_a, 500);
  beep(note_a, 500);
  beep(note_a, 500);
  beep(note_ff, 350);
  beep(note_cH, 150);  
  beep(note_a, 500);
  beep(note_ff, 350);
  beep(note_cH, 150);
  beep(note_a, 650);
  delay (1000);
  LCD_Clear(0x3E19);

  FillRect(0, 0, 320, 240, 0x421b);
  String text1 = "PROYECTO 2!";
  LCD_Print(text1, 20, 160, 2, 0xffff, 0x421b);
  String text2 = "Alex Maas";
  LCD_Print(text2, 20, 180, 2, 0xffff, 0x421b);
  String text3 = "Carlos Alonzo";
  LCD_Print(text3, 20, 200, 2, 0xffff, 0x421b);
  String text4 = "Reglas:";
  LCD_Print(text4, 90, 25, 2, 0xffff, 0x421b);
  String text5 = "Debes conectar 10 golpes.";
  LCD_Print(text5, 20, 50, 1, 0xffff, 0x421b);
  String text6 = "Los golpes no cuentan con la defensa";
  LCD_Print(text6, 20, 70, 1, 0xffff, 0x421b);
  String text7 = "activa.";
  LCD_Print(text7, 20, 80, 1, 0xffff, 0x421b);
  beep(note_a, 500);
  beep(note_a, 500);
  beep(note_a, 500);
  beep(note_ff, 350);
  beep(note_cH, 150);  
  beep(note_a, 500);
  beep(note_ff, 350);
  beep(note_cH, 150);
  beep(note_a, 650);
  delay (1000);
  LCD_Clear(0x3E19);

  

  //LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset);
  //LCD_Sprite(50, 100, 18, 38, mov,2, 1, 1, 0);

  //LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]);
  //LCD_Bitmap(50, 160, 18, 38, inicio);


  for (int x = 0; x < 319; x++) {
    LCD_Bitmap(x, 207, 16, 16, tile);
    LCD_Bitmap(x, 223, 16, 16, tile);
    x += 15;
  }

  String title1 ="P1";
  LCD_Print(title1, 70, 50, 2, 0xffff, 0x421b);
  String golp1_init ="0";
  LCD_Print(golp1_init, 80, 75, 2, 0xffff, 0x421b);

  String title2 ="P2";
  LCD_Print(title2, 200, 50, 2, 0xffff, 0x421b);
  String golp2_init ="0";
  LCD_Print(golp2_init, 210, 75, 2, 0xffff, 0x421b);

  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(buttonPin3, INPUT_PULLUP);
  pinMode(buttonPin4, INPUT_PULLUP);
  pinMode(buttonPin5, INPUT_PULLUP);
  pinMode(buttonPin6, INPUT_PULLUP);
  pinMode(buttonPin7, INPUT_PULLUP);
  pinMode(buttonPin8, INPUT_PULLUP);
  pinMode(buttonPin9, INPUT_PULLUP);
  pinMode(buttonPin10, INPUT_PULLUP);
  pinMode(buzzerPin,OUTPUT);

}
//***************************************************************************************************************************************
// Loop Infinito
//***************************************************************************************************************************************
void loop() {
  int boton1 = digitalRead(buttonPin1);
  int boton2 = digitalRead(buttonPin2);
  int boton3 = digitalRead(buttonPin3);
  int boton4 = digitalRead(buttonPin4);
  int boton5 = digitalRead(buttonPin5);
  int boton6 = digitalRead(buttonPin6);
  int boton7 = digitalRead(buttonPin7);
  int boton8 = digitalRead(buttonPin8);
  int boton9 = digitalRead(buttonPin9);
  int boton10 = digitalRead(buttonPin10);

  
  //------------------Jugador1--------------------------------------------------***************************************************************************

  
// --------------------------------------------caminar hacia adelante --------------------------------------
  if (boton3 == 0) {
    if (contmov1 + 18 < contmov2) {
      contmov1 = contmov1 + 1;
    }
    else {
      contmov1 = contmov1;
    }
    for (int x = 50; x < 68; x++) {
      delay(1);
      int anim1 = (x / 10) % 2;
      LCD_Sprite(contmov1, 168, 18, 38, mov, 2, anim1, 0, 0);
      //FillRect(contmov1-1, 128, 5, 40, 0x3E19);
      V_line( contmov1 - 1, 160, 45, 0x3E19);
    }
  } else {
  }
//------------------------------------------- caminar hacia atras -----------------------------------------------
  if (boton4 == 0) {
    if (contmov1 == 0) {
      contmov1 = contmov1;
    } else {
      contmov1 = contmov1 - 1;
    }
    for (int x = 50; x < 68; x++) {
      delay(1);
      int anim2 = (x / 10) % 2;
      LCD_Sprite(contmov1, 168, 18, 38, mov, 2, anim2, 0, 0);
      V_line( contmov1 + 1, 160, 45, 0x3E19);
    }
  } else {
  }
//------------------------------golpes jugador 1--------------------------------
  if (boton1 == 0) {
    if (contmov1 + 33 > contmov2 && def2 == false) {
      golpes1 = golpes1 + 1;
      if(golpes1 ==1){
        String g1 = "1";
        LCD_Print(g1, 80, 75, 2, 0xffff, 0x421b);  
      }
      if(golpes1 ==2){
        String g2 = "2";
        LCD_Print(g2, 80, 75, 2, 0xffff, 0x421b);
      }
      if(golpes1 ==3){
        String g3= "3";
        LCD_Print(g3, 80, 75, 2, 0xffff, 0x421b);
      }
      if(golpes1 ==4){
        String g4 = "4";
        LCD_Print(g4, 80, 75, 2, 0xffff, 0x421b);
      }
      if(golpes1 ==5){
        String g5= "5";
        LCD_Print(g5, 80, 75, 2, 0xffff, 0x421b);
      }else{
        //String g6 ="0";
        //LCD_Print(g6, 80, 75, 2, 0xffff, 0x421b);
      }
    }
    for (int x = 50; x < 100; x++) {
      delay(1);
      int anim3 = (x / 10) % 4;
      LCD_Sprite(contmov1, 168, 33, 39, attack, 4, anim3, 0, 0);
      V_line( contmov1 - 1, 160, 45, 0x3E19);
    }
  } else {
  }
//---------------------------------------- defensa jugador 1 ----------------------------------------------------------------------------------------------
  if (boton5 == 0 && defensa == true) {
    def1 = true;
    for (int x = 0; x < 30; x++) {
      delay(1);
      int anim4 = (x / 10) % 12;
      LCD_Sprite(contmov1 - 5, 168, 28, 38, defense, 9, anim4, 0, 0);
      V_line( contmov1 - 6, 160, 45, 0x3E19);
    }
    defensa = false;
  }
  if (boton5 == 0 && defensa == false) {
    LCD_Sprite(contmov1 - 5, 168, 28, 38, defense, 9, 8, 0, 0);
    V_line( contmov1 - 6, 160, 45, 0x3E19);
  }
  else {
    defensa = true;
    def1 = false;
  }
  //---------------------------------Jugador2----------------*****************************************************************************************************
//-----------------------------------------------caminar hacia adelante J2 ----------------------------
  if (boton6 == 0) {
    if (contmov2 > contmov1 + 18) {
      contmov2 = contmov2 - 1;
    }
    else {
      contmov2 = contmov2;
    }
    for (int x = 250; x < 300; x++) {
      delay(1);
      int anim5 = (x / 10) % 2;
      LCD_Sprite(contmov2, 168, 18, 38, mov2, 2, anim5, 0, 0);
      V_line( contmov2 + 18, 160, 45, 0x3E19);
    }
  } else {
  }
// ---------------------------------------------------- caminar hacia atras J2------------------------------------------------
  if (boton7 == 0) {
    if (contmov2 == 301) {
      contmov2 = contmov2;
    } else {
      contmov2 = contmov2 + 1;
    }
    for (int x = 250; x < 300; x++) {
      delay(1);
      int anim6 = (x / 10) % 2;
      LCD_Sprite(contmov2, 168, 18, 38, mov2, 2, anim6, 0, 0);
      V_line( contmov2 - 1, 160, 45, 0x3E19);
    }
  } else {
  }
// --------------------------------------------- ataque J2 -------------------------------------------------------------------
  if (boton2 == 0) {
    if (contmov2 < contmov1 + 18 && def1 == false) {
      golpes2 = golpes2 + 1;
      if(golpes2 == 1){
        String g7 = "1";
        LCD_Print(g7, 210, 75, 2, 0xffff, 0x421b);
      }
      if(golpes2 ==2){
        String g8 = "2";
        LCD_Print(g8, 210, 75, 2, 0xffff, 0x421b);
      }
      if(golpes2 ==3){
        String g9= "3";
        LCD_Print(g9, 210, 75, 2, 0xffff, 0x421b);
      }
      if(golpes2 ==4){
        String g10 = "4";
        LCD_Print(g10, 210, 75, 2, 0xffff, 0x421b);
      }
      if(golpes2 ==5){
        String g11= "5";
        LCD_Print(g11, 210, 75, 2, 0xffff, 0x421b);
      }else{
        //String g6 ="0";
        //LCD_Print(g6, 80, 75, 2, 0xffff, 0x421b);
      }
    }  
    for (int x = 250; x < 300; x++) {
      delay(1);
      int anim7 = (x / 10) % 4;
      LCD_Sprite(contmov2, 168, 33, 39, attack2, 4, anim7, 0, 0);
      V_line( contmov2 - 1, 160, 45, 0x3E19);
    }
  } else {
  }
// ------------------------------------------------- defensa J2 ------------------------------------------------------------------------
  if (boton8 == 0 && defensa2 == true) {
    def2 = true;
    for (int x = 0; x < 30; x++) {
      delay(1);
      int anim8 = (x / 10) % 12;
      LCD_Sprite(contmov2 , 168, 28, 38, defense2, 9, anim8, 1, 0);
      V_line( contmov2 - 1, 160, 45, 0x3E19);
    }
    defensa2 = false;
  }
  if (boton8 == 0 && defensa2 == false) {
    LCD_Sprite(contmov2, 168, 28, 38, defense2, 9, 8, 1, 0);
    V_line( contmov2 - 1, 160, 45, 0x3E19);
  }
  else {
    defensa2 = true;
    def2 = false;
  }
}
//***************************************************************************************************************************************
// Función para inicializar LCD
//***************************************************************************************************************************************
void LCD_Init(void) {
  pinMode(LCD_RST, OUTPUT);
  pinMode(LCD_CS, OUTPUT);
  pinMode(LCD_RS, OUTPUT);
  pinMode(LCD_WR, OUTPUT);
  pinMode(LCD_RD, OUTPUT);
  for (uint8_t i = 0; i < 8; i++) {
    pinMode(DPINS[i], OUTPUT);
  }
  //****************************************
  // Secuencia de Inicialización
  //****************************************
  digitalWrite(LCD_CS, HIGH);
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, HIGH);
  digitalWrite(LCD_RD, HIGH);
  digitalWrite(LCD_RST, HIGH);
  delay(5);
  digitalWrite(LCD_RST, LOW);
  delay(20);
  digitalWrite(LCD_RST, HIGH);
  delay(150);
  digitalWrite(LCD_CS, LOW);
  //****************************************
  LCD_CMD(0xE9);  // SETPANELRELATED
  LCD_DATA(0x20);
  //****************************************
  LCD_CMD(0x11); // Exit Sleep SLEEP OUT (SLPOUT)
  delay(100);
  //****************************************
  LCD_CMD(0xD1);    // (SETVCOM)
  LCD_DATA(0x00);
  LCD_DATA(0x71);
  LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0xD0);   // (SETPOWER)
  LCD_DATA(0x07);
  LCD_DATA(0x01);
  LCD_DATA(0x08);
  //****************************************
  LCD_CMD(0x36);  // (MEMORYACCESS)
  LCD_DATA(0x40 | 0x80 | 0x20 | 0x08); // LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0x3A); // Set_pixel_format (PIXELFORMAT)
  LCD_DATA(0x05); // color setings, 05h - 16bit pixel, 11h - 3bit pixel
  //****************************************
  LCD_CMD(0xC1);    // (POWERCONTROL2)
  LCD_DATA(0x10);
  LCD_DATA(0x10);
  LCD_DATA(0x02);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC0); // Set Default Gamma (POWERCONTROL1)
  LCD_DATA(0x00);
  LCD_DATA(0x35);
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC5); // Set Frame Rate (VCOMCONTROL1)
  LCD_DATA(0x04); // 72Hz
  //****************************************
  LCD_CMD(0xD2); // Power Settings  (SETPWRNORMAL)
  LCD_DATA(0x01);
  LCD_DATA(0x44);
  //****************************************
  LCD_CMD(0xC8); //Set Gamma  (GAMMASET)
  LCD_DATA(0x04);
  LCD_DATA(0x67);
  LCD_DATA(0x35);
  LCD_DATA(0x04);
  LCD_DATA(0x08);
  LCD_DATA(0x06);
  LCD_DATA(0x24);
  LCD_DATA(0x01);
  LCD_DATA(0x37);
  LCD_DATA(0x40);
  LCD_DATA(0x03);
  LCD_DATA(0x10);
  LCD_DATA(0x08);
  LCD_DATA(0x80);
  LCD_DATA(0x00);
  //****************************************
  LCD_CMD(0x2A); // Set_column_address 320px (CASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x3F);
  //****************************************
  LCD_CMD(0x2B); // Set_page_address 480px (PASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0xE0);
  //  LCD_DATA(0x8F);
  LCD_CMD(0x29); //display on
  LCD_CMD(0x2C); //display on

  LCD_CMD(ILI9341_INVOFF); //Invert Off
  delay(120);
  LCD_CMD(ILI9341_SLPOUT);    //Exit Sleep
  delay(120);
  LCD_CMD(ILI9341_DISPON);    //Display on
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para enviar comandos a la LCD - parámetro (comando)
//***************************************************************************************************************************************
void LCD_CMD(uint8_t cmd) {
  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = cmd;
  digitalWrite(LCD_WR, HIGH);
}
//***************************************************************************************************************************************
// Función para enviar datos a la LCD - parámetro (dato)
//***************************************************************************************************************************************
void LCD_DATA(uint8_t data) {
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = data;
  digitalWrite(LCD_WR, HIGH);
}
//***************************************************************************************************************************************
// Función para definir rango de direcciones de memoria con las cuales se trabajara (se define una ventana)
//***************************************************************************************************************************************
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
  LCD_CMD(0x2a); // Set_column_address 4 parameters
  LCD_DATA(x1 >> 8);
  LCD_DATA(x1);
  LCD_DATA(x2 >> 8);
  LCD_DATA(x2);
  LCD_CMD(0x2b); // Set_page_address 4 parameters
  LCD_DATA(y1 >> 8);
  LCD_DATA(y1);
  LCD_DATA(y2 >> 8);
  LCD_DATA(y2);
  LCD_CMD(0x2c); // Write_memory_start
}
//***************************************************************************************************************************************
// Función para borrar la pantalla - parámetros (color)
//***************************************************************************************************************************************
void LCD_Clear(unsigned int c) {
  unsigned int x, y;
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  SetWindows(0, 0, 319, 239); // 479, 319);
  for (x = 0; x < 320; x++)
    for (y = 0; y < 240; y++) {
      LCD_DATA(c >> 8);
      LCD_DATA(c);
    }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una línea horizontal - parámetros ( coordenada x, cordenada y, longitud, color)
//***************************************************************************************************************************************
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + x;
  SetWindows(x, y, l, y);
  j = l;// * 2;
  for (i = 0; i < l; i++) {
    LCD_DATA(c >> 8);
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una línea vertical - parámetros ( coordenada x, cordenada y, longitud, color)
//***************************************************************************************************************************************
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + y;
  SetWindows(x, y, x, l);
  j = l; //* 2;
  for (i = 1; i <= j; i++) {
    LCD_DATA(c >> 8);
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  H_line(x  , y  , w, c);
  H_line(x  , y + h, w, c);
  V_line(x  , y  , h, c);
  V_line(x + w, y  , h, c);
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo relleno - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  unsigned int i;
  for (i = 0; i < h; i++) {
    H_line(x  , y  , w, c);
    H_line(x  , y + i, w, c);
  }
}
//***************************************************************************************************************************************
// Función para dibujar texto - parámetros ( texto, coordenada x, cordenada y, color, background)
//***************************************************************************************************************************************
void LCD_Print(String text, int x, int y, int fontSize, int color, int background) {
  int fontXSize ;
  int fontYSize ;

  if (fontSize == 1) {
    fontXSize = fontXSizeSmal ;
    fontYSize = fontYSizeSmal ;
  }
  if (fontSize == 2) {
    fontXSize = fontXSizeBig ;
    fontYSize = fontYSizeBig ;
  }

  char charInput ;
  int cLength = text.length();
  Serial.println(cLength, DEC);
  int charDec ;
  int c ;
  int charHex ;
  char char_array[cLength + 1];
  text.toCharArray(char_array, cLength + 1) ;
  for (int i = 0; i < cLength ; i++) {
    charInput = char_array[i];
    Serial.println(char_array[i]);
    charDec = int(charInput);
    digitalWrite(LCD_CS, LOW);
    SetWindows(x + (i * fontXSize), y, x + (i * fontXSize) + fontXSize - 1, y + fontYSize );
    long charHex1 ;
    for ( int n = 0 ; n < fontYSize ; n++ ) {
      if (fontSize == 1) {
        charHex1 = pgm_read_word_near(smallFont + ((charDec - 32) * fontYSize) + n);
      }
      if (fontSize == 2) {
        charHex1 = pgm_read_word_near(bigFont + ((charDec - 32) * fontYSize) + n);
      }
      for (int t = 1; t < fontXSize + 1 ; t++) {
        if (( charHex1 & (1 << (fontXSize - t))) > 0 ) {
          c = color ;
        } else {
          c = background ;
        }
        LCD_DATA(c >> 8);
        LCD_DATA(c);
      }
    }
    digitalWrite(LCD_CS, HIGH);
  }
}
//***************************************************************************************************************************************
// Función para dibujar una imagen a partir de un arreglo de colores (Bitmap) Formato (Color 16bit R 5bits G 6bits B 5bits)
//***************************************************************************************************************************************
void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);

  unsigned int x2, y2;
  x2 = x + width;
  y2 = y + height;
  SetWindows(x, y, x2 - 1, y2 - 1);
  unsigned int k = 0;
  unsigned int i, j;

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k + 1]);
      //LCD_DATA(bitmap[k]);
      k = k + 2;
    }
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una imagen sprite - los parámetros columns = número de imagenes en el sprite, index = cual desplegar, flip = darle vuelta
//***************************************************************************************************************************************
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[], int columns, int index, char flip, char offset) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);

  unsigned int x2, y2;
  x2 =   x + width;
  y2 =    y + height;
  SetWindows(x, y, x2 - 1, y2 - 1);
  int k = 0;
  int ancho = ((width * columns));
  if (flip) {
    for (int j = 0; j < height; j++) {
      k = (j * (ancho) + index * width - 1 - offset) * 2;
      k = k + width * 2;
      for (int i = 0; i < width; i++) {
        LCD_DATA(bitmap[k]);
        LCD_DATA(bitmap[k + 1]);
        k = k - 2;
      }
    }
  } else {
    for (int j = 0; j < height; j++) {
      k = (j * (ancho) + index * width + 1 + offset) * 2;
      for (int i = 0; i < width; i++) {
        LCD_DATA(bitmap[k]);
        LCD_DATA(bitmap[k + 1]);
        k = k + 2;
      }
    }


  }
  digitalWrite(LCD_CS, HIGH);
}
