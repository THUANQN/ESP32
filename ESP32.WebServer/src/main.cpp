#include <Arduino.h>
//Load WiFi library
#include <WiFi.h>
//Your network credentials
const char* ssid = "MAI MEO";
const char* password = "hoidelamgi";
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
        // Check if new client connects
        if (client) {
                Serial.println("New client."); //Print a message out in the serial port
                String currentLine = ""; //Make a String to hold incomming data from client
                // Check while client still connect
                while (client.connected()) {
                        // Check if there is bytes to read from the client
                        if (client.available()) {
                                char c = client.read(); // read a byte
                                Serial.write(c);
                                header +=c; // add character to header
                                // If the byte is a newline character
                                if (c == '\n') {
                                        if (currentLine.length() == 0) {
                                                // if the current line is blank, you got two newline characters in a row.
                                                // that's the end of the client HTTP request, so send a response.
                                                // checking if header is valid
                                                // YWRtaW46YWRtaW4= = 'admin:admin' (user:pass) base64 encode
                                                // Finding the right credential string, then loads web page
                                                if(header.indexOf("YWRtaW46YWRtaW4=") >= 0) {
                                                        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                                                        // and a content-type so the client knows what's coming, then a blank line:
                                                        client.println("HTTP/1.1 200 OK");
                                                        client.println("Content-type:text/html");
                                                        client.println("Connection: close");
                                                        client.println();

                                                        // turns the GPIOs on and off
                                                        if (header.indexOf("GET /26/on") >= 0) {
                                                                Serial.println("GPIO 26 on");
                                                                output26State = "on";
                                                                digitalWrite(output26, HIGH);
                                                        } else if (header.indexOf("GET /26/off") >= 0) {
                                                                Serial.println("GPIO 26 off");
                                                                output26State = "off";
                                                                digitalWrite(output26, LOW);
                                                        } else if (header.indexOf("GET /27/on") >= 0) {
                                                                Serial.println("GPIO 27 on");
                                                                output27State = "on";
                                                                digitalWrite(output27, HIGH);
                                                        } else if (header.indexOf("GET /27/off") >= 0) {
                                                                Serial.println("GPIO 27 off");
                                                                output27State = "off";
                                                                digitalWrite(output27, LOW);
                                                        }

                                                        // Display the HTML web page
                                                        client.println("<!DOCTYPE html><html>");
                                                        client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
                                                        client.println("<link rel=\"icon\" href=\"data:,\">");
                                                        // CSS to style the on/off buttons
                                                        // Feel free to change the background-color and font-size attributes to fit your preferences
                                                        client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
                                                        client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
                                                        client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
                                                        client.println(".button2 {background-color: #555555;}</style></head>");

                                                        // Web Page Heading
                                                        client.println("<body><h1>ESP32 Web Server</h1>");

                                                        // Display current state, and ON/OFF buttons for GPIO 26
                                                        client.println("<p>GPIO 26 - State " + output26State + "</p>");
                                                        // If the output26State is off, it displays the ON button
                                                        if (output26State=="off") {
                                                                client.println("<p><a href=\"/26/on\"><button class=\"button\">ON</button></a></p>");
                                                        } else {
                                                                client.println("<p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p>");
                                                        }

                                                        // Display current state, and ON/OFF buttons for GPIO 27
                                                        client.println("<p>GPIO 27 - State " + output27State + "</p>");
                                                        // If the output27State is off, it displays the ON button
                                                        if (output27State=="off") {
                                                                client.println("<p><a href=\"/27/on\"><button class=\"button\">ON</button></a></p>");
                                                        } else {
                                                                client.println("<p><a href=\"/27/off\"><button class=\"button button2\">OFF</button></a></p>");
                                                        }
                                                        client.println("</body></html>");

                                                        // The HTTP response ends with another blank line
                                                        client.println();
                                                        // Break out of the while loop
                                                        break;
                                                } else {
                                                        client.println("HTTP/1.1 401 Unauthorized");
                                                        client.println("WWW-Authenticate: Basic realm=\"Secure\"");
                                                        client.println("Content-Type: text/html");
                                                        client.println();
                                                        client.println("<html>Authentication failed</html>");
                                                }
                                        } else { // if you got a newline, then clear currentLine
                                                currentLine = "";
                                        }
                                } else if (c != '\r') { // If the byte is not a newline and return character
                                        currentLine += c; // add character to currentLine
                                }
                        }
                }
                // If client doesn't connect
                header = ""; // Clear the header variable
                client.stop(); // Close the connection
                Serial.println("Client disconnected");
                Serial.println("");
        }
}
