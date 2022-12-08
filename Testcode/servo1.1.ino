
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <ezTime.h>

// Wifi and MQTT
#include "arduino_secrets.h" 

const char* mqttServer = "mqtt.cetools.org";
const int mqttPort = 1884;
const char* ssid     = SECRET_SSID;
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

  client.subscribe("UCL/OPS/107/SLS/WS1361_01/dB");
  //time= millis();

  startservo();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected()){ 
    reconnect();
  }

  client.loop();

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

void startservo(){
    
  for (pos0 = 0; pos0 <= 150; pos0++) {
    myservo.write(pos0);
    delay(15);
  }
  for (pos0 = 150; pos0 >= 0; pos0--) {
    myservo.write(pos0);
    delay(15);
  }

  }




void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived ");
  Serial.println(topic);
  if (String(topic) == "UCL/OPS/107/SLS/WS1361_01/dB") {
    Serial.print("Message:");

    char distance[length+1];

    for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
      distance[i] = (char)payload[i];
    }
  

    Serial.println();
    Serial.println(distance);
    Serial.println("-----------------------");
    delay(10000);

    distance[length]= '\0';
     String stringOne = String(distance);// converting a constant char into a String
    pos1 = stringOne.toInt();
    pos = map(pos1, 50, 70, 5, 150);
    Serial.println(pos);

    //while (pos = pos, pos <= 150) {
        myservo.write(pos);
        delay(15);
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
      client.subscribe("UCL/OPS/107/SLS/WS1361_01/dB");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}