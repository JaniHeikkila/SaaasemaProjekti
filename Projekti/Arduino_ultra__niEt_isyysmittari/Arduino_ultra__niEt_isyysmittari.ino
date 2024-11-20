// Määritellään HC-SR04 sensorin pinnit
const int trigPin = 9;
const int echoPin = 10;

// Määritellään muuttujat etäisyydelle ja ajalle
long duration;
int distance;

void setup() {
  // Alustetaan sarjaportti nopeudella 9600 bps
  Serial.begin(9600);

  // Asetetaan trigPin OUTPUT-tilaan ja echoPin INPUT-tilaan
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  // Lähetetään 10 mikrosekunnin pituinen pulssi trigPinille
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Luetaan echoPinin tila
  duration = pulseIn(echoPin, HIGH);

  // Lasketaan etäisyys senttimetreinä
  distance = duration * 0.034 * 10 / 2;

  // Tulostetaan etäisyys sarjaporttiin
  Serial.print("Etäisyys: ");
  Serial.print(distance, 1);
  Serial.println(" mm");

  // Pieni viive ennen seuraavaa mittauskierrosta
  delay(1000);
}
