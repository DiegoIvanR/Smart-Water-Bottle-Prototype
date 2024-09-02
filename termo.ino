#include <Adafruit_Sensor.h>

#include <DHT.h>
#include <DHT_U.h>

#include <TM1637Display.h>
#include <Servo.h>

//display
#define CLK 11
#define DIO 10
TM1637Display display = TM1637Display(CLK, DIO);
//ultrasonico
const int Trigger = 7;
const int Echo = 6; //Pin digital 6 para el Echo del sensor
int displayInt;
bool nigga = false;
//touch
#define BUTTON_PIN 12

#define toggleButton 13

const uint8_t celsius[] = {
  SEG_A | SEG_B | SEG_F | SEG_G,  // Circle
  SEG_A | SEG_D | SEG_E | SEG_F   // C
};

const uint8_t P[] = {
  SEG_A | SEG_B | SEG_E | SEG_F | SEG_G   // 
};

enum DisplayState {
  distance,
  temperature
};

DisplayState state;

//temp
const int DHTPin = 5;
DHT dht(DHTPin, 11);
//servo
const int pinServo = 9;
Servo servoMotor;

void setup() {
    Serial.begin(9600);//iniciailzamos la comunicación
    pinMode(Trigger, OUTPUT); //pin como salida
    pinMode(Echo, INPUT); //pin como entrada
    digitalWrite(Trigger, LOW);//Inicializamos el pin con 0
     
     //temp humedad
    Serial.println("DHTxx test!");
    dht.begin();
    
    //display
    display.clear();
    display.setBrightness(7);
    
    //touch
    pinMode(BUTTON_PIN, INPUT); 
    
    //servo
    servoMotor.attach(pinServo);  

    // toggle button
    pinMode(toggleButton, INPUT_PULLUP);
    state = distance;

}
void loop() {
      long d = readUltrasonic();
      
      //temp
      delay(200);
    
//    if(contador >= 20){
      float h = dht.readHumidity();
      float temp = dht.readTemperature();
      if (isnan(h) || isnan(temp)) {
        Serial.println("Error en medicion");
        return;
      }
      Serial.print("Humedad: ");
      Serial.print(h);
      Serial.print(" %\t");
      Serial.print("Temperature: ");
      Serial.print(temp);
      Serial.print(" *C ");
      //contador = 0;
    //} else if(contador<20){
    //    contador +=1;
    //}
    moveServo();
    //display
    if(state == distance) {
        displayDistance(d);
        Serial.println("distance");
    } else if(state == temperature) {
      displayTemperature(temp);
      Serial.println("temp");
    }
    
    
    readButton();
}

void readButton() {
  if (digitalRead(toggleButton) == HIGH && nigga == false) {
    display.clear();
      switch(state) {
        case distance:
          state = temperature;
          Serial.println("niggabitch dis-temp");

          break;
        case temperature:
          state = distance;
          Serial.println("niggabitch temp-dis");
          break;
      }
    nigga = true;
  }
  else if(digitalRead(toggleButton) == LOW && nigga == true){
    nigga = false;
  }
}

long readUltrasonic() {
    long t; //timepo que demora en llegar el eco
    long d; //distancia en centimetros
    digitalWrite(Trigger, HIGH);
    delayMicroseconds(10); //Enviamos un pulso de 10us
    digitalWrite(Trigger, LOW);
    t = pulseIn(Echo, HIGH); //obtenemos el ancho del pulso
    d = t/59; //escalamos el tiempo a una distancia en cm
    return d;
}

void moveServo() {
  if(digitalRead(BUTTON_PIN)==HIGH){
    servoMotor.write(0);
  } else {
      servoMotor.write(90);
  }
}

void displayDistance(long d) {
  if(d> 15){
      d = 15;
    } else if(d<2){
      d = 2;
    }
  int porcentaje = map(d, 2, 15, 99, 0);
    
    display.showNumberDecEx(porcentaje, 0, false, 2, 0);
    display.setSegments(P, 1, 3);
}

void displayTemperature(float t) {
  display.showNumberDecEx(t, 0, false, 2, 0);
  display.setSegments(celsius, 2, 2);
}

bool isTouchPressed(int pin) 
{ 
   return digitalRead(pin) == HIGH; 
}
