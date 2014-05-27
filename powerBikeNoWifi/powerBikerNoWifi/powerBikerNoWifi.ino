 //#include <LiquidCrystal.h>

#include <SPI.h>
//#include <WiFi.h>
//#include <SD.h>

#define RELAY1 31  
#define RELAY2 32
#define RELAY3 33  
#define RELAY4 34
#define RELAY5 35  
#define RELAY6 36
#define RELAY7 37  
#define RELAY8 38
#define RELAY9 39
#define RELAY10 40  
#define RELAY11 41

/* This sketch describes how to connect a ACS715 Current Sense Carrier 
(http://www.pololu.com/catalog/product/1186) to the Arduino, 
and read current flowing through the sensor.

*/

//LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

/*

Vcc on carrier board to Arduino +5v
GND on carrier board to Arduino GND
OUT on carrier board to Arduino A0



Insert the power lugs into the loads positive lead circuit, 
arrow on carrier board points to load, other lug connects to 
power supply positive

*/
int batMonPin = A4;    // input pin for the voltage divider
int batVal = 0;       // variable for the A/D value
float pinVoltage = 0; // variable to hold the calculated voltage
float batteryVoltage = 12;
float ratio = 2.55;  // Change this to match the MEASURED ration of the circuit, 12k R1 and 5k R2
int analogInPin = A0;  // Analog input pin that the carrier board OUT is connected to
int sensorValue = 0;        // value read from the carrier board
int outputValue = 0; // output in milliamps

int analogRefPin = A1;
int refValue = 0;
int sensorDif = 0;
unsigned long msec = 0;
float time = 0.0;
int sample = 0;
float totalCharge = 0.0;
float averageAmps = 0.0;
float ampSeconds = 0.0;
float ampHours = 0.0;
float wattHours = 0.0;
float amps = 0.0;
float watts =0.0;

float highWatts=0.0;

float oldAmps = 0.0;

int ampAdj = 30;

String levelStr = "";  

int lvl0 = 0; // 0 watts     nothing
int lvl1 = 25; //10 watts   8 cell phones
int lvl2 = 50; //100 watts   laptop
int lvl3 = 75; // 250 watts  Stereo 
int lvl4 = 120; // 350 watts  TV
int lvl5 = 160; // 500 watts  Large TV
int lvl6 = 225; //Fridge
int lvl7 = 300;
int lvl8 = 400;
int lvl9 = 500;

String lvl1Str = "8 Phones";
String lvl2Str = "Laptop";
String lvl3Str = "Stereo";
String lvl4Str = "TV";
String lvl5Str = "Big TV";
String lvl6Str = "Fridge";

//wifi stuff
char ssid[] = "david";      //  your network SSID (name) 
char pass[] = "vagary2539";   // your network password
int keyIndex = 0;                 // your network key Index number (needed only for WEP)

//int status = WL_IDLE_STATUS;

// WiFiServer server(80);
// WiFiClient client;


//switch stuff


const int switchPin=2;

int switchState=0; //var for reading switch state
boolean warmUp = false;

//adjustment

int adjPin = A8;


//sd stuff
//File myFile;


// Define the number of samples to keep track of.  The higher the number,
// the more the readings will be smoothed, but the slower the output will
// respond to the input.  Using a constant rather than a normal variable lets
// use this value to determine the size of the readings array.
const int numReadings = 25;

int readings[numReadings];      // the readings from the analog input
int index = 0;                  // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average


//
///digital display
//
/*
  Code for interfacing with 7 segment displays
 using the multiplexing method
 and the TPIC6B595 Shift Register (1 per digit)
 By K.O.
 */

//Pin Assignments (You should change these)
const int CLK       = 13;           //Connected to TPIC pin 13: SRCLK (aka Clock)
const int LATCH     = 10;          //Connected to TPIC pin 12: RCLK (aka Latch/load/CS/SS...)
const int OE        = 9;          //Connected to TPIC pin 9: OE (Output Enable)
const int DOUT      = 11;          //Connected to TPIC pin 3: SER (aka MOSI)

