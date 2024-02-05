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

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

#ifndef APSSID
#define APSSID "Matze"
#define APPSK  "PP1234PP"
#endif

IPAddress local_IP(192,168,1,22);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;

float humidity=0;
float abs_humidity=0;
float temperature=0;
float abs_humidity_outside=0;

ESP8266WebServer server(80); //Server on port 80
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
DHTesp dht;

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
  ptr +="<p>Temperatur: " + String(temperature) + "C";
  ptr +="</p>";
  ptr +="<p>rel. Luftfeuchtigkeit: " + String(humidity) + "%";
  ptr +="</p>";
  ptr +="<p>abs. Luftfeuchtigkeit: " + String(abs_humidity) + " g/m3";
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

void fetchABS(){

 if (server.hasArg("abs_value")) { // this is the variable sent from the client
    String readingStr = server.arg("abs_value");
    
    abs_humidity_outside = readingStr.toFloat();
    Serial.print("Abs Außehn:");
    Serial.println(abs_humidity_outside);
    server.send(200, "text/html", "Data received");
  }
  
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.on("/data/", HTTP_GET, fetchABS);
  server.begin();
  Serial.println("HTTP server started");

  dht.setup(D0, DHTesp::DHT11);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 8);
  display.clearDisplay();

  pinMode(D3,OUTPUT);
 // digitalWrite(D3,HIGH);
  
}
int minu=0;
int h=0;
long curr = millis();
int sek = curr/1000;

bool firstsec = true;
bool firstmin = true;

void loop() {
 humidity = dht.getHumidity();
 temperature = dht.getTemperature();
 abs_humidity = absolute(temperature,humidity);
 
 display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  
  display.clearDisplay();

  display.setCursor(0, 8);
  display.print("relativ: ");
  display.print(humidity, 1);
  display.println("%");

  display.setCursor(0, 16);
  display.print("Temperatur: ");
  display.print(temperature, 1);
  display.println("C");

  display.setCursor(0, 24);
  display.print("innen abs: ");
  display.print(absolute(temperature,humidity), 1);
  display.println(" g/m3");

  display.setCursor(0, 32);
  display.print("aussen abs: ");
  display.print(abs_humidity_outside, 1);
  display.println(" g/m3");

  if(sek%60==0 && !firstsec){
        sek=sek%60;
         minu++;
         firstmin =false;
      if(minu%60 == 0 && !firstmin){
          minu = minu%60;
          h++;
          firstmin=true;
     
    }
  }
      
    firstsec=false;
  
  
   
    
  
  

  display.setCursor(0, 40);
  display.print("Laueft seit:");

  display.print(h, 1);
  display.print("h ");
  display.print(minu, 1);
  
  display.print("m ");
  display.print(sek, 1);
  display.print("s");
  
  display.display();
  sek++;
 handleRoot(); 
 server.handleClient();
 delay(1000);



 if(abs_humidity_outside<absolute(temperature,humidity)){
  digitalWrite(D3,HIGH);
    
 }else{

  digitalWrite(D3,LOW);
 }
}
