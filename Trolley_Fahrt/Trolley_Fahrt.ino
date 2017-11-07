#include <Arduino.h>
#include <HardwareSerial.h>

//Konstanten: Pins
const int MOTOR_VL = 10;
const int MOTOR_VR = 11;
const int MOTOR_HL = 12;
const int MOTOR_HR = 13;

const int REED_IN = 9;
const int BEDINGUNG_IN = 8;
const int HINDERIS_1_TRIG = 5;
const int HINDERIS_1_ECHO = 4;
const int HINDERIS_2_TRIG = 6;
const int HINDERIS_2_ECHO = 7;
const int HINDERIS_3_TRIG = 2;
const int HINDERIS_3_ECHO = 3;

//Konstanten: Einstellungen
const int REIHENMENGE = 10;

//Status Variablen
boolean IstFahrt = true;
boolean IstBedient = false;
boolean IstWarten=false;
boolean IstHindernis=false;

//Werte Variablen
int ReedVal= 0;
unsigned long WartenEndeZeit;
int Reihenzaehler = 0;
int Hindernis_Entfernung=100;
int Hindernis_Entfernung2;

void setup() 
{
  //Pin Setup Motoren
pinMode (MOTOR_VR,OUTPUT);
pinMode (MOTOR_VL,OUTPUT);
pinMode (MOTOR_HR,OUTPUT);
pinMode (MOTOR_HL,OUTPUT);

// Pin Setup Sensoren
pinMode (REED_IN,INPUT);
pinMode (BEDINGUNG_IN,INPUT);
pinMode (HINDERIS_1_TRIG,OUTPUT);
pinMode (HINDERIS_1_ECHO,INPUT);
pinMode (HINDERIS_2_TRIG,OUTPUT);
pinMode (HINDERIS_2_ECHO,INPUT);
pinMode (HINDERIS_3_TRIG,OUTPUT);
pinMode (HINDERIS_3_ECHO,INPUT);

// Motoren Ausschalten 
digitalWrite (MOTOR_VR,HIGH);
digitalWrite (MOTOR_VL,HIGH);
digitalWrite (MOTOR_HR,HIGH);
digitalWrite (MOTOR_HL,HIGH);

//Serial Setup
Serial.begin(9600);

}

void loop() {
//******    Sensoren Auswertung    ******

//** Reed Sensor
ReedVal = digitalRead (REED_IN);
Serial.println(ReedVal);

//** Bedingungssensor
//IstBedient= (digitalRead(BEDINGUNG_IN)==1);

//** 3 Ultraschalsensoren
Hindernis_Entfernung = GetDistance (HINDERIS_1_TRIG,HINDERIS_1_ECHO);
Hindernis_Entfernung2 = GetDistance (HINDERIS_2_TRIG,HINDERIS_2_ECHO);
if (Hindernis_Entfernung > Hindernis_Entfernung2)
  Hindernis_Entfernung = Hindernis_Entfernung2;
Hindernis_Entfernung2 = GetDistance (HINDERIS_3_TRIG,HINDERIS_3_ECHO);
if (Hindernis_Entfernung > Hindernis_Entfernung2)
  Hindernis_Entfernung = Hindernis_Entfernung2;
  
if (Hindernis_Entfernung < 20)
{
  IstHindernis = true;
}



//******    Aktionen    ******

//** Aktion: Bedingung
//** Priorität: 1
if (IstBedient)
{
  Serial.println("Wird bedient");
     digitalWrite (MOTOR_VR,HIGH);
     digitalWrite (MOTOR_VL,HIGH);
  
}


//** Aktion: Hindernis
//** Priorität: 2
else if (IstHindernis)
{  Serial.println("Hindernis: " + String(Hindernis_Entfernung));
  digitalWrite (MOTOR_VR,HIGH);
     digitalWrite (MOTOR_VL,HIGH);
if (Hindernis_Entfernung > 30)
{
  IstHindernis = false;
}}
else 
{

  
  //** Aktion: Fahrt: Schritt Fahrt in die nächste Reihe
  //** Priorität: 3
  if (IstFahrt)
  {
    if (ReedVal==0)
    {
      Serial.println("Fahrt");
      digitalWrite (MOTOR_VR,LOW);
      digitalWrite (MOTOR_VL,LOW);
    }
    else
    {
      Serial.println("Fahrt Ende");
      digitalWrite (MOTOR_VR,HIGH);
      digitalWrite (MOTOR_VL,HIGH);
      IstFahrt=false;
      IstWarten=true;
      WartenEndeZeit=millis()+10000;
      Reihenzaehler++;
    }
    
  }


  //** Aktion: Warten: 10 Sekunden fürs Selbstbedingung
  //** Priorität: 3
  if (IstWarten)
  {
    Serial.println("Warten, Reihe: " + String(Reihenzaehler));
    if (WartenEndeZeit <= millis())
    {
     Serial.println("Warten Ende");
     IstWarten=false;
     if (Reihenzaehler <= REIHENMENGE){  
     IstFahrt=true;
     while (digitalRead (REED_IN)==1)
     {
      digitalWrite (MOTOR_VR,LOW);
      digitalWrite (MOTOR_VL,LOW); 
     }
      digitalWrite (MOTOR_VR,HIGH);
      digitalWrite (MOTOR_VL,HIGH);
     }
    }
  }
}


}

//**Hilfsfunktionen für Motoren

//** Hilfsfunction: GetDistance - Ultraschalsensor Auslesen und die Entfernung in cm Ermitteln
int GetDistance(int trigPin, int echoPin)
{  
    long duration;
    int distance;
    
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    
    // Calculating the distance
    distance= duration*0.034/2;  
    
    return distance;
}
