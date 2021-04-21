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
 * SERVO       Arduino       
 * MOTOR      Uno/101       
 * Pin         Pin          
 * -------------------------------------
 * SG          8           
 * -------------------------------------
 * -------------------------------------
 * -------------------------------------
 */
//includes
#include <SPI.h> //allows communication between arduino and rfid reader
#include <MFRC522.h>// allows the reading of frid tag ids
#include "LinkedListLib.h"//allows creation of a linked list to store and maneuver through list of authorized uids
#include <Servo.h>//allows communication with servo motor

#define m1 25 //defined constants for uid of master card
#define m2 178//second value 
#define m3 200//third value 
#define m4 115//fourth value

//all pins used
#define SS_PIN 10
#define RST_PIN 9//rfid reader pins    
const int buzzer = 7;//buzzer pin
int led = 6;//acces led light pin
int master = 5;//mastercard led light pin
int touch = 2;//touch sensor pin

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

int pos = 0//position of servor;
Servo myservo;  // create servo object to control a servo

boolean adding;//flags to turn on and off adding mode
boolean sub = false;//flag to turn removing mode on and off

byte nuidPICC[4];//this holds the previously scanned card
LinkedList<byte> uid1;
LinkedList<byte> uid2;//linked list for all 4 uid numbers
LinkedList<byte> uid3;
LinkedList<byte> uid4;

int count;//count variable

int currentState; //the current state of the touch sensor 
int lastState; //the previous state of the touch sensor

void setup() { 
  Serial.begin(9600);//begin serial communication 9600 baud rate
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 
  count = 0;//initialize counter
  pos = 0;//initialize variable to tract servo position
  
  myservo.attach(8);//attach servo object to associated servo pin
  pinMode(buzzer, OUTPUT);
  pinMode(led, OUTPUT);//output pins 
  pinMode(master, OUTPUT);//set all pin modes 
  pinMode(touch, INPUT);//input pin
  
  myservo.write(pos);//set servo to 0 when starting 
}
 
void loop() {
  currentState = digitalRead(touch);//read current state of touch sensor
  
  if (lastState == LOW && currentState == HIGH){//this if stament will only be true when the sensor switched from low state to a high state 
    opendoor();//lock/open the lock
  }
  lastState = currentState;//recod the current state as the last state for the next loop
 
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
   
    
    if( nuidPICC[0] == m1 && nuidPICC[1] == m2 && nuidPICC[2] == m3 && nuidPICC[3] == m4)//checks if last scanned card id was the mastercard id 
    { 
      if (rfid.uid.uidByte[0] != nuidPICC[0] || //if it was check if current card id is not the same as master card id
        rfid.uid.uidByte[1] != nuidPICC[1] || 
        rfid.uid.uidByte[2] != nuidPICC[2] || 
        rfid.uid.uidByte[3] != nuidPICC[3] ) 
      {//if it is 
        if (sub == false)//if the previous id was the mastercard id and you are not in removing mode then you are in adding mode
        {
          addtolist(rfid.uid.uidByte[0], rfid.uid.uidByte[1], rfid.uid.uidByte[2],rfid.uid.uidByte[3]);//call adding function to add the card id into the list 
        }
        if(sub == true)//you are in removing mode
        {
          removefromlist(rfid.uid.uidByte[0], rfid.uid.uidByte[1], rfid.uid.uidByte[2],rfid.uid.uidByte[3]);//call removing function to remove card id from the list
        }
        
      }
      else//if currently scanned card is also the master card meaning its been scanned twice in a row 
      {
        if (rfid.uid.uidByte[0] == m1 && 
        rfid.uid.uidByte[1] == m2 && 
        rfid.uid.uidByte[2] == m3 && 
        rfid.uid.uidByte[3] == m4 )
        {
          sub = true; //then put program into removing mode
        }
      }
    }
    //if any other card id other than the master card id has been read then
    
    if(inhead(rfid.uid.uidByte[0], rfid.uid.uidByte[1], rfid.uid.uidByte[2],rfid.uid.uidByte[3]))//check if scanned card is in the head of the list 
    {
      Serial.println(F("Open door"));//if it is open the door
      opendoor();
    }
    else
    {
    /*if(intail(rfid.uid.uidByte[0], rfid.uid.uidByte[1], rfid.uid.uidByte[2],rfid.uid.uidByte[3])){//check if scanned card is in the tail of the list 
        Serial.println(F("Open door"));//if it is open door 
       digitalWrite(led, HIGH); // Turn the LED on
       opendoor();  
       }*/
      if(inbody(rfid.uid.uidByte[0], rfid.uid.uidByte[1], rfid.uid.uidByte[2],rfid.uid.uidByte[3]))//check if scanned card id is in the body of the list 
      {
        Serial.println(F("open door"));//if it is open door
        opendoor(); 
      }
    }
                
                  
  
    if(rfid.uid.uidByte[0] == m1 && rfid.uid.uidByte[1] == m2 && rfid.uid.uidByte[2] == m3 && rfid.uid.uidByte[3] == m4)//if currently scanned card is master card 
    {
      nuidPICC[0] = m1;
      nuidPICC[1] = m2;//make previously scanned card varable into marster card id
      nuidPICC[2] = m3;
      nuidPICC[3] = m4;
      Serial.println(F("mastercard scanned"));
      digitalWrite(master, HIGH); // Turn the martercard LED on
      delay(100);//100 milisecond delay
      digitalWrite(master, LOW); // Turn the mastercard LED off
    }
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
  if(pos == 0){// if the lock is closed
    pos = 150;//then set the next position to open 
    }
  else{//if the lock is open
    pos = 0; //then set the new position to lock  
  }
  myservo.write(pos);//change the servo to its new location
  tone(buzzer, 400); //ring the buzzer 
  delay(500);//wait 500 miliseconds
  noTone(buzzer);//end buzzer
  delay(500);//wait another 500 milisecond
  digitalWrite(led, LOW); // Turn the access LED on
}


