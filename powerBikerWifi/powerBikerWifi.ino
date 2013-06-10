//#include <LiquidCrystal.h>

#include <SPI.h>
#include <WiFi.h>
//#include <SD.h>

#define RELAY1 31  
#define RELAY2 32
#define RELAY3 33  
#define RELAY4 34
#define RELAY5 35  
#define RELAY6 36
#define RELAY7 37  
#define RELAY8 38

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
int lvl2 = 60; //100 watts   laptop
int lvl3 = 100; // 250 watts  Stereo 
int lvl4 = 150; // 350 watts  TV
int lvl5 = 180; // 500 watts  Large TV
int lvl6 = 220; //Fridge

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

int status = WL_IDLE_STATUS;

WiFiServer server(80);
WiFiClient client;


//switch stuff


const int switchPin=2;

int switchState=0; //var for reading switch state

//sd stuff
//File myFile;


void setup() {
  pinMode(RELAY1, OUTPUT);          // tells arduino RELAY is an output
  pinMode(RELAY2, OUTPUT); 
  pinMode(RELAY3, OUTPUT);          // tells arduino RELAY is an output
  pinMode(RELAY4, OUTPUT); 
  pinMode(RELAY5, OUTPUT);          // tells arduino RELAY is an output
  pinMode(RELAY6, OUTPUT); 
  pinMode(RELAY7, OUTPUT);          // tells arduino RELAY is an output
  pinMode(RELAY8, OUTPUT); 
  
  pinMode(switchPin, INPUT); 
  pinMode(10, OUTPUT);
  
  digitalWrite(RELAY1, HIGH);
  digitalWrite(RELAY2, LOW);
  digitalWrite(RELAY3, LOW);                           
  digitalWrite(RELAY4, LOW);
  digitalWrite(RELAY5, LOW);
  digitalWrite(RELAY6, LOW);
  digitalWrite(RELAY7, LOW);
  digitalWrite(RELAY8, LOW);
  
 
  
  // initialize serial communications at 9600 bps:
  Serial.begin(9600); 
  //lcd.begin(20, 4);

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present"); 
    // don't continue:
    while(true);
  }
    int i=0;
   // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) { 
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:    
    status = WiFi.begin(ssid  );
    // status = WiFi.begin(ssid, pass);
    i++;
    if (i >=                   0){
      Serial.print("Unable to connect to SSID: ");
      Serial.println(ssid);
      return;
    }
    // wait 10 seconds for connection:
    delay(1000);
  } 
  server.begin();
  
 /* if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    //return;
  }*/
  // you're connected now, so print out the status:
  printWifiStatus();
 // myFile = SD.open("wattHours.txt", FILE_WRITE);
  
}

void loop() {
  
  // read the analog in value:
  sensorValue = analogRead(analogInPin);  
  refValue = analogRead(  analogRefPin);
  
  sensorDif = sensorValue - refValue -ampAdj;//512;//  //495; //refValue -33; //
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
  pinVoltage = batVal * 0.00635;       //  Calculate the voltage on the A/D pin
                                    //  A reading of 1 for the A/D = 0.0048mV
                                    //  if we multiply the A/D reading by 0.00488 then 
                                    //  we get the voltage on the pin.  

  batteryVoltage = pinVoltage * ratio;    //  Use the ratio calculated for the voltage divider
                                          //  to calculate the battery voltage
                                          
  oldAmps = amps;
  
                                            
 
  amps = (float) outputValue / 1000;
  
  //normalize -- if the difference between this cycle and last is bigger than 5 then take the average.
   if(oldAmps - 5 > amps){
   //  amps = (amps + oldAmps)/2;
      Serial.print("averaging\t");   
   }
  
  if(amps < 0)amps = 0;
  
   watts = amps * batteryVoltage;
    
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
 
 switchState = digitalRead(switchPin);
  Serial.print("\t switch Pin = ");   
  Serial.print(switchState);  
 
 if (switchState == LOW){
  
    highWatts = 0;
    levelStr = ""; 
}   
  
 if (watts > highWatts){
  highWatts = watts;
 } 
 
 if (highWatts < lvl1 ){

    levelStr = ""; 
}  

if (highWatts >= lvl1 && highWatts < lvl2){
 
    levelStr = lvl1Str; 
    
  
}

if (highWatts >= lvl2 && highWatts < lvl3 ){
   
      levelStr = lvl2Str;
  
}

if (highWatts >= lvl3 && highWatts < lvl4 ){
   
    levelStr = lvl3Str;
  
}
if (highWatts >= lvl4 && highWatts < lvl5 ){
   
    levelStr = lvl4Str;
  
}
if (highWatts >= lvl5 && highWatts < lvl6 ){
   
  levelStr = lvl5Str;
}

if (highWatts >= lvl6 ){
  levelStr = lvl6Str;
   
  
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
 
}

if (watts >= lvl6 && switchState == HIGH){
 
    digitalWrite(RELAY1, LOW);
    digitalWrite(RELAY2, HIGH);
    digitalWrite(RELAY3, HIGH);
    digitalWrite(RELAY4, HIGH);
    digitalWrite(RELAY5, HIGH);
    digitalWrite(RELAY6, HIGH);
    
    digitalWrite(RELAY7, HIGH);
    digitalWrite(RELAY8, LOW);
    delay(200);
    digitalWrite(RELAY7, LOW);
    digitalWrite(RELAY8, HIGH);
    delay(200);
  
}

 



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
   // listen for incoming clients
 client = server.available();
  if (client) {
    buildPage();
  }
  

 

  // wait 10 milliseconds before the next loop
  // for the analog-to-digital converter to settle
  // after the last reading:
  delay(10);                     
}



void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  
    
}

void buildPage(){
   Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
       // Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          // add a meta refresh tag, so the browser pulls again every 5 seconds:
          client.println("<meta http-equiv=\"refresh\" content=\"1\">");
          
          //add meta for full screen
          client.println("<meta name=\"apple-mobile-web-app-capable\" content=\"yes\">");
          client.println("<body style=\" background-color:#000; font-size:86px;color:#F00;font:sans-serif; \">");
          //  client.print("<br />Amps: ");
          //client.print(amps);
          //  client.print("<br />Volts: ");
        //  client.print(batteryVoltage);
        
          client.print("<p style=\"font-family:sans-serif;\"><strong><br />&nbsp;&nbsp;&nbsp;Watts: ");
          client.print(watts);
        
        //  client.print("<br /><br />&nbsp;&nbsp;&nbsp;Watt Hours: ");
          
       //   client.print(wattHours);
          
         if (switchState == LOW ){
           client.print("<br /><br />&nbsp;&nbsp;&nbsp;Warming Up");
         }else{
            client.print("<br /><br />&nbsp;&nbsp;&nbsp;<span style=\"color:#00FF00;\">GO!</span>");
            client.print("<br /><br />&nbsp;&nbsp;&nbsp;Peak Watts:&nbsp;");
            client.print(highWatts);
             client.print("<br /><br />&nbsp;&nbsp;&nbsp;You can power a: &nbsp;");
             client.print(levelStr);
         }
          
          client.println("</strong</p></body></html>");
           break;
        }//currentline blank
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } //c==n
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }//else if
      }//client avail
    }//while conectted
    // give the web browser time to receive the data
    delay(1);
      // close the connection:
      client.stop();
      Serial.println("client disonnected");
  }//build page
  
//}

