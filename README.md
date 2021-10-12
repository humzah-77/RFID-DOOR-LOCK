# RFID-DOOR-LOCK
An arduino based rfid door lock

This code is based on the READNIUD example sketch from the MFRC522 library: https://github.com/miguelbalboa/rfid and built using an arduino uno.

This program makes use of the built in EEPROM which allows storage of data indefinetly. The 

Essentially how the lock system will work is on the outside of the room, there will be an RFID card reader placed next to the door. The program will have the masercard id hard coded in for convenience. To add a card to the authorised id list, you first need to scan the master card once then scan the card you would like to add on the reader, this will add the uid of the card to the list, only if the card is not already on the list. To remove a card from the list, you need to scan the master card twice, then scan the card you would like to delete. If id of the scanned card is on the list of authorized ids, it will then be removed.
for the inside of the room, ther wil be a touch sensor position next to the door which will allow for locking and unlocking 
with a simple touch


attached below is an activity diagram of the lock system
<img width="441" alt="rfidmap" src="https://user-images.githubusercontent.com/58381410/136876874-9b2fba10-d758-4c68-892c-76ac9421513f.png">
