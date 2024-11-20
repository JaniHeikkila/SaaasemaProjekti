#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define SCL_PIN 21
#define SDA_PIN 20

#define BME280_ADDRESS 0x76 // BME280:n osoite

Adafruit_BME280 bme; // Alusta BME280-objekti

void setup() {
  Serial.begin(9600);
  
  // Alusta I2C-väylä
  Wire.begin();

  // Alusta OLED-näyttö
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  // Alusta BME280-anturi oletusosoitteella
  if (!bme.begin(BME280_ADDRESS)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  // Puhdista näyttö
  display.clearDisplay();
}

void loop() {
    Serial.print("Lämpötila = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");

  Serial.print("Ilmanpaine = ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");

  Serial.print("Ilmankosteus = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");

  Serial.println();
  

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);

  // Lue anturin tiedot
  float temperature = bme.readTemperature();
  float pressure = bme.readPressure() / 100.0F; // Pa to hPa
  float humidity = bme.readHumidity();

  // Tulosta tiedot näytölle
  display.print("Lampotila: \n");
  display.print(temperature);
  display.println(" *C \n");
  display.print("Ilmanpaine: \n");
  display.print(pressure);
  display.println(" hPa \n");
  display.print("Ilmankosteus: \n");
  display.print(humidity);
  display.println(" % \n");

  // Päivitä näyttö
  display.display();
  
  delay(1000);
}
