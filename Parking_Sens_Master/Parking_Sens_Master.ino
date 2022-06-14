#include <Wire.h> // Includes Wire Library in code, to enable Arduino to communicate via I2C bus
#include <LiquidCrystal.h> //Includes LiquidCrystal Library to simplify usage of a LCD 16,2 display

#define SLAVE_ADDR 9 //Defines Slave Address to the same as in sender code

LiquidCrystal lcd(12, 10, 5, 4, 8, 13); // Pins are defined for LCD {Rs, E, D4, D5, D6, D7}
int bcount; // counter defined to count bytes in data responce


byte distance[1]; //Return Data defined in byte( 1 used for 1 sensor but could be 5 for use of 5 sensors)

void setup() {
  lcd.begin(16,2); // defines the type of LCD dsplay being used (in this case 16 x 2)
  Wire.begin(); // defines code as Master (No address)
 
}
  byte readI2C( int address){ // veriable defined to hold each byte of data received
    byte bval ;
    long entry = millis();
    Wire.requestFrom(address, 1); // each byte of data is recieved sequentially (one by one)

     while( Wire.available() ==0 &&(millis() - entry)) lcd.print("Waiting \n"); //wait 100 millisecs until the data arrived into master.
     if (millis() - entry <100) bval = Wire.read();// store data into the byte and return as bval
     return bval;
  }

void loop () {
  lcd.setCursor(0,0); // LCD given a start coordinate on the LCD to start from (0,0 = Top Left Corner)
  lcd.print("Distance:"); // "Distance:" printed and held in top left corner before data is printed

  while (readI2C(SLAVE_ADDR) <255){ // while waiting on 255th byte to enter master print waiting on lcd
    lcd.print("Waiting");
  }
  for (bcount=0; bcount<1; bcount++){ // for loop set up to count sequentially up to 1 then back to 0 to start over array setup for additional sensors to be added
    distance[bcount] = readI2C(SLAVE_ADDR);    // assign the counted byte to the data received from slave
  }
  for (int i = 0; i <1; i++) { //same array used as before to add more sensors if desired (1 used in this case)
    lcd.print(distance[i]); // value is printed on the lcd beside "Distance:"
    lcd.print("   cm"); // units of cm added to show user clarity of measurement
  }
  delay(200); // delay by 200ms then run again
  }
