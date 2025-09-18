#include<stdbool.h>

/*Pines para los sensores*/
#define ENCODERPIN 25   //Pin del encoder para medir la velocidad.
#define VOLTPIN1 34     //Pin para realizar la lectura del voltaje.

#define CO2PIN 35       //Pin para realizar la lectura del sensor de CO2.
#define BPMPIN 26       //Pin del sensor de ritmo cardiaco.

/*Constantes*/
#define RADIO_RUEDA 0.21//[m]   //Radio de la rueda donde se coloca el encoder.
#define LECTURA_DISTANCIA 5000000//[us] = 5[s]  //Tiempo para medir distancia (y velocidad).

/*Definiciones para los tiempos.*/
#define TIEMPO_BPM 5000000//[us] = 5[s]         //Definición del tiempo para realizar la lectura del sensor de ritmo cardiaco.
#define TIEMPO_VALORES 50000//[us] = 50[ms]     //Definición del tiempo para realizar la lectura de los sensores.
#define TIEMPO_CORAZON 1000000//[us] = 1[s]     //Definición del tiempo para cambiar el estado del corazón.
#define TIEMPO_MAX_SLEEP 30000000//[us] = 30[s] //Definición del tiempo para que la pantalla entre en modo sleep.

/*Variables de entrada y salida*/
/*Distancia*/
float distanciaAngular = 0;                     //Distancia angular de la rueda.
float distancia = 0;//[km]                      Distancia lineal recorrida.
float distanciaAnterior = 0;//[km]              Distancia lineal anterior.
float velocidad = 0;//[km/s]                    Velocidad lineal.
int pulsosDistancia = 0;                        //Pulsaciones del encoder para medir la distancia.
int contadorDistancia = 0;                      //Variable para determinar si se debe hacer la medición de distancia en un momento dado.
/*BPM*/
int pulsosBPM = 0;      //Contador de pulsos para medir el ritmo cardiaco.
int bpmValor = 0;            //Variable para guardar el ritmo cardiaco.
/*Potenciometros*/
float co2Valor = 0;     //Variable para guardar el valor del sensor de CO2.
int voltValor = 0;      //Variable para guardar el valor del voltaje.
int bateriaValor = 0;   //Variable para guardar la carga de la batería.
/*Sleep*/
int tiempoActualSleep = 0;//[s] //Contador para determinar si el display entra en modo sleep.

/*Timer*/
hw_timer_t *temporizadorBPM = NULL;         //Apuntador al temporizador para el ritmo cardiaco.   
hw_timer_t *temporizadorValores = NULL;     //Apuntador al temporizador para leer los valores de los sensores.
hw_timer_t *temporizadorCorazon = NULL;     //Apuntador al temporizador para cambiar el estado del corazón.

/*Banderas*/
bool leerValores = false;                   //Bandera para leer los valores de los sensores. 
bool leerBPM = false;                       //Bandera para leer el ritmo cardiaco.
bool cambiarCorazon = false;                //Bandera para cambiar el estado del corazón.
bool corazonEncendido = true;               //Variable para determinar si el corazón se muestra o no.
bool pantallaSleep = false;                 //Variable para determinar si la pantalla está en modo sleep.


/*Funciones de interrupción*/

/** \fn void IRAM_ATTR InterrupcionBPM()
 * \brief Cambia a verdadera la bandera para leer el BPM. */
void IRAM_ATTR InterrupcionBPM(){
    leerBPM = true;
}

/** \fn void IRAM_ATTR InterrupcionLeerValores()
 * \brief Cambia a verdadera la bandera para leer los valores de los sensores. */
void IRAM_ATTR InterrupcionLeerValores(){
    leerValores = true;
}

/** \fn void IRAM_ATTR InterrupcionCambiarCorazon()
 * \brief Cambia a verdadera la bandera para cambiar la visibilidad del corazón. */
void IRAM_ATTR InterrupcionCambiarCorazon(){
    cambiarCorazon = true;
}

