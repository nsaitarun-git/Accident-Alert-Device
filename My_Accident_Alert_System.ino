#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1331.h>
#include <SPI.h>

#define SCL 13
#define SDA 11
#define RES 9
#define DC 8
#define CS 10

SoftwareSerial Serial1(7,6);
SoftwareSerial ss(2,3);
TinyGPSPlus gps;
Adafruit_SSD1331 display=Adafruit_SSD1331(&SPI,CS,DC,RES);


#define BLACK          0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF


int xPin=A0;
int yPin=A1;
int zPin=A2;
int xrest;
int yrest;
int zrest;
int xread;
int yread;
int zread;
float x;
float y;
float z;
#define max 1.10     
#define min -1.10 

int switchPin=A3; 
int switchState;
int buzzer=5;
int led=12;

int i=20;
int flag=0;
int flag2=0;


int engine=A5;
int speed;

void setup() {
  Serial.begin(9600);
  display.begin();
  Serial1.begin(9600);
  pinMode(xPin,INPUT);
  pinMode(yPin,INPUT);
  pinMode(zPin,INPUT);
  pinMode(buzzer,OUTPUT);
  pinMode(switchPin,INPUT);
  pinMode(engine,OUTPUT);
  pinMode(led,OUTPUT);
  digitalWrite(switchPin,HIGH);
  digitalWrite(engine,LOW);
  
  startUp(); 
  
  ss.begin(9600);  
  Serial.println(F("Starting GPS"));
}


void loop() {

gpsData(); 
accReadings();
accCalibration();
digitalWrite(led,HIGH);

if(x<=min||x>=max||y<=min||y>=max||z<=min||z>=max){
  digitalWrite(engine,HIGH);
  clearScreen();
  display.setCursor(25,15);
  display.print(F("Collision"));
  display.setCursor(25,25);
  display.print(F("Detected!"));
  display.setCursor(20,40);
  display.print(F("Engine off"));
  tone(buzzer,2000,500);
  delay(3000);
  clearScreen();
  
  for(i=20;i>0;i--){
  switchState=digitalRead(switchPin);
  display.setCursor(3,5);
  display.print(F("Sending Message"));
  display.setCursor(5,15);
  display.print(F("in:"));
  display.setCursor(25,15);
  display.print(i);
  display.print(F(" secs "));
  display.setCursor(3,35);
  display.print(F("Press button if"));
  display.setCursor(3,45);
  display.print(F("you're okay"));
  digitalWrite(led,HIGH);
  tone(buzzer,2000);
  delay(50);
  noTone(buzzer);
  digitalWrite(led,LOW);
  delay(950);
  
  if(switchState==0){
    digitalWrite(engine,LOW);
    clearScreen2();
    display.setCursor(27,25);
    display.print(F("Message"));
    display.setCursor(20,35);
    display.print(F("Cancelled"));
    display.setCursor(20,50);
    display.print(F("Engine On"));
    delay(2000);
    clearScreen();
    flag=1;
    break;
  }
 }
 if(flag==0){
    sendMessage();
    }
    flag=0;
    i=20;
  }
 }





void startUp(){ 
  digitalWrite(led,HIGH);
  clearScreen();
  display.setTextColor(WHITE,BLACK);
  display.setTextSize(1);
  display.setCursor(25,25);
  display.print(F("Accident"));
  display.setCursor(15,35);
  display.print(F("Alert System"));
  display.setCursor(15,50);
  display.print(F("By Saitarun"));
  tone(buzzer,2000);
  delay(100);
  noTone(buzzer);
  delay(2000);
  clearScreen();
  display.setCursor(5,25);
  display.print(F("Initilizing"));
  display.setCursor(5,35);
  display.print(F("GSM Module..."));
  delay(1000);
  
  gsmHandShake();
  delay(200);
  gsmSimCheck();
  delay(200);
  gsmNetworkCheck();
  
  clearScreen2();
  display.setCursor(5,25);
  display.print(F("GSM Initilized!"));
  delay(2000);

  clearScreen();
  display.setCursor(5,25);
  display.print(F("Calibrating"));
  display.setCursor(5,35);
  display.print(F("ADXL335..."));
  xrest=analogRead(xPin);
  yrest=analogRead(yPin);
  zrest=analogRead(zPin);
  delay(2000);

  clearScreen2();
  display.setCursor(7,25);
  display.print(F("We're good to"));
  display.setCursor(42,35);
  display.print(F("go!"));
  delay(2000);
  clearScreen();

}





void gsmHandShake(){
  clearScreen();
  display.setCursor(5,25);
  display.print(F("Finding GSM..."));
  delay(1000);
  boolean handShake=1;
  while(handShake){
    Serial1.println("AT");
    while(Serial1.available()>0){
      if(Serial1.find("OK"))
      handShake=0;
    }
    delay(1000);
  }
  Serial.println("GSM Connected");
  clearScreen2();
  display.setCursor(8,25);
  display.print(F("GSM Connected!"));
  delay(1500);
}