//Number Patterns (0-9)
//***Drains 0-7 must be connected to segments A-DP respectively***
const byte numTable[] =
{
  B11011110,  //0x
  B00000110,  //1x
  B10111010,  //2x
  B10101110,  //3x
  B01100110,  //4x
  B11101100,  //5x
  B11111100,  //6x
  B10000110,  //7x
  B11111110,  //8 x
  B11100110,  //9
  B00000000  //10 -- -nothing on
};

//Global Variables
int numDevices = 1;                       //The number of x-digit display modules you plan to use
int maxDisplays = 1;                      //The maximum displays that could be accommodated (see note 1)
int maxDigits = 3;                        //The maximum digits you plan on displaying per display module (each SR can handle a max of 8 digits)
int SRData[1][3];                         //The storage location for the digit information. We must specify a fixed array at compile time (see note 2)
boolean debug = false;                     //Change to true to print messages
int delayTime =0;                     //Optional (just for demonstrating multiplexing)
int hiWatts = 0;
/*
  Notes
 1. It is recommended to use an external power supply to avoid oversource/sinking the microcontroller
    or if you need to power high voltage, high current displays. This code will turn on/off all segments in a digit for ***each*** display.
    So, if using 2x 3-digit displays all displaying an 8 + DP, the max consumption will be:
       20mA (desired forward current) * 8 (segments that are on) * 2 (displays showing identical info) = 320mA
 2. The first dimension should equal maxDisplays. The second dimension should equal the number of digits
 */
 

void setup() {
  pinMode(RELAY1, OUTPUT);          // tells arduino RELAY is an output
  pinMode(RELAY2, OUTPUT); 
  pinMode(RELAY3, OUTPUT);          // tells arduino RELAY is an output
  pinMode(RELAY4, OUTPUT); 
  pinMode(RELAY5, OUTPUT);          // tells arduino RELAY is an output
  pinMode(RELAY6, OUTPUT); 
  pinMode(RELAY7, OUTPUT);          // tells arduino RELAY is an output
  pinMode(RELAY8, OUTPUT);
  pinMode(RELAY9, OUTPUT); 
  pinMode(RELAY10, OUTPUT); 
  pinMode(RELAY11, OUTPUT);  
  
  pinMode(switchPin, INPUT); 
  pinMode(10, OUTPUT);
  
  digitalWrite(RELAY1, LOW);
  digitalWrite(RELAY2, LOW);
  digitalWrite(RELAY3, LOW);                           
  digitalWrite(RELAY4, LOW);
  digitalWrite(RELAY5, LOW);
  digitalWrite(RELAY6, LOW);
  digitalWrite(RELAY7, LOW);
  digitalWrite(RELAY8, LOW);
  digitalWrite(RELAY9, LOW);
  digitalWrite(RELAY10, LOW);
  digitalWrite(RELAY11, LOW);
  
 
  
  // initialize serial communications at 9600 bps:
  Serial.begin(9600); 
  //lcd.begin(20, 4);
  
   // initialize all the readings to 0: 
  for (int thisReading = 0; thisReading < numReadings; thisReading++)
    readings[thisReading] = 0; 

//digi displays
 //Set pin modes
  pinMode(CLK,OUTPUT);
  pinMode(LATCH,OUTPUT);
  pinMode(DOUT, OUTPUT);
  pinMode(OE, OUTPUT);

  //7-Segment Display Init
  digitalWrite(OE,LOW);        //Enables SR Operation
  initializeSRData();          //Prepares SR and clears data on serial line

  //Test
  setDigit(0,0,0,false);
  setDigit(0,1,0,false);
  setDigit(0,2,0,false);
 
}

