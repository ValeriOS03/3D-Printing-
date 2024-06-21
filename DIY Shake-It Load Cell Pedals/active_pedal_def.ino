#include <AS5600.h>
#include <HX711.h>
#include <Joystick.h>
#include <Wire.h>

// Pin configurazione per HX711
const uint8_t DATA_PIN = 5;  // Cambiato da 2 a 5
const uint8_t CLOCK_PIN = 6; // Cambiato da 3 a 6

HX711 scale;

// Pin configurazione per BTS7960
#define RPWM 9
#define LPWM 10

// Pin configurazione per Endstop
#define ENDSTOP_PIN 4

// Pin configurazione per input analogico
#define ANALOG_INPUT_PIN A0

AS5600 encoder;

// Parametri del PID
float kp = 5.0;
float ki = 0.1;
float kd = 1.0;

int previousError = 0;
float integral = 0;

float targetForce = 1;
float forceSensitivity = 10;

const int stepsPerRevolution = 4096;
const float leadScrewPitch = 4.0;
const float maxTravel = 70.0; // Lunghezza massima dell'asse

const float forceDeadzone = 5.0; // Imposta la zona morta per la forza

const float scaleFactor = 1.5; // Fattore di scala per il calcolo della forza target

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_GAMEPAD, 0, 0, true, false, false, false, false, false, false, false, false, false, false);

void setup() {
  Wire.begin();
  Serial.begin(9600);
  scale.tare();

  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
  pinMode(ENDSTOP_PIN, INPUT_PULLUP);

  if (!encoder.begin(4)) {
    Serial.println("AS5600 non trovato!");
    while (1);
  }

  scale.begin(DATA_PIN, CLOCK_PIN);

  findHome();

  Joystick.begin();

  Serial.println("Setup completato!");
}

void loop() {
    // Leggi la forza effettiva esercitata sul pedale
    float force = scale.get_units(10);

    // Calcola la posizione del pedale
    int currentAngle = encoder.getCumulativePosition();
    float position = map(currentAngle, 0, stepsPerRevolution, 0, maxTravel);

    // Calcola la forza target in base alla posizione del pedale
    targetForce = calculateTargetForce(position);

    // Calcola l'errore di forza
    float forceError = targetForce - force;

    // Calcola la direzione dell'errore di forza
    int errorDirection = (forceError > 0) ? 1 : -1;

    // Calcola l'uscita del PID per controllare il motore
    int output = kp * forceError + ki * integral + kd * (forceError - previousError);
    integral += forceError;
    previousError = forceError;

    // Aggiungere resistenza al limite superiore dell'asse (80mm)
    if (position >= maxTravel && output > 0) {
        output = 0; // Impedisce al motore di muoversi oltre il limite superiore
    }

    output = constrain(output, -255, 255);

    // Controlla la direzione del motore in base all'uscita del PID
    if (output > 0) {
        analogWrite(RPWM, output);
        analogWrite(LPWM, 0);
    } else if (output < 0) {
        analogWrite(RPWM, 0);
        analogWrite(LPWM, -output);
    } else {
        analogWrite(RPWM, 0);
        analogWrite(LPWM, 0);
    }

    // Mappa la posizione del pedale per l'uso con il joystick
    int joystickValue = map(position, 0, maxTravel, 0, 1023);
    Joystick.setThrottle(joystickValue);

    // Leggi l'intensità della vibrazione dall'input analogico
    int vibrationInput = analogRead(ANALOG_INPUT_PIN);
    int vibrationIntensity = map(vibrationInput, 0, 1023, 0, 255);

    // Se l'intensità della vibrazione è maggiore di zero, crea la vibrazione
    if (vibrationIntensity > 0) {
        createVibration(vibrationIntensity);
    }

    // Se la direzione dell'errore cambia, muovi il motore nella direzione opposta
    if (forceError * errorDirection < 0) {
        // Inverti la direzione del motore
        output *= -1;
        // Assicurati che l'output sia all'interno dei limiti
        output = constrain(output, -255, 255);
        // Applica l'output ai pin del motore
        if (output > 0) {
            analogWrite(RPWM, output);
            analogWrite(LPWM, 0);
        } else if (output < 0) {
            analogWrite(RPWM, 0);
            analogWrite(LPWM, -output);
        } else {
            analogWrite(RPWM, 0);
            analogWrite(LPWM, 0);
        }
    }
}



