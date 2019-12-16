/*
  Basic ESP8266 MQTT example

  This sketch demonstrates the capabilities of the pubsub library in combination
  with the ESP8266 board/library.

  It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off

  It will reconnect to the server if the connection is lost using a blocking
  reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
  achieve the same result without blocking the main loop.

  To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"

*/


#include <WiFi.h>
#include <PubSubClient.h>


// Update these with values suitable for your network.
const char* ssid = "jc";
const char* password = "juancarlos";
const char* mqtt_server = "damaso";
#define mqtt_port 1883
#define MQTT_USER "vperhmxp"
#define MQTT_PASSWORD "Zgc7-jpzdJ_v"
#define MQTT_SERIAL_PUBLISH_CH "/dsh/damaso/proactive/sensor"
#define MQTT_SERIAL_RECEIVER_CH "/icircuit/ESP32/serialdata/rx"
const int inputPressure = 34;
const int inputDetection = 35;
WiFiClient wifiClient;

PubSubClient client(wifiClient);

int inputPressureVal;
int inputDetectionVal;
bool isPerson;
bool playing;

void setup_wifi() {
    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    randomSeed(micros());
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(),MQTT_USER,MQTT_PASSWORD)) {
      Serial.println("connected");
      //Once connected, publish an announcement...
      //client.publish("/dsh/damaso/playing", "hello world");
      // ... and resubscribe
      client.subscribe(MQTT_SERIAL_RECEIVER_CH);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte *payload, unsigned int length) {
    Serial.println("-------new message from broker-----");
    Serial.print("channel:");
    Serial.println(topic);
    Serial.print("data:");  
    Serial.write(payload, length);
    Serial.println();
}

void setup() {
  Serial.begin(115200);
  isPerson = false;
  playing = false;
  inputPressureVal = 0;
  inputDetectionVal = 0;
  pinMode(inputPressure, INPUT);
  pinMode(inputDetection, INPUT);
  
  Serial.setTimeout(1000);// Set time out for 
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  randomSeed(analogRead(0));
  reconnect();
}

void publishSerialData(char *serialData){
  if (!client.connected()) {
    reconnect();
  }
  client.publish(MQTT_SERIAL_PUBLISH_CH, serialData);
}
void loop() {
  inputPressureVal = analogRead(inputPressure);
  inputDetectionVal = analogRead(inputDetection);
  Serial.println(inputPressureVal);
  Serial.println(inputDetectionVal);
  if(isPerson){
    if(!detectIfPersonOnTheRoom(inputDetectionVal) && !detectIfPersonSeated(inputPressureVal)){
      publishSerialData("OFF");
      isPerson = false;
      }
  }
  else{
    if(detectIfPersonSeated(inputPressureVal)){
        isPerson = true;
        publishSerialData("ON");
      }
    }
  delay(1000);
 }

bool detectIfPersonSeated(int inputPressureValue){
  return inputPressureValue > 2000;
}

bool detectIfPersonOnTheRoom(int inputDetectionValue){
  return inputDetectionValue > 0;
}
 
