/*
 * Dijital Elektrik Sayacı
 * --------------------------------------------------------------
 * Donanım:
 *   - Arduino Uno
 *   - PZEM-004T v3.0 (Voltaj / Akım / Güç ölçüm sensörü)
 *   - 16x2 I2C LCD ekran (adres 0x27)
 *
 * Ölçülenler: Voltaj (V), Akım (A), Güç (W), Tüketim (kWh)
 * Hesaplanan: Tüketim Bedeli (TL)
 *
 * Bağlantı (PZEM-004T <-> Arduino Uno, SoftwareSerial):
 *   PZEM TX  -> Arduino D2 (RX)
 *   PZEM RX  -> Arduino D3 (TX)
 *   PZEM 5V  -> Arduino 5V
 *   PZEM GND -> Arduino GND
 *
 * Gerekli kütüphaneler (Library Manager'dan yükleyin):
 *   - "PZEM004Tv30" by Jakub Mandula
 *   - "LiquidCrystal I2C" by Frank de Brabander
 * --------------------------------------------------------------
 */

#include <Wire.h>              // I2C iletişimi için dahili kütüphane
#include <LiquidCrystal_I2C.h> // I2C LCD ekran kütüphanesi
#include <PZEM004Tv30.h>      // PZEM-004T v3.0 sensör kütüphanesi
#include <SoftwareSerial.h>   // Yazılımsal seri port (Uno için)

// ---------------- AYARLAR ----------------
// Elektrik birim fiyatı (TL / kWh) - kendi tarifenize göre değiştirin
const float BIRIM_FIYAT = 2.50;

// Ekran güncelleme sıklığı (ms)
const long GUNCELLEME_ARALIGI = 1000;
// -----------------------------------------

// LCD nesnesi (Adres: 0x27, 16 Sütun, 2 Satır)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// SoftwareSerial: RX = D2, TX = D3
//SoftwareSerial pzemSerial(8, 9);

// PZEM nesnesi (yazılımsal seri port üzerinden)
PZEM004Tv30 pzem(8,9);

// Zaman yönetimi (millis için)
unsigned long eskiZaman = 0;

// Hangi ekran sayfasının gösterileceğini takip eder (0 veya 1)
bool sayfa = false;

void setup() {
  // LCD başlat
  lcd.begin();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Elektrik Sayaci");
  lcd.setCursor(0, 1);
  lcd.print("Baslatiliyor...");
  delay(2000);
  lcd.clear();
}

void loop() {
  unsigned long suAnkiZaman = millis();

  if (suAnkiZaman - eskiZaman >= GUNCELLEME_ARALIGI) {
    eskiZaman = suAnkiZaman;

    // Sensörden değerleri oku
    float voltaj   = pzem.voltage();    // Volt (V)
    float akim     = pzem.current();    // Amper (A)
    float guc      = pzem.power();      // Watt (W)
    float enerji   = pzem.energy();     // Tüketim (kWh)

    // Okuma başarısız olduysa (NaN) hata göster
    if (isnan(voltaj) || isnan(akim) || isnan(guc) || isnan(enerji)) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("PZEM okunamadi!");
      lcd.setCursor(0, 1);
      lcd.print("Baglanti kontrol");
      return;
    }

    // Tüketim bedelini hesapla (TL)
    float bedel = enerji * BIRIM_FIYAT;

    // İki sayfa arasında geçiş yaparak tüm verileri gösteriyoruz
    if (sayfa) {
      gosterSayfa1(voltaj, akim);
    } else {
      gosterSayfa2(guc, bedel);
    }
    sayfa = !sayfa;
  }
}

// 1. Sayfa: Voltaj ve Akım
void gosterSayfa1(float voltaj, float akim) {
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Voltaj: ");
  lcd.print(voltaj, 1);
  lcd.print(" V");

  lcd.setCursor(0, 1);
  lcd.print("Akim:   ");
  lcd.print(akim, 2);
  lcd.print(" A");
}

// 2. Sayfa: Güç ve Tüketim Bedeli
void gosterSayfa2(float guc, float bedel) {
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Guc: ");
  lcd.print(guc, 1);
  lcd.print(" W");

  lcd.setCursor(0, 1);
  lcd.print("Bedel: ");
  lcd.print(bedel, 2);
  lcd.print(" TL");
}
