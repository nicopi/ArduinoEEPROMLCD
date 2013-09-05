
//Copyright(c) 2013 Nicola Piovesan
//This program allow Arduino to read an analog value, print it on a display and write/read it on/from an external EEPROM.
//You can find the Seeprom library on https://github.com/nicopi/Seeprom

#include <LiquidCrystal.h>
#include <Seeprom.h>
#include <Wire.h>

const byte DEVADDR = 0x50; //Address of the EEPROM

LiquidCrystal lcd(12, 11, 2, 7, 8, 9, 10); // initialize the library with the numbers of the interface pins

int val=0; //Contains the analog data on pin 

//define used pin
int potentiometerPin = 2; //Analog input
int readbuttonPin = 5; //digital
int writebuttonPin = 4; //digital
int dumpPin=3; //digital

Seeprom eeprom(DEVADDR); //create the eeprom object

//
//I use this function to print on my "strange" 16x1 LCD Display. If you have a "normal" display you can use the lcd.print() function
//
void  LCDprint(String data){
  String buffer;
  String white="                ";

  if (data.length()<16){
    for(int i=data.length();i<16;i++){
      data=data+" ";
    }
  }

  buffer=data.substring(0,8);
  lcd.setCursor(0,0);
  lcd.print(buffer);

  buffer=data.substring(8,9);
  lcd.setCursor(0,43);
  lcd.print(buffer);

  buffer=data.substring(9,16);
  lcd.setCursor(65,0);
  lcd.print(buffer);
}

//
// SETUP
//
void setup(){
  // set up the LCD's number of rows and columns: 
  lcd.begin(16, 2);
  lcd.clear();

  //set pin mode for pin 5 (read) and pin 4 (write)
  pinMode(readbuttonPin, INPUT);
  pinMode(writebuttonPin, INPUT);
  pinMode(dumpPin, INPUT_PULLUP);

  // initialize the serial communications:
  Serial.begin(9600);
  
  //Connect digital pin 3 to GND and reset to dump memory to serial
  if (digitalRead(dumpPin)==LOW){
     LCDprint("Dumping memory");
     eeprom.dump(0,2048);
  }  
  // print a welcome message and wait 2 seconds
  LCDprint("Welcome!");
  delay(2000);
}

//
// LOOP
//
void loop()
{
  //Controls the read button
  if(digitalRead(readbuttonPin)==HIGH){
    int valeep;
    //Read from the EEPROM
    valeep=eeprom.readInt16(0);
    //print the read data on Serial
    Serial.println(String(valeep));
    //write the EEPROM read value on the LCD display
    LCDprint(String(valeep));
    delay(1500);
  }

  //Controls the write button
  if(digitalRead(writebuttonPin)==HIGH){
    //Write data to EEPROM
    eeprom.writeInt16(0,val);
    Serial.println("Memory written");
    LCDprint("Memory written.");

    //Blinks the pin13 led to confirm
    digitalWrite(13, HIGH);
    delay(600);
    digitalWrite(13, LOW);
    delay(600);
    digitalWrite(13, HIGH);
    delay(600);
    digitalWrite(13, LOW);
  }

  //read the potentiometer value (0-1023)
  val = analogRead(potentiometerPin);
  //write the read value on the LCD display
  LCDprint(String(val));
}