float calculateTargetForce(float position) {
  // Calcola la forza target in base alla posizione del pedale
  // Ad esempio, la forza target aumenta linearmente in base alla posizione
  return position * scaleFactor; // scaleFactor è un valore che determina il tasso di aumento della forza target
}

void findHome() {
  // Controllo se il pedale è già in posizione home
  while (digitalRead(ENDSTOP_PIN) == LOW) {
    analogWrite(RPWM, 50);
    analogWrite(LPWM, 0 ); // Muovi nella direzione opposta
  }

  analogWrite(RPWM, 0);
  analogWrite(LPWM, 0);

  // Memorizza la posizione massima (endstop)
  int maxPosition = encoder.getCumulativePosition(); 

  // Calcola la posizione iniziale (80 mm indietro rispetto alla posizione massima)
  int stepsToInitialPosition = maxTravel / leadScrewPitch * stepsPerRevolution;
  int initialPosition = maxPosition - stepsToInitialPosition;

  // Muovi il motore alla posizione iniziale
  moveMotorToPosition(initialPosition, stepsToInitialPosition); // usa la posizione iniziale come riferimento
}


void moveMotorToPosition(int targetAngle, int steps) {
  int currentAngle = encoder.getCumulativePosition();
  while (abs(currentAngle - targetAngle) > 1) {
    int direction = (currentAngle > targetAngle) ? -1 : 1;
    int pwmValue = 255 * direction;
    if (direction > 0) {
      analogWrite(RPWM, pwmValue);
      analogWrite(LPWM, 0);
    } else {
      analogWrite(RPWM, 0);
      analogWrite(LPWM, -pwmValue);
    }

    currentAngle = encoder.getCumulativePosition();
    
  }

  analogWrite(RPWM, 0);
  analogWrite(LPWM, 0);
}
void createVibration(int intensity) {
  // Mappare l'intensità su un valore PWM
  int pwmValue = map(intensity, 0, 255, 0, 255); // Mappare l'intensità su un valore PWM massimo

  // Ottenere l'angolo corrente dell'encoder
  int currentAngle = encoder.getCumulativePosition();
  
  // Calcolare i target di vibrazione senza eccedere i limiti dell'asse
  int vibrationAmplitude = 1000; // Ampiezza fissa della vibrazione
  int targetAngle1 = constrain(currentAngle + vibrationAmplitude, 0, stepsPerRevolution);
  int targetAngle2 = constrain(currentAngle - vibrationAmplitude, 0, stepsPerRevolution);

  // Eseguire la vibrazione muovendo avanti e indietro tra i target
  for (int i = 0; i < 5; i++) {
    // Muovi verso il primo target con intensità regolata
    moveMotorToPositionWithPWM(targetAngle1, pwmValue, currentAngle);
    // Muovi verso il secondo target con intensità regolata
    moveMotorToPositionWithPWM(targetAngle2, pwmValue, currentAngle);
  }
}

void moveMotorToPositionWithPWM(int targetAngle, int pwmValue, int currentAngle) {
  while (abs(currentAngle - targetAngle) > 1) {
    int direction = (currentAngle > targetAngle) ? -1 : 1;
    int adjustedPWMValue = pwmValue * direction;
    if (direction > 0) {
      analogWrite(RPWM, adjustedPWMValue);
      analogWrite(LPWM, 0);
    } else {
      analogWrite(RPWM, 0);
      analogWrite(LPWM, -adjustedPWMValue);
    }

    currentAngle = encoder.getCumulativePosition();
    
  }

  analogWrite(RPWM, 0);
  analogWrite(LPWM, 0);
}
