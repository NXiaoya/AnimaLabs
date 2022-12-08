

/*********
  use press button as a timer
*********/
//The library for neopixel strip
#include <Adafruit_NeoPixel.h>

//Define neopixel initialization
//Connect button pin as pin 14 on board
#define PIN 14
#define NUMPIXELS 8
#define DELAYVAL 500
Adafruit_NeoPixel pixels(NUMPIXELS, PIN);


//Total time, here I set as 20 senconds. You can change the time here.
float timeSeconds = 20;
float timeInter= 2.5 ;

//Connect button pin as pin 13 on board
const int buttonPin = 13;

//Neopixel led
int RedLED = 8;
int GreenLED = 0;

// Timer: Auxiliary variables
unsigned long now = millis();
//the time trigger when start timer
unsigned long lastTrigger = 0;
//the time trigger when end timer
unsigned long pauseTrigger = 0;
//time left for timer
unsigned long TimeLeft = 10000;

//The flag
boolean startTimer = false;
boolean StartProgram = true;
boolean endTimer = false;
boolean Task = true;


// This is the interrupt service routine
ICACHE_RAM_ATTR void detectButton() {
      now = millis();
      if ( (now-lastTrigger)> 50){
          //Print when button pressed
        Serial.println("Button pressed");
                // Current time
        

        //If the timer task not finished 
        if (Task){
          //The first time enter the for function, flag for start and pause the timer are both false
          if((startTimer == false) && (endTimer == false)){
          Serial.println("enter first if");
          Serial.println("Timer start");
          Serial.println(" ");
          //start the timer
          startTimer = true;
          endTimer = false;
          lastTrigger = millis();
          } else if (startTimer){
            //pause the timer
            Serial.println("enter else");
            startTimer = false;
            endTimer = true;
            Serial.println("Timer pause");
            //calculate the time left
            pauseTrigger = millis();
            // Turn off the timer and end the task when time left is 0
              if(endTimer && (TimeLeft*1000 <= (pauseTrigger - lastTrigger))) {
                //Timer end, the dog walking task achieved, all led green
                for(int i=0; i<8; i++) {
                    pixels.setPixelColor(i, 0, 150, 0);
                    pixels.show();
                  }
                Serial.println("Timer end...");
                startTimer = false;
                Task = false;
              }else {
                Serial.print("last trigger:");
                Serial.println(lastTrigger);
                Serial.print("Pause at:");
                Serial.println(pauseTrigger);
                TimeLeft = timeSeconds-(pauseTrigger - lastTrigger)/1000;
                //Calculate the number of red and green led
                RedLED = TimeLeft/timeInter;
                GreenLED = 8 - RedLED;
                //configure neopixel strip
                for(int i=0; i<=RedLED; i++) {
                    pixels.setPixelColor(i, 150, 0, 0);
                    pixels.show();
                  }
                for(int i=0; i<GreenLED; i++) {
                  //add green after red leds
                    pixels.setPixelColor(i+RedLED, 0, 150, 0);
                    pixels.show();
                  }
                timeSeconds = TimeLeft;
                Serial.print("Time Left: ");
                Serial.println(TimeLeft);
                Serial.println(" ");
              }
          } else if(endTimer){
            //start the timer again
            startTimer = true;
            endTimer = false;
            lastTrigger = millis();
            Serial.println("Timer start again");
            Serial.println(" ");
          }
          
        } else {// task=false, no task there, today walking achieved
          Serial.println("Today's task achieved");
          Serial.println(" ");
        }
      }
      
  
}

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);
  // Initialize neopixel
  pixels.begin();
  // Button mode INPUT_PULLUP
  pinMode(buttonPin, INPUT_PULLUP);
  // Set button as interrupt, assign interrupt function and set RISING mode
  attachInterrupt(digitalPinToInterrupt(buttonPin), detectButton, FALLING);

}

void loop() {
   if(StartProgram){
        for(int i=0; i<=8; i++) {
      pixels.setPixelColor(i, 150, 0, 0);
      pixels.show();
    }  
    StartProgram = false;
   }

 
}
