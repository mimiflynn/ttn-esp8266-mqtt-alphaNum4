#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

// Update these with values suitable for your network and Things Network application and device.
const char* ssid = "wifi name";
const char* password = "wifi password";
const char* mqtt_server = "<region>.thethings.network";
const char* mqtt_user = "App Id";
const char* mqtt_password = "App Key";
const String topic = "<app id>/devices/<dev id>/up/<field>";

Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();
WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;

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

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  updateDisplay(payload);
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);
  } else {
    digitalWrite(BUILTIN_LED, HIGH);
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(),mqtt_user,mqtt_password)) {
      Serial.println("connected");
      client.subscribe(topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  Serial.begin(115200);
  Serial.println("Setting up");
  Serial.println();
  
  setup_wifi();
  
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  alpha4.begin(0x70);
  alpha4.writeDigitAscii(0, 'H');
  alpha4.writeDigitAscii(1, 'I');
  alpha4.writeDigitAscii(2, 'Y');
  alpha4.writeDigitAscii(3, 'A');
  alpha4.writeDisplay();
}

void updateDisplay(byte* temp) {
  alpha4.writeDigitRaw(0, 0x0);
  alpha4.writeDigitRaw(1, 0x0);
  alpha4.writeDigitAscii(2, temp[0]);
  alpha4.writeDigitAscii(3, temp[1]);
  alpha4.writeDisplay();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
