#include <U8g2lib.h>

#define DISPLAY_I2C 1
#define DISPLAY_SPI 0

//Estructura para los íconos de la batería.
struct RectanguloBateria{
    int x;
    int y;
    int w;
    int h;
};

struct Bateria{
    int x;
    int y;
    int w;
    int h;
};

#define LONGITUD_REC_BAT 14
#define ALTURA_REC_BAT 7

#if DISPLAY_I2C
/** \fn DibujarGUI(U8G2_SSD1306_128X64_NONAME_F_SW_I2C* apuntadorDisplay)
 * \brief Dibuja la interfaz en una pantalla OLED 128x64 I2C.
 * \param apuntadorDisplay Apuntador con la dirección del display sobre el que se dibujará.
*/
void DibujarGUI(U8G2_SSD1306_128X64_NONAME_F_SW_I2C* apuntadorDisplay){
    U8G2_SSD1306_128X64_NONAME_F_SW_I2C display = *apuntadorDisplay;
    
    //Batería.
    struct Bateria bateria = {105, 5, 20, 45};
    display.drawFrame(bateria.x+4, bateria.y, bateria.w-8, bateria.h-39); //Cuadro pequeño.
    display.drawFrame(bateria.x, bateria.y+5, bateria.w, bateria.h - bateria.y);
    
    //Rectángulos de la batería.
    struct RectanguloBateria recBat1 = {108, 13, LONGITUD_REC_BAT, ALTURA_REC_BAT};
    struct RectanguloBateria recBat2 = {recBat1.x, recBat1.y + ALTURA_REC_BAT + 3, LONGITUD_REC_BAT, ALTURA_REC_BAT};
    struct RectanguloBateria recBat3 = {recBat1.x, recBat2.y + ALTURA_REC_BAT + 3, LONGITUD_REC_BAT, ALTURA_REC_BAT};
    struct RectanguloBateria recBat4 = {recBat1.x, recBat3.y + ALTURA_REC_BAT + 3, LONGITUD_REC_BAT, ALTURA_REC_BAT};
    display.drawFrame(recBat1.x, recBat1.y, recBat1.w, recBat1.h);
    display.drawFrame(recBat2.x, recBat2.y, recBat2.w, recBat2.h);
    display.drawFrame(recBat3.x, recBat3.y, recBat3.w, recBat3.h);
    display.drawFrame(recBat4.x, recBat4.y, recBat4.w, recBat4.h);

    //Caminito de la escuela.
    int x_offset = 20;
    display.drawFrame(8+x_offset, 50, 57+x_offset, 10);
    display.drawLine(13+x_offset, 55, 21+x_offset, 55);
    display.drawLine(26+x_offset, 55, 34+x_offset, 55);
    display.drawLine(39+x_offset, 55, 47+x_offset, 55);
    display.drawLine(52+x_offset, 55, 60+x_offset, 55);

    //Velocímetro.
    display.drawArc(65, 40, 35, 0, 128);
    display.drawLine(30, 40, 100, 40);
    
    display.sendBuffer();
}
#endif

#if DISPLAY_SPI
/** \fn DibujarGUI(U8G2_SSD1312_128X64_NONAME_F_4W_SW_SPI* apuntadorDisplay)
 * \brief Dibuja la interfaz en una pantalla OLED 128x64 SPI.
 * \param apuntadorDisplay Apuntador con la dirección del display sobre el que se dibujará.
*/
void DibujarGUI(U8G2_SSD1312_128X64_NONAME_F_4W_SW_SPI* apuntadorDisplay){
    U8G2_SSD1312_128X64_NONAME_F_4W_SW_SPI display = *apuntadorDisplay;
}
#endif

void DibujarGUIOriginal(U8G2_SSD1306_128X64_NONAME_F_SW_I2C* apuntadorDisplay){
    U8G2_SSD1306_128X64_NONAME_F_SW_I2C display = *apuntadorDisplay;
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
