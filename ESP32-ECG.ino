/****************************************
 * Include Libraries
 ****************************************/
#include <WiFi.h>
#include <PubSubClient.h>

#define WIFISSID "Bahria Faculty" // Put your Wifi SSID here
#define PASSWORD "bulc@fac-01" // Put your WIFI password here
#define TOKEN "BBUS-llkCCBCGG4YJGJjsl12mywraAcfQkV" // Put your Ubidots' TOKEN
#define MQTT_CLIENT_NAME "apneasense1.0" // MQTT client Name, please enter your own 8-12 alphanumeric character ASCII string; 
                                           //it should be a random and unique ascii string and different from all other devices

/****************************************
 * Define Constants
 ****************************************/
#define VARIABLE_LABEL "ecg" // Assign the variable label
#define DEVICE_LABEL "apneasense" // Assign the device label

#define SENSOR A0 // Set the A0 as SENSOR

char mqttBroker[]  = "industrial.api.ubidots.com";
char payload[100];
char topic[150];
double voltage = 0.0;

/****************************************
 * Auxiliar Functions
 ****************************************/
WiFiClient ubidots;
PubSubClient client(ubidots);

void callback(char* topic, byte* payload, unsigned int length) {
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;
  Serial.write(payload, length);
  Serial.println(topic);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    
    // Attempt to connect
    if (client.connect(MQTT_CLIENT_NAME, TOKEN, "")) {
      Serial.println("Connected");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      // Wait 2 seconds before retrying
      delay(2000);
    }
  }
}

/****************************************
 * Main Functions
 ****************************************/
void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFISSID, PASSWORD);
  // Assign the pin as INPUT 
  pinMode(SENSOR, INPUT);

  Serial.println();
  Serial.print("Press any key to start conversion");
  while (Serial.available() == 0) ; // Wait until user presses a key
  Serial.read(); // Clear the buffer
  
  Serial.println();
  Serial.print("Waiting for WiFi...");
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqttBroker, 1883);
  client.setCallback(callback);  
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  sprintf(topic, "%s%s", "/v1.6/devices/", DEVICE_LABEL);
  sprintf(payload, "%s", ""); // Clean the payload
  double analogValue = analogRead(SENSOR); 
  Serial.println("ECG ADC Value :: ");
  Serial.println(analogValue);
  voltage = analogValue * (3.3 / 4095.0); 
  Serial.println("Voltage :: ");
  Serial.println(voltage);
  sprintf(payload, "{\"%s\": %.2f}", VARIABLE_LABEL,voltage);
  Serial.println("Publishing data to Ubidots Cloud");
  client.publish(topic, payload);
  client.loop();
  delay(1000);
}
