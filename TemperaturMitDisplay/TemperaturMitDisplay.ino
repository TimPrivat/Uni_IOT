
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHTesp.h"

#ifdef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP8266 ONLY!)
#error Select ESP8266 board.
#endif

// OLED FeatherWing buttons map to different pins depending on board.
// The I2C (Wire) bus may also be different.

  #define BUTTON_A  0
  #define BUTTON_B 16
  #define BUTTON_C  2
  #define WIRE Wire

//double absolute(float,float);

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &WIRE);
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

void setup() {
 
  Serial.begin(9600);
  
  Serial.println("OLED FeatherWing test");
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
  dht.setup(D0, DHTesp::DHT11);
  
  Serial.println("OLED begun");

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(1000);

  // Clear the buffer.
  display.clearDisplay();
  display.display();

  Serial.println("IO test");


  // text display tests
  display.setTextSize(1);
 display.setTextColor(SSD1306_WHITE);
 
  display.clearDisplay();

 //display.print("TEST2");
  display.display();
  //delay(1000);
}
  
//  display.display(); // actually display all of the above
//}

void loop() {
  display.setCursor(0,0);
  display.clearDisplay();
  float relhumidity = dht.getHumidity();
  float temperature = dht.getTemperature();
  float abshumidity = absolute(temperature,relhumidity);
  
  display.print("Temperatur: ");
  display.print(temperature);
  display.println(" C");
  display.print("Rel. Luft.: ");
  display.print(relhumidity);
  display.println(" %");
  display.print("Abs. Luft.: ");
  display.print(abshumidity);
  display.println(" g/m3");

  display.display();
  
  delay(3000);
  display.clearDisplay();
}
