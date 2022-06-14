#include <NewPing.h> //includes New ping Library in code, simplifying usage of Trig/ Echo to usage of 1 I/O Pin

#include <Wire.h> //Includes Wire Library to enable Arduino to communicate using I2C Bus

#define SLAVE_ADDR 9 //Defines Slave to an I2C Address 

#define TRIGGER_PIN 8//Defines both the Trigger and Echo Pins to same port on Arduino board  
#define ECHO_PIN    8 

#define MAX_DISTANCE 260 //Maximum distance of the sensor is set to 260cm (Well within the requierment of a parking sensor)

NewPing sensor(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);//Object is created using the Trig Echo and Max Distance and is defined as sensor 

const int reverse_gear = 4; //reverse gear is given as a const int and attatched to pin 4
const int buzzer = 6; //buzzer is given as a const int and attatched to pin 6
const int ledPin = 13; //LED for Interrupt warning defined to pin 13

int led = LOW;
int distance; //return data is defined (could be defined as an array if more sensors were added)  
int buttonState = 0; // origenal Button state is defined as 0 for use in reverse gear and interrupt
int bcount = 0; //counter is defined to count bytes in response 


void setup() {
  
pinMode (reverse_gear, INPUT); //reverse gear push button is set as an input 
pinMode (2, INPUT_PULLUP); // sets interrupt pin to input pullup, Any sensor could be used here for a car aplication e.g. oil pressure sensor to trigger if oil pressure is too high
pinMode (buzzer, OUTPUT);// buzzer is set to an output 
pinMode (ledPin,OUTPUT); // LED for interrupt defined as output. used as a warning light for interrupt button 

attachInterrupt(digitalPinToInterrupt(2), InterruptFunction1, HIGH); // Interrupt set to pin 2, set to run "void InterruptFunction1" on a HIGH input 

Wire.begin(SLAVE_ADDR); // I2C communications are Initialised as a Slave 
Wire.onRequest(requestEvent);//Function runs when the mater module requests data (distance in cm)
}

void requestEvent() { 
  
  byte bval; // bval is the byte defined to hold data 

switch (bcount) {// this cycles through the data 
  case 0:
  bval = 255; // 255 used to mark beginning of sequence (start)
  break;
  case 1:
  bval = distance; //distance assigned to the bval byte 
  break;
}

Wire.write(bval); // bval byte with data sent by I2C bus to Master controller 

bcount = bcount +1; // incrament by 1 each time 
if (bcount> 1) bcount = 0; // once greater than 1 return to 0 to start over (if more sensors are added no. should be changed to count through each one)
}

void readDistance(){  // reads the distance from sensor 
  distance = sensor.ping_cm();
  if (distance>254){  // as the number 255 is being used as the start byte, any higher number will cause an error 
    distance = 254;   // therfore no. higher than 254 set to 254cm 
  }
  delay(20);
}

void Buzzer(){  // used as an audable output of the system to indicate distance 
  if ((distance <10)&&(distance >0)){ // Buzzer on constant when distance is between 0 and 12cm 
  digitalWrite(buzzer, HIGH);
}
else if ((distance >10) &&(distance <30)){ // buzzer to sound at different frequencies as a result of the distance 
  digitalWrite(buzzer, HIGH);
  delay(10*(distance-10)); // timing of the blinking buzzer set by multiplying the distance by 10 and subtracting 10 
  digitalWrite(buzzer, LOW);// this enables the buzzer  to increese frequency the shorter the distance 
  delay(10*(distance-10));
}
else if ((distance >31) &&(distance<40)){ // buzzer will sound at 1 sec on 1 sec off at 31-40cm distance
  digitalWrite(buzzer, HIGH);
  delay(1000);
  digitalWrite(buzzer, LOW);
  delay(1000);
}
else (distance > 40);{  // buzzer off (LOW) after 40cm  (ALL DISTANCES THEORETICAL AND SHOULD BE SCALED UP FOR INDUSTRY APPLICATION)
  digitalWrite(buzzer, LOW);
}
}

void loop()   { // Main loop to run 
  buttonState = digitalRead(reverse_gear); // check the state of the button on reverse gear 
  if (buttonState == HIGH){ // if high run sequence  to read and transmit data to Master and sound buzzer as required.
 readDistance();

 Buzzer ();

  }
}

void InterruptFunction1 () { // Interrupt function triggered once Interrupt button is pressed, sets LED to high to indicate warning 
    buttonState = digitalRead(2);
    if (buttonState ==HIGH){
    digitalWrite(ledPin, HIGH);
}
else {
  digitalWrite(ledPin, LOW);
}
}
