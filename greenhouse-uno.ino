#include <NewPing.h>
#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

// Pin Ultrasonik A (Tangki Utama)
#define TRIG_A 7
#define ECHO_A 6

// Pin Ultrasonik B (Pembuangan)
#define TRIG_B 5
#define ECHO_B 4

// Pin Relay untuk Water Pump
#define RELAY_PIN 8

// Konfigurasi Ultrasonik
#define MAX_DISTANCE 200 // Maksimum jarak pengukuran (dalam cm)
NewPing sonarA(TRIG_A, ECHO_A, MAX_DISTANCE); // Sensor Ultrasonik Tangki Utama
NewPing sonarB(TRIG_B, ECHO_B, MAX_DISTANCE); // Sensor Ultrasonik Pembuangan

// LCD I2C
hd44780_I2Cexp lcd;

// Variabel level air
int waterLevelA; // Level air di Tangki Utama
int waterLevelB; // Level air di Pembuangan

// Batasan level
#define FULL_WATER_LEVEL 2       // Level penuh untuk Tangki Utama (dalam cm)
#define DRAINAGE_PUMP_LEVEL 10   // Level minimal air di Pembuangan agar pompa aktif (dalam cm)

void setup() {
  Serial.begin(9600);

  pinMode(RELAY_PIN, OUTPUT); // Atur pin relay sebagai output
  digitalWrite(RELAY_PIN, HIGH); // Matikan pompa (relay aktif LOW)

  // Inisialisasi LCD
  int status = lcd.begin(16, 2);
  if (status) {
    status = -status;
    Serial.print("LCD Error: ");
    Serial.println(status);
    while (1); // Berhenti jika ada error
  }
  lcd.print("Water Monitoring");
  delay(2000);
  lcd.clear();
}

void loop() {
  // Membaca jarak dari sensor ultrasonik
  waterLevelA = sonarA.ping_cm(); // Membaca level Tangki Utama
  waterLevelB = sonarB.ping_cm(); // Membaca level Pembuangan

  // Logika kontrol water pump berdasarkan prioritas
  if (waterLevelA <= FULL_WATER_LEVEL) { 
    // Prioritas 1: Jika Tangki Utama penuh, matikan pompa untuk mencegah overflow
    digitalWrite(RELAY_PIN, HIGH); // Matikan pompa
  } else if (waterLevelA > FULL_WATER_LEVEL && waterLevelB >= DRAINAGE_PUMP_LEVEL) { 
    // Prioritas 2: Hidupkan pompa jika Tangki belum penuh dan air pembuangan cukup banyak
    digitalWrite(RELAY_PIN, LOW); // Nyalakan pompa
  } else { 
    // Safety Net: Matikan pompa jika tidak ada kondisi yang terpenuhi
    digitalWrite(RELAY_PIN, HIGH); // Matikan pompa
  }

  // Tampilkan data ke LCD
  lcd.setCursor(0, 0);
  lcd.print("Tangki: ");
  lcd.print(waterLevelA);
  lcd.print(" cm    "); // Tambah spasi untuk menghapus data lama

  lcd.setCursor(0, 1);
  lcd.print("Pembuangan: ");
  lcd.print(waterLevelB);
  lcd.print(" cm    "); // Tambah spasi untuk menghapus data lama

  // Debugging
  Serial.print("Water Level A (Tangki): ");
  Serial.print(waterLevelA);
  Serial.println(" cm");

  Serial.print("Water Level B (Pembuangan): ");
  Serial.print(waterLevelB);
  Serial.println(" cm");

  delay(1000); // Delay 1 detik untuk pembacaan ulang
}