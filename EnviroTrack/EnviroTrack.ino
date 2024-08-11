#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include "RTClib.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10
#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C

const int chipSelect = 10; // CS pin dari shield SD terhubung ke pin 10.
RTC_DS1307 RTC;
File dataFile;
DateTime now;

#include <TinyGPS++.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);
double latitude = 0.0, longitude = 0.0;

TinyGPSPlus gps;

void setup() {
  Serial.begin(9600);
  lcd1();
  
  while (!Serial);
  Serial.println(F("BME280 test"));
  if (!bme.begin(0x76)) {
    Serial.println(F("Could not find a valid BME280 sensor, check wiring, address, sensor ID!"));
    while (1) delay(10);
  }

  Wire.begin();
  RTC.begin();
  if (!RTC.isrunning()) {
    Serial.println(F("RTC is NOT running!"));
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }

  Serial.print(F("Initializing SD card..."));
  if (!SD.begin(chipSelect)) {
    Serial.println(F("Card failed, or not present"));
    return;
  }
  Serial.println(F("Card initialized."));
  now = RTC.now();
  dataFile = SD.open("datalog.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.print(F("Start logging on: "));
    dataFile.print(now.year(), DEC);
    dataFile.print('/');
    dataFile.print(now.month(), DEC);
    dataFile.print('/');
    dataFile.print(now.day(), DEC);
    dataFile.println();
    dataFile.println(F("data               Time"));
    dataFile.close();
  } else {
    Serial.println(F("error opening datalog.txt"));
  }
  Serial.println();
}

void loop() {
  printValues();
  Gps1();
  delay(1000);
  if (latitude !=0.0 && longitude !=0.0) { 
    kartu();
    delay(1000);
  } 
}

void printValues() {
  float temperature = bme.readTemperature();
  float pressure = bme.readPressure();
  float altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  float humidity = bme.readHumidity();

  Serial.print(F("Temperature = "));
  Serial.print(temperature);
  Serial.println(F(" °C"));
  lcd.setCursor(0,0);
  lcd.print("T:");
  lcd.print(temperature);
  lcd.print("C");

  Serial.print(F("Pressure = "));
  Serial.print(pressure);
  Serial.println(F(" Pa"));
  lcd.setCursor(0,1);
  lcd.print("P:");
  lcd.print(pressure,0);
  lcd.print("Pa");

  Serial.print(F("Approx. Altitude = "));
  Serial.print(altitude);
  Serial.println(F(" m"));
   lcd.setCursor(11,1);
  lcd.print("h:");
  lcd.print(altitude,0);
  lcd.setCursor(17,1);
  lcd.print("m");

  Serial.print(F("Humidity = "));
  Serial.print(humidity);
  Serial.println(F(" %"));
  lcd.setCursor(11,0);
  lcd.print("RH:");
  lcd.print(humidity);
  lcd.print("%");


  
}

void kartu() {
  now = RTC.now();
  dataFile = SD.open("datalog.txt", FILE_WRITE);

  if (dataFile) {
    dataFile.print(latitude, 6);
    dataFile.print(",");
    dataFile.println(longitude, 6);

    dataFile.print(F("Suhu: "));
    dataFile.print(bme.readTemperature());
    dataFile.print(F(" °C               "));
    dataFile.print(now.hour(), DEC);
    dataFile.print(':');
    dataFile.print(now.minute(), DEC);
    dataFile.print(':');
    dataFile.println(now.second(), DEC);

    dataFile.print(F("Tekanan udara: "));
    dataFile.print(bme.readPressure());
    dataFile.print(F(" Pa               "));
    dataFile.print(now.hour(), DEC);
    dataFile.print(':');
    dataFile.print(now.minute(), DEC);
    dataFile.print(':');
    dataFile.println(now.second(), DEC);

    dataFile.print(F("Tinggi Permukaan: "));
    dataFile.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    dataFile.print(F(" m               "));
    dataFile.print(now.hour(), DEC);
    dataFile.print(':');
    dataFile.print(now.minute(), DEC);
    dataFile.print(':');
    dataFile.println(now.second(), DEC);

    dataFile.print(F("Kelembaban udara: "));
    dataFile.print(bme.readHumidity());
    dataFile.print(F(" %               "));
    dataFile.print(now.hour(), DEC);
    dataFile.print(':');
    dataFile.print(now.minute(), DEC);
    dataFile.print(':');
    dataFile.println(now.second(), DEC);

    dataFile.close();
    Serial.println(F("Data stored"));
  } else {
    Serial.println(F("error opening datalog.txt"));
  }
}

void lcd1() { 
  lcd.begin();
  lcd.setCursor(0,1);
  lcd.print("     GPS U-Blox   ");
  lcd.setCursor(0,2);
  lcd.print("  Teknik Pertanian  ");
  delay(2000);
  lcd.clear();
}

void Gps1 (){ 
  while(Serial.available() > 0){
    if(gps.encode(Serial.read())){
      if(gps.location.isValid()){
        latitude = gps.location.lat();
        longitude = gps.location.lng();
        //link = "www.google.com/maps/place/" + String(latitude, 6) + "," + String(longitude, 6) ;
      }
      //Serial.println(link);
      lcd.setCursor(0,2);
      lcd.print("Ltd: "+String(latitude, 6)+"   ");
      Serial.print(String(latitude, 6));
      Serial.print(",");
      lcd.setCursor(0,3);
      lcd.print("Lng: "+String(longitude, 6));
      Serial.println(String(longitude, 6)+"   ");
      //Serial.print(link);
      
    }
  }
}
