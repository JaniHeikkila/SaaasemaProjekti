#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Ticker.h>


#define TRIG_PIN 18
#define ECHO_PIN 19
#define VALVE_PIN 32
Adafruit_BME280 bme; // Temperature, Humidity and pressure sensor
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire); // Oled screen
unsigned long delayTime;


//Rain gauge
float alpha = 0.3;
float filteredDistance = 0;
unsigned long valveTimer = 0;
unsigned long lastMeasurementTime = 0; // Aika viimeisimmästä mittauksesta
unsigned long lastDrainMessageTime = 0; // Aika viimeisimmästä viestistä
bool isDraining = false;
const unsigned long drainDuration = 10000; // 10 seconds
const unsigned long measurementInterval = 500; // Mittausväli millisekunteina

float calibrationDistances[4] = {300, 200, 100, 40}; // mm
float calibrationVolumes[4] = {0, 10, 30, 40}; // ml


//Wind speed sensor
const int Sensor_Pin = 23;
int marker1 = 0;            // Wind speed            
int icounter = 0;           // Interrupt counter for rotation detection
int icounterold = 0;        // Old interrupt counter for rotation detektion
unsigned long counter1;     // Wind spped
float time1;                // Wind speed (time in [ms] for one rotation)
float radius = 0.0816;        // Radius between center and middle of half hemisphere position, meters
float lamda = 0.3;          // Lambda is a constant for amemometer type with 3 hemisphere, lamda = 0,3
float pi = 3.14159265359;   // Pi constant
float windspeed_hz;         
float windspeed_mps;       


Ticker Timer;
 
// Interrupt routine for wind speed
void interruptRoutine1() {
  if(marker1 == 0){
    time1 = float(counter1);
    counter1 = 0;
    marker1 = 1;
    icounter += 1;
  }
  else{
    marker1 = 0;
  }
}

// 1ms counter
void counter() {
  if(marker1 == 1){
    counter1 += 1;
   }
} 

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(VALVE_PIN, OUTPUT);
  digitalWrite(VALVE_PIN, LOW); // Ensure the valve is closed on startup

  pinMode(Sensor_Pin, INPUT_PULLUP);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x64)
   // init done
  display.display();
  delay(100);
  display.clearDisplay();
  display.display();
  display.setTextSize(1.2);
  display.setTextColor(WHITE);
  
  bool status;
  // default settings
  // (you can also pass in a Wire library object like &Wire2)
  status = bme.begin(0x76);  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  Serial.println("-- Default Test --");
  delayTime = 1000;

  Serial.println();

  Timer.attach_ms(1, counter);     // Start timer function "counter" each 1ms
  // Start interrupts
  attachInterrupt(Sensor_Pin, interruptRoutine1, FALLING);
}

//RAIN GAUGE
float calculateVolume(float distance) {
  if (distance <= calibrationDistances[3]) {
    return calibrationVolumes[3];
  } else if (distance >= calibrationDistances[0]) {
    return calibrationVolumes[0];
  }
  for (int i = 3; i > 0; i--) {
    if (distance >= calibrationDistances[i] && distance <= calibrationDistances[i-1]) {
      float volumeDifference = calibrationVolumes[i-1] - calibrationVolumes[i];
      float distanceDifference = calibrationDistances[i-1] - calibrationDistances[i];
      float proportion = (distance - calibrationDistances[i]) / distanceDifference;
      return calibrationVolumes[i] + proportion * volumeDifference;
    }
  }
  return 0;
}


void loop() {
  unsigned long currentTime = millis();
  display.setCursor(0,0);
  display.clearDisplay();
  

  //WIND SPEED
  if(time1 == 0.0){
    time1 = 0.0001;
  }

    // Wind speed n[Hz] = 1 / time1[ms] *1000
  windspeed_hz = 1.0 / time1 * 1000;
  // If rotation off then windspeed_hz = 0
  if(icounter == icounterold){
    windspeed_hz = 0.0;
  }
  icounterold = icounter;
  // Wind speed, v[m/s] = (2 * Pi * n[Hz] * r[m]) / lamda[1]
  windspeed_mps = (2 * pi * windspeed_hz * radius) / lamda;


  //RAIN GAUGE
  if (isDraining && currentTime - lastDrainMessageTime >= 1000) {
    int secondsLeft = (drainDuration - (currentTime - valveTimer)) / 1000;
    Serial.print("Drainage in progress... ");
    Serial.print(secondsLeft);
    Serial.println(" seconds remaining.");
    lastDrainMessageTime = currentTime;
  }

  if (currentTime - valveTimer >= drainDuration && isDraining) {
    digitalWrite(VALVE_PIN, LOW);
    Serial.println("Drainage complete.");
    isDraining = false;
  }

  if (currentTime - lastMeasurementTime >= measurementInterval) {
    lastMeasurementTime = currentTime; // Update the last measurement time

    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH);
    float distance_mm = (duration * 0.343) / 2;
    filteredDistance = alpha * distance_mm + (1 - alpha) * filteredDistance;

    float volume_ml = calculateVolume(filteredDistance);

    Serial.print("Distance: ");
    Serial.print(filteredDistance);
    Serial.print(" mm, Volume: ");
    Serial.println(volume_ml);
    

    if (volume_ml >= 40 && !isDraining) {
      digitalWrite(VALVE_PIN, HIGH);
      valveTimer = currentTime;
      isDraining = true;
      lastDrainMessageTime = currentTime - 1000; // Force immediate first messag
    }
  }
  display.print("T:"); display.print(bme.readTemperature()); display.print(" C, ");
  display.print("H:"); display.print(bme.readHumidity()); display.println(" %");
  display.print("P:"); display.print(bme.readPressure() / 100.0F); display.println(" hPa");
  display.print("W:"); display.print(windspeed_mps); display.println("m/s");
  display.print("R:"); display.print(volume_ml); display.println("ml");
    
}