void loop() {
  //read the adj value
 float adj = analogRead(adjPin) * (2.00 / 1024.00);
  
  
  // read the analog in value:
//  sensorValue = analogRead(analogInPin);  
  
  //average the amp sensor
  // subtract the last reading:
  total= total - readings[index];         
  // read from the sensor:  
  readings[index] = analogRead(analogInPin); 
  // add the reading to the total:
  total= total + readings[index];       
  // advance to the next position in the array:  
  index = index + 1;                    

  // if we're at the end of the array...
  if (index >= numReadings)              
    // ...wrap around to the beginning: 
    index = 0;                           

  // calculate the average:
  average = total / numReadings;         
  
 //assign sensort value to average, then calc stuff
  sensorValue = average;
  
  
 // refValue = analogRead(  analogRefPin);
  
 // sensorDif = sensorValue - refValue -ampAdj;//512;//  //495; //refValue -33; //
  // convert to milli amps
 //outputValue = (((long)sensorDif * 5000 / 1024)   ) * 1000 / 28;  
 
 // outputValue = (((long)sensorDif * 5000 / 1024)   ) * 1000 / 28;  
  // outputValue = (((long)sensorValue * 5000 / 1024) - 2500  ) * 1000 / 28;  
  
  
  //orig 
  outputValue = (((long)sensorValue * 5000 / 1024) - 500 ) * 1000 / 133; 
/* sensor outputs about 100 at rest. 
Analog read produces a value of 0-1023, equating to 0v to 5v.
"((long)sensorValue * 5000 / 1024)" is the voltage on the sensor's output in millivolts.
There's a 500mv offset to subtract. 
The unit produces 133mv per amp of current, so
divide by 0.133 to convert mv to ma

changed 1000/133 to 1000/28 for the 75 amp range sensor no offset either
          
*/
 
  
  batVal = analogRead(batMonPin);    // read the voltage on the divider 
  pinVoltage = batVal * 0.00635;       //  Calculate the voltage on the A/D pin
                                    //  A reading of 1 for the A/D = 0.0048mV
                                    //  if we multiply the A/D reading by 0.00488 then 
                                    //  we get the voltage on the pin.  

  batteryVoltage = pinVoltage * ratio;    //  Use the ratio calculated for the voltage divider
                                          //  to calculate the battery voltage
                                          
  oldAmps = amps;
  
                                            
 
  amps = (float) outputValue / 1000;
  
  //normalize -- if the difference between this cycle and last is bigger than 5 then take the average.
  /* if(oldAmps - 5 > amps){
   //  amps = (amps + oldAmps)/2;
      Serial.print("averaging\t");   
   }
  
  if(amps < 0)amps = 0;
  */
  
  
  
  
   watts = amps * batteryVoltage * adj;
    
 // Serial.print("Volts = " );                       
 // Serial.print(batteryVoltage);   
 // Serial.print("\t sensor  = ");      
 // Serial.print(sensorValue); 
 // Serial.print("\t ref  = ");      
 // Serial.print(refValue); 
  Serial.print("Adj = " );                       
  Serial.print(adj); 
  Serial.print("\t Current (amps) = ");      
  Serial.print(amps);  
  Serial.print("\t Power (Watts) = ");   
  Serial.print(watts);   
  
  

  
    
  sample = sample + 1;
  
  msec = millis();
  
  
  
 time = (float) msec / 1000.0;
  
 totalCharge = totalCharge + amps;
  
 averageAmps = totalCharge / sample;
  
 ampSeconds = averageAmps*time;

 ampHours = ampSeconds/3600;
  
 wattHours = batteryVoltage * ampHours;
 
 switchState = digitalRead(switchPin);
  Serial.print("\t switch Pin = ");   
  Serial.print(switchState);  
 
 if (switchState == LOW  ){
  
    highWatts = 0;
    hiWatts = 0;
    levelStr = ""; 
    refreshDisplay(0); 
}  
  
 if (watts > highWatts  && switchState == HIGH ){
  highWatts = watts;
  hiWatts = int(highWatts);
  refreshDisplay(hiWatts); 
  
 } 
 

  
if (watts < lvl1 || switchState == LOW){
    digitalWrite(RELAY1, HIGH);
    digitalWrite(RELAY2, LOW);
    digitalWrite(RELAY3, LOW);
    digitalWrite(RELAY4, LOW);
    digitalWrite(RELAY5, LOW);
    digitalWrite(RELAY6, LOW);
    digitalWrite(RELAY7, LOW);
    digitalWrite(RELAY8, LOW);
    digitalWrite(RELAY9, LOW);
    digitalWrite(RELAY10, LOW);
    digitalWrite(RELAY11, LOW);
    
}  

if (watts >= lvl1 && watts < lvl2 && switchState == HIGH){
    digitalWrite(RELAY1, LOW);
    digitalWrite(RELAY2, HIGH);
    digitalWrite(RELAY3, LOW);
    digitalWrite(RELAY4, LOW);
    digitalWrite(RELAY5, LOW);
    digitalWrite(RELAY6, LOW);
    digitalWrite(RELAY7, LOW);
    digitalWrite(RELAY8, LOW);
    digitalWrite(RELAY9, LOW);
    digitalWrite(RELAY10, LOW);
    digitalWrite(RELAY11, LOW);
    
  
}

if (watts >= lvl2 && watts < lvl3 && switchState == HIGH){
    digitalWrite(RELAY1, LOW);
    digitalWrite(RELAY2, HIGH);
    digitalWrite(RELAY3, HIGH);
    digitalWrite(RELAY4, LOW);
    digitalWrite(RELAY5, LOW);
    digitalWrite(RELAY6, LOW);
    digitalWrite(RELAY7, LOW);
    digitalWrite(RELAY8, LOW);
    digitalWrite(RELAY9, LOW);
    digitalWrite(RELAY10, LOW);
    digitalWrite(RELAY11, LOW);
      
  
}

if (watts >= lvl3 && watts < lvl4 && switchState == HIGH){
    digitalWrite(RELAY1, LOW);
    digitalWrite(RELAY2, HIGH);
    digitalWrite(RELAY3, HIGH);
    digitalWrite(RELAY4, HIGH);
    digitalWrite(RELAY5, LOW);
    digitalWrite(RELAY6, LOW);
    digitalWrite(RELAY7, LOW);
    digitalWrite(RELAY8, LOW);
    digitalWrite(RELAY9, LOW);
    digitalWrite(RELAY10, LOW);
    digitalWrite(RELAY11, LOW);
   
  
}
if (watts >= lvl4 && watts < lvl5 && switchState == HIGH){
    digitalWrite(RELAY1, LOW);
    digitalWrite(RELAY2, HIGH);
    digitalWrite(RELAY3, HIGH);
    digitalWrite(RELAY4, HIGH);
    digitalWrite(RELAY5, HIGH);
    digitalWrite(RELAY6, LOW);
    digitalWrite(RELAY7, LOW);
    digitalWrite(RELAY8, LOW);
    digitalWrite(RELAY9, LOW);
    digitalWrite(RELAY10, LOW);
    digitalWrite(RELAY11, LOW);
   
  
}
if (watts >= lvl5 && watts < lvl6 && switchState == HIGH){
    digitalWrite(RELAY1, LOW);
    digitalWrite(RELAY2, HIGH);
    digitalWrite(RELAY3, HIGH);
    digitalWrite(RELAY4, HIGH);
    digitalWrite(RELAY5, HIGH);
    digitalWrite(RELAY6, HIGH);
    digitalWrite(RELAY7, LOW);
    digitalWrite(RELAY8, LOW);
    digitalWrite(RELAY9, LOW);
    digitalWrite(RELAY10, LOW);
    digitalWrite(RELAY11, LOW);
 
}
if (watts >= lvl6 && watts < lvl7 && switchState == HIGH){
    digitalWrite(RELAY1, LOW);
    digitalWrite(RELAY2, HIGH);
    digitalWrite(RELAY3, HIGH);
    digitalWrite(RELAY4, HIGH);
    digitalWrite(RELAY5, HIGH);
    digitalWrite(RELAY6, HIGH);
    digitalWrite(RELAY7, HIGH);
    digitalWrite(RELAY8, LOW);
    digitalWrite(RELAY9, LOW);
    digitalWrite(RELAY10, LOW);
    digitalWrite(RELAY11, LOW);
 
}
if (watts >= lvl7 && watts < lvl8 && switchState == HIGH){
    digitalWrite(RELAY1, LOW);
    digitalWrite(RELAY2, HIGH);
    digitalWrite(RELAY3, HIGH);
    digitalWrite(RELAY4, HIGH);
    digitalWrite(RELAY5, HIGH);
    digitalWrite(RELAY6, HIGH);
    digitalWrite(RELAY7, HIGH);
    digitalWrite(RELAY8, HIGH);
    digitalWrite(RELAY9, LOW);
    digitalWrite(RELAY10, LOW);
    digitalWrite(RELAY11, LOW);
 
}
if (watts >= lvl8 && watts < lvl9 && switchState == HIGH){
    digitalWrite(RELAY1, LOW);
    digitalWrite(RELAY2, HIGH);
    digitalWrite(RELAY3, HIGH);
    digitalWrite(RELAY4, HIGH);
    digitalWrite(RELAY5, HIGH);
    digitalWrite(RELAY6, HIGH);
    digitalWrite(RELAY7, HIGH);
    digitalWrite(RELAY8, HIGH);
    digitalWrite(RELAY9, HIGH);
    digitalWrite(RELAY10, LOW);
    digitalWrite(RELAY11, LOW);
 
}

if (watts >= lvl9 && switchState == HIGH){
 
    digitalWrite(RELAY1, LOW);
    digitalWrite(RELAY2, HIGH);
    digitalWrite(RELAY3, HIGH);
    digitalWrite(RELAY4, HIGH);
    digitalWrite(RELAY5, HIGH);
    digitalWrite(RELAY6, HIGH);
    digitalWrite(RELAY7, HIGH);
    digitalWrite(RELAY8, HIGH);
    digitalWrite(RELAY9, HIGH);
    digitalWrite(RELAY10, HIGH);
    digitalWrite(RELAY11, LOW);
    delay(200);
    digitalWrite(RELAY10, LOW);
    digitalWrite(RELAY11, HIGH);
    delay(200);
  
}

 



  Serial.print("\t Time (hours) = ");
  Serial.print(time/3600);
  
  Serial.print("\t Amp Hours (ah) = ");
  Serial.print(ampHours);
  Serial.print("\t Watt Hours (wh) = ");
  Serial.println(wattHours);
  
  
 


 

  // wait 10 milliseconds before the next loop
  // for the analog-to-digital converter to settle
  // after the last reading:
  delay(10);                     
}


