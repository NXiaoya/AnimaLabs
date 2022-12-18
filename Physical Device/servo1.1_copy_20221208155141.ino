
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <ezTime.h>

#include <Adafruit_NeoPixel.h>

//Define neopixel initialization
//Connect button pin as pin 12 on board
#define PIN 12
#define NUMPIXELS 8
#define DELAYVAL 500
Adafruit_NeoPixel pixels(NUMPIXELS, PIN);

// Wifi and MQTT
#include "arduino_secrets.h"

const char* mqttServer = "mqtt.cetools.org";
const int mqttPort = 1884;
const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;
const char* mqttuser = SECRET_MQTTUSER;
const char* mqttpass = SECRET_MQTTPASS;

char distance;
int pos0;
int pos;
int pos1;
//unsigned long time;
Servo myservo;

//ESP8266WebServer server(80);
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

//Total time, here I set as 20 senconds. You can change the time here.
float timeSeconds = 20;
float timeInter = 2.5;

//Connect button pin as pin 13 on board
const int buttonPin = 13;

//Neopixel led
int RedLED = 8;
int GreenLED = 0;


// Timer: Auxiliary variables
unsigned long current = millis();
//the time trigger when start timer
unsigned long lastTrigger = 0;
//the time trigger when end timer
unsigned long pauseTrigger = 0;
//time left for timer
unsigned long TimeLeft = 10000;

unsigned long timerDelay = 1000;

unsigned long lastTime = 0;

//The flag
boolean startTimer = false;
boolean StartProgram = true;
boolean endTimer = false;
boolean Task = true;

// Date and time
Timezone GB;

void setup() {
  // put your setup code here, to run once:
  myservo.attach(14);

  Serial.begin(115200);

  startWifi();
  //startservo();
  client.setServer(mqttServer, mqttPort);

  client.setCallback(callback);

  client.subscribe("student/CASA0022/AnimaLabs/distance");
  //time= millis();

  startservo();

  // Initialize neopixel
  pixels.begin();
  // Button mode INPUT_PULLUP
  pinMode(buttonPin, INPUT_PULLUP);
  // Set button as interrupt, assign interrupt function and set RISING mode
  attachInterrupt(digitalPinToInterrupt(buttonPin), detectButton, FALLING);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  checkDate();

  if (StartProgram) {
    for (int i = 0; i <= 8; i++) {
      pixels.setPixelColor(i, 150, 0, 0);
      pixels.show();
    }
    StartProgram = false;
  }

  if ((millis() - lastTime) > timerDelay) {
    Serial.println((millis() - lastTime));
    sendMQTTPos();
    sendMQTTLight();
    lastTime = millis();
  }
}

