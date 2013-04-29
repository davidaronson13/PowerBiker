#include <LiquidCrystal.h>

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
float batteryVoltage = 0;
float ratio = 2.1;  // Change this to match the MEASURED ration of the circuit, 12k R1 and 5k R2
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

int lvl0 = 0; // 0 watts     nothing
int lvl1 = 10; //10 watts    cell phone charge 
int lvl2 = 100; //100 watts   light bulb
int lvl3 = 250; // 250 watts  TV 
int lvl4 = 350; // 350 watts  blender
int lvl5 = 500; // 500 watts  flashing light and speakers

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600); 
  //lcd.begin(20, 4);
}

void loop() {
  

  
  
  // read the analog in value:
  sensorValue = analogRead(analogInPin);  
  refValue = analogRead(  analogRefPin);
  
  sensorDif = sensorValue - refValue;
  // convert to milli amps
 outputValue = (((long)sensorDif * 5000 / 1024)   ) * 1000 / 28;  
 
 // outputValue = (((long)sensorDif * 5000 / 1024)   ) * 1000 / 28;  
  // outputValue = (((long)sensorValue * 5000 / 1024) - 2500  ) * 1000 / 28;  
  
  
  //orig  outputValue = (((long)sensorValue * 5000 / 1024) - 500 ) * 1000 / 133; 
/* sensor outputs about 100 at rest. 
Analog read produces a value of 0-1023, equating to 0v to 5v.
"((long)sensorValue * 5000 / 1024)" is the voltage on the sensor's output in millivolts.
There's a 500mv offset to subtract. 
The unit produces 133mv per amp of current, so
divide by 0.133 to convert mv to ma

changed 1000/133 to 1000/28 for the 75 amp range sensor no offset either
          
*/
 
  
  batVal = analogRead(batMonPin);    // read the voltage on the divider 
  pinVoltage = batVal * 0.00488;       //  Calculate the voltage on the A/D pin
                                    //  A reading of 1 for the A/D = 0.0048mV
                                    //  if we multiply the A/D reading by 0.00488 then 
                                    //  we get the voltage on the pin.  

  batteryVoltage = pinVoltage * ratio;    //  Use the ratio calculated for the voltage divider
                                          //  to calculate the battery voltage
                                          
                                            
  amps = (float) outputValue / 1000;
  float watts = amps * batteryVoltage;
    
  Serial.print("Volts = " );                       
  Serial.print(batteryVoltage);   
 Serial.print("\t sensor  = ");      
  Serial.print(sensorValue); 
  Serial.print("\t ref  = ");      
  Serial.print(refValue); 
  
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
  
 



  Serial.print("\t Time (hours) = ");
  Serial.print(time/3600);
  
  Serial.print("\t Amp Hours (ah) = ");
  Serial.print(ampHours);
  Serial.print("\t Watt Hours (wh) = ");
  Serial.println(wattHours);
  
/*
  lcd.setCursor(0,0);
    lcd.print(batteryVoltage);
    lcd.print(" V ");
    lcd.print(amps);
    lcd.print(" A ");
  
  lcd.setCursor(0,1);
  lcd.print(watts);
  lcd.print(" W ");
  lcd.print(time/3600);
  lcd.print(" H ");
  
  lcd.setCursor(0,2);
  lcd.print(ampHours);
  lcd.print(" Ah ");
  lcd.print(wattHours);
  lcd.print(" Wh ");
  */
  
  
  // wait 10 milliseconds before the next loop
  // for the analog-to-digital converter to settle
  // after the last reading:
  delay(10);                     
}