//==========BEGIN SR Functions==========
void initializeSRData()
{
  //Display Scanner (Iterates through each display module)
  digitalWrite(LATCH,LOW);      //Tells all SRs that uController is sending data

  for(int dispID = 0; dispID < maxDisplays; dispID++)
  {    
    //Digit Scanner (Iterates through each SR (digit) in a display module)
    for(int digit = 0; digit < maxDigits; digit++)
    {
      //Clears any garbage on the serial line
      shiftOut(DOUT,CLK,LSBFIRST,0);          //Shift out 0s to all displays
      SRData[dispID][digit] = 0;              //Stores a 0 for each digit so its completely off
    }
  }
  digitalWrite(LATCH,HIGH);      //Tells all SRs that uController is done sending data
}

void printSRData()
{
  if(!debug)
    return;

  Serial.println("Printing SR Data...");

  //Display Scanner
  for(int dispID = 0; dispID < maxDisplays; dispID++)
  {    
    Serial.print("Display # ");
    Serial.println(dispID);

    //Digit Scanner
    for(int digit = 0; digit < maxDigits; digit++)
    {
      Serial.print("Digit ");
      Serial.print(digit);
      Serial.print(": ");
      Serial.println(SRData[dispID][digit],BIN);
    }
    Serial.println();
  }
}

