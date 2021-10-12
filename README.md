# RFID-DOOR-LOCK
An arduino based rfid door lock

This code is based on the READNIUD example sketch from the MFRC522 library: https://github.com/miguelbalboa/rfid and built using an arduino uno.

This program makes use of the built in EEPROM which allows storage of data indefinetly. The benifit of using the EEPROM is that is keeps the data in the event of power loss or accidently hitting the reset button on the arduino board.

how to run:
1. Upload setupeeprom.ino to the Arduino board
2. Once uploaded scan a RFID card
3. Once scanned the id will be saved into eeprom along with EEPROM poisition curser variable
4. Upload rfiddoorlock.ino to Arduino board
5. Now the system is ready

Essentially how the lock system will work is, on the outside of the room there will be an RFID card reader placed next to the door. To add a card to the authorised ID list, you first need to scan the mastercard once, this will initiate add mode, then scan the card you would like to add, this will add the UID of the card to the list, only if the card is not already on the list. To remove a card from the list, you need to scan the mastercard twice, this will initiate delete mode, then scan the card you would like to delete. If the ID of the scanned card is on the list of authorized IDs, it will then be removed. For the inside of the room, there wil be a touch sensor positioned next to the door which will allow for locking and unlocking with a simple touch.



attached below is an activity diagram of the lock system


<img width="441" alt="rfidmap" src="https://user-images.githubusercontent.com/58381410/136876874-9b2fba10-d758-4c68-892c-76ac9421513f.png">
