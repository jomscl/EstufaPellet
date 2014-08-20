// includes
#include <debug.h>

// para arduino pro mini 16mhz
#define fIn 2 // pin fisico 2, interrupcion 0
#define fOut 9
#define pot A3
#define led 13

//constantes
#define ledOn 255
#define ledOff 255
#define ledMid 100
#define maxRecorte 700

// variables
 unsigned long tPulso=0;
 unsigned long tInicio;
 unsigned long tCorte=0;

void setup() {
  // Configuración HW
  pinMode(fIn, INPUT_PULLUP);
  pinMode(fOut, OUTPUT);
  pinMode(pot, INPUT);
  pinMode(led, OUTPUT);
  
  // estado inicial
  digitalWrite(fOut, LOW);
  analogWrite(led,ledOn);
  delay(500);
  analogWrite(led,ledMid);
  delay(500);
  analogWrite(led,ledOff);
  
  // debug
  Serial.begin(9600);
  Serial.println("Sistema encendido");
  
  // interrupcion
  //attachInterrupt(0, atiendeFeeder, CHANGE);
}

void loop() {
  // solo se valida que el timer no se haya agotado, razon para apagar el feeder
  if (digitalRead(fOut)==HIGH){
    if (tPulso>0 && tCorte>0){
      if (tCorte>=millis()){
        // en este punto, se debe apagar el feeder
        analogWrite(led,ledMid);
        digitalWrite(fOut,LOW);
        DEBUGLN("Apagado por timer");
      }
    }       
  }
}

void atiendeFeeder(){
  if (digitalRead(fIn==HIGH)){
    // manejo de variables y salidas
    analogWrite(led,ledOn);
    tInicio=millis();
    digitalWrite(fOut,HIGH);
    
    // calculo del instante del proximo corte, basado en el pulso anterior
    tCorte=millis();
    int vpot=analogRead(pot);
    // restricciones a la medición
    vpot = constrain(vpot, 0, maxRecorte);
    
    if (tPulso>0){
        tCorte+=map(vpot, 0, 1024, tPulso, 0);
    }else{
      tCorte+=20000;
    }
    DEBUGLN("Encendido por flanco de subida");
  }else{
    // revisar si todavía esta prendido
    if (digitalRead(fOut)==HIGH){
      digitalWrite(fOut,LOW);
      DEBUG("Apagado por flanco de Bajada");
    }
    analogWrite(led,ledOff);
    // calculo del tamaño del pulso
    tPulso=millis()-tInicio;
    DEBUG("Pulso=");
    DEBUGLN(tPulso);
  } // fin if lectura del pulso de entrada
}
