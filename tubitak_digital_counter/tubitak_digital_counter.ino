#include <PZEM004Tv30.h>
#include <LiquidCrystal_I2C.h>

#define buttonPin 13


PZEM004Tv30 pzem(8,9); // sensör objesi TX, RX pinleri
LiquidCrystal_I2C lcd(0x27,16,2); // lcd ekran objesi

//Tüketim bedeli kw ile tüketim süresinin saat cinsinden çarpımına eşitlendirir.
float kw;
unsigned long sure;
float tuketim;
float const tarife = 2.00;
int tipNo = 1;


String tip[] = {"Gerilim", "Akım", "Güç", "Tüketim"};

void setup() {
  lcd.begin(16, 2); // 16cols, 2rows
  Serial.begin(9600);
}

void loop() {
  ekrandaGoster(setShowTip(), );
  kw = pzem.voltage() * pzem.current() / 1000;
  tuketim = kw * sure * tarife;
  printValues();
}

void printValues(){
  Serial.println("***************");
  Serial.println("Voltage: " + String(pzem.voltage()));
  Serial.println("Current: " + String(pzem.current()));
  Serial.println("Power: " + String(pzem.power()));
  Serial.println("Energy: " + String(pzem.energy()));
  //delay(1000);
}


void ekrandaGoster(String tip, float miktar){
  int ilkSure = millis();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(tip);
  lcd.setCursor(0, 1);
  lcd.print(miktar);
}

int setShowTip(){
  if(digitalRead(buttonPin) && tipNo <= 3){
    tipNo = tipNo + 1;
    delay(150);
  } else if(digitalRead(buttonPin) && tipNo >= 4){
    tipNo = tipNo  % 3;
  }
  return tipNo;
}
/*
long sureHesapla(){
  if(pzem.voltage() != 0 || pzem.voltage() != "nan" || pzem.current() != 0){
    return millis();
  }
  else {
    return 0;
  }
}
*/