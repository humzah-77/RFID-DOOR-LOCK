# RFID-DOOR-LOCK
An arduino based rfid door lock

This code is based on the READNIUD example sketch from the MFRC522 library: https://github.com/miguelbalboa/rfid
essentially how thi lock system will work is on the outside of the room, there will be an rfid card reader placed next to the door.
the program will have the masercard id hard coded in for convenience
to add a card to the authorised id list, you first need to scan the master card once
then scan the card you would like to add on the reader, this will add the uid of the card to the list if the card is not already on the list
to remove a card from the list, you need to scan the master card twice, then scan the card you would like to delete
if id of the scanned card is on the list of authorized ids, it will then be removed.
for the inside of the room, ther wil be a touch sensor position next to the door which will allow for locking and unlocking 
with a simple touch

Pin layout used:
-------------------------------------
MFRC522      Arduino       
Reader/PCD   Uno/101       
Pin          Pin          
-------------------------------------
RST          9             
SDA          10            
MOSI         11   
MISO         12    
SCK          13    
-------------------------------------
-------------------------------------
-------------------------------------
Touch       Arduino       
Sensor      Uno/101       
Pin         Pin          
-------------------------------------
SG          2           
-------------------------------------
-------------------------------------
-------------------------------------
 LED      Arduino       
          Uno/101       
          Pin          
-------------------------------------
1        6             
2        5
-------------------------------------
-------------------------------------
-------------------------------------
             Arduino       
 BUZZER      Uno/101       
 Pin         Pin          
-------------------------------------
+           7           
-------------------------------------
-------------------------------------
-------------------------------------
SERVO       Arduino       
MOTOR      Uno/101       
Pin         Pin          
-------------------------------------
SG          8           
-------------------------------------
-------------------------------------
-------------------------------------
