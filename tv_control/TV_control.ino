#include <Arduino.h>
#include <IRremote.h> 

IRsend irsend;
int light_intensity = 0;  
int const LIGHT_THRESHOLD = 950;
int const khz = 38; // 38kHz carrier frequency for the NEC protocol

unsigned int power_tcl[] = {4000,3900, 550,1950, 500,1950, 500,1950, 550,1950, 500,1000, 500,950, 550,1950, 500,1000, 500,1950, 500,1000, 500,1950, 500,1000, 500,950, 550,950, 500,1000, 500,1000, 500,1950, 500,1950, 550,950, 500,2000, 500,950, 500,2000, 500,950, 550,1950, 500};
unsigned int source_tcl[] = {4000,3900, 550,1950, 500,1950, 500,1950, 550,1950, 500,1950, 500,1000, 500,1950, 500,1000, 500,1000, 500,1000, 500,1950, 500,1950, 500,1000, 500,1000, 500,950, 550,950, 500,1000, 500,1950, 550,950, 500,1950, 550,1950, 500,1950, 500,1000, 500,1000, 500};
unsigned int exit_tcl[] = {4000,3950, 500,1950, 500,2000, 500,1950, 500,1950, 550,950, 500,1000, 500,1000, 500,950, 500,1000, 500,1950, 550,1950, 500,1000, 500,950, 550,950, 500,1000, 500,950, 550,1950, 500,1950, 500,2000, 500,1950, 500,1950, 550,950, 500,1000, 500,1950, 500};
unsigned int enter_tcl[] = {4000,3950, 550,1900, 500,1950, 550,1950, 550,1900, 550,1950, 500,1950, 500,1950, 550,1950, 550,900, 550,1950, 550,950, 500,950, 550,950, 550,950, 550,950, 500,950, 500,1000, 550,950, 550,950, 500,950, 550,1950, 500,950, 550,1950, 500,1950, 550};
unsigned int up_tcl[] = {4000,3950, 500,1950, 500,1950, 550,1950, 500,1950, 500,1000, 500,1950, 550,950, 500,1950, 550,1950, 500,1000, 500,950, 550,1950, 500,1000, 500,950, 500,1000, 500,1000, 500,1950, 500,1000, 500,1950, 500,1000, 500,1000, 500,1950, 500,1950, 550,950, 500};
unsigned int down_tcl[] = {4000,3900, 550,1950, 550,1900, 550,1950, 500,1950, 550,950, 500,1950, 550,950, 550,1900, 550,1950, 500,950, 550,950, 550,950, 550,950, 500,950, 550,950, 550,950, 550,1900, 550,950, 550,1900, 550,950, 550,950, 500,1950, 550,1950, 500,1950, 550};
unsigned int left_tcl[] = {4000,3950, 500,1950, 500,1950, 550,1950, 500,1950, 500,1000, 500,1950, 550,950, 500,1950, 550,950, 500,2000, 500,1950, 500,1000, 500,950, 550,950, 500,1000, 500,1000, 500,1950, 500,1000, 500,1950, 500,1000, 500,1950, 500,1000, 500,1000, 500,1950, 500};
unsigned int right_tcl[] = {4000,3950, 500,1950, 500,2000, 500,1950, 500,1950, 500,1000, 500,1950, 550,950, 500,2000, 500,950, 500,2000, 500,1950, 500,1950, 550,950, 500,1000, 500,1000, 500,950, 550,1950, 500,950, 550,1950, 500,1000, 500,1950, 500,1000, 500,950, 550,950, 500};

const int numReadings = 10;
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average

int inputPin = A0;

void setup() {  
  // Open Serial port with:
  // baud rate: 9600
  // Data size: 8 bits
  // Parity: None
  // Stop Bits: 1 bit
  Serial.begin(9600, SERIAL_8N1);  

  // initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
}

void loop() {
  String content = " ";
  char character = ' ';
  char value[50];
  char *eptr;
  unsigned long hex_code;
    
  while(Serial.available()) {
    character = Serial.read();
    if (character == '+') 
    {
       break;
    }
    content.concat(character);
  }
  
  if (content != "") {
    strcpy(value, content.c_str());
    hex_code = strtoul(value, &eptr, 16);
    
    sendCodeTVs(hex_code);
  }
  checkStatus();
  delay(50);
}

void checkStatus() {
  // subtract the last reading:
  total = total - readings[readIndex];
  readings[readIndex] = analogRead(inputPin);
  total = total + readings[readIndex];
  readIndex = readIndex + 1;

  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average = total / numReadings;

  if (average > LIGHT_THRESHOLD){
    Serial.print(0x1111);
  }
  else {
    Serial.print(0x0000);
  }
}

void sendCodeTVs(unsigned long code){
  irsend.sendNEC(code, 32); //send code TV ASANZO
  
  if(code == 0x6A68351E) {
    irsend.sendRaw(power_tcl, sizeof(power_tcl) / sizeof(power_tcl[0]), khz);
  }
  else if(code == 0x40C1789F) {
    irsend.sendRaw(source_tcl, sizeof(source_tcl) / sizeof(source_tcl[0]), khz);
  }
  else if(code == 0x7E940902) {
    irsend.sendRaw(exit_tcl, sizeof(exit_tcl) / sizeof(exit_tcl[0]), khz);
  }
  else if(code == 0xB8E7B4FE) {
    irsend.sendRaw(enter_tcl, sizeof(enter_tcl) / sizeof(enter_tcl[0]), khz);
  }
  else if(code == 0x1039789F) {
    irsend.sendRaw(up_tcl, sizeof(up_tcl) / sizeof(up_tcl[0]), khz);
  }
  else if(code == 0xBB8B3E9E) {
    irsend.sendRaw(down_tcl, sizeof(down_tcl) / sizeof(down_tcl[0]), khz);
  }
  else if(code == 0x45481702) {
    irsend.sendRaw(left_tcl, sizeof(left_tcl) / sizeof(left_tcl[0]), khz);
  }
  else if(code == 0xF0B4BB43) {
    irsend.sendRaw(right_tcl, sizeof(right_tcl) / sizeof(right_tcl[0]), khz);
  }
  else{}
}
