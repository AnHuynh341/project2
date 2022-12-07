#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "RTClib.h"
#include <EEPROM.h>

#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

#include "DHT.h"
#define DHTpin 2 //D2
#define DHTTYPE DHT11
DHT dht(DHTpin,DHTTYPE); 

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

int mode;
int plant;
int pump;
#define RL1 4

RTC_DS1307 RTC;
DateTime now;
int sec_last;

int lcdColumns = 16;
int lcdRows = 2;

 const int dry = 3499; // value for dry sensor
 const int wet = 1600; // value for wet sensor

 char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};


LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  // d21 scl,  d22 sda


void setup() {
   Serial.begin(9600);
     WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
   lcd.init();       
   lcd.backlight();
   lcd.clear();
   Wire.begin();
    dht.begin();
   RTC.begin();
   sec_last=EEPROM.read(0);
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
  pinMode(RL1, OUTPUT);
  

  



}

void loop() {
   timeDisplay();

    moisDisplay();

    TempHum();

    readMode();

    reconnect();

  delay(700);
}

void timeDisplay() {
     now = RTC.now();
       int sec = now.second();
  int mnt = now.minute();
  int hr = now.hour();
   lcd.setCursor(0, 0);
   lcd.print("     "); 
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

void moisDisplay() {
    int sensorVal = analogRead(34); //d34, mois sensor
    int percentageHumididy = map(sensorVal, dry, wet, 0, 99); 
    Firebase.RTDB.setInt(&fbdo, "LivingRoom/soilMos",percentageHumididy);
   Serial.print(percentageHumididy);
   Serial.println("%"); 
   lcd.setCursor(3, 1);
   lcd.print("  "); 
   lcd.setCursor(0, 1);
   lcd.print("M:");
   lcd.print(percentageHumididy);
   lcd.print("%");
}


void TempHum(){
    Firebase.RTDB.setFloat(&fbdo, "LivingRoom/Humidity",dht.readHumidity());
    Firebase.RTDB.setFloat(&fbdo, "LivingRoom/Temperature",dht.readTemperature());

    int hm=dht.readHumidity();
    Serial.print("Humidity ");
    Serial.println(hm);
     lcd.setCursor(11, 1);
    lcd.print("H:");
    lcd.setCursor(13, 1);
    lcd.print("  "); 
    lcd.setCursor(13, 1);
    lcd.print(hm);
    lcd.print("%");

    int temp=dht.readTemperature();
    Serial.print("Temperature ");
    Serial.println(temp);
    lcd.setCursor(5, 1);
    lcd.print("T:");
    lcd.setCursor(7, 1);
    lcd.print("  "); 
    lcd.setCursor(7, 1);
    lcd.print(temp);
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

void readMode() {
    if (Firebase.RTDB.getInt(&fbdo, "/LivingRoom/mode")) {
      if (fbdo.dataType() == "int") {
        mode = fbdo.intData();
        Serial.println(mode);
         if (mode==0) {
          lcd.setCursor(9, 0);
          lcd.print("       ");
          lcd.setCursor(9, 0);
          lcd.print("Manual");
              Firebase.RTDB.getInt(&fbdo, "/LivingRoom/Pump");
              if (fbdo.dataType() == "int") {
              pump = fbdo.intData();
                     if (pump == 0) {
                      digitalWrite(RL1, HIGH);
                     }
                     else {
                     digitalWrite(RL1, LOW);
                          }
              }
         }
         else if (mode==1) {
          lcd.setCursor(9, 0);
          lcd.print("       ");
          lcd.setCursor(9, 0);
          lcd.print("Auto");
          
          Firebase.RTDB.getInt(&fbdo, "/LivingRoom/Plant");
                if (fbdo.dataType() == "int") {
                 plant = fbdo.intData();
          lcd.setCursor(15, 0);
          lcd.print(" ");
          lcd.setCursor(15, 0);
          lcd.print(plant);
         }
      }
    }
      else {
       Serial.println(fbdo.errorReason());
       }

}
}