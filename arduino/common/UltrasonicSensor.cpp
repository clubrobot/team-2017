#include "UltrasonicSensor.h"

#define VALMIN 2
#define VALMAX 200
#define TEMPORISATION 3600

UltrasonicSensor::UltrasonicSensor(int ep): EchoPin(ep)
{
  pinMode(ep, INPUT);
  debutMesure = 0;
  finMesure = 0;
  currentTime = 0;
  mesure = 1000;
  currentState = PRET;
}

void UltrasonicSensor::impulsion_US(int pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);           // Permet de s'assurer que le pin est à l'état bas
  delayMicroseconds(2);
  digitalWrite(pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pin, LOW);
  delayMicroseconds(416);           // Temps nécessaire pour avoir un signal "haut" sur la sortie
}


void UltrasonicSensor::update() {
 unsigned long cm;
  switch (currentState) {
    case EMISSION :
      /*code EMISSION : Emission du signal par le module SR04 (ultrason)
        et début de la mesure
      */
      debutMesure = micros();               // Mesure du temps à l'émission de l'onde
      currentState = RECEPTION;                    // Changement d'état
      break;

    case RECEPTION :
      /*code RECEPTION : Reception du signal, mesure du temps d'aller-retour,
        calcul de la distance correspondante, vérification de la cohérence
      */
      if (digitalRead(EchoPin) == LOW) {             // Attente d'avoir un signal bas sur la sortie echo => retour de l'onde
        finMesure = micros();                         // Mesure du temps marquant la fin de la mesure
        cm = ConversionCm(debutMesure, finMesure);    // Calcul de la distane correspondante

        if (cm > VALMIN) {
          currentState = ATTENTE;                    // Ne prends pas en compte les valeurs inférieures à VALMIN, portée minimale
          mesure = cm;
        }
      }

      currentTime = micros();                         // Arrêt de la mesure au dela d'une distance supérieure à VALMAX
      if ((ConversionCm(debutMesure, currentTime) > VALMAX) && (digitalRead(EchoPin) == HIGH)){
        currentState = ATTENTE;
        mesure = -1;
      }
      break;

    case ATTENTE :
      //code ATTENTE : Temporisation entre deux mesures
      currentTime = micros();

      if (((currentTime - finMesure) >= TEMPORISATION) || (digitalRead(EchoPin) == LOW)) currentState = PRET;
      break;
  }

}