void setDigit(int dispID, int digit, int value, boolean dp)
{
  //Parameter checker
  if(dispID < 0 || dispID >= numDevices)
  {
    Serial.println("dispID OoB!");         //OoB = Out of bounds
    return;
  }

  if(digit < 0 || digit > maxDigits)
  {
    Serial.println("digit OoB!"); 
    return;
  }

  if(value < 0 || value > 10)
  {
    Serial.println("Invalid value!"); 
    return;
  }

  value = numTable[value];

  //Toggle dp if needed
  if(dp)
    value |= B00000001;          //Turns on the first binary digit (segment) using an OR bitmask

  //Store the digit
  SRData[dispID][digit] = value;

  if(debug)
    printSRData();
}

void setSegments(int dispID, int digit, byte value)
{
  //Parameter checker
  if(dispID < 0 || dispID >= numDevices)
  {
    Serial.println("dispID OoB!"); 
    return;
  }

  if(digit < 0 || digit > maxDigits)
  {
    Serial.println("digit OoB!"); 
    return;
  }

  if(value < 0 || value > 255)
  {
    Serial.println("Invalid byte!"); 
    return;
  }

  //Store the digit
  SRData[dispID][digit] = value;

  if(debug)
    printSRData();
}

void clearDisplay(int dispID)
{
  initializeSRData();
  refreshDisplay(000);
 
}

