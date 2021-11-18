/*
 * --------------------------------------------------------------------------------------------------------------------
 * This program was created by Humzah Okadia on April, 18, 2021
 * This program is an arduino based rfid card reader door lock.
 * --------------------------------------------------------------------------------------------------------------------
 * This code is based on the READNIUD example sketch from the MFRC522 library: https://github.com/miguelbalboa/rfid
 * essentially how thi lock system will work is on the outside of the room, there will be an rfid card reader placed next to the door.
 * the program will have the masercard id hard coded in for convenience
 * to add a card to the authorised id list, you first need to scan the master card once
 * then scan the card you would like to add on the reader, this will add the uid of the card to the list if the card is not already on the list
 * to remove a card from the list, you need to scan the master card twice, then scan the card you would like to delete
 * if id of the scanned card is on the list of authorized ids, it will then be removed.
 * for the inside of the room, ther wil be a touch sensor position next to the door which will allow for locking and unlocking 
 * with a simple touch
 * 
 * Pin layout used:
 * -------------------------------------
 * MFRC522      Arduino       
 * Reader/PCD   Uno/101       
 * Pin          Pin          
 * -------------------------------------
 * RST          9             
 * SDA          10            
 * MOSI         11   
 * MISO         12    
 * SCK          13    
 * -------------------------------------
 * -------------------------------------
 * -------------------------------------
 * Touch       Arduino       
 * Sensor      Uno/101       
 * Pin         Pin          
 * -------------------------------------
 * SG          2           
 * -------------------------------------
 * -------------------------------------
 * -------------------------------------
 * LED      Arduino       
 *          Uno/101       
 *          Pin          
 * -------------------------------------
 * 1        6             
 * 2        5
 * -------------------------------------
 * -------------------------------------
 * -------------------------------------
 *             Arduino       
 * BUZZER      Uno/101       
 * Pin         Pin          
 * -------------------------------------
 * +           7           
 * -------------------------------------
 * -------------------------------------
 * -------------------------------------
 * Relay       Arduino       
 * Module      Uno/101       
 * Pin         Pin          
 * -------------------------------------
 * S          8           
 * -------------------------------------
 * -------------------------------------
 * -------------------------------------
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

int pos = 0;//position of servor;
int master1;
int master2;
int master3;
int master4;

boolean adding;//flags to turn on and off adding mode
boolean sub = false;//flag to turn removing mode on and off
boolean fix = false;
byte nuidPICC[4];//this holds the previously scanned card


int count;//count variable

int currentState; //the current state of the touch sensor 
int lastState; //the previous state of the touch sensor

void setup() { 
  master1 = EEPROM.read(1);
  master2 = EEPROM.read(2);
  master3 = EEPROM.read(3);
  master4 = EEPROM.read(4);
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
  int curser = EEPROM.read(0);
  currentState = digitalRead(touch);//read current state of touch sensor
  if (lastState == LOW && currentState == HIGH){//this if stament will only be true when the sensor switched from low state to a high state 
    opendoor();//lock/open the lock
  }
  else{}
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial())//check if there is a card being scanned and if the card is readable by the frid reader
  {
    Serial.print(F("PICC type: "));
    MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);//prints out the type of rfid card in the serial moniter
    Serial.println(rfid.PICC_GetTypeName(piccType));
  
    // Check is the PICC of Classic MIFARE type
    if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
      piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
      piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
      Serial.println(F("Your tag is not of type MIFARE Classic."));
      return;
    }
   
    
    if( nuidPICC[0] == master1 && nuidPICC[1] == master2 && nuidPICC[2] == master3 && nuidPICC[3] == master4)//checks if last scanned card id was the mastercard id 
    { 
      if (rfid.uid.uidByte[0] != nuidPICC[0] || //if it was check if current card id is not the same as master card id
        rfid.uid.uidByte[1] != nuidPICC[1] || 
        rfid.uid.uidByte[2] != nuidPICC[2] || 
        rfid.uid.uidByte[3] != nuidPICC[3] ) 
      {//if it is 
        if (sub == false)//if the previous id was the mastercard id and you are not in removing mode then you are in adding mode
        {
          Serial.println(F("adding"));
          addtolist(rfid.uid.uidByte[0], rfid.uid.uidByte[1], rfid.uid.uidByte[2],rfid.uid.uidByte[3]);//call adding function to add the card id into the list 
        }
        if(sub == true)//you are in removing mode
        {
          Serial.println(F("removing"));
          removefromlist(rfid.uid.uidByte[0], rfid.uid.uidByte[1], rfid.uid.uidByte[2],rfid.uid.uidByte[3]);//call removing function to remove card id from the list
          nuidPICC[0] = 0;
          nuidPICC[1] = 0;
          nuidPICC[2] = 0;
          nuidPICC[3] = 0;
        }
        
      }
      else//if currently scanned card is also the master card meaning its been scanned twice in a row 
      {
        if (rfid.uid.uidByte[0] == master1 && 
        rfid.uid.uidByte[1] == master2 && 
        rfid.uid.uidByte[2] == master3 && 
        rfid.uid.uidByte[3] == master4 )
        {
          if(sub == false)
          {
            sub = true; //then put program into removing mode
            tone(buzzer, 400); //ring the buzzer 
            delay(500);//wait 500 miliseconds
            noTone(buzzer);//end buzzer
          }
          else{
            Serial.println(F("cancelled"));
            nuidPICC[0] = 0;
            nuidPICC[1] = 0;
            nuidPICC[2] = 0;
            nuidPICC[3] = 0;
            sub = false;
            tone(buzzer, 400); //ring the buzzer 
            delay(500);//wait 500 miliseconds
            noTone(buzzer);//end buzzer
          }
        }
      }
    }
    //if any other card id other than the master card id has been read then
                             
    if(rfid.uid.uidByte[0] == master1 && rfid.uid.uidByte[1] == master2 && rfid.uid.uidByte[2] == master3 && rfid.uid.uidByte[3] == master4)//if currently scanned card is master card 
    {
      nuidPICC[0] = master1;
      nuidPICC[1] = master2;//make previously scanned card varable into marster card id
      nuidPICC[2] = master3;
      nuidPICC[3] = master4;
      Serial.println(F("mastercard scanned"));
      digitalWrite(master, HIGH); // Turn the martercard LED on
      delay(100);//100 milisecond delay
      digitalWrite(master, LOW); // Turn the mastercard LED off
    }
    for(int i = 5; i < curser; i += 4){
      if (rfid.uid.uidByte[0] == EEPROM.read(i) && 
        rfid.uid.uidByte[1] == EEPROM.read(i+1) && 
        rfid.uid.uidByte[2] == EEPROM.read(i+2) && 
        rfid.uid.uidByte[3] == EEPROM.read(i+3) )
        {
          Serial.println(F("Open door"));//if it is open the door
          opendoor();
        }
    }
    lastState = currentState;//recod the current state as the last state for the next loop
    Serial.println(F("The NUID tag is:"));//show info about scanned card
    Serial.print(F("In hex: "));
    printHex(rfid.uid.uidByte, rfid.uid.size);//uid in hex format
    Serial.println();
    Serial.print(F("In dec: "));
    printDec(rfid.uid.uidByte, rfid.uid.size);//uid in decimal format
    Serial.println(nuidPICC[1]);
        
    // Halt PICC
    rfid.PICC_HaltA();
  
    // Stop encryption on PCD
    rfid.PCD_StopCrypto1();
  }
}

//-------------------------
//-------------------------
//-------------------------

void opendoor()//this function opens and locks the lock
{
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




void addtolist(byte id1, byte id2, byte id3, byte id4)//add a uid to the list 
{
        Serial.println(F("Adding into list"));
        nuidPICC[0] = 0;
        nuidPICC[1] = 0; 
        nuidPICC[2] = 0; //clear previously read card so it can be reassigned 
        nuidPICC[3] = 0;
        adding = false;
        if(EEPROM.read(0) > 5){
          for(int i = 5; i < EEPROM.read(0); i += 4){
          if (id1 == EEPROM.read(i) && 
            id2 == EEPROM.read(i+1) && 
            id3 == EEPROM.read(i+2) && 
            id4 == EEPROM.read(i+3) )
            {
            adding = true;
                }
            }
        }
        else{
          adding = false;
        }
            if(adding == false){// if not in list then add to list 
              int x = EEPROM.read(0);
              EEPROM.write(0,x+4);
              EEPROM.write(x,id1);//insert each id number into its respective list
              EEPROM.write(x+1,id2);
              EEPROM.write(x+2,id3);
              EEPROM.write(x+3,id4);
            }
        
        count++;//incerement counter
        Serial.println();
     
        for(int i = 5; i < EEPROM.read(0); i += 4){//show list of authorised ids in serial moniter
          Serial.print( EEPROM.read(i));
          Serial.print( EEPROM.read(i + 1));//display the list
          Serial.print( EEPROM.read(i + 2));
          Serial.println( EEPROM.read(i + 3));
          Serial.println( EEPROM.read(0));
        }
   
        Serial.println();
}

void removefromlist(byte id1, byte id2, byte id3, byte id4)//removes a uid from the list 
{
  sub = false;
  LinkedList<byte> uid1;
  LinkedList<byte> uid2;//linked list for all 4 uid numbers
  LinkedList<byte> uid3;
  LinkedList<byte> uid4;
  int z = 0;
  for(int i = 5; i <= EEPROM.read(0); i+=4){
    uid1.InsertHead(EEPROM.read(i));
    uid2.InsertHead(EEPROM.read(i + 1));
    uid3.InsertHead(EEPROM.read(i + 2));
    uid4.InsertHead(EEPROM.read(i + 3));
    }
    for(int i = 0; i <= uid1.GetSize(); i++){
       if(uid1.GetAt(i) == id1 && uid2.GetAt(i) == id2 && uid3.GetAt(i) == id3 && uid4.GetAt(i) == id4){//check if uid is in body of list
             uid1.RemoveAt(i);
             uid2.RemoveAt(i);
             uid3.RemoveAt(i);
             uid4.RemoveAt(i);
             int cerser = EEPROM.read(0);
             cerser -= 4;
             EEPROM.write(0,cerser);
             for(int i = 5; i < EEPROM.read(0); i+=4){
              EEPROM.write(i,0);
              EEPROM.write(i + 1,0);
              EEPROM.write(i + 2,0);
              EEPROM.write(i + 3,0);
            }
            EEPROM.update(0,cerser);
            for(int i = 5; i < EEPROM.read(0); i+=4){
              EEPROM.update(i,uid1.GetAt(z));
              EEPROM.update(i + 1,uid2.GetAt(z));
              EEPROM.update(i + 2,uid3.GetAt(z));
              EEPROM.update(i + 3,uid4.GetAt(z));
              z++;
            }
            for(int i = 5; i < EEPROM.read(0); i += 4){//show list of authorised ids in serial moniter
              Serial.print( EEPROM.read(i));
              Serial.print( EEPROM.read(i + 1));//display the list
              Serial.print( EEPROM.read(i + 2));
              Serial.println( EEPROM.read(i + 3));
              Serial.println( EEPROM.read(0));
            }
       
            Serial.println();
       }         
                
    }
  /*Serial.println(F("removing"));
    for(int i = 5; i < EEPROM.read(0); i += 4){
          if (id1 == EEPROM.read(i) && 
            id2 == EEPROM.read(i+1) && 
            id3 == EEPROM.read(i+2) && 
            id4 == EEPROM.read(i+3) )
            {
            EEPROM.write(i,0);
            EEPROM.write(i+1,0);
            EEPROM.write(i+1,0);
            EEPROM.write(i+3,0);
            if(i+4 < EEPROM.read(0)){
              
            }
            }
        if(fix){
          int m1 = EEPROM.read(i) ;
          int m2 = EEPROM.read(i +1);
          int m3 = EEPROM.read(i+2);
          int m4 = EEPROM.read(i + 3);   
          EEPROM.update(i-4,m1);
          EEPROM.update(i-3,m2);
          EEPROM.update(i-2,m3);
          EEPROM.update(i-1,m4);
          
        }
  }
  fix = false;
  int y = EEPROM.read(0);
  if (y>5){ 
    y = y - 4;
    EEPROM.update(0,y); 
  }
  nuidPICC[0] = 0;//reset last read card to be reassigned a value
  nuidPICC[1] = 0; 
  nuidPICC[2] = 0; 
  nuidPICC[3] = 0;
  sub = false;//turn off removing mode*/
  }
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
