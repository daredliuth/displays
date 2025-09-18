//Bibliotecas de terceros.
#include <U8g2lib.h>
#include <Wire.h>
//Bibliotecas propias
#include <sensores.h>
#include <GUI-OLED.h>

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
char textoVelocidad[3] = "99";  //[km/h]
char textoDistancia[4] = "9.9"; //[km]
char textoBateria[4] = "100";   //[%]
char textoVoltaje[4] = "120";   //[v]

char textoAnalog[3] = "99";
char textoBPM[4] = "200";
char textoCO2[4] = "9.9";
char aux3[3] = "";
char aux4[4] = "";

void DibujarBPM(){
  SetLeerBPM(false);
  String(GetBPM()).toCharArray(textoBPM, 4);
  display.setFont(u8g2_font_BBSesque_tr);
  display.setDrawColor(0);
  display.drawBox(98, 7, 20, 10);
  display.setDrawColor(1);
  display.drawStr(98, 17, textoBPM);
  display.sendBuffer();
}

void DibujarValores(){
  (GetAnalog() < 10) ? String("0"+String(GetAnalog())).toCharArray(textoAnalog,3) : String(GetAnalog()).toCharArray(textoAnalog,3);
  String(GetCO2()).toCharArray(textoCO2, 4);

  //display.setDrawColor(0);
  //display.drawBox(23, 12, 22, 30);
  //display.sendBuffer();
  display.setFontMode(0);
  display.setFont(u8g2_font_logisoso18_tf);
  display.setDrawColor(0);
  display.drawBox(23, 20, 25, 25);
  display.setDrawColor(1);
  display.drawStr(23, 40, textoAnalog);
  display.sendBuffer();

  display.setFont(u8g2_font_BBSesque_tr);
  display.setDrawColor(0);
  display.drawBox(100, 27, 20, 10);
  display.setDrawColor(1);
  display.drawStr(100, 37, textoCO2);
  display.sendBuffer();
}

void DibujarCorazon(){
  display.setFont(u8g2_font_unifont_t_symbols);
  if(GetCorazonEncendido()){
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

void setup(void) {
  Serial.begin(115200);
  //¿Modificar la velocidad de la comunicación I2C?
  //display.setBusClock(8000000);
  display.setBusClock(400000);

  //Iniciamos el display.
  display.begin();
  delay(50);
  display.clearBuffer();
  DibujarGUI(&display);

  InicializarTimers(); //Inicializamos los temporizadores.

  //Establecemos la fuente.
  display.setFontMode(0);
  display.setDrawColor(1);
  display.setFont(u8g2_font_logisoso18_tf);
  //display.drawStr(23, 40, textoAnalog);
  display.sendBuffer();
  display.setFont(u8g2_font_BBSesque_tr);
  //display.drawStr(98, 17, textoBPM);
  //display.drawStr(100, 37, textoCO2);
  display.sendBuffer();
}

void loop(void) {
  DibujarGUI(&display);
  if(GetLeerBPM()){
    //LeerBPM();
    //DibujarBPM();
  }
  if(GetLeerValores()){
    //LeerValores();
    //DibujarValores();
  }
  if(GetCambiarCorazon()){
    //CambiarCorazon();
    //DibujarCorazon();
  }
}