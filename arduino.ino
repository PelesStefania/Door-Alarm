#include <SPI.h>
#include <MFRC522.h>

#define reedPin 2          
#define buzzerPin 3        
#define redLedPin 8        
#define greenLedPin 4      
#define lockPin 7          

#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);

byte authorizedUID[4] = {0xE7, 0xF2, 0x18, 0xB2};

bool doorIsOpen = false;
bool cardAuthorized = false;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  pinMode(reedPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(lockPin, OUTPUT);

  digitalWrite(buzzerPin, LOW);
  digitalWrite(redLedPin, LOW);
  digitalWrite(greenLedPin, LOW);
  digitalWrite(lockPin, LOW);

  Serial.println("Apropie un card RFID...");
}

void loop() {

  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    bool authorized = true;

    for (byte i = 0; i < mfrc522.uid.size; i++) {
      if (mfrc522.uid.uidByte[i] != authorizedUID[i]) {
        authorized = false;
        break;
      }
    }

    if (authorized) {
      Serial.println("Card autorizat!");
      cardAuthorized = true;

      digitalWrite(greenLedPin, HIGH);
      digitalWrite(redLedPin, LOW);
      digitalWrite(lockPin, HIGH);

      tone(buzzerPin, 1000);
      delay(100);
      noTone(buzzerPin);
      
    } else {
      Serial.println("Card necunoscut!");
      digitalWrite(redLedPin, HIGH);
      digitalWrite(greenLedPin, LOW);
    }
  }

 
  bool currentDoorIsOpen = (digitalRead(reedPin) == HIGH); // HIGH = deschis

if (currentDoorIsOpen != doorIsOpen) {
  doorIsOpen = currentDoorIsOpen;

  if (doorIsOpen) {
    Serial.println("Usa s-a deschis!");

    if (!cardAuthorized) {
      tone(buzzerPin, 1000); 
      digitalWrite(redLedPin, HIGH);
    }

  } else {
    Serial.println("Usa s-a inchis!");

    noTone(buzzerPin);
    digitalWrite(lockPin, LOW);
    digitalWrite(greenLedPin, LOW);
    digitalWrite(redLedPin, LOW);
    cardAuthorized = false;
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();

    mfrc522.PCD_Init(); 
    delay(100);

    Serial.println("Apropie un card RFID...");
  }
}

  if (doorIsOpen && !cardAuthorized) {
    tone(buzzerPin, 1000);
    digitalWrite(redLedPin, HIGH);
  }

  delay(50);
}