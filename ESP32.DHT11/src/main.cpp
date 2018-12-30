#include <Arduino.h>
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

// Uncomment one of the lines below for whatever DHT sensor type you're using!
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

// Replace with your network credentials
const char* ssid     = "MAI MEO";
const char* password = "hoidelamgi";

WiFiServer server(80);

// DHT Sensor
const int DHTPin = 16;
// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);

// Temporary variables
static char celsiusTemp[7];
static char fahrenheitTemp[7];
static char humidityTemp[7];

// Client variables
char linebuf[80];
int charcount=0;

void setup() {
  // initialize the DHT sensor
  dht.begin();

  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  while(!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  // attempt to connect to Wifi network:
  while(WiFi.status() != WL_CONNECTED) {
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  // listen for incoming clients
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New client");
    memset(linebuf,0,sizeof(linebuf));
    charcount=0;
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        //read char by char HTTP request
        linebuf[charcount]=c;
        if (charcount<sizeof(linebuf)-1) charcount++;
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
            // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
            float h = dht.readHumidity();
            // Read temperature as Celsius (the default)
            float t = dht.readTemperature();
            // Read temperature as Fahrenheit (isFahrenheit = true)
            float f = dht.readTemperature(true);
            // Check if any reads failed and exit early (to try again).
            if (isnan(h) || isnan(t) || isnan(f)) {
              Serial.println("Failed to read from DHT sensor!");
              strcpy(celsiusTemp,"Failed");
              strcpy(fahrenheitTemp, "Failed");
              strcpy(humidityTemp, "Failed");
            }
            else{
              // Computes temperature values in Celsius + Fahrenheit and Humidity
              float hic = dht.computeHeatIndex(t, h, false);
              dtostrf(hic, 6, 2, celsiusTemp);
              float hif = dht.computeHeatIndex(f, h);
              dtostrf(hif, 6, 2, fahrenheitTemp);
              dtostrf(h, 6, 2, humidityTemp);
          }
          // and a content-type so the client knows what's coming, then a blank line:
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println("Connection: close");
          client.println();

          // Display the HTML web page
          client.println("<!DOCTYPE html><html>");
          client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
          client.println("<link rel=\"icon\" href=\"data:,\">");
          // CSS to style the table
          client.println("<style>body { text-align: center; font-family: \"Trebuchet MS\", Arial;}");
          client.println("table { border-collapse: collapse; width:35%; margin-left:auto; margin-right:auto; }");
          client.println("th { padding: 12px; background-color: #0043af; color: white; }");
          client.println("tr { border: 1px solid #ddd; padding: 12px; }");
          client.println("tr:hover { background-color: #bcbcbc; }");
          client.println("td { border: none; padding: 12px; }");
          client.println(".sensor { color:white; font-weight: bold; background-color: #bcbcbc; padding: 1px; }");

          // Web Page Heading
          client.println("</style></head><body><h1>ESP32 with DHT11</h1>");
          client.println("<table><tr><th>MEASUREMENT</th><th>VALUE</th></tr>");
          client.println("<tr><td>Temp. Celsius</td><td><span class=\"sensor\">");
          client.println(celsiusTemp);
          client.println(" *C</span></td></tr>");
          client.println("<tr><td>Temp. Fahrenheit</td><td><span class=\"sensor\">");
          client.println(fahrenheitTemp);
          client.println(" *F</span></td></tr>");
          client.println("<tr><td>Humidity</td><td><span class=\"sensor\">");
          client.println(humidityTemp);
          client.println(" %</span></td></tr>");
          client.println("</body></html>");

          // The HTTP response ends with another blank line
          client.println();
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
          memset(linebuf,0,sizeof(linebuf));
          charcount=0;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);

    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}
