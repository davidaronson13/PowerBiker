// This code just lets you turn a digital out pin on and off.  That's
// all that is needed to verify a relay curcuit is working.
// Press the space bar to toggle the relay on and off.

#define RELAY_8 38
#define RELAY_7 37
//#define RELAY_PIN7 37
void setup()
{
  pinMode(RELAY_8, OUTPUT);
  pinMode(RELAY_7, OUTPUT);
  Serial.begin(9600); // open serial
  Serial.println("Press the spacebar to toggle relay on/off");
}

void loop()
{
  static int relayVal = 0;
  int cmd;
  
//  if (Serial.available() > 0)
  //{
   
    cmd = Serial.read();
      
    switch (cmd)
    {
    case ' ':
      {
        Serial.println("spacebar pressed");
        relayVal ^= 1; // xor current value with 1 (causes value to toggle)
        if (relayVal)
          Serial.println("Relay on");
        else
          Serial.println("Relay off");
        break;
      }
    default:
      {
        Serial.println("Loop Press the spacebar to toggle relay on/off");
      
       digitalWrite(RELAY_8, HIGH);
        digitalWrite(RELAY_7, LOW);
   
        delay(3000);   
        digitalWrite(RELAY_8, LOW);
        digitalWrite(RELAY_7, HIGH);
         delay(1000); 
      }
    }
      
   // if (relayVal)
    //  digitalWrite(RELAY_PIN, HIGH);
  //  else
     // digitalWrite(RELAY_PIN, LOW);
  // }
}