void gsmSimCheck(){
  clearScreen();
  display.setCursor(5,25);
  display.print(F("Finding SIM"));
  display.setCursor(5,35);
  display.print(F("card..."));
  delay(1000);
  boolean sim=1;
  while(sim){
    Serial1.println("AT+CCID");
    while(Serial1.available()>0){
      if(Serial1.find("OK"))
      sim=0;
   }
  delay(1000);
 }
  Serial.println(F("SIM card found"));
  clearScreen2();
  display.setCursor(5,25);
  display.print(F("SIM card found!"));
  delay(1500);
}


void gsmNetworkCheck(){
  
  clearScreen();
  display.setCursor(3,25);
  display.print(F("Waiting for GSM"));
  display.setCursor(3,35);
  display.print(F("network REG..."));
  delay(1000);
  boolean network=1;
  while(network){
    Serial1.println("AT+CGREG?");
    while(Serial1.available()>0){
      if(Serial1.find("+CGREG: 0,1"))
      network=0;
   }
  delay(1000);
 }
  Serial.println(F("Registred to Network"));
  clearScreen2();
  display.setCursor(3,25);
  display.print(F("REGD to network"));
  delay(1500);
}




void accReadings(){
  
  xread=analogRead(xPin)-xrest;
  yread=analogRead(yPin)-yrest;
  zread=analogRead(zPin)-zrest;

  x=xread/67.584;
  y=yread/67.584;
  z=zread/67.584;
  
  } 

 
 
 void accCalibration(){
 switchState=digitalRead(switchPin);
 switch(switchState){
   case 0:
  clearScreen();
  display.setCursor(5,25);
  display.print(F("Re-calibrating"));
  display.setCursor(5,35);
  display.print(F("ADXL335..."));
  xrest=analogRead(xPin);
  yrest=analogRead(yPin);
  zrest=analogRead(zPin);
  delay(2000);
  clearScreen();
  break;
   }
 }

 
 
 
 void gpsData(){
   
   while(ss.available()){
    gps.encode(ss.read());
    }
  
  if(gps.location.isUpdated()){
  
  speed=gps.speed.mph();
  
  Serial.print(F("Latitude:"));
  Serial.println(gps.location.lat(),6);
  Serial.print(F("Longitude:"));
  Serial.println(gps.location.lng(),6);
  Serial.print(F("Speed:"));
  Serial.println(speed);

  display.setCursor(5,5);
  display.print(F("Speed:"));
  display.setCursor(40,5);
  display.print(speed);
  display.setCursor(55,5);
  display.print(F("mph "));

  display.setCursor(5,15);
  display.print(F("Lat:"));
  display.setCursor(30,15);
  display.print(gps.location.lat(),6);
  Serial.println(gps.location.lat(),6);
  display.setCursor(5,25);
  display.print(F("Long:"));
  display.setCursor(35,25);
  display.print(gps.location.lng(),6);

  }else if(!gps.location.isUpdated() && !gps.altitude.isUpdated()){
     clearScreen(); 
     while(flag2==0){
      while(ss.available()){
         gps.encode(ss.read());
        }
      display.setCursor(3,25);
      display.print(F("Waiting for GPS"));
      display.setCursor(35,35);
      display.print(F("data..."));
      if(gps.location.isUpdated()){
        clearScreen();
        break;
      }
    }
   }
 }






void sendMessage(){
  
Serial.println(F("Sending Message..."));

Serial1.print(F("AT+CMGF=1\r"));
delay(100);
Serial1.print(F("AT+CMGS=\"07459489629\"\r"));
Serial1.println(F("Alert from Accident Alert System"));
Serial1.println();
Serial1.println(F("Saitarun has met with an accident,he needs medical attention now!"));
Serial1.println();
Serial1.println(F("Car engine has been turned off."));
Serial1.println();
Serial1.println(F("Location and Speed:"));
Serial1.println();
Serial1.print(F("Latitude: "));
Serial1.print(gps.location.lat(),6);
Serial1.println();
Serial1.print(F("Longitude: "));
Serial1.println(gps.location.lng(),6);
Serial1.print(F("Speed: "));
Serial1.print(speed);
Serial1.println(F("  mph"));
Serial1.print(F("Google Maps Link: "));
Serial1.print(F("https://www.google.com/maps/search/?api=1&query="));
Serial1.print(gps.location.lat(),6);
Serial1.print(F(","));
Serial1.print(gps.location.lng(),6);
Serial1.print(F("\r"));
delay(200);
Serial1.print(char(26));
delay(100);
Serial1.println();
Serial.println(F("Message Sent"));

clearScreen2();
display.setCursor(12,25);
display.print(F("Message Sent"));
display.setCursor(5,35);
display.print(F("Help is coming!"));
delay(3000);
clearScreen();
}


 
 void clearScreen(){
  display.fillScreen(BLACK);
  display.drawRect(0,0,96,64,RED);
 }

 void clearScreen2(){
  display.fillScreen(BLACK);
  display.drawRect(0,0,96,64,GREEN);
 }
