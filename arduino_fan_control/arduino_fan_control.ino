#include <EEPROM.h>
#define EE_VERSION 1

int tempPin0 = A1;   // the output pin of LM35
int tempPin1 = A2;   // the output pin of LM35
int tempPin2 = A3;   // the output pin of LM35
int fan0 = 11;       // the pin where fan is
int fan1 = 10;       // the pin where fan is
int fan2 = 9;       // the pin where fan is
int led = LED_BUILTIN;        // led pin
boolean ledOn = true;
int warnLed = 8;        // led pin
int tempMin = 30;   // the temperature to start the fan
int tempMax = 70;   // the maximum temperature when fan is at 100%

boolean active0 = true;
int temp0;
int fanSpeed0;

boolean active1 = true;
int temp1;
int fanSpeed1;

boolean active2 = true;
int temp2;
int fanSpeed2;

int fanAdd = 0;
int fanSleep = 0;
int delayTime = 500;
double tempMul = 0.48828125;
float tempAdd = 0;
String inputString = "";
boolean stringComplete = false;
boolean monitorOn = false;
boolean warn = false;
int testWarn = 20;
long alerts = 0;
boolean serialEcho = true;
String prompt = "fancontrol";

void load() {
  int a = 0;
  int xversion = 0;
  EEPROM.get(a, xversion);
  if (xversion != EE_VERSION) {
    Serial.print("*** Can't load from EEPROM, wrong version ");
    Serial.print(xversion);
    Serial.print(" expected version ");
    Serial.println(EE_VERSION);
    return;
  }
  a=a+sizeof(xversion);
  EEPROM.get(a,active0);
  a=a+sizeof(active0);
  EEPROM.get(a,active1);
  a=a+sizeof(active1);
  EEPROM.get(a,active2);
  a=a+sizeof(active2);
  EEPROM.get(a,fanAdd);
  a=a+sizeof(fanAdd);
  EEPROM.get(a,fanSleep);
  a=a+sizeof(fanSleep);
  EEPROM.get(a,tempMin);
  a=a+sizeof(tempMin);
  EEPROM.get(a,tempMax);
  a=a+sizeof(tempMax);
  EEPROM.get(a,tempMul);
  a=a+sizeof(tempMul);
  EEPROM.get(a,tempAdd);
  a=a+sizeof(tempAdd);
  EEPROM.get(a,delayTime);
  a=a+sizeof(delayTime);
  EEPROM.get(a,serialEcho);
  a=a+sizeof(serialEcho);
  EEPROM.get(a,monitorOn);
  a=a+sizeof(monitorOn);
  EEPROM.get(a,prompt);
  a=a+sizeof(monitorOn);
}

void configClean() {
  int a = 0;
  int xversion = 0;
  EEPROM.put(a, xversion);
}

void save() {
  int a = 0;
  int xversion = EE_VERSION;
  EEPROM.put(a, xversion);
  a=a+sizeof(xversion);
  EEPROM.put(a,active0);
  a=a+sizeof(active0);
  EEPROM.put(a,active1);
  a=a+sizeof(active1);
  EEPROM.put(a,active2);
  a=a+sizeof(active2);
  EEPROM.put(a,fanAdd);
  a=a+sizeof(fanAdd);
  EEPROM.put(a,fanSleep);
  a=a+sizeof(fanSleep);
  EEPROM.put(a,tempMin);
  a=a+sizeof(tempMin);
  EEPROM.put(a,tempMax);
  a=a+sizeof(tempMax);
  EEPROM.put(a,tempMul);
  a=a+sizeof(tempMul);
  EEPROM.put(a,tempAdd);
  a=a+sizeof(tempAdd);
  EEPROM.put(a,delayTime);
  a=a+sizeof(delayTime);
  EEPROM.put(a,serialEcho);
  a=a+sizeof(serialEcho);
  EEPROM.put(a,monitorOn);
  a=a+sizeof(monitorOn);
  EEPROM.put(a,prompt);
  a=a+sizeof(prompt);
  
}

