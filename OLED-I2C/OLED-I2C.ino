#include <U8g2lib.h>
#include <Wire.h>


/*
Distribución de pines:
Display - ESP32
GND - GND
VDD - VDD
SCL - G22 / 36
SDA - G21 / 33
*/

//Creamos el objeto display utilizando el modelo que tenemos, en este caso: U8G2_SSD1306_128X64_NONAME_F_SW_I2C.
U8G2_SSD1306_128X64_NONAME_F_SW_I2C display(U8G2_R0, SCL, SDA, U8X8_PIN_NONE);
//U8G2_SSD1306_128X64_NONAME_1_SW_I2C display(U8G2_R0, SCL, SDA, U8X8_PIN_NONE);

//Variables del programa.
char textoPot[3] = "99";
char textoBPM[4] = "200";
char textoCO2[4] = "9.9";
char aux3[3] = "";
char aux4[4] = "";

//Potenciometros
#define POTPIN 34
#define POTCO2 35
float co2Valor = 0;
int potValor = 0;

//Timer
hw_timer_t *temporizadorBPM = NULL;
hw_timer_t *temporizadorValores = NULL;
hw_timer_t *temporizadorCorazon = NULL;

bool leerValores = false;
bool leerBPM = false;
bool cambiarCorazon = false;
bool corazonEncendido = true;

//Interrupción para leer el valor de BPM.
void IRAM_ATTR InterrupcionBPM() {
  //Leer valor del BPM.
  leerBPM = true;
}

//Interrupción para leer los valores de los potenciómetros.
void IRAM_ATTR InterrupcionLeerValores(){
  leerValores = true;
}

//Interrupción para cambiar el estado del corazón.
void IRAM_ATTR InterrupcionCambiarCorazon(){
  cambiarCorazon = true;
}

void LeerBPM(){
  leerBPM = false;
  String(random(60,200)).toCharArray(textoBPM, 4);
  display.setFont(u8g2_font_BBSesque_tr);
  display.setDrawColor(0);
  display.drawBox(98, 7, 20, 10);
  display.setDrawColor(1);
  display.drawStr(98, 17, textoBPM);
  display.sendBuffer();
}

void LeerValores(){
  leerValores = false;

  potValor = (int)(((float)analogRead(POTPIN)/4095.00)*99);
  co2Valor = (float)(analogRead(POTCO2)/4095.00)*9.9;

  (potValor < 10) ? String("0"+String(potValor)).toCharArray(textoPot,3) : String(potValor).toCharArray(textoPot,3);
  String(co2Valor).toCharArray(textoCO2, 4);

  //display.setDrawColor(0);
  //display.drawBox(23, 12, 22, 30);
  //display.sendBuffer();
  display.setFontMode(0);
  display.setFont(u8g2_font_logisoso18_tf);
  display.setDrawColor(0);
  display.drawBox(23, 20, 25, 25);
  display.setDrawColor(1);
  display.drawStr(23, 40, textoPot);
  display.sendBuffer();

  display.setFont(u8g2_font_BBSesque_tr);
  display.setDrawColor(0);
  display.drawBox(100, 27, 20, 10);
  display.setDrawColor(1);
  display.drawStr(100, 37, textoCO2);
  display.sendBuffer();
}

void CambiarCorazon(){
  cambiarCorazon = false;
  corazonEncendido = !corazonEncendido;

  display.setFont(u8g2_font_unifont_t_symbols);
  if(corazonEncendido){
    display.drawGlyph(80, 17, 0x2665);
  }
  else{
    //display.setDrawColor(1);
    //display.drawGlyph(80, 17, 0x2665);
    //display.sendBuffer();
    //display.setDrawColor(0);
    display.drawGlyph(80, 17, 0x2661);
  }

  display.sendBuffer();
}

void DibujarGUI(){
  //Cuadro POT.
  display.drawRFrame(10,7,50,50,10);
  //Cuadro BPM.
  display.drawRFrame(75,5,50,15,7);
  //Cuadro CO2.
  display.drawRFrame(75,25,50,15,7);
  //Establecemos la fuente con dibujitos.
  display.setFont(u8g2_font_unifont_t_symbols);
  //Dibujamos el corazón relleno (0x2665) en 5,20.
  display.drawGlyph(80, 17, 0x2665);
  //CO
  display.drawHLine(82, 29, 2);
  display.drawHLine(86, 29, 2);
  display.drawHLine(82, 34, 2);
  display.drawHLine(86, 34, 2);
  display.drawVLine(81, 30, 4);
  display.drawVLine(85, 30, 4);
  display.drawVLine(88, 30, 4);
  //2
  display.drawHLine(91, 33, 2);
  display.drawHLine(90, 37, 4);
  display.drawPixel(90,34);
  display.drawPixel(93,34);
  display.drawPixel(92,35);
  display.drawPixel(91,36);

  //Cuadro Info.
  display.drawRFrame(75, 45, 50, 15, 7);
  display.drawBox(80,46,40,13);
  display.drawVLine(76,50,5);
  display.drawVLine(77,48,10);
  display.drawBox(78,47,2,11);
  display.drawBox(120,47,2,11);
  display.drawVLine(123,50,5);
  display.drawVLine(122,48,10);
  //Info
  display.setFont(u8g2_font_BBSesque_tr);
  display.setFontMode(0);
  display.setDrawColor(0);
  display.drawStr(87, 57, "Info");
  display.setDrawColor(1);
  display.setFontMode(1);
  for(int i=0; i<132; i+=2){
    display.drawPixel(i,0);
    display.drawPixel(i+1,63);
  }
  for(int i=0; i<70; i+=2){
    display.drawPixel(0,i);
    display.drawPixel(127,i+1);
  }

  display.sendBuffer();
}

void setup(void) {
  //¿Modificar la velocidad de la comunicación I2C?
  //display.setBusClock(8000000);
  display.setBusClock(400000);
  //Iniciamos el display.
  display.begin();
  delay(50);
  DibujarGUI();
  //Iniciamos los temporizadores a 1[MHz].
  temporizadorBPM = timerBegin(1000000);
  temporizadorValores = timerBegin(1000000);
  temporizadorCorazon = timerBegin(1000000);
  //Attachear las interrupciones a las funciones.
  timerAttachInterrupt(temporizadorBPM, &InterrupcionBPM);
  timerAttachInterrupt(temporizadorValores, &InterrupcionLeerValores);
  timerAttachInterrupt(temporizadorCorazon, &InterrupcionCambiarCorazon);
  timerAlarm(temporizadorBPM, 5000000, true, 0); // Interrumpir cada 5 segundos con recarga.
  timerAlarm(temporizadorValores, 100, true, 0); // Interrumpir cada mili segundo con recarga.
  timerAlarm(temporizadorCorazon, 1000000, true, 0); // Interrumpir cada segundo con recarga.
  //Establecemos la fuente.
  
  display.setFontMode(0);
  display.setDrawColor(1);
  display.setFont(u8g2_font_logisoso18_tf);
  display.drawStr(23, 40, textoPot);
  display.sendBuffer();
  display.setFont(u8g2_font_BBSesque_tr);
  display.drawStr(98, 17, textoBPM);
  display.drawStr(100, 37, textoCO2);
  display.sendBuffer();
}

void loop(void) {
  if(leerBPM){
    LeerBPM();
  }
  if(leerValores){
    LeerValores();
  }
  if(cambiarCorazon){
    CambiarCorazon();
  }
}