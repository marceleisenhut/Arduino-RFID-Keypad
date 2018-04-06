# ZBW-Project-Week (IoT), 06.04.2018

# Arduino-RFID-Keypad
RFID with PIN Locker (base with PowerShell Server and CSV-File)

- ZIP-Files are required library for the arduino code.
- RFID_Server.ino: Arduino Code for (RFID MFRC522, LCD LiquidCrysta and Keypad)
- Auth_Server_v3.ps1: PowerShell-"Server", Data from external CSV (RFID-UID, PIN-Code, Access)

# Arduino with RFID-Reader, LCD 2x16 and Keypad
![alt text](https://github.com/marceleisenhut/Arduino-RFID-Keypad/blob/master/arduino.jpg)



# PowerShell-Background (with CSV-Data)
running on the PC with Serial connection to the arduino (gives true or false for the permission from the cards and pins)

to start the PowerShell-Console, you have to give some Parameters: <br>
<strong>.\Auth_Server_v3.ps1 -COMPORT COM5 -Database .\aut_database.csv</strong>

![alt text](https://github.com/marceleisenhut/Arduino-RFID-Keypad/blob/master/PowerShell_Console.jpg)


# CSV File
Includes the RFID UIDs, PIN-Codes and the Access (true or false)
![alt text](https://github.com/marceleisenhut/Arduino-RFID-Keypad/blob/master/CSV.jpg)


# Debugging with 2nd LCD
for debugging some messages, we installed a 2nd lcd monitor
![alt text](https://github.com/marceleisenhut/Arduino-RFID-Keypad/blob/master/Arduino_with_Debugging_Monitor.jpg)


# Demo: Zugriff erlaubt
![alt text](https://github.com/marceleisenhut/Arduino-RFID-Keypad/blob/master/Zugriff_erlaubt.gif)


# Demo: Zugriff verweigert
![alt text](https://github.com/marceleisenhut/Arduino-RFID-Keypad/blob/master/Zugriff_verweigert.gif)
