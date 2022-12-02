#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "RTClib.h"
#include <EEPROM.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

const int oneWireBus = 2;    //d2, dht11 

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "Hanh Tram"
#define WIFI_PASSWORD "12356789"

#define USER_EMAIL "c0nvit@duck.com"
#define USER_PASSWORD "12356789"
#define API_KEY "AIzaSyC87A38QuE8E06DE9ceJc95G724zz0Ledw"
#define DATABASE_URL "https://anhuynhsmarthome-default-rtdb.asia-southeast1.firebasedatabase.app/" 

FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int intValue;
float floatValue;
bool signupOK = false;

int test;

OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

RTC_DS1307 RTC;
DateTime now;
int sec_last;

int lcdColumns = 16;
int lcdRows = 2;

 const int dry = 3450; // value for dry sensor
 const int wet = 1699; // value for wet sensor

 char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};


LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  // d21 scl,  d22 sda


void setup() {
   Serial.begin(9600);
     WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  config.api_key = API_KEY;
    auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  }
  else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
   Wire.begin();
   sensors.begin(); 
   RTC.begin();
   sec_last=EEPROM.read(0);
  
   lcd.init();       
   lcd.backlight();
   lcd.clear();


}

void loop() {
    timeDisplay();
    mosDisplay();
    Temp();
    readData();
    reconnect();

  delay(500);
  
}

void timeDisplay() {
     now = RTC.now();
       int sec = now.second();
  int mnt = now.minute();
  int hr = now.hour();
   lcd.setCursor(0, 0);
   lcd.print("          "); 
  
    lcd.setCursor(0, 0);
    if(now.hour()<=9)
    {
      lcd.print("0");
      lcd.print(now.hour());
    }
    else {
     lcd.print(now.hour()); 
    }
    lcd.print(':');
    if(now.minute()<=9)
    {
      lcd.print("0");
      lcd.print(now.minute());
    }
    else {
     lcd.print(now.minute()); 
    }
    lcd.print(':');
    if(now.second()<=9)
    {
      lcd.print("0");
      lcd.print(now.second());
    }
    else {
     lcd.print(now.second()); 
    }

  EEPROM.write(0,sec);

}

void mosDisplay() {
    int sensorVal = analogRead(34); //d34, mois sensor
    int percentageHumididy = map(sensorVal, dry, wet, 0, 100); 
   Serial.print(percentageHumididy);
   Serial.println("%"); 
   lcd.setCursor(4, 1);
   lcd.print("    "); 
   lcd.setCursor(0, 1);
   lcd.print("Mos:");
   lcd.print(percentageHumididy);
   lcd.print("%");
}

void reset_time(){
    RTC.adjust(DateTime(0,0,0,0,0,0));
}

void Temp(){
   sensors.requestTemperatures(); 
  int temperatureC = sensors.getTempCByIndex(0);
    Serial.print(temperatureC);
  Serial.println("ºC");
     lcd.setCursor(13, 1);
   lcd.print("    "); 
   lcd.setCursor(8, 1);
   lcd.print("Temp:");
   lcd.print(temperatureC);
   lcd.print("c");
}

void reconnect()
{
 if (Firebase.isTokenExpired())
  {
    Firebase.refreshToken(&config);
    Serial.println("Token Refreshed");
  }
}

void readData() {
    if (Firebase.RTDB.getInt(&fbdo, "/LivingRoom/test")) {
      if (fbdo.dataType() == "int") {
        test = fbdo.intData();
        Serial.println(test);
         lcd.setCursor(16, 0);
         lcd.print(" "); 
         lcd.setCursor(10, 0);
         lcd.print("Mode:");
         lcd.print(test);
      }
    }
      else {
       Serial.println(fbdo.errorReason());
       }

}