void setup() {
  pinMode(fan0, OUTPUT);
  pinMode(fan1, OUTPUT);
  pinMode(fan2, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(warnLed, OUTPUT);
  pinMode(tempPin0, INPUT);
  pinMode(tempPin1, INPUT);
  pinMode(tempPin2, INPUT);

  digitalWrite(fan0, LOW);
  digitalWrite(fan1, LOW);
  digitalWrite(fan2, LOW);
  
  Serial.begin(9600);
  inputString.reserve(200);

  load();

  printPrompt();

}

void(* resetFunc) (void) = 0; //declare reset function @ address 0

void loop() {

   temp0=0;
   fanSpeed0=0;
   if (active0) {
     temp0 = readTemp(tempPin0);     // get the temperature
     if(temp0  < tempMin) { // if temp is lower than minimum temp 
        fanSpeed0 = fanSleep; // fan is not spinning 
     } else
     if((temp0  >= tempMin) && (temp0 <= tempMax)) { // if temperature is higher than minimum temp 
        fanSpeed0 = map(temp0, tempMin, tempMax, 32, 255) + fanAdd; // the actual speed of fan 
     } 
     if (fanSpeed0 > 255) fanSpeed0 = 255;
     if (fanSpeed0 < 0) fanSpeed0 = 0;
   }

   temp1=0;
   fanSpeed1=0;
   if (active1) {
     temp1 = readTemp(tempPin1);     // get the temperature
     if(temp1  < tempMin) { // if temp is lower than minimum temp 
        fanSpeed1 = fanSleep; // fan is not spinning 
     } else
     if((temp1  >= tempMin) && (temp1 <= tempMax)) { // if temperature is higher than minimum temp 
        fanSpeed1 = map(temp1, tempMin, tempMax, 32, 255) + fanAdd; // the actual speed of fan 
     } 
     if (fanSpeed1 > 255) fanSpeed1 = 255;
     if (fanSpeed1 < 0) fanSpeed1 = 0;
   }

   temp2=0;
   fanSpeed2=0;
   if (active2) {
     temp2 = readTemp(tempPin2);     // get the temperature
     if(temp2  < tempMin) { // if temp is lower than minimum temp 
        fanSpeed2 = fanSleep; // fan is not spinning 
     } else
     if((temp2  >= tempMin) && (temp2 <= tempMax)) { // if temperature is higher than minimum temp 
        fanSpeed2 = map(temp2, tempMin, tempMax, 32, 255) + fanAdd; // the actual speed of fan 
     } 
     if (fanSpeed2 > 255) fanSpeed2 = 255;
     if (fanSpeed2 < 0) fanSpeed2 = 0;
   }

   warn = false;
   if(testWarn > 0 || temp0  > tempMax || temp1 > tempMax || temp2 > tempMax) {        // if temp is higher than tempMax
    if (testWarn == 0) alerts++;
     warn = true;
     if (active0) fanSpeed0=255;
     if (active1) fanSpeed1=255;
     if (active2) fanSpeed2=255;
     testWarn--;
     if (testWarn < 0) testWarn = 0;
     if (alerts > 100000) alerts = 100000;
   }
   
   if (fanSpeed0 == 0) {
      digitalWrite(fan0, LOW);
   } else {
      analogWrite(fan0, fanSpeed0); // spin the fan at the fanSpeed speed 
   }
   if (fanSpeed1 == 0) {
      digitalWrite(fan1, LOW);
   } else {
      analogWrite(fan1, fanSpeed1); // spin the fan at the fanSpeed speed 
   }
   if (fanSpeed2 == 0) {
      digitalWrite(fan2, LOW);
   } else {
      analogWrite(fan2, fanSpeed2); // spin the fan at the fanSpeed speed 
   }

   if (stringComplete) {
     int pos = inputString.indexOf('=');
     if (pos < 0) {
       // command
       if (inputString == "") {
         // ignore empty lines
       } else
       if (inputString == "sensor") {
         Serial.print("tempMul:");
         Serial.print(tempMul);
         Serial.print(",tempAdd:");
         Serial.println(tempAdd);
       } else
       if (inputString == "boundaries") {
        Serial.print("tempMin:");
        Serial.print(tempMin);
        Serial.print(",tempMax:");
        Serial.println(tempMax);
       } else
       if (inputString == "fan") {
        Serial.print("fanAdd:");
        Serial.print(fanAdd);
        Serial.print(",fanSleep:");
        Serial.println(fanSleep);
       } else
       if (inputString == "delay") {
        Serial.print("delay:");
        Serial.println(delayTime);
       } else
       if (inputString == "alerts.clean") {
        alerts = 0;
        Serial.println("ok");
       } else
       if (inputString == "config.save") {
        save();
        Serial.println("ok");
       } else
       if (inputString == "config.load") {
        load();
        Serial.println("ok");
       } else
       if (inputString == "config.clean") {
        configClean();
        Serial.println("ok");
       } else
       if (inputString == "uptime") {
        Serial.print("uptime:");
        long now = millis() / 1000; // in seconds
        long sec = now % 60;
        now = now / 60;
        long minutes = now % 60;
        now = now / 60;
        long hours = now % 24;
        now = now / 24;
        Serial.print(now);
        Serial.print("t ");
        Serial.print(hours);
        Serial.print("h ");
        Serial.print(minutes);
        Serial.print("m ");
        Serial.print(sec);
        Serial.println("s");
       } else
       if (inputString == "reset") {
         resetFunc();  //call reset
       } else
       if (inputString == "status") {
         printStatus();
       } else {
        Serial.print("unknown cmd:");
        Serial.println(inputString);
       }
     } else {
       // set
       String key = inputString.substring(0,pos);
       String val = inputString.substring(pos+1);
       if (key == "tempMul") {
         tempMul = val.toFloat();
         Serial.println("ok");
       } else
       if (key == "tempAdd") {
         tempAdd = val.toInt();
         Serial.println("ok");
       } else
       if (key == "tempMin") {
         tempMin = val.toInt();
         Serial.println("ok");
       } else
       if (key == "tempMax") {
         tempMax = val.toInt();
         Serial.println("ok");
       } else
       if (key == "fanAdd") {
         fanAdd = val.toInt();
         Serial.println("ok");
       } else
       if (key == "fanSleep") {
         fanSleep = val.toInt();
         Serial.println("ok");
       } else
       if (key == "testWarn") {
         testWarn = val.toInt();
         Serial.println("ok");
       } else
       if (key == "monitor") {
         monitorOn = val == "true";
         Serial.println("ok");
       } else
       if (key == "active0") {
         active0 = val == "true";
         digitalWrite(fan0, LOW);
         Serial.println("ok");
       } else
       if (key == "active1") {
         active1 = val == "true";
         digitalWrite(fan1, LOW);
         Serial.println("ok");
       } else
       if (key == "active2") {
         active2 = val == "true";
         digitalWrite(fan2, LOW);
         Serial.println("ok");
       } else
       if (key == "serialEcho") {
         serialEcho = val == "true";
         Serial.println("ok");
       } else
       if (key == "delay") {
         delayTime = val.toInt();
         if (delayTime < 200) delayTime = 200;
         Serial.println("ok");
       } else {
        Serial.print("unknown set:");
        Serial.println(inputString);
       }
     }
     printPrompt();
     inputString = "";
     stringComplete = false;
   }
   if (monitorOn) {
     printStatus();
   }   

   digitalWrite(led, ledOn ? HIGH : LOW); 
   if (warn || ledOn) {
     digitalWrite(warnLed, HIGH);  // turn on led 
   } else {                    // else turn of led
     digitalWrite(warnLed, LOW); 
   }
   
   ledOn = !ledOn;
   delay(delayTime);
}

void printPrompt() {
  if (monitorOn || !serialEcho) return;
  Serial.print(prompt);
  Serial.print("$ ");

}
void printStatus() {
  Serial.print("TEMP0:");
  Serial.print(temp0);
  Serial.print(",FAN0:");
  Serial.print(fanSpeed0);
  Serial.print(",TEMP1:");
  Serial.print(temp1);
  Serial.print(",FAN1:");
  Serial.print(fanSpeed1);
  Serial.print(",TEMP2:");
  Serial.print(temp2);
  Serial.print(",FAN2:");
  Serial.print(fanSpeed2);
  Serial.print(",WARN:");
  Serial.print(warn);
  Serial.print("/");
  Serial.print(testWarn);
  Serial.print(",ALERTS:");
  Serial.print(alerts);
  Serial.println();
}

int readTemp(int pin) {  // get the temperature and convert it to celsius
  int temp = analogRead(pin);
  return temp * tempMul + tempAdd;
}

void serialEvent() {
  if (stringComplete) return;
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    if (inChar == '\n' || inChar == ';' || inChar == '\r') {
      stringComplete = true;
    } else {
      inputString += inChar;
    }
    if (serialEcho) {
      Serial.print(inChar);
      if (inChar == '\r') Serial.print('\n');
    }
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
  }
}
  
