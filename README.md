# RFID-DOOR-LOCK
An Arduino based rfid door lock

This code is based on the READNIUD example sketch from the MFRC522 library: https://github.com/miguelbalboa/rfid and built using an Arduino Uno.

This program makes use of the built in EEPROM which allows storage of data indefinetly. The benifit of using the EEPROM is that is keeps the data in the event of power loss or accidently hitting the reset button on the Arduino board.

<img width="1324" alt="Screen Shot 2021-11-24 at 8 17 49 PM" src="https://user-images.githubusercontent.com/58381410/143334827-b9958c0e-c1cc-41a4-a383-f498098eecce.png">


How To Run:

1. Upload setupeeprom.ino to the Arduino board
2. Once uploaded scan a RFID card
3. Once scanned the ID will be saved into EEPROM as the mastercard in slots 1,2,3,4 along with EEPROM poisition curser variable stored in slot 0
4. Upload rfiddoorlock.ino to Arduino board
5. Now the system is ready

How It Works:

Essentially how the lock system works is, on the outside of the room there will be an RFID card reader placed next to the door. To add a card to the authorised ID list, you first need to scan the mastercard once, this will initiate add mode, then scan the card you would like to add, this will add the UID of the card to the list, only if the card is not already on the list. To remove a card from the list, you need to scan the mastercard twice, this will initiate delete mode, then scan the card you would like to delete. If the ID of the scanned card is on the list of authorized IDs, it will then be removed. For the inside of the room, there wil be a touch sensor positioned next to the door which will allow for locking and unlocking with a simple touch.

EEPROM Position Curser Variable:

This variable is used to show upto which position is filled in the EEPROM. Once the setupeeprom.ino is run the curser variable is set to 5, 1 for the curser variable and 4 for the mastercard ID numbers. When a new ID is added it will add the four ID numbers into the next four positions after the curser position. When removing a card first the list of all the IDs are copied onto a linkedlist, then a for loop is used. The loop checks every 4th slot of the list and compares it with the first ID number of the card scanned. Once the ID is found it is removed from the linked list. Then the EEPROM list is cleared and the updated linkedlist is transferred back into the EEPROM and the curser variable gets subtracted by 4.

# ACTIVITY-DIAGRAM

Using StarUML

<img width="441" alt="rfidmap" src="https://user-images.githubusercontent.com/58381410/136876874-9b2fba10-d758-4c68-892c-76ac9421513f.png">
