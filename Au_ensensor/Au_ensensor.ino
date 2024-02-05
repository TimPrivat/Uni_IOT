/*
  https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/soft-access-point-class.html
*/

/* Create a WiFi access point and provide a web server on it. */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DHTesp.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "ESPAsyncWebServer.h"

// OLED FeatherWing buttons map to different pins depending on board.
// The I2C (Wire) bus may also be different.
#if defined(ESP8266)
  #define BUTTON_A  0
  #define BUTTON_B 16
  #define BUTTON_C  2
  #define WIRE Wire
#endif
DHTesp dht;
AsyncWebServer server(80);

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &WIRE);

#ifndef APSSID
#define APSSID "IchMagZüge"
#define APPSK  "TschuTschu"
#endif

IPAddress local_IP(192,168,1,22);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;

float humidity=0;
float temperature=0;


ESP8266WebServer server(80); //Server on port 80

double absolute(double t, double r) 
{
      
  // Konstanten
  double mw = 18.016; // Molekulargewicht des Wasserdampfes (kg/kmol)
  double gk = 8314.3; // universelle Gaskonstante (J/(kmol*K))
  double t0 = 273.15; // Absolute Temperatur von 0 °C (Kelvin)
  double tk = t + t0; // Temperatur in Kelvin
   
  double a, b;
  if (t >= 0) {
    a = 7.5;
    b = 237.3;
  } else {
    a = 7.6;
    b = 240.7;
  }
   
  // Sättigungsdampfdruck in hPa
  double sdd = 6.1078 * pow(10, (a*t)/(b+t));
   
  // Dampfdruck in hPa
  double dd = sdd * (r/100);
   
  // Wasserdampfdichte bzw. absolute Feuchte in g/m3
  double af = pow(10,5) * mw/gk * dd/tk;
   
  // v-Parameter
  double v = log10(dd/6.1078);
   
  // Taupunkttemperatur (°C)
  double tt = (b*v) / (a-v);
  
  return af;  
}

String SendHTML()
{
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<meta http-equiv=\"refresh\" content=\"30; URL=http://192.168.1.22/\">";
  ptr +="<title>Sensor</title>\n";
   ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<center><br><h1>Sensor</h1>\n";
  ptr +="<p>Temperatur: " + String(temperature);
  ptr +="</p>";
  ptr +="<p>rel. Luftfeuchtigkeit: " + String(humidity);
  ptr +="</p>";
  ptr +="<p>abs. Luftfeuchtigkeit: " + String(absolute(temperature, humidity));
  ptr +="</p><br><br>";
  ptr +=" </center>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
   connected to this access point to see it.
*/
void handleRoot() {
  server.send(200, "text/html", SendHTML());
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
  display.display();
  delay(1000);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.clearDisplay();
  display.display();
  dht.setup(D0, DHTesp::DHT11); // Connect DHT sensor to D0
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");

  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", dht.getTemperature.c_str());
  });
  
}

void loop() {
  server.handleClient();
  delay(dht.getMinimumSamplingPeriod());

  humidity = dht.getHumidity();
  temperature = dht.getTemperature();

  if (dht.getStatusString()=="OK")
  {
    display.clearDisplay();
   // display.display();
    display.setCursor(0,0);
    display.println("-------------------");
    display.print("rel. F.:");
    display.print(humidity, 1);
    display.print("%");
    display.println("");
    display.println("-------------------");
    display.print("Temperatur: ");
    display.print(temperature, 1);
    display.print("C");
    display.println("");
    display.println("-------------------");
    display.print("abs. F.:");
    display.print(absolute(temperature,humidity), 1);
    display.println(" g/m3");
    display.println("-------------------");
    display.display();
  }
  else
  {
    Serial.println("reading error");
  }
  delay(3000);
  
}
