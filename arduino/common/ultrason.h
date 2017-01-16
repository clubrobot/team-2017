#ifndef __SENSOR_H__
#define __SENSOR_H__

#define VALMIN 2
#define VALMAX 200
#define TEMPORISATION 3600

enum State {EMISSION = 0, RECEPTION = 1, ATTENTE = 2, PRET = 3};

class Sensor {
  public :
    
    Sensor(int echo);
    void impulsion_US(int pin);
    void setcurrentState(State st ) { currentState = st;}
    int getDistance() { return mesure;}
    State getState() { return currentState;}
    void update();
     
    //Fonction renvoyant la valeur en cm correspondant au temps d'un aller-retour de l'onde, en microsecondes.
    unsigned long ConversionCm(unsigned long debut, unsigned long fin) {
      return (fin - debut) * 343 / 20000;
    }

  private :
    
    const int EchoPin;  
    unsigned long debutMesure;
    unsigned long finMesure;
    unsigned long currentTime;
    unsigned long dureeMesure;
    int mesure;

    State currentState;
    /* Définition de la variable d'état "currentState" et des différents états :
      EMISSION   : Demarrage d'une nouvelle mesure
      RECEPTION  : Mesure du temps nécessaire à l'onde pour parcourir un aller-retour entre l'objet et le capteur
      ATTENTE    : Temps d'attente entre deux mesures (évite des interférences)
      PRET       : Pret à effectuer une nouvelle mesure
    */
};
#endif
