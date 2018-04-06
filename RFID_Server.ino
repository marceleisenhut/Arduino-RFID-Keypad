#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN 10
#define RST_PIN 9
#include <LiquidCrystal_PCF8574.h>
#include <Wire.h>
#include "Keypad.h"

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.

// initialize the library
LiquidCrystal_PCF8574 lcd(0x3F);
LiquidCrystal_PCF8574 debug(0x27);

// Init array that will store new NUID 
byte nuidPICC[4];
String CardNumber;

// LED Lampen
int LEDRed = A0;
int LEDGreen = A1;

// Piezo Speaker
int PiezoPIN = A2;

// ----------------------- Keypad --------------------------------------
// Keypad Global Vars
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
String aPIN;

char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {2, 3, 4, 5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {6, 7, 8}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

// ----------------------- Keypad - ENDE --------------------------------

void setup() {
  Serial.begin(9600); // Initialize serial communications with the PC
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card

  pinMode(LEDRed, OUTPUT);
  pinMode(LEDGreen, OUTPUT);
  pinMode(PiezoPIN, OUTPUT);
 // digitalWrite(LEDRed, LOW);
  //digitalWrite(LEDGreen, LOW);

  // initialize LCD (columns,rows)
  lcd.begin(16, 2);
  debug.begin(16, 2);
  // set cursor to line 0 column 0
  lcd.setCursor(0, 0);
  debug.setCursor(0, 0);
  debug.clear();
  debug.clear(); debug.print("Debugger");
  debug.scrollDisplayRight();
  lcd.print("Zutritt");
  // Backlight zu maximum (0 to 255)
  lcd.setBacklight(255);
  debug.setBacklight(255);
  lcd.setCursor(0,1);
  lcd.print("Chip vorweisen:");
  
  
}

void loop() {
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Dump debug info about the card. PICC_HaltA() is automatically called.
  // mfrc522.PICC_DumpToSerial(&(mfrc522.uid));

  // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = mfrc522.uid.uidByte[i];
    }
   
    // debug.print("Chip wird gelesen...");
    // Serial.print(F("In dec: "));
    // printDec(mfrc522.uid.uidByte, mfrc522.uid.size);
    checkCard();
    
    
    delay(1500);
}

void checkCard() {
  // CardNumber in Variable speichern
  getCardNumber(mfrc522.uid.uidByte, mfrc522.uid.size);

  // Karte prüfen
  if(isCardValid() == true) {

    // Karte OK
    lcdKarteOK();
  
    // Eingabe von PIN empfangen (Abschluss mit #) 
    aPIN = getPIN();
    // Serial.print("PIN: ");
    debug.clear(); debug.print(aPIN);

    // Autorisiere Card und PIN
    if(authServer(CardNumber, aPIN) == true) {
      // Zugriff gewährt
      ZugriffOK();
    } else {
      // Zugriff verweigert
      ZugriffNOK();
    }
  } else {
    ZugriffNOK();
  }
  delay(3000);
  SystemDefault();
}

String getPIN() {
  char* pin = new char[10];
  int keyCounter = 0;
  boolean keyFinished = false;
  String ReturnPIN = "";
  
  // Pin
  debug.clear(); debug.print("PIN Eingabe erforderlich");
  lcdPINEingabe();
  while(keyFinished == false) {
    char key = keypad.getKey();
    if (key){
      // Wenn # gedrückt ist der PIN fertig eingegeben
      if(key != 35)
      {
        // debug.print(key);
        pin[keyCounter] = key;
        lcdStar(keyCounter);
        keyCounter++;  
      }
        else
      {
        // Serial.print("Key finshed: ");
        for(int i = 0; i < keyCounter; i++)
          {
            // Serial.print(pin[i]);
            ReturnPIN = ReturnPIN + pin[i];  
          }
          
          keyFinished = true;
          keyCounter = 0;
      }
    }

  }
  return ReturnPIN;
}