void startWifi() {
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // check to see if connected and wait until you are
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void startservo() {

  for (pos0 = 0; pos0 <= 104; pos0++) {
    myservo.write(pos0);
    delay(15);
  }
  for (pos0 = 104; pos0 >= 0; pos0--) {
    myservo.write(pos0);
    delay(15);
  }
}




void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived ");
  Serial.print(topic);

  if (String(topic) == "student/CASA0022/AnimaLabs/distance") {
    Serial.print("Message:");

    char distance[length + 1];

    for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
      distance[i] = (char)payload[i];
    }

    //distance[length] = '\0';

    Serial.println();
    Serial.println(distance);
    Serial.println("-----------------------");
    //delay(10000);

    distance[length] = '\0';
    String stringOne = String(distance);  // converting a constant char into a String
    pos1 = stringOne.toInt();
    pos = map(pos1, 2, 17, 104, 0);
    if (pos1 >= 17) {
      pos = 104;
    }
    if (pos1 <= 2) {
      pos = 0;
    }
    Serial.println(pos);
    myservo.write(pos);
    //delay(15);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);

    // Attempt to connect with clientID, username and password
    if (client.connect(clientId.c_str(), mqttuser, mqttpass)) {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe("student/CASA0022/AnimaLabs/distance");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// This is the interrupt service routine
ICACHE_RAM_ATTR void detectButton() {
  current = millis();
  if ((current - lastTrigger) > 50) {
    //Print when button pressed
    Serial.println("Button pressed");
    // Current time


    //If the timer task not finished
    if (Task) {
      //The first time enter the for function, flag for start and pause the timer are both false
      if ((startTimer == false) && (endTimer == false)) {
        Serial.println("enter first if");
        Serial.println("Timer start");
        Serial.println(" ");
        //start the timer
        startTimer = true;
        endTimer = false;
        lastTrigger = millis();
      } else if (startTimer) {
        //pause the timer
        Serial.println("enter else");
        startTimer = false;
        endTimer = true;
        Serial.println("Timer pause");
        //calculate the time left
        pauseTrigger = millis();
        // Turn off the timer and end the task when time left is 0
        if (endTimer && (TimeLeft * 1000 <= (pauseTrigger - lastTrigger))) {
          //Timer end, the dog walking task achieved, all led green
          for (int i = 0; i < 8; i++) {
            pixels.setPixelColor(i, 0, 25, 0);
            pixels.show();
          }
          Serial.println("Timer end...");
          startTimer = false;
          Task = false;
        } else {
          Serial.print("last trigger:");
          Serial.println(lastTrigger);
          Serial.print("Pause at:");
          Serial.println(pauseTrigger);
          TimeLeft = timeSeconds - (pauseTrigger - lastTrigger) / 1000;
          //Calculate the number of red and green led
          RedLED = TimeLeft / timeInter;
          GreenLED = 8 - RedLED;
          //configure neopixel strip
          for (int i = 0; i <= RedLED; i++) {
            pixels.setPixelColor(i, 25, 0, 0);
            pixels.show();
          }
          for (int i = 0; i < GreenLED; i++) {
            //add green after red leds
            pixels.setPixelColor(i + RedLED, 0, 25, 0);
            pixels.show();
          }
          timeSeconds = TimeLeft;
          Serial.print("Time Left: ");
          Serial.println(TimeLeft);
          Serial.println(" ");
        }
      } else if (endTimer) {
        //start the timer again
        startTimer = true;
        endTimer = false;
        lastTrigger = millis();
        Serial.println("Timer start again");
        Serial.println(" ");
      }

    } else {  // task=false, no task there, today walking achieved
      Serial.println("Today's task achieved");
      Serial.println(" ");
    }
  }
}

void checkDate() {
  // get real date and time
  waitForSync();
  //Serial.println("UTC: " + UTC.dateTime());
  GB.setLocation("Europe/London");
  String currenttime = GB.dateTime("G");
  //Serial.println("London hour time: " + GB.dateTime("G"));
  if (currenttime == 0) {
    StartProgram = false;
  }
}

void sendMQTTLight() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  Serial.println(GreenLED);
  snprintf(msg, 50, "%.1i", GreenLED);
  Serial.print("Publish message for green Led number: ");
  Serial.println(msg);
  client.publish("student/CASA0022/AnimaLabs/green", msg);

  Serial.println(RedLED);
  snprintf(msg, 50, "%.1i", RedLED);
  Serial.print("Publish message for red Led number: ");
  Serial.println(msg);
  client.publish("student/CASA0022/AnimaLabs/red", msg);

  Serial.println(TimeLeft);
  snprintf(msg, 50, "%.0i", TimeLeft);
  Serial.print("Publish message for time left: ");
  Serial.println(msg);
  client.publish("student/CASA0022/AnimaLabs/timeLeft", msg);
}

void sendMQTTPos() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();


  snprintf(msg, 50, "%.0i", pos);
  Serial.print("Publish message for pointer position: ");
  Serial.println(msg);
  client.publish("student/CASA0022/AnimaLabs/degree", msg);
}