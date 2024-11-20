// Määritellään pinnit
const int trigPin = 2;
const int echoPin = 3;

// Määritellään muuttujat
long duration;
int distance;
int rainAmount;

void setup() {
  // Alustetaan sarjaportti viestintää varten
  Serial.begin(9600);

  // Määritellään pinnien toiminnot
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  // Lähetetään ultraäänipulssi
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Lasketaan vastausaika
  duration = pulseIn(echoPin, HIGH);

  // Muunnetaan vastausaika etäisyydeksi
  distance = duration * 0.034 / 2 * 10;

  // Lasketaan sademäärä kaavan mukaan
  rainAmount = (150 / 3) - (distance / 3);
  if (rainAmount < 0) rainAmount = 0;

  // Tulostetaan etäisyys ja sademäärä sarjaportille
  Serial.print("Etäisyys: ");
  Serial.print(distance);
  Serial.print(" mm, Sademäärä: ");
  Serial.print(rainAmount);
  Serial.println(" mm");

  // Viive ennen seuraavaa mittauskierrosta
  delay(1000); // 1000ms = 1s
}