/** \fn void IRAM_ATTR InterrupcionPulsosBPM()
 * \brief Aumenta en uno el valor de los pulsos del sensor de ritmo cardiaco. */
void IRAM_ATTR InterrupcionPulsosBPM(){
    pulsosBPM ++;
}

/** \fn void IRAM_ATTR InterrupcionPulsosDistancia()
 * \brief Aumenta en uno el valor de los pulsos del encoder para distancia.
 * Manda a false la bandera para que el display entre en sleep.
 * Reinicia el contador para entrar en sleep.  */
void IRAM_ATTR InterrupcionPulsosDistancia(){
    pulsosDistancia ++;
    pantallaSleep = false;
    tiempoActualSleep = 0;
}

/*Funciones para hacer las lecturas de los sensores*/

/** \fn void LeerBPM()
 * \brief Cambia la bandera para leer el BPM a falso y hace la lectura del sensor de ritmo cardiaco. */
void LeerBPM(){
    leerBPM = false;
    bpmValor = pulsosBPM*(60000000/TIEMPO_BPM);
    pulsosBPM = 0;
}

/** \fn void LeerBPM()
 * \brief Cambia a falsa la bandera para leer los valores.
 * Determina si es necesario entrar en el modo sleep.
 * Calcula la velocidad, distancia y lee los valores de los sensores. */
void LeerValores(){
    leerValores = false;
    
    //Sleep
    tiempoActualSleep ++;
    if(tiempoActualSleep >= (TIEMPO_MAX_SLEEP/TIEMPO_VALORES)){
        pantallaSleep = true;
    }
    
    //Hacemos los cálculos de distancia y velocidad.
    contadorDistancia ++;
    if(contadorDistancia >= (LECTURA_DISTANCIA/TIEMPO_VALORES)){
        contadorDistancia = 0;
        distanciaAnterior = distancia;
        distancia += (pulsosDistancia*PI*2*RADIO_RUEDA)/1000;
        velocidad = ((distancia-distanciaAnterior)/(LECTURA_DISTANCIA/1000000))*3600;
        if(velocidad < 0){
            velocidad = 0;
        }
        pulsosDistancia = 0;
    }

    //Guardamos los valores de los sensores.
    voltValor = (int)(((float)analogRead(ANALOGPIN)/4095.00)*120);
    bateriaValor = (voltValor/120)*100;
    co2Valor = (float)(analogRead(CO2PIN)/4095.00)*9.9;
}

/*Funciones varias*/

/** \fn void CambiarCorazon()
 * \brief Cambia a falsa la bandera para cambiar el estado del corazón.
 * Cambia el estado del corazón. */
void CambiarCorazon(){
    corazonEncendido = !corazonEncendido;
    cambiarCorazon = false;   
}

/** \fn void InicializarTimers()
 * \brief Inicializa los temporizadores y les asigna las interrupciones. */
void InicializarTimers(){
    //Iniciamos los temporizadores a 1[MHz].
    temporizadorBPM = timerBegin(1000000);
    temporizadorValores = timerBegin(1000000);
    temporizadorCorazon = timerBegin(1000000);
    //Insertar las interrupciones a las funciones.
    timerAttachInterrupt(temporizadorBPM, &InterrupcionBPM);
    timerAttachInterrupt(temporizadorValores, &InterrupcionLeerValores);
    timerAttachInterrupt(temporizadorCorazon, &InterrupcionCambiarCorazon);
    timerAlarm(temporizadorBPM, TIEMPO_BPM, true, 0);           //Interrumpir cada X segundos con recarga.
    timerAlarm(temporizadorValores, TIEMPO_VALORES, true, 0);   // Interrumpir cada X segundos con recarga.
    timerAlarm(temporizadorCorazon, TIEMPO_CORAZON, true, 0);   // Interrumpir cada X segundos con recarga.
}

