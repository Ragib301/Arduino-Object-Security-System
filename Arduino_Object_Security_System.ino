/*  
 Arduino Object Security System:
 *  Developed by - Ragib Yasar Rahman
 *  Modfied on: 21 March, 2021
 *  Copyright: Ragib Yasar Rahman.
*/

#include <NewPing.h>
#include <Servo.h>
#include "Wire.h"
#include <MPU6050_light.h>
#include <LiquidCrystal_I2C.h>

//Configuring Serial Ports
#define BTSerial Serial2

#define GSMSerial Serial1
String SMS_message;
const int Baund = 9600;

//Servo Object
Servo myServo;
int pos;

//Ultrasonic Sensor
#define TRIGGER_PIN  A4  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     A5  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 400 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
int distance;

//LCD Screen Cofigure
LiquidCrystal_I2C lcd(0x27, 16, 2);

//IR Sensors
const int IRpin1 = 3;
const int IRpin2 = 4;
int state1, state2;

//Push Button Pin
#define buttonPin A0
int buttonState;

//MPU6050 Accelometer & Gyro Sensor
MPU6050 mpu6050(Wire);
int X_axis, Y_axis;

//Buzzer & LED pin
#define BuzzerLed 8
bool breakLoop = false, breakLoop2 = false;

void setup() {
  Serial.begin(Baund);
  BTSerial.begin(Baund);
  GSMSerial.begin(Baund);
  
  myServo.attach(9); // Defines on which pin is the servo motor attached
  myServo.write(90);

  // Initialize lcd screen
  lcd.init();
  // Turn on the backlight
  lcd.backlight();

  pinMode(IRpin1, INPUT);
  pinMode(IRpin2, INPUT);
  pinMode(buttonPin, INPUT);

  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets();

  pinMode(BuzzerLed, OUTPUT);

  lcd.setCursor(0, 0);
  lcd.print("/Arduino  Object");
  lcd.setCursor(0, 1);
  lcd.print("Security System/");
  delay(3500);
  lcd.clear();

  digitalWrite(BuzzerLed, HIGH);
  delay(350);
  digitalWrite(BuzzerLed, LOW);
  delay(300); 
}

void loop() {
  state1 = digitalRead(IRpin1);
  state2 = digitalRead(IRpin2);
  buttonState = digitalRead(buttonPin);

  lcd.setCursor(0, 0);
  lcd.print("|Please Place|");
  lcd.setCursor(0, 1);
  lcd.print("|Your Object!|");
  delay(100);

  if (buttonState == LOW) {
    if (state1 == LOW && state2 == LOW) {
        delay(300);
        while(true) {
          ObjectSecurity();
          Tilting_Device();
          Bluetooth_Control();
          RadarSystem();
        }
    }
    else{
      lcd.setCursor(0, 0);
      lcd.print("|Please Place|");
      lcd.setCursor(0, 1);
      lcd.print("|Your Object!|");
    }
  }
}

void ObjectSecurity() {
  state1 = digitalRead(IRpin1);
  state2 = digitalRead(IRpin2);

  if (state1 == HIGH && state2 == HIGH) {
    delay(200);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" Object is not");
    lcd.setCursor(0, 1);
    lcd.print("  available.");
    
    digitalWrite(BuzzerLed, HIGH);
    delay(1000);
    digitalWrite(BuzzerLed, LOW);
    delay(500);
    lcd.clear();
     
    for(int j=15+1; j>0; j--) {
      lcd.setCursor(0, 0);
      lcd.print("Return Object in:");
      lcd.setCursor(0, 1);
      lcd.print(j-1);
      lcd.print(" Seconds.");
      delay(1002);
    }

    state1 = digitalRead(IRpin1);
    state2 = digitalRead(IRpin2);

    if (state1 == LOW && state2 == LOW) {}
    else{
      breakLoop = true;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Object has been");
      lcd.setCursor(0, 1);
      lcd.print("stolen. ALERT!");
      SMS_message = "From Device: Your object has been stolen! Please take necessary action!";
      emergency_SMS();
      while(true) {
        digitalWrite(BuzzerLed, HIGH);
        delay(900);
        digitalWrite(BuzzerLed, LOW);
        delay(500);
      }
    }
  }
  else{
    lcd.setCursor(0, 0);
    lcd.print("Object Security");
    lcd.setCursor(0, 1);
    lcd.print(" is Activated!");
  }
  if (breakLoop2 == true) { exit(0); }
}

