#include <math.h>   
#include <SoftwareSerial.h>
SoftwareSerial BTserial(9, 10);  // RX, TX
#include <LiquidCrystal.h>
#include <stdio.h>
#include <ctype.h>
#include "I2Cdev.h"
#include "MPU6050.h"
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif
MPU6050 accelgyro;
int16_t ax, ay, az;
int16_t gx, gy, gz;
int led = 6;
float initx;
float inity;
float Acc_angle[2];
float Gyro_angle[2];
float Total_angle[2];
int i;
float rad_to_deg = 180 / 3.141592654;
int b;
int count = 0;
int cur = 0;
int first = 0;
int lastfive[5] = {0,0,0,0,0};
int pos = 0;
int bend = 0;
int good = 0; 
double ans = 0;
char state;
int count2 = 0;
int count3 = 0;
int sum = 0;
LiquidCrystal lcd(12, 11, 5, 4, 3, 7);
void setup() {
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif
  pinMode(led, OUTPUT);
  // join I2C bus (I2Cdev library doesn't do this automatically)


  // initialize serial communication
  Serial.begin(38400);

  // initialize device
  Serial.println("Initializing I2C devices...");
  accelgyro.initialize();

  // verify connection
  Serial.println("Testing device connections...");
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
  analogWrite(led, 0);
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  BTserial.begin(9600);
  lcd.print("Good Posture!");
  time = millis();
  //time = millis();

}

void loop() {
    if(BTserial.available() > 0){
      state = BTserial.read();
      delay(50);
      if (state == '#'){
        ans = 100*(bend)/(bend+good);
        BTserial.print(int(ans));
        delay(50);
        //Serial.println(int(ans));
      }
    }
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    Acc_angle[0] = atan((ay / 16384.0) / sqrt(pow((ax / 16384.0), 2) + pow((az / 16384.0), 2))) * rad_to_deg; // X Axis Angle
    Acc_angle[1] = atan(-1 * (ax / 16384.0) / sqrt(pow((ay / 16384.0), 2) + pow((az / 16384.0), 2))) * rad_to_deg; // Y Axis Angle
    
    
    if(count3 == 0){
      Serial.println(int(Acc_angle[1]));
      count2++;
      count2 = count2 % 5;
      //Serial.println(count2);
    }
    if(count2 == 0){
      count2++;
      if(Acc_angle[1] > -60 && Acc_angle[1] < 0){
        bend++;
        lastfive[pos] = 1;
        pos++;
      }
      else{
        good++;
        lastfive[pos] = 0;
        pos++;
      }
      pos = pos % 5;
      sum = 0;
      for(int i = 0;i < 5; i++){
        sum = sum + lastfive[i];
      }
      //Serial.print("Sum:");
      //Serial.println(sum);
      if(sum > 2){
        analogWrite(led, 100);
        lcd.setCursor(0,0);
        lcd.print("Fix your posture");
        
      }
      else{
        analogWrite(led, 0);
        lcd.setCursor(0,0);
        lcd.clear();
        lcd.print("Good Posture!");
        
      }
    }
    
    count3++;
    count3 = count3 % 200;
}
