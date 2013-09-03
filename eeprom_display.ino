//Copyright(c) 2013 Nicola Piovesan
//This program allow Arduino to read an analog value, print it on a display and write/read it on/from an external EEPROM.

#include <LiquidCrystal.h>
#include <Wire.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 2, 7, 8, 9, 10);


const byte DEVADDR = 0x50; //Address of the EEPROM
byte msg[16]; //

int val=0; //Contains the analog data on pin 

int potentiometerPin = 2; //Analog input
int readbuttonPin = 5; //digital
int writebuttonPin = 4; //digital


//Write multiple bytes on EEPROM
void eeprom_write_page(byte deviceaddress, unsigned eeaddr,
const byte * data, byte length)
{
  // Three lsb of Device address byte are bits 8-10 of eeaddress
  byte devaddr = deviceaddress | ((eeaddr >> 8) & 0x07);
  byte addr    = eeaddr;
  Wire.beginTransmission(devaddr);
  Wire.write(int(addr));
  for (int i = 0; i < length; i++) {
    Wire.write(data[i]);
  }
  Wire.endTransmission();
  delay(10);
}

//Read a buffer from EEPROM
int eeprom_read_buffer(byte deviceaddr, unsigned eeaddr,
byte * buffer, byte length)
{
  // Three lsb of Device address byte are bits 8-10 of eeaddress
  byte devaddr = deviceaddr | ((eeaddr >> 8) & 0x07);
  byte addr    = eeaddr;

  Wire.beginTransmission(devaddr);
  Wire.write(int(addr));
  Wire.endTransmission();

  Wire.requestFrom(devaddr, length);
  int i;
  for (i = 0; i < length && Wire.available(); i++) {
    buffer[i] = Wire.read();
  }
  return i;
}


//I use this function to print on my "strange" 16x1 LCD Display. If you have a "normal" display you can use the lcd.print() function
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

void setup(){
  // set up the LCD's number of rows and columns: 
  lcd.begin(16, 2);
  lcd.clear();

  //set pin mode for pin 5 (read) and pin 4 (write)
  pinMode(readbuttonPin, INPUT);
  pinMode(writebuttonPin, INPUT);


  // initialize the serial communications:
  Serial.begin(9600);

  Wire.begin();

  // print a welcome message and wait 2 seconds
  LCDprint("Welcome!");
  delay(2000);
}


void loop()
{
  //Controls the read button
  if(digitalRead(readbuttonPin)==HIGH){
    byte buffer[16];
    char text[16];
    //Read from the EEPROM
    eeprom_read_buffer(DEVADDR, 0, buffer, sizeof(buffer));
    for (int i=0;i<16;i++)
      text[i]=buffer[i];
    //print the read data on Serial
    Serial.println(String(text));
    //write the EEPROM read value on the LCD display
    LCDprint(String(text));
    delay(1500);
  }

  //Controls the write button
  if(digitalRead(writebuttonPin)==HIGH){
    //Cast val (integet) to a String
    String tstval=String(val);
    //set al chars of msg to the empty char ' '
    for(int i=0;i<16;i++){
      msg[i]=' ';
    }

    for(int i=0;i<tstval.length();i++){
      msg[i]=tstval.charAt(i);
    }
    //Write data to EEPROM
    eeprom_write_page(DEVADDR, 0x000, msg, sizeof(msg));
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
