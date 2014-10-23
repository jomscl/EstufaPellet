// includes
//#include <debug.h>
#include <Metro.h>

// para arduino pro mini 16mhz
#define fIn 2 // pin fisico 2, interrupcion 0
#define fOut 9 // salida al triac
#define pot A3
#define leds 13  // led estado
#define led 11 // led indicador del feeder

#define MODO_DEBUG 

#ifdef MODO_DEBUG
  #define DEBUG(x)  Serial.print (x)
  #define DEBUGLN(x)  Serial.println (x)
  #define DEBUGDEC(x)  Serial.print (x, DEC)
  #define DEBUGW(x)  Serial.write (x)
  #define DEBUGFULL(x)    \
    Serial.print(millis());     \
    Serial.print(": ");    \
    Serial.print(__PRETTY_FUNCTION__); \
    Serial.print(' ');      \
    Serial.print(__FILE__);     \
    Serial.print(':');      \
    Serial.print(__LINE__);     \
    Serial.print(' ');      \
    Serial.println(x);
#else
  #define DEBUG(x)
  #define DEBUGLN(x) 
  #define DEBUGDEC(x)
  #define DEBUGW(x) 
  #define DEBUGFULL(x)
#endif

//constantes
#define maxRecorte 700

// variables
volatile unsigned long tPulso=0;
volatile unsigned long tInicio;
volatile unsigned long tCorte=0;

Metro ledMetro = Metro(250);  

void setup() {
  // Configuración HW
  pinMode(fIn, INPUT_PULLUP);
  pinMode(fOut, OUTPUT);
  pinMode(pot, INPUT);
  pinMode(led, OUTPUT);
  pinMode(leds, OUTPUT);
    
  // estado inicial
  digitalWrite(fOut, LOW);
  digitalWrite(led,LOW);
  digitalWrite(leds,LOW);
  
  // debug
  Serial.begin(9600);
  DEBUGLN("Sistema encendido");
  
  // interrupcion
  attachInterrupt(0, atiendeFeeder, CHANGE);
  interrupts();
}

void loop() {
  // solo se valida que el timer no se haya agotado, razon para apagar el feeder
  if (digitalRead(fOut)==HIGH){
    if (tPulso>0 && tCorte>0){
      if (tCorte>=millis()){
        // en este punto, se debe apagar el feeder
        digitalWrite(led,LOW);
        digitalWrite(fOut,LOW);
        DEBUGLN("Apagado por timer");
      }
    }       
  }
  
  if (ledMetro.check() == 1){
    digitalWrite(leds,!digitalRead(leds));  
  }
}

void atiendeFeeder(){
  if (digitalRead(fIn==HIGH)){
    // manejo de variables y salidas
    digitalWrite(led,HIGH);
    tInicio=millis();
    digitalWrite(fOut,HIGH);
    
    // calculo del instante del proximo corte, basado en el pulso anterior
    tCorte=millis();
    int vpot=analogRead(pot);
    DEBUGDEC(vpot);
    // restricciones a la medición
    vpot = constrain(vpot, 0, maxRecorte);
    DEBUGDEC(vpot);
    
    if (tPulso>0){
        tCorte+=map(vpot, 0, 1024, tPulso, 0);
    }else{
      tCorte+=20000;
    }
    DEBUGLN(" Encendido por flanco de subida");
  }else{
    // revisar si todavía esta prendido
    if (digitalRead(fOut)==HIGH){
      digitalWrite(fOut,LOW);
      DEBUG(" Apagado por flanco de Bajada");
    }
    digitalWrite(led,LOW);
    // calculo del tamaño del pulso
    tPulso=millis()-tInicio;
    DEBUG(" Pulso=");
    DEBUGLN(tPulso);
  } // fin if lectura del pulso de entrada
}
