#include <Arduino.h>
//Load WiFi library
#include <WiFi.h>
//Your network credentials
const char* ssid = "";
const char* password = "";
//Set webserver with port number 80
WiFiServer server(80);
//Variable to store HTTP request
String header;
//Auxiliar variables to store the current status ouput
String output26State = "off";
String output27State = "off";
//Assign variables to output pin
const int output26 = 26;
const int output27 = 27;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  //Initialize pin as Output
  pinMode(output26, OUTPUT);
  pinMode(output27, OUTPUT);
  //Set output to LOW
  digitalWrite(output26, LOW);
  digitalWrite(output27, LOW);
  //Connecting to your WiFi network with ssid and password
  Serial.println("Connecting to ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(".");
  }
  //Get and print local IP adress
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address :");
  Serial.println(WiFi.localIP());
  //Start webserver
  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  WiFiClient client = server.available();
  
}
