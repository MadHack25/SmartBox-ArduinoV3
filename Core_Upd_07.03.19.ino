/*
 * 
 *  SmarTBox By MadHackers
 *  Libraries Used DHT , SevenSegmentTM1637
 *  Fourh/Optimized Version 
 *  .Upgrade at 17.03.19
 *  .Latest Upgrade at 03.07.19
 */

/*
 *   Libraries
 */

#include "DHT.h"
#include "SevenSegmentTM1637.h"

/*
 *  PINS
 */

const byte PR_CLK = 5;   // Primary Display CLK PIN
const byte PR_DIO = 4;   // Primary Display DATA PIN

const byte SEC_CLK = 3;   // Secondary Display CLK PIN
const byte SEC_DIO = 2;   // Secondary Display DATA PIN

#define DHTPIN 13     // DHT Digital PIN 13
#define DHTTYPE DHT22   // DHT 22  (AM2302) Version
#define pot A0  // 10KoM Potentiometer PIN
#define relay 10  // Relay Module PIN
#define action_LED 6  // Green LED PIN
#define idle_LED 7  // RED LED PIN

/*
 *   VARIABLES
 */

int pot_value = 0;  // 10KoM Potentiometer Calue
byte h_humid = 0;   // DHT22 Humidity Value

/*
 *   INITS
 */
 
SevenSegmentTM1637    prim_display(PR_CLK, PR_DIO);   // INIT Primary Display
SevenSegmentTM1637    sec_display(SEC_CLK, SEC_DIO);  // INIT Secondary Display
DHT dht(DHTPIN, DHTTYPE); // IMIT DHT22 (AM2302) Temperature & Humidity Sensor

// run setup code
void setup() {
  Serial.begin(9600);  // initializes the Serial connection @ 9600 baud
  TM1637INITS(); // INIT TM1637 7Segment Displays

  dht.begin();    // INIT DHT

  // Set Outputs
  pinMode(relay, OUTPUT);
  pinMode(action_LED, OUTPUT);
  pinMode(idle_LED, OUTPUT);
  
  digitalWrite(relay, HIGH); // Turn OFF Relay By Default
  delay(1000);      // wait 1000 ms
};

/*
 *   L O O P
 */

void loop() {

      /*
       *   ASSIGN VALUES
       */
      setValues();
      
      secondaryShow(); // Set Desired Humidity Level From 10KoM Potentiometer
      primaryShow(); // Print Current Humidity Value
      
      comparator();  // Start The Process
      delay(2000);
};

void TM1637INITS(){
   prim_display.begin();
   sec_display.begin();

   prim_display.setBacklight(50);   // Adjust Brightnesses
   sec_display.setBacklight(50);    // Adjust Brightnesses
   
   prim_display.print("INIT");   // INIT Immitation

   for (uint8_t i=0; i <= 100; i+=10){
      sec_display.print(i);
      delay(100);
   }

   sec_display.print("DONE");
   sec_display.blink();
   delay(100);
   TM1637ScreenClear();
}

/*
 *   TM1637 Screen Clear
 */

void TM1637ScreenClear(){
  sec_display.clear();
  prim_display.clear();
}


/*
 *   SETTERS
 */

void setValues(){
    // Get Value of 10KoM Potentiometer
    pot_value = analogRead(pot); 
    pot_value /= 10;
    h_humid =  dht.readHumidity();
}

/*
 *    GETTERS
 */

 void secondaryShow(){
      sec_display.clear();
      sec_display.print(pot_value);   // Show Humidity
 }

 void primaryShow(){

     // Added Display H and T With Values AT 03.26.19
     // Show H + Humidity Level
     prim_display.clear(); 
     prim_display.setCursor(0,0);
     prim_display.print("H");   // Show H Symbol  
     prim_display.setCursor(1,2);
     prim_display.print(h_humid);  // Show Current Humidity Level
 }

void switchRelay(bool key){
  // LOW for Turning ON
  // HIGH for Turning OFF
  if (key) digitalWrite(relay, LOW);
  else digitalWrite(relay, HIGH);
}

bool setBaunds(){

  // Sensor Error
  if(h_humid == 0)
  {
    blinkError();
    prim_display.print("SENSOR ERROR");
  }

  else
  {
    // If Potentiometer Returned Values are Less than 10 and More than 90 SwitchOff Relay
    // Edited At 03.26.19 was 90
    if (pot_value < 10 || pot_value > 95){
  
      // Stop
      switchRelay(false);
      blinkError();
      // Edited At 03.26.19 was 10
      if (pot_value < 5) sec_display.print("LOU");
      else sec_display.print("HI");
      return false;
    }
    else return true;
  }
}

void comparator(){
   if (setBaunds()){
    // Edited At 03.26.19 WAS +2
     if((h_humid+5) <= pot_value){
        // Start Process
        switchRelay(true);
        digitalWrite(action_LED, HIGH);
        digitalWrite(idle_LED, LOW);
        sec_display.blink();
     }
     else{
        // Stop Process
        switchRelay(false);
        digitalWrite(action_LED, LOW);
        digitalWrite(idle_LED, HIGH);
     }
   }
}

void blinkError(){
      digitalWrite(idle_LED, HIGH);
      digitalWrite(action_LED, HIGH);
      delay(600);
      digitalWrite(idle_LED, LOW);
      digitalWrite(action_LED, LOW);
}
