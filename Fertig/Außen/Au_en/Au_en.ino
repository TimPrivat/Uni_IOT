#include <ESP8266WiFi.h>
#include <DHTesp.h>
#define APSSID "Matze"
#define APPSK  "PP1234PP"

const char *ssid = APSSID;
const char *password = APPSK;
//
float humidity=0;
float abs_humidity=0;
float temperature=0;
DHTesp dht;

void setup() {
  delay(1000);
  Serial.begin(115200);
 
  Serial.println("Sensor Setup");
  dht.setup(2, DHTesp::DHT11);
  // Explicitly set the ESP8266 to be a WiFi-client
  Serial.println("Wifi Setup");
 WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Wifi begin");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Waiting for wifi");
  }

   Serial.println("Setup fertig");
}

void loop() {
  // put your main code here, to run repeatedly:
   WiFiClient client;
  const char * host = "192.168.1.22";
  const int httpPort = 80;
Serial.println("connecting to Server...");
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }else{

     Serial.println("connection succeeded");
    
  }
   Serial.println("Getting Data...");

 humidity = dht.getHumidity();
 temperature = dht.getTemperature();
 abs_humidity = absolute(temperature,humidity);

   Serial.print("humidity:");
   Serial.println(String(humidity));
   Serial.print("temperature:");
   Serial.println(temperature);
   Serial.print("absolute humidity:");
   Serial.println(abs_humidity);
    
  String url = "/data/";
  url += "?abs_value=";
  url += abs_humidity;
  

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

}



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
