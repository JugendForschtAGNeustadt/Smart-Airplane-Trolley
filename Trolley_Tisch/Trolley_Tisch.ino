#include <SoftwareSerial.h>

char junk;
String inputString = "";
SoftwareSerial MeinSerial(10, 11); //RX TX
int motorXrechts = 2;
int motorXlinks = 3;
int motorYrechts = 4;
int motorYlinks = 5;
char inChar;
void setup() {
  MeinSerial.begin(9600);
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  pinMode(motorXrechts, OUTPUT);
  pinMode(motorXlinks, OUTPUT);
  pinMode(motorYrechts, OUTPUT);
  pinMode(motorYlinks, OUTPUT);
//HIGH ist der AUS-Zustand
  digitalWrite(motorXlinks, HIGH);
  digitalWrite(motorXrechts, HIGH);
  digitalWrite(motorYlinks, HIGH);
  digitalWrite(motorYrechts, HIGH);
}
void loop() {
  if (MeinSerial.available() > 0) {
    inChar = (char)MeinSerial.read();
    Serial.print(inChar);
    if (inChar == 'A') {
      //MotorX rechts
      digitalWrite(motorXlinks, HIGH);
      digitalWrite(motorXrechts, LOW);
      // delay(500);
    } else if (inChar == 'B') {
      //MotorX links
      digitalWrite(motorXrechts, HIGH);
      digitalWrite(motorXlinks, LOW);
      // delay(500);
    } else if (inChar == 'C') {
      //MotorY rechts
      digitalWrite(motorYlinks, HIGH);
      digitalWrite(motorYrechts, LOW);
      // delay(500);
    } else if (inChar == 'D') {
      //MotorY links
      digitalWrite(motorYrechts, HIGH);
      digitalWrite(motorYlinks, LOW);
    }
    else if (inChar == 'X') {
      //X-Achse innerhalb der Null-Lage
      digitalWrite(motorXlinks, HIGH);
      digitalWrite(motorXrechts, HIGH);
    }
    else if (inChar == 'Y') {
      //Y-Achse innerhalb der Null-Lage
      digitalWrite(motorYlinks, HIGH);
      digitalWrite(motorYrechts, HIGH);
    }
  }
}