/** \fn void InicializarSensores()
 * \brief Inicializa los pines de entrada de los sensores
 * Añade las interrupciones a los pines. */
void InicializarSensores(){
    //Iniciamos los pines de entrada.
    pinMode(BPMPIN,INPUT_PULLDOWN);
    pinMode(ENCODERPIN, INPUT_PULLDOWN);
    //Insertar las interrupciones a las funciones.
    attachInterrupt(BPMPIN, &InterrupcionPulsosBPM, RISING);
    attachInterrupt(ENCODERPIN, &InterrupcionPulsosDistancia, RISING);
}

/** \fn float GetDistancia()
 * \brief Devuelve la distancia lineal recorrida.
 * \return Un float con el valor de la distancia lineal recorrida. */
float GetDistancia(){
    return distancia;
}

/** \fn float GetVelocidad()
 * \brief Devuelve la velocidad.
 * \return Un float con el valor de la distancia recorrida. */
float GetVelocidad(){
    return velocidad;
}

/** \fn int GetBPM()
 * \brief Devuelve el valor del ritmo cardiaco.
 * \return Un entero con el valor del ritmo cardiaco. */
int GetBPM(){
    return bpmValor;
}

/** \fn int GetAnalog()
 * \brief Devuelve el valor del sensor analógico.
 * \return Un entero con el valor del pin analógico. */
int GetAnalog(){
    return analogValor;
}

/** \fn float GetCO2()
 * \brief Devuelve el valor del sensor de CO2.
 * \return Un float con el valor del sensor de CO2. */
float GetCO2(){
    return co2Valor;
}

/** \fn bool GetLeerValores()
 * \brief Devuelve el valor de la bandera para leer valores.
 * \return Un bool con el valor de la bandera leerValores.
*/
bool GetLeerValores(){
    return leerValores;
}

/** \fn void SetLeerValores(bool activado)
 * \brief Asigna el valor a la bandera para leer valores.
 * \param activado El valor que se asignará a la bandera.
*/
void SetLeerValores(bool activado){
    leerValores = activado;
}

/** \fn bool GetLeerBPM()
 * \brief Devuelve el valor de la bandera para leer el ritmo cardiaco.
 * \return Un bool con el valor de la bandera leerBPM.
*/
bool GetLeerBPM(){
    return leerBPM;
}

/** \fn void SetLeerBPM(bool activado)
 * \brief Asigna el valor a la bandera para leer el ritmo cardiaco.
 * \param activado El valor que se asignará a la bandera.
*/
void SetLeerBPM(bool activado){
    leerBPM = activado;
}

/** \fn bool GetCambiarCorazon()
 * \brief Devuelve el valor de la bandera para cambiar el estado del corazón.
 * \return Un bool con el valor de la bandera cambiarCorazon.
*/
bool GetCambiarCorazon(){
    return cambiarCorazon;
}

/** \fn void SetCambiarCorazon(bool activado)
 * \brief Asigna el valor a la bandera para cambiar el estado del corazón..
 * \param activado El valor que se asignará a la bandera.
*/
void SetCambiarCorazon(bool activado){
    cambiarCorazon = activado;
}

/** \fn bool GetCorazonEncendido()
 * \brief Devuelve el valor de la bandera con el estado del corazón.
 * \return Un bool con el valor de la bandera corazonEncendido.
*/
bool GetCorazonEncendido(){
    return corazonEncendido;
}

/** \fn bool GetPantallaSleep()
 * \brief Devuelve el valor de la bandera con el estado de la pantalla.
 * \return Un bool con el valor de la bandera pantallaSleep.
*/
bool GetPantallaSleep(){
    return pantallaSleep;
}


/*Debugging*/

/** \fn void randomValues()
 * \brief Genera valores aleatorios para los valores de los sensores. */
void randomValues(){
    analogValor = random(0,99);
    bpmValor = random(60,200);
    co2Valor = random(0,99)/10.00;
    voltValor = random(0,120);
    bateriaValor = random(0,100);
}