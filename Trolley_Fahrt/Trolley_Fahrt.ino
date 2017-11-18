//Konstanten: Pins
const int MOTOR_VL = 10; 
const int MOTOR_VR = 11; 
const int MOTOR_HL = 12;
const int MOTOR_HR = 13;

const int REED_IN = 9;
const int BEDINGUNG_IN = 8;
const int analogInPin = A0;

const int HINDERIS_1_TRIG = 5;
const int HINDERIS_1_ECHO = 4;
const int HINDERIS_2_TRIG = 6;
const int HINDERIS_2_ECHO = 7;
const int HINDERIS_3_TRIG = 2;
const int HINDERIS_3_ECHO = 3;

//Konstanten: Einstellungen
const int REIHENMENGE = 2;
const int WARTEZEITBEDINGUNG=10000;

//Status Variablen
boolean IstFahrt = false;
boolean IstBedient = false;
boolean IstWarten=false;
boolean IstHindernis=false;
boolean IstFahrtZurueck=false;

//Werte Variablen
int ReedVal= 0;
unsigned long WartenEndeZeit;
int Reihenzaehler = 0;
int Hindernis_Entfernung=100;
int Hindernis_Entfernung2;
int Starttaste_out = 0;

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
Motoren_Aus();

//Serial Setup
Serial.begin(9600);

}

void loop() {
//******    Sensoren Auswertung    ******
//Warten auf Starttaste
Starttaste_out = analogRead(analogInPin);
if(!IstFahrt && !IstFahrtZurueck && !IstWarten)
{
  while(Starttaste_out < 100) 
  {
    Starttaste_out = analogRead(analogInPin);
    Serial.println("Warten auf Reset: " + String(Starttaste_out));
    delay(10);
  }
  delay(200);
  Reed_Raus(true);
  IstFahrt = true;
}
else
{
  
     if (Starttaste_out>100)
     {
          Serial.println("Reset!!!");
          IstFahrtZurueck = false;
          IstWarten = false;
          IstFahrt = false;
          Motoren_Aus();
          delay(1000);
      }
}


//** Reed Sensor
ReedVal = digitalRead (REED_IN);
Serial.println("Reed: " + String(ReedVal));

//** Bedingungssensor
IstBedient= (digitalRead(BEDINGUNG_IN)==1);

//** 3 Ultraschalsensoren
Hindernis_Entfernung = GetDistance (HINDERIS_1_TRIG,HINDERIS_1_ECHO);
Hindernis_Entfernung2 = GetDistance (HINDERIS_2_TRIG,HINDERIS_2_ECHO);
if (Hindernis_Entfernung > Hindernis_Entfernung2)
  Hindernis_Entfernung = Hindernis_Entfernung2;
Hindernis_Entfernung2 = GetDistance (HINDERIS_3_TRIG,HINDERIS_3_ECHO);
if (Hindernis_Entfernung > Hindernis_Entfernung2)
  Hindernis_Entfernung = Hindernis_Entfernung2;
  
if (Hindernis_Entfernung < 50)
{
  IstHindernis = true;
}



//******    Aktionen    ******

//** Aktion: Bedingung
//** Priorität: 1
if (IstBedient)
{
  Serial.println("Wird bedient");
  Motoren_Aus();
  
}


//** Aktion: Hindernis
//** Priorität: 2
else if (IstHindernis)
{  
  Serial.println("Hindernis: " + String(Hindernis_Entfernung));
  Motoren_Aus();
  if (Hindernis_Entfernung > 80)
  {
    IstHindernis = false;
  }
}
else 
{

  
  
  if (!IstFahrtZurueck)
  {  
  //** Aktion: Fahrt: Schritt Fahrt in die nächste Reihe
  //** Priorität: 3
    if (IstFahrt)
    {
      if (ReedVal==0)
      {
        Serial.println("Fahrt, Reihe: " + String(Reihenzaehler));
        Motoren_Vorwaerts();
      }
      else
      {
        Serial.println("Fahrt Ende");
        
        Motoren_Aus();
        
        IstFahrt=false;
        IstWarten=true;
        WartenEndeZeit=millis()+WARTEZEITBEDINGUNG;
        Reihenzaehler++;
      }
      
    }
  
  
    //** Aktion: Warten: 10 Sekunden fürs Selbstbedingung
    //** Priorität: 3
    if (IstWarten)
    {
      Serial.println("Warten, Reihe: " + String(Reihenzaehler));
      Motoren_Aus();
      if (WartenEndeZeit <= millis())
      {
       Serial.println("Warten Ende");
       IstWarten=false;
       if (Reihenzaehler < REIHENMENGE){  
        IstFahrt=true;
        Reed_Raus(true);
       }
       else
       {
         IstFahrtZurueck=true;
         IstWarten=false;
         IstFahrt=false;
          Serial.println("Starte Fahrt Zurück");
          Reed_Raus(false);
       }
      }
    }
  }
  else
  {
     //** Aktion: Fahrt Zurück: zur Start-Reihe
     //** Priorität: 3
      if (ReedVal==0)
      {
        Serial.println("Fahrt Zurück Reihe: " + String(Reihenzaehler));
        Motoren_Rueckwaerts();
      }
      else
      {
        Reihenzaehler--;
        if (Reihenzaehler == 0)
        {
          Serial.println("Fahrt Zurück Fertig");
          IstFahrtZurueck = false;
          IstWarten = false;
          IstFahrt = false;
          Motoren_Aus();
        }
       else
       {
         Reed_Raus(false);
       } 
      }
      
    
  }

}


}

//**Hilfsfunktionen für Motoren

void Motoren_Aus()
{
digitalWrite (MOTOR_VR,HIGH);
digitalWrite (MOTOR_VL,HIGH);
digitalWrite (MOTOR_HR,HIGH);
digitalWrite (MOTOR_HL,HIGH);
}

void Motoren_Vorwaerts()
{
 digitalWrite (MOTOR_VR,LOW);
 digitalWrite (MOTOR_VL,LOW);
 
 digitalWrite (MOTOR_HR,HIGH);
 digitalWrite (MOTOR_HL,HIGH);
}


void Motoren_Rueckwaerts()
{
 digitalWrite (MOTOR_HR,LOW);
 digitalWrite (MOTOR_HL,LOW);
 
 digitalWrite (MOTOR_VR,HIGH);
 digitalWrite (MOTOR_VL,HIGH);
}

//Fahren aus Reed Sensor mit Rauschfilter
void Reed_Raus(boolean parVorwaerts)
{
  unsigned long debounceDelay = 500;
  unsigned long lastDebounceTime = millis();

  while ((millis() - lastDebounceTime) < debounceDelay)
  {
    int iReed=digitalRead (REED_IN)==1;
   
      
      
    if (parVorwaerts)
    {
      Serial.println("Reed Raus Vorwärts, Schon ms: " + String((millis() - lastDebounceTime)) + " Reed Wert: " + String(iReed));

      Motoren_Vorwaerts();
    }
    else
    {
       Serial.println("Reed Raus Rückwärts, Schon ms: " + String((millis() - lastDebounceTime)) + " Reed Wert: " + String(iReed));

      Motoren_Rueckwaerts();
    }
    if (iReed==1)
      lastDebounceTime = millis();
  }
Motoren_Aus();
}


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