void emergency_SMS() {
  GSMSerial.println("AT"); //Once the handshake test is successful, it will back to OK
  delay(100);
  GSMSerial.println("AT+CMGF=1"); // Configuring TEXT mode
  delay(100);
  GSMSerial.println("AT+CMGS=\"+8801718613791\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
  delay(100);
  GSMSerial.print(SMS_message); //text content
  delay(100);
  GSMSerial.write(26);
  delay(100);
}

void Tilting_Device() {
  mpu6050.update();
  X_axis = mpu6050.getAngleX();
  Y_axis = mpu6050.getAngleY();
  const int val = 30;

  while (X_axis >= val || X_axis <= -val || Y_axis >= val || Y_axis <= -val) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("**  Device is **");
      lcd.setCursor(0, 1);
      lcd.print("tilting. ALERT!");
      SMS_message = "From Device: Anybody is Tilting the Device. Suspicious Activity Detected!";
      emergency_SMS();

      while(true) {
        digitalWrite(BuzzerLed, HIGH);
        delay(900);
        digitalWrite(BuzzerLed, LOW);
        delay(500);
      }
  }
  if (breakLoop == true) { exit(0); }
}

void Bluetooth_Control() {
  while (BTSerial.available()) {
    String command = BTSerial.readString();

    if (command.toInt() == 0)  {
      digitalWrite(BuzzerLed, HIGH);
      delay(350);
      digitalWrite(BuzzerLed, LOW);
      delay(300);
      myServo.write(90);
      breakLoop = true;
      breakLoop2 = true;
      state1 = NULL;
      state2 = NULL;
      delay(300);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Object Security");
      lcd.setCursor(0, 1);
      lcd.print("is Deactivated.");
      exit(0);
    }
    else if (command.toInt() == 1) {
      while(true) {
        ObjectSecurity();
        Tilting_Device();
        Bluetooth_Control();
        RadarSystem();
      }
    }
    else{
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Invalid Command!");
      lcd.setCursor(0, 1);
      lcd.print(" of Bluetooth.");
    }
  }
}

void RadarSystem() {
  // rotates the servo motor from 15 to 165 degrees
  for (pos=15; pos<=180; pos++) {  
    myServo.write(pos);

    distance = readPing();// Calls a function for calculating the distance measured by the Ultrasonic sensor for each degree
    Serial.print(pos); // Sends the current degree into the Serial Port
    Serial.print(","); // Sends addition character right next to the previous value needed later in the Processing IDE for indexing
    Serial.print(distance); // Sends the distance value into the Serial Port
    Serial.print("."); // Sends addition character right next to the previous value needed later in the Processing IDE for indexing
  }
  
  // Repeats the previous lines from 165 to 15 degrees
  for(pos=180; pos>=15; pos--) {  
    myServo.write(pos);

    distance = readPing();// Calls a function for calculating the distance measured by the Ultrasonic sensor for each degree
    Serial.print(pos); // Sends the current degree into the Serial Port
    Serial.print(","); // Sends addition character right next to the previous value needed later in the Processing IDE for indexing
    Serial.print(distance); // Sends the distance value into the Serial Port
    Serial.print("."); // Sends addition character right next to the previous value needed later in the Processing IDE for indexing
  }
  if (breakLoop == true) {exit(0);}
}

// Function for calculating the distance measured by the Ultrasonic sensor
int readPing() {
  delay(60);
  int cm = sonar.ping_cm();
  if (cm==0) { cm=255; }
  return cm;
}
