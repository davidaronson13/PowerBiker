/ Test Relay signal when button is pressed
// Use PIN 7 to send signal to relay
// Turn on LED when the button is pressed to test Sketch code
// and keep it on after it is released
// including simple de-bouncing
//

 #define LED 13                    // the pin for the LED
 #define BUTTON 2              // the input pin where the pushbutton is connected
 #define RELAY 7                 // PIN for relay signal
 
 int val=0;                               // val will be used to store the state
                                                // of the input pin
 int old_val=0;                      // this variable stores the previous
                                               // value of "val"
 int state=0;                          //0=LED off and 1 = LED on

void setup(){
 pinMode(LED, OUTPUT);               // tell arduino LED is an output
 pinMode(BUTTON, INPUT);           // and BUTTON is an input
 pinMode(RELAY, OUTPUT);          // tells arduino RELAY is an output
}

void loop(){
 val=digitalRead(BUTTON);         // read input value and store it -- new input
                                  
 // check if there was a transition
 if ((val==HIGH) && (old_val==LOW)){
   state = 1 - state;
   delay(10);
 }

 old_val=val;                    // val is now old, let's store it

 if (state == 1){
  digitalWrite(LED,HIGH);           // turn LED ON
  digitalWrite(RELAY,HIGH);      // turn RELAY ON
 }else{
  digitalWrite(LED,LOW);            // turn LED OFF
  digitalWrite(RELAY,LOW);       // turn RELAY OFF
 }
} 
                      
