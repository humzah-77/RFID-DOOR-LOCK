/*
 * --------------------------------------------------------------------------------------------------------------------
 * This program was created by Humzah Okadia on April, 18, 2021
 * This program is an arduino based rfid card reader door lock.
 * --------------------------------------------------------------------------------------------------------------------
 * this program is to set up the master card on the eeprom for the rfid door lock program
  */

//includes
#include <SPI.h> //allows communication between arduino and rfid reader
#include <MFRC522.h>// allows the reading of frid tag ids
#include "LinkedListLib.h"//allows creation of a linked list to store and maneuver through list of authorized uids
#include <Servo.h>//allows communication with servo motor
#include <EEPROM.h>



//all pins used
#define SS_PIN 10
#define RST_PIN 9//rfid reader pins    
const int buzzer = 7;//buzzer pin
int led = 6;//acces led light pin
int master = 5;//mastercard led light pin
int touch = 2;//touch sensor pin
int lock = 8;
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

int pos = 0;//position of servo;

boolean adding;//flags to turn on and off adding mode
boolean sub = false;//flag to turn removing mode on and off

byte nuidPICC[4];//this holds the previously scanned card
LinkedList<byte> uid1;
LinkedList<byte> uid2;//linked list for all 4 uid numbers
LinkedList<byte> uid3;
LinkedList<byte> uid4;

int count = 0;//count variable

int currentState; //the current state of the touch sensor 
int lastState; //the previous state of the touch sensor

void setup() { 
  Serial.begin(9600);//begin serial communication 9600 baud rate
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 
  count = 0;//initialize counter
  pos = 0;//initialize variable to tract servo position
  
  pinMode(lock, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(led, OUTPUT);//output pins 
  pinMode(master, OUTPUT);//set all pin modes 
  pinMode(touch, INPUT);//input pin
}
 
void loop() {
 
 
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been read
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }
  //if tag is of right format
  if(count == 0){
    printDec(rfid.uid.uidByte, rfid.uid.size);//scanned card will become master card
    EEPROM.write(0, 5);//write current curser for eeprom
    EEPROM.write(1, rfid.uid.uidByte[0]);
    EEPROM.write(2, rfid.uid.uidByte[1]);
    EEPROM.write(3, rfid.uid.uidByte[2]);//write each uid digit into eeprom 
    EEPROM.write(4, rfid.uid.uidByte[3]);
    count = 1;
    //opens lock to confirm adding mastercard
     digitalWrite(led, HIGH); // Turn the access LED on
    digitalWrite(lock, HIGH);
    tone(buzzer, 400); //ring the buzzer 
    delay(500);//wait 500 miliseconds
    noTone(buzzer);//end buzzer
    delay(500);//wait another 500 milisecond
    digitalWrite(led, LOW); // Turn the access LED on
    delay(3000);
    digitalWrite(lock,LOW);
  }
   
  
}

//-------------------------
//-------------------------
//-------------------------

/**
 * Helper routine to dump a byte array as hex values to Serial. 
 */
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

/**
 * Helper routine to dump a byte array as dec values to Serial.
 */
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}
