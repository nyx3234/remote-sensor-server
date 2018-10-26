// JEFFREY LAKEMAN @ 18-10-2018
// Arduino/C++ 1.9.7
// Gets temperature data from a I2C connected Si7021 module, then uploads that data to a ThingSpeak channel
// Also has a blue flashing "working" led and a red "too hot" flashing led

// Include libraries
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <SI7021.h>

// Network access credentials
const char* ssid = "YOUR-SSID";
const char* password = "YOUR-PASSWORD";

const char* host = "api.thingspeak.com";
String apiKey = "YOUR-WRITE-API-KEY";

// Define variables
#define SDA 4
#define SCL 5

SI7021 sensor;

bool led = false;
float temp;
float temp_max = 30;

void setup() {
  // Connect to the wifi
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nIP address: ");
  Serial.println(WiFi.localIP());

  // Begin connection to sensor and setup leds
  sensor.begin(SDA,SCL);
  Serial.println("Sensor started");
  pinMode(0, OUTPUT);
  pinMode(2, OUTPUT);
}

int value = 0;

void loop() {
  // Connect to thingspeak api and upload sensor data
  value++;
  if (value % 8 == 0) {
  WiFiClient client;
  const int httpPort = 80;

  if(!client.connect(host, httpPort)) {
    Serial.println("Connection Failed");
    delay(300);
    return;
  }

  int temperature = sensor.getCelsiusHundredths();
  temp = temperature / 100.0;
  String link = "GET /update?api_key=" + apiKey + "&field1=";
  link = link + String(temp);
  link = link + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n";
  client.print(link);
  Serial.println(link + "\n");
  delay(100);

  int timeout = 0;
  while((!client.available()) && (timeout < 1000)) {
    delay(10);
    timeout++;
  }

  if(timeout < 500) {
    while(client.available()) {
      // Print response from thingspeak server
      Serial.println(client.readString());
    }
  } else {
    Serial.println("Request timeout...");
  }
  }

  if (value % 3 == 0) {
    // Flash blue led to show that module is working
    digitalWrite(2, LOW);
    delay(100);
    digitalWrite(2, HIGH);
  }

  if(temp >= temp_max) {
    // If sensor data is above max, begin flashing red led
    led = !led;
    if(led == true) {
      digitalWrite(0, HIGH);
    } else {
      digitalWrite(0, LOW);
    }
  } else {
    led = false;
    digitalWrite(0, HIGH);
  }
  delay(500);
}
