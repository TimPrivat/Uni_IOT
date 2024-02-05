void setup() {
  // put your setup code here, to run once:
  pinMode(D4, OUTPUT);
}

void loop() {
boolean Hallo = true;

  while (Hallo) {
//Es blinkt
//SOS HILFE
//ALARM
    morse();

  }

}

void morse() {

  blink(300);
  blink(300);
  blink(300);

  blink(900);
  blink(900);
  blink(900);

  blink(300);
  blink(300);
  blink(300);

  delay(2000);
}

void blink(int time) {

  digitalWrite(D4, LOW);
  delay(time);
  digitalWrite(D4, HIGH);
  delay(time);




}
