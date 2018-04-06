# Arduino-RFID-Keypad
RFID with PIN Locker (base with PowerShell Server and CSV-File)

- ZIP-Files are required library for the arduino code.
- RFID_Server.ino: Arduino Code for (RFID MFRC522, LCD LiquidCrysta and Keypad)
- Auth_Server_v3.ps1: PowerShell-"Server", Data from external CSV (RFID-UID, PIN-Code, Access)

# Arduino with RFID-Reader, LCD 2x16 and Keypad
![alt text](https://github.com/marceleisenhut/Arduino-RFID-Keypad/blob/master/20180405_120625.jpg)



# PowerShell-Background (with CSV-Data)
running on the PC with Serial connection to the arduino (gives true or false for the permission from the cards and pins)
![alt text](https://github.com/marceleisenhut/Arduino-RFID-Keypad/blob/master/PowerShell.jpg)


# CSV File
Includes the RFID UIDs, PIN-Codes and the Access (true or false)
![alt text](https://github.com/marceleisenhut/Arduino-RFID-Keypad/blob/master/CSV.jpg)
