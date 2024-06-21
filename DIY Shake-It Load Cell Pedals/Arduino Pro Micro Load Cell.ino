#include <HX711.h>
#include <Joystick.h>

const uint8_t DATA_PIN = 5;  
const uint8_t CLOCK_PIN = 6; 

HX711 scale;


const float minWeight = 0;  // Peso minimo
const float maxWeight = 900; // Peso massimo, valore regolabile

Joystick_ Joystick;

void setup() {
  Serial.begin(9600); // Inizializzazione della comunicazione seriale
  Joystick.begin();
  scale.begin(DATA_PIN, CLOCK_PIN); // Inizializzazione della cella di cagrico
  scale.tare();
  scale.set_scale(46000);
}

void loop() {
  float weight = abs(scale.get_units(1) * 100);
  weight = constrain(weight, minWeight, maxWeight); // Limita il valore del peso

  int joystickValue = map(weight, minWeight, maxWeight, 0, 1023); // Mappatura del peso sul valore del joystick
  //Joystick.setThrottleRange(0, 255);
  Joystick.setThrottle(joystickValue);
  
  // Stampa su seriale ciò che avviene
  Serial.print("Peso: ");
  Serial.print(weight);
  Serial.print("\tJoystick Value: ");
  Serial.println(joystickValue);
  
  delay(1); // Aggiungi un piccolo ritardo per evitare letture troppo frequenti
}
