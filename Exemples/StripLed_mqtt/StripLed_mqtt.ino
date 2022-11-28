#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "FLOlib_StripLed.h"

WiFiClient espClient;
PubSubClient client(espClient);
StripLed_Manager led;



typedef struct { 
  char* topic_name;
  char* topic_type;
  char* topic_path;
  void (*function)();
} Topic;

const char* ssid     = "Flo-WiFi_2,4GHz";
const char* password = "#Flo-W!F!78#92400**2,4";
// MQTT server
const char* mqtt_server   = "flo-machines.dynv6.net"; // IP Broker MQTT
const int mqtt_port = 1034;
const char* mqtt_username = "mqtt";     // Broker Username login
const char* mqtt_password = "YGGHUG2376673Ggghhhj6755758VBVGHVhjhhjg66"; // Broker Password login
String data; 


void T_Coul_R(){led.Update_strip_led(String("Coul,0,0,")+data);}
#define TOPICS_SIZE 1

 Topic TOPICS[] {
    {"Etoile Noire: Red"  , "rp", "iot/appart_flo/salon/plafond/led/red"   , T_Coul_R}
};

void setup() {
  Serial.begin(9600);
 led.Start_stripLed();

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
 
 led.Update_strip_led(String("Coul,0,0,0"));
 led.Update_strip_led(String("Bright,100"));
}

void loop() {
   if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
 led.StripLed_loop();

}


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

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client",mqtt_username,mqtt_password)) {
      Serial.println("connected");

      // Définition des topics
      for(word k=0; k<TOPICS_SIZE; k++){
        if(TOPICS[k].topic_type == "rp"){
          Serial.println(TOPICS[k].topic_name);
          client.subscribe(TOPICS[k].topic_path); 
         
        }
        if(TOPICS[k].topic_type == "r"){
          Serial.println(TOPICS[k].topic_name);
          client.subscribe(TOPICS[k].topic_path); 
        }

      }
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void callback(char* topic, byte* payload, unsigned int length) {
 // Affiche le topic entrant - display incoming Topic
 data = String();
 Serial.print("Message arrived [");
 Serial.print(topic);
 Serial.print("] ");
 // décode le message - decode payload message
 for (int i = 0; i < length; i++) {
 data+=((char)payload[i]); 
 }
 // Affiche le message entrant - display incoming message
 Serial.println(data);

for(word k=0; k<TOPICS_SIZE; k++){
  if ( strcmp(topic, TOPICS[k].topic_path) == 0 ) {
    Serial.print(TOPICS[k].topic_name);
    Serial.println(data);
    TOPICS[k].function();
 }
}
 delay(15); 
}
