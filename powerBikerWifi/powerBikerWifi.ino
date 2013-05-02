//#include <LiquidCrystal.h>

#include <SPI.h>
#include <WiFi.h>

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
float batteryVoltage = 0;
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



int lvl0 = 0; // 0 watts     nothing
int lvl1 = 30; //10 watts    cell phone charge 
int lvl2 = 55; //100 watts   light bulb
int lvl3 = 80; // 250 watts  TV 
int lvl4 = 105; // 350 watts  blender
int lvl5 = 130; // 500 watts  flashing light and speakers
int lvl6 = 180;

//wifi stuff
char ssid[] = "bikeMind";      //  your network SSID (name) 
char pass[] = "secretPassword";   // your network password
int keyIndex = 0;                 // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

WiFiServer server(80);
WiFiClient client;



void setup() {
  pinMode(RELAY1, OUTPUT);          // tells arduino RELAY is an output
  pinMode(RELAY2, OUTPUT); 
  pinMode(RELAY3, OUTPUT);          // tells arduino RELAY is an output
  pinMode(RELAY4, OUTPUT); 
  pinMode(RELAY5, OUTPUT);          // tells arduino RELAY is an output
  pinMode(RELAY6, OUTPUT); 
  pinMode(RELAY7, OUTPUT);          // tells arduino RELAY is an output
  pinMode(RELAY8, OUTPUT); 
  
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
  
   // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) { 
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:    
    status = WiFi.begin(ssid);

    // wait 10 seconds for connection:
    delay(10000);
  } 
  server.begin();
  // you're connected now, so print out the status:
  printWifiStatus();
  
}

void loop() {
  
  // read the analog in value:
  sensorValue = analogRead(analogInPin);  
  refValue = analogRead(  analogRefPin);
  
  sensorDif = sensorValue - 510;// refValue; //495; //
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
                                          
                                            
  amps = (float) outputValue / 1000;
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
  
if (watts < lvl1){
    digitalWrite(RELAY1, HIGH);
    digitalWrite(RELAY2, LOW);
    digitalWrite(RELAY3, LOW);
    digitalWrite(RELAY4, LOW);
    digitalWrite(RELAY5, LOW);
    digitalWrite(RELAY6, LOW);
    digitalWrite(RELAY7, LOW);
    digitalWrite(RELAY8, LOW);
}  

if (watts >= lvl1 && watts < lvl2){
    digitalWrite(RELAY1, LOW);
    digitalWrite(RELAY2, HIGH);
    digitalWrite(RELAY3, LOW);
    digitalWrite(RELAY4, LOW);
    digitalWrite(RELAY5, LOW);
    digitalWrite(RELAY6, LOW);
    digitalWrite(RELAY7, LOW);
    digitalWrite(RELAY8, LOW);
  
}

if (watts >= lvl2 && watts < lvl3){
    digitalWrite(RELAY1, LOW);
    digitalWrite(RELAY2, HIGH);
    digitalWrite(RELAY3, HIGH);
    digitalWrite(RELAY4, LOW);
    digitalWrite(RELAY5, LOW);
    digitalWrite(RELAY6, LOW);
    digitalWrite(RELAY7, LOW);
    digitalWrite(RELAY8, LOW);
  
}

if (watts >= lvl3 && watts < lvl4){
    digitalWrite(RELAY1, LOW);
    digitalWrite(RELAY2, HIGH);
    digitalWrite(RELAY3, HIGH);
    digitalWrite(RELAY4, HIGH);
    digitalWrite(RELAY5, LOW);
    digitalWrite(RELAY6, LOW);
    digitalWrite(RELAY7, LOW);
    digitalWrite(RELAY8, LOW);
  
}
if (watts >= lvl4 && watts < lvl5){
    digitalWrite(RELAY1, LOW);
    digitalWrite(RELAY2, HIGH);
    digitalWrite(RELAY3, HIGH);
    digitalWrite(RELAY4, HIGH);
    digitalWrite(RELAY5, HIGH);
    digitalWrite(RELAY6, LOW);
    digitalWrite(RELAY7, LOW);
    digitalWrite(RELAY8, LOW);
  
}
if (watts >= lvl5 && watts < lvl6){
    digitalWrite(RELAY1, LOW);
    digitalWrite(RELAY2, HIGH);
    digitalWrite(RELAY3, HIGH);
    digitalWrite(RELAY4, HIGH);
    digitalWrite(RELAY5, HIGH);
    digitalWrite(RELAY6, HIGH);
    digitalWrite(RELAY7, LOW);
    digitalWrite(RELAY8, LOW);
  
}

if (watts >= lvl6 ){
    digitalWrite(RELAY1, LOW);
    digitalWrite(RELAY2, HIGH);
    digitalWrite(RELAY3, HIGH);
    digitalWrite(RELAY4, HIGH);
    digitalWrite(RELAY5, HIGH);
    digitalWrite(RELAY6, HIGH);
    
    digitalWrite(RELAY7, HIGH);
    digitalWrite(RELAY8, LOW);
    delay(500);
     digitalWrite(RELAY7, LOW);
    digitalWrite(RELAY8, HIGH);
    delay(500);
  
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
        Serial.write(c);
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
          client.println("<body style=\"margin:100px 100px 100px 100px; background-color:#000; font-size:72px;color:#0F0;font:sans-serif; \">");
          //  client.print("<br />Amps: ");
          //client.print(amps);
          //  client.print("<br />Volts: ");
        //  client.print(batteryVoltage);
        
          client.print("<br />Watts: ");
          client.print(watts);
        
          client.print("<br /><br />Watt Hours: ");
          
          client.print(wattHours);
          // output the value of each analog input pin
        /*  for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            client.print("analog input ");
            client.print(analogChannel);
            client.print(" is ");
            client.print(sensorReading);
            client.println("<br />");       
          }*/
          client.println("</body></html>");
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