boolean authServer(String CardNumber, String PIN) {
  
  // Daten an Server senden  
  // int von Server (0: OK, 1: FAIL)
  
  if(isPINValid() == true) {
    return true;
  }
  return false;
}

boolean isCardValid() {
  lcdAnfrage();
  sendToSerial("RFID/" + CardNumber);
  if(readFromSerial() == 0) {
    return true;
  }
  return false;
}

boolean isPINValid() {
  lcdAnfrage();
  sendToSerial("PIN/" + aPIN + "/" + CardNumber);
  delay(500);
  if(readFromSerial() == 0) {
    return true;
  }
  return false;
}

int readFromSerial() {
  debug.clear(); debug.print("Wait for Serial Input...");
  boolean finish = false;
  int answer = 0;
  delay(500);
  while (finish == false) {
    delay(10);
    // Schlaufe
    answer = Serial.read();
    if(answer != -1) {
      finish = true;
    }
  }
  delay(10);
  // serial.print("Ausgabe: ");
  debug.clear(); debug.print(answer);
  // 48 ASCII --> 0
  if(answer==48) {
    return 0;
  }
  return 1;
}

void sendToSerial(String Text) {
  Serial.println(Text);
}

void getCardNumber(byte *buffer, byte bufferSize) {
  CardNumber = "";
  for (byte i = 0; i < bufferSize; i++) {
    CardNumber = CardNumber + (buffer[i] < 0x10 ? " 0" : " ");
    CardNumber = CardNumber +buffer[i], DEC;
  }
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

void ZugriffOK() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Zutritt");
  lcd.setCursor(0,1);
  lcd.print("zugelassen!");
  debug.clear(); debug.print("zugelassen!");
  digitalWrite(LEDRed, LOW);
  digitalWrite(LEDGreen, HIGH);
  makeNoiceOK();
}

void ZugriffNOK() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Zutritt");
  lcd.setCursor(0,1);
  lcd.print("verweigert!");
  debug.clear(); debug.print("verweigert!");
  digitalWrite(LEDRed, HIGH);
  digitalWrite(LEDGreen, LOW);
  makeNoiceFail();
}

void SystemDefault() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Zutritt");
  lcd.setCursor(0,1);
  lcd.print("Chip vorweisen:");
  digitalWrite(LEDRed, LOW);
  digitalWrite(LEDGreen, LOW);
  debug.clear();
  debug.print("Debugger");
  
  
  CardNumber = "";
}

void lcdPINEingabe() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PIN eingeben:");
  lcd.setCursor(0,1);
  lcd.print("");
}

void lcdStar(int anz) {
  lcd.setCursor(anz,1);
  lcd.print("*");
}

void lcdAnfrage() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Zutritt wird");
  lcd.setCursor(0,1);
  lcd.print("angefragt...");
}

void lcdKarteOK() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Karte wurde");
  lcd.setCursor(0,1);
  lcd.print("zugelassen");
  delay(1000);
}

void makeNoiceOK() {
  tone(PiezoPIN, 750, 261);
  delay(500);
  tone(PiezoPIN, 1000, 523);
  delay(500);
}

void makeNoiceFail() {
  // digitalWrite(PiezoPIN, HIGH);
  // Serial.println("Test Noice Fail");
  tone(PiezoPIN, 329 , 500);
  delay(500);
  tone(PiezoPIN, 294 , 500);
  delay(500);
}

/**
 * Helper routine to dump a byte array as dec values to Serial.
 */
void printDec(byte *buffer, byte bufferSize) {
  CardNumber = "";
  for (byte i = 0; i < bufferSize; i++) {
    // serial.print(buffer[i] < 0x10 ? " 0" : " ");
    // serial.print(buffer[i], DEC);
  }
  
  // serial.print("Ausgabe von String: ");
  // debug.print(CardNumber);
  
  // debug.print("Bitte Karte vorweisen...");
}
