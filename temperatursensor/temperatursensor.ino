#include "DHTesp.h" // Click here to get the library: http://librarymanager/All#DHTesp
#include <ESP8266WebServer.h>
#include <WiFiClient.h>

#ifdef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP8266 ONLY!)
#error Select ESP8266 board.
#endif

#ifndef APSSID
#define APSSID "IchMagZüge"
#define APPSK  "TschuTschu"
#endif

IPAddress local_IP(192,168,1,23);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

DHTesp dht;
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

void handleRoot() {
  server.send(200, "text/html", SendHTML());
}

void setup()
{
  Serial.begin(115200);
  Serial.println();
  dht.setup(A0, DHTesp::DHT11); // Connect DHT sensor to D0
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);

  server.begin();
  Serial.println("HTTP server started");

  
}

void loop()
{
  server.handleClient();
  delay(dht.getMinimumSamplingPeriod());

  humidity = dht.getHumidity();
   temperature = dht.getTemperature();

  if (dht.getStatusString()=="OK")
  {
    Serial.print("rel. Feuchtigkeit: ");
    Serial.print(humidity, 1);
    Serial.print("%");
    Serial.print("\t\tTemperatur: ");
    Serial.print(temperature, 1);
    Serial.print("°C");
    Serial.print("\t\tabs. Feuchtigkeit: ");
    Serial.print(absolute(temperature,humidity), 1);
    Serial.println(" g/m3");
  }
  else
  {
    Serial.println("reading error");
  }
  delay(2000);
}
