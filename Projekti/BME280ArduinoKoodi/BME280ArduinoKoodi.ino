#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define VIN_PIN 5   // Määritä VIN-pinni
#define GND_PIN 6   // Määritä GND-pinni
#define SCL_PIN 21  // Määritä SCL-pinni
#define SDA_PIN 20  // Määritä SDA-pinni

#define BME280_ADDRESS 0x76 // Oletusosoite BME280:lle

Adafruit_BME280 bme; // Alusta BME280-objekti

// Määritä merenpinnan ilmanpaine hPa:ssa
//const float SEALEVELPRESSURE_HPA = 1013.25;

void setup() {
  Serial.begin(9600);

    // Määritä VIN ja GND -pinnit
  pinMode(VIN_PIN, OUTPUT);
  digitalWrite(VIN_PIN, HIGH); // Syötä jännite VIN-pinille
  pinMode(GND_PIN, OUTPUT);
  digitalWrite(GND_PIN, LOW); // Liitä GND-pin maaan

  // Alusta I2C-väylä
  Wire.begin();

  // Alusta BME280-anturi oletusosoitteella
  if (!bme.begin(BME280_ADDRESS)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
}

void loop() {
  Serial.print("Lämpötila = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");

  Serial.print("Ilmanpaine = ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");

  //Serial.print("Approx. Altitude = ");
  //Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  //Serial.println(" m");

  Serial.print("Ilmankosteus = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");

  Serial.println();

  delay(1000);
}
