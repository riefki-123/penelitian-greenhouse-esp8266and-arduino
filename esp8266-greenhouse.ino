#define BLYNK_TEMPLATE_ID "xxx"
#define BLYNK_TEMPLATE_NAME "xxx"

// Include the library files
#include <Wire.h>
#include <hd44780.h>           // Core hd44780 library
#include <hd44780ioClass/hd44780_I2Cexp.h> // I2C expander class
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>               // Library untuk sensor DHT

#define BLYNK_PRINT Serial

// Initialize the LCD display
hd44780_I2Cexp lcd; // Use hd44780_I2Cexp object

// Blynk credentials
char auth[] = "xxx";   // Enter your Auth token
char ssid[] = "xxx";   // Enter your WIFI name
char pass[] = "xxx";   // Enter your WIFI password

BlynkTimer timer;
bool Relay = 0;

// Define component pins
#define soilSensor A0
#define waterPump D3
#define DHTPIN D4        // Pin untuk DHT sensor
#define DHTTYPE DHT11    // Tipe DHT sensor: DHT11 atau DHT22

DHT dht(DHTPIN, DHTTYPE);

// Function to print on LCD with clearing previous data
void printLCD(int col, int row, const String& text) {
  lcd.setCursor(col, row);
  lcd.print(text);
  for (int i = text.length(); i < 20; i++) { // Clear remaining characters
    lcd.print(" ");
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(waterPump, OUTPUT);
  digitalWrite(waterPump, HIGH);

  // Initialize LCD
  int status = lcd.begin(20, 4); // 20 columns, 4 rows
  if (status) { // Check for initialization errors
    Serial.print("LCD initialization failed, status code: ");
    Serial.println(status);
    while (1); // Stop if LCD initialization failed
  }
  lcd.setBacklight(1); // Turn on the backlight

  // Display static text on LCD
  printLCD(0, 3, "Greenhouse"); // Display "Greenhouse" on row 3

  // Connect to Blynk
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  dht.begin();  // Initialize the DHT sensor

  // Call the functions at intervals
  timer.setInterval(100L, soilMoistureSensor);
  timer.setInterval(2000L, readDHTSensor);  // Read DHT sensor every 2 seconds
}

// Get the button value
BLYNK_WRITE(V1) {
  Relay = param.asInt();

  if (Relay == 1) {
    digitalWrite(waterPump, LOW); // Turn ON water pump
    Serial.println("Water Pump: ON");
  } else {
    digitalWrite(waterPump, HIGH); // Turn OFF water pump
    Serial.println("Water Pump: OFF");
  }
}

// Get the soil moisture values
void soilMoistureSensor() {
  int value = analogRead(soilSensor);
  value = map(value, 0, 1024, 0, 100);
  value = (value - 100) * -1;

  Blynk.virtualWrite(V0, value);
  printLCD(0, 0, "Soil Moisture: " + String(value) + "%");
}

// Read temperature and humidity from DHT sensor
void readDHTSensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();  // Celsius

  // Check if reading is valid
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    printLCD(0, 1, "Temp/Humid Error!");
    return;
  }

  // Send data to Blynk
  Blynk.virtualWrite(V2, t);  // Temperature on Virtual Pin V2
  Blynk.virtualWrite(V3, h);  // Humidity on Virtual Pin V3

  // Display data on LCD
  printLCD(0, 1, "Temp: " + String(t) + " C");
  printLCD(0, 2, "Humidity: " + String(h) + "%");
}

void loop() {
  Blynk.run();   // Run the Blynk library
  timer.run();   // Run the Blynk timer
}
