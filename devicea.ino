#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

#define pin D5
// wifi
const char* ssid = "your-ssid";
const char* password = "yourpassword";
const char* topic_rec = "esp_b";
const char* topic_send = "esp_a";
// mqtt online
const char* MQTT_CLIENT_ID = "esp88_b";
const char* MQTT_SERVER_IP = "your-server.cloudmqtt.com";
const uint16_t MQTT_SERVER_PORT = 18557;
const char* MQTT_USER = "user";
const char* MQTT_PASSWORD = "password";
bool pre=1;
unsigned long pre_time=0;
int intervallo = 10000, pre_rdn=3;
char* arr_st[] = {"A","B","C"};
uint8_t cuore[]={
  B00000000,
  B01100110,
  B11100111,
  B11111111,
  B11111111,
  B01111110,
  B00111100,
  B00011000
},
frecciasx[]={
  B00000000,
  B01100110,
  B11100111,
  B11111111,
  B11111111,
  B01111110,
  B00111100,
  B00011000
},
frecciaup[]={
  B00000000,
  B01100110,
  B11100111,
  B11111111,
  B11111111,
  B01111110,
  B00111100,
  B00011000
},
full[]={
  B11111111,
  B11111111,
  B11111111,
  B11111111,
  B11111111,
  B11111111,
  B11111111,
  B11111111
},
empty[]={
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0
};
WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_8x8matrix matrix = Adafruit_8x8matrix();

void setup_wifi() {
    // pass in the address
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  randomSeed(micros());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    if (client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD)) {
    client.subscribe(topic_rec);
    Serial.println("MQTT connesso");
    //client.publish(topic_send,"OK");
    } else {
      delay(3000);
    }
  }
}

void cmd_8x8(uint8_t img[]){
  matrix.clear();
  matrix.drawBitmap(0, 0, img, 8, 8, LED_ON);
  matrix.writeDisplay();
}
void callback(char* topic, byte* payload, unsigned int length) {
String stringa;

for(int i=0;i<length;i++){
    stringa+=((char)payload[i]);
  }
  Serial.print(stringa);
if (stringa=="A") cmd_8x8(cuore);
if (stringa=="B") cmd_8x8(frecciasx);
if (stringa=="C") cmd_8x8(frecciaup);
pre_time = millis();
}

void setup() {
  Serial.begin(9600);
  Wire.pins(D1, D2);
  Serial.println("8x8 LED Matrix Test");
  matrix.begin(0x70);
  matrix.setRotation(-1);
  matrix.clear();
setup_wifi();
client.setServer(MQTT_SERVER_IP, MQTT_SERVER_PORT);
client.setCallback(callback);
Serial.println("tutto connesso");
pinMode(pin,INPUT_PULLUP);
randomSeed(micros());
cmd_8x8(full);
delay(1000);
cmd_8x8(empty);
}

void loop() {
 if ( !client.connected() ) {
    reconnect();
}
bool in = digitalRead(pin);
if((in==0) && (in!=pre)){
  int rd;
  do{
  rd = random(0,3);
  }while(rd==pre_rdn);
  pre_rdn = rd;
  Serial.println(arr_st[rd]);
  Serial.print("in");
  client.publish(topic_send,arr_st[rd]);
  
}
if (millis() > (pre_time+intervallo)){
  cmd_8x8(empty);
}
pre=in;
client.loop();
}
