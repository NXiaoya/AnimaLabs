/*  
 *  Based on:
 *  https://howtomechatronics.com/tutorials/arduino/ultrasonic-sensor-hc-sr04/ 
 *  
 */

 ////add the library
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ezTime.h>
#include <PubSubClient.h>

#define trigPin 12
#define echoPin 13

// Wifi and MQTT
#include "arduino_secrets.h" 

const char* ssid     = SECRET_SSID;
const char* password = SECRET_PASS;
const char* mqttuser = SECRET_MQTTUSER;
const char* mqttpass = SECRET_MQTTPASS;

ESP8266WebServer server(80);
const char* mqtt_server = "mqtt.cetools.org";
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
float distance = 100;

// Date and time
Timezone GB;


void setup() {
  //start serial communication
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

    // run initialisation functions
  startWifi();
  syncDate();

  // start MQTT server
  client.setServer(mqtt_server, 1884);
  client.setCallback(callback);
}

void loop() {
  // handler for receiving requests to webserver
  server.handleClient();
    if (minuteChanged()) {
    readDistance();
    sendMQTT();
    Serial.println(GB.dateTime("H:i:s")); // UTC.dateTime("l, d-M-y H:i:s.v T")
  }

  client.loop();

}

void readDistance(){
 // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  long duration = pulseIn(echoPin, HIGH); 
  distance = 34400*duration/2000000;
  
  Serial.print("Distance in cm: ");
  Serial.println(distance);

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

void syncDate() {
  // get real date and time
  waitForSync();
  Serial.println("UTC: " + UTC.dateTime());
  GB.setLocation("Europe/London");
  Serial.println("London time: " + GB.dateTime());

}

void sendMQTT() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  int depth = distance;
  Serial.println(depth);
  snprintf (msg, 50, "%.0i", depth);
  Serial.print("Publish message for m: ");
  Serial.println(msg);
  client.publish("student/CASA0022/ucfnnie/distance", msg);

}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
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
      client.subscribe("student/CASA0022/ucfnnie/distance/inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}