void refreshDisplay(int num)
{
 
 
  int third=num/100;
 int second=num%100/10;
 int first=num%10;
 
 if(third < 1){
 setDigit(0,2,10,false);
 }else {
   
    setDigit(0,2,third,false);
 }
 
 if(third < 1 && second < 1)  {
  setDigit(0,1,10,false);
 }else{
   
   setDigit(0,1,second,false);
 }
  if(third < 1 && second < 1 && first < 1)  {
  setDigit(0,0,10,false);
 }else{
   
   setDigit(0,0,first,false);
 }
 
// setDigit(0,0,first,false);
 
  //Digit Scanner
  for(int digit = 0; digit < maxDigits; digit++)
  {   
   // Serial.print("digit ");
     //Serial.println(digit);
    //Display Scanner
    digitalWrite(LATCH,LOW);
    for(int dispID = numDevices -  1; dispID >= 0; dispID--)
    {
    //  Serial.print("dispID ");
    // Serial.println(dispID);
      //Pre-Digit blanker (shifts out 0s to correct digits before sending segment data to desired digit)
      for(int blanks = (maxDigits - 1 - digit); blanks > 0; blanks--){
      //  shiftOut(DOUT,CLK,LSBFIRST,0);
      }

      shiftOut(DOUT,CLK,LSBFIRST,SRData[dispID][digit]);
  //    Serial.print("SRData ");
    // Serial.println(SRData[dispID][digit],BIN);
      

      //Post-Digit blanker (shifts out 0s to remaining digits)
      for(int blanks = digit; blanks > 0; blanks--){
     //   shiftOut(DOUT,CLK,LSBFIRST,0);
       // Serial.print("blanks ");
    // Serial.println(blanks);
     
        
      }
   //   Serial.println(" ");
    }
    digitalWrite(LATCH,HIGH);

    //Demonstrates multiplexing operation
   // delay(delayTime);
   // delayTime -= 10;
    //if(delayTime <= 0)
      //delayTime = 0;
  }
}

//==========END SR Functions==========