boolean inhead(byte id1, byte id2, byte id3, byte id4)//check if id is in head of linked list
{
  if(uid1.GetHead() == id1 && uid2.GetHead() == id2 && uid3.GetHead() == id3 && uid4.GetHead() == id4){//check if uid is in head of list 
          return true;//if it is return true
  }
  else{
    return false;// if not return false
  }
}

boolean intail(byte id1, byte id2, byte id3, byte id4)//checks if id is in tail of linked list
{
  if(uid1.GetTail() == id1 && uid2.GetTail() == id2 && uid3.GetTail() == id3 && uid4.GetTail() == id4){//check if uid is in tail of list 
          return true;//if it is return true
  }
  else{
    return false;//if not return false
  }
}
boolean inbody(byte id1, byte id2, byte id3, byte id4)//checks if id is in body of linked list
{
    for(int i = 0; i <= uid1.GetSize(); i++){
       if(uid1.GetAt(i) == id1 && uid2.GetAt(i) == id2 && uid3.GetAt(i) == id3 && uid4.GetAt(i) == id4){//check if uid is in body of list
             return true;//if it is return true
        }         
                
    }
    return false;//if not return false
}

void addtolist(byte id1, byte id2, byte id3, byte id4)//add a uid to the list 
{
        Serial.println(F("Adding into list"));
        nuidPICC[0] = 0;
        nuidPICC[1] = 0; 
        nuidPICC[2] = 0; //clear previously read card so it can be reassigned 
        nuidPICC[3] = 0;
        if(inhead(id1, id2, id3,id4))//check if id is in head
        {
          Serial.println(F("already in list"));//if it is return
          adding = true;
          return;
                }
         else{
           adding = false;// if its not then make adding list false
         }
         if(intail(id1, id2, id3,id4))//check if in list 
         {             
            Serial.println(F("already in list"));//if it is then leave 
            adding = true;
            return;
                }
          else{
             adding = false;//if not then make adding list false
                    }
            for(int i = 0; i < uid1.GetSize(); i++){//check if it is in the body of the list
              if(uid1.GetAt(i) == id1 && uid2.GetAt(i) == id2 && uid3.GetAt(i) == id3 && uid4.GetAt(i) == id4){
                  Serial.println(F("already in list"));//if it is then leave 
                  adding = true;
                  return;
                }
                else{
                      adding = false;//if not then make adding false
                    }
                
            }
            if(adding == false){// if not in list then add to list 
              uid1.InsertHead(id1);//insert each id number into its respective list
              uid2.InsertHead(id2);
              uid3.InsertHead(id3);
              uid4.InsertHead(id4);
            }
        
        count++;//incerement counter
        Serial.println();
     
        for(int i = 0; i < uid1.GetSize(); i++){//show list of authorised ids in serial moniter
          Serial.print(uid1.GetAt(i));
          Serial.print(uid2.GetAt(i));//display the list
          Serial.print(uid3.GetAt(i));
          Serial.println(uid4.GetAt(i));
        }
   
        Serial.println();
}

void removefromlist(byte id1, byte id2, byte id3, byte id4)//removes a uid from the list 
{
  Serial.println(F("removing"));
  if(inhead(id1, id2, id3,id4))//if it is in head then remove
  {
    uid1.RemoveHead();
    uid2.RemoveHead();
    uid3.RemoveHead();
    uid4.RemoveHead();
    Serial.println(F("removed"));
    sub = false;//turn off removing mode
  }
  else
  {
  Serial.println(F("not in list"));
  sub = false;//if not in list turn off removing mode and return
  }
  if(intail(id1, id2, id3,id4)) //check if in list if it is remove it
  {       
    uid1.RemoveTail();
    uid2.RemoveTail();
    uid3.RemoveTail();
    uid4.RemoveTail();
    Serial.println(F("removed"));
    sub = false;//turn off removing mode
  }
  else
  {
    Serial.println(F("not in list"));
    sub = false;//if not in list turn off removing mode and return
  }
  for(int i = 0; i < uid1.GetSize(); i++)
  {
    if(uid1.GetAt(i) == rfid.uid.uidByte[0] && uid2.GetAt(i) == rfid.uid.uidByte[1] && uid3.GetAt(i) ==rfid.uid.uidByte[2] && uid4.GetAt(i) == rfid.uid.uidByte[3])//check if id is in bpdy if it is then remove it
    {
      uid1.RemoveAt(i);
      uid2.RemoveAt(i);
      uid3.RemoveAt(i);
      uid4.RemoveAt(i);
      Serial.println(F("removed"));
      sub = false;//turn off removing mode
    }
    else
    {
      Serial.println(F("not in list"));
      sub = false;//if not in list turn off removing mode and return
    }             
  }
  nuidPICC[0] = 0;//reset last read card to be reassigned a value
  nuidPICC[1] = 0; 
  nuidPICC[2] = 0; 
  nuidPICC[3] = 0;
  sub = false;//turn off removing mode
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
