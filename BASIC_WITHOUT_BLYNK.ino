#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// ===================== PIN DEFINITIONS =====================
#define SS_PIN         2    // D4 -> RC522 SDA
#define RST_PIN        0    // D3 -> RC522 RST
#define SLOT1_IR_PIN   16   // D0 -> Slot 1 IR sensor
#define SERVO_PIN      15   // D8 -> Servo signal
#define EXIT_IR_PIN    3    // RX -> Exit IR sensor
#define SLOT2_IR_PIN   1    // TX -> Slot 2 IR sensor (for later)

// ===================== OBJECTS =====================
MFRC522 rfid(SS_PIN, RST_PIN);
Servo gateServo;
LiquidCrystal_I2C lcd(0x27, 20, 2);

// ===================== SETTINGS =====================
const int GATE_CLOSED_ANGLE = 0;
const int GATE_OPEN_ANGLE   = 90;

// IMPORTANT:
// Keep this false for now since you have not connected Slot 2 IR yet.
// Change to true after wiring Slot 2 IR sensor to TX (GPIO1).
const bool SLOT2_INSTALLED = false;

// Change this UID to your real RFID UID
byte validUID[4] = {0xab, 0xcd, 0xef, 0xgh};

// Most IR obstacle sensors give LOW when object is detected.
// If your IR works opposite, change LOW to HIGH.
const int IR_DETECTED = LOW;

// Timing
unsigned long lastRFIDTime = 0;
unsigned long lastExitTriggerTime = 0;
const unsigned long RFID_COOLDOWN = 2000;
const unsigned long EXIT_COOLDOWN = 2500;

// ===================== HELPER FUNCTIONS =====================

bool isValidCard(byte *uid, byte uidSize) {
  if (uidSize != 4) return false;

  for (byte i = 0; i < 4; i++) {
    if (uid[i] != validUID[i]) {
      return false;
    }
  }
  return true;
}

bool isSlot1Occupied() {
  return digitalRead(SLOT1_IR_PIN) == IR_DETECTED;
}

bool isSlot2Occupied() {
  if (!SLOT2_INSTALLED) return false;   // if not installed, treat as not used
  return digitalRead(SLOT2_IR_PIN) == IR_DETECTED;
}

bool isExitDetected() {
  return digitalRead(EXIT_IR_PIN) == IR_DETECTED;
}

int getTotalSlots() {
  if (SLOT2_INSTALLED) return 2;
  return 1;
}

int getOccupiedSlots() {
  int occupied = 0;

  if (isSlot1Occupied()) occupied++;
  if (SLOT2_INSTALLED && isSlot2Occupied()) occupied++;

  return occupied;
}

int getAvailableSlots() {
  return getTotalSlots() - getOccupiedSlots();
}

bool isAnySlotAvailable() {
  return getAvailableSlots() > 0;
}

void openGate() {
  gateServo.write(GATE_OPEN_ANGLE);
  delay(1500);
  gateServo.write(GATE_CLOSED_ANGLE);
  delay(500);
}

void showMessage(String line1, String line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

void updateLCDRealtime() {
  // Line 1: Availability / Total slots
  lcd.setCursor(0, 0);
  lcd.print("Avail: ");
  lcd.print(getAvailableSlots());
  lcd.print("/");
  lcd.print(getTotalSlots());
  lcd.print("   ");  // clear extra chars

  // Line 2: User prompt
  lcd.setCursor(0, 1);
  lcd.print("Tap entry card ");
}

void checkRFIDEntry() {
  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  if (millis() - lastRFIDTime < RFID_COOLDOWN) {
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    return;
  }

  lastRFIDTime = millis();

  bool valid = isValidCard(rfid.uid.uidByte, rfid.uid.size);

  if (!valid) {
    showMessage("Access Denied", "");
    delay(1500);
  } else {
    if (!isAnySlotAvailable()) {
      showMessage("Valid RFID", "No Slot Available");
      delay(1500);
    } else {
      showMessage("Valid RFID", "Gate Opening");
      openGate();
      showMessage("Vehicle Entered", "");
      delay(1000);
    }
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

void checkExitSensor() {
  if (millis() - lastExitTriggerTime < EXIT_COOLDOWN) return;

  if (isExitDetected()) {
    lastExitTriggerTime = millis();

    showMessage("Exit Detected", "Gate Opening");
    openGate();
    showMessage("Thank You", "");
    delay(1000);
  }
}

void setup() {
  // Avoid Serial.begin because TX (GPIO1) may be used later for Slot 2 IR
  SPI.begin();
  rfid.PCD_Init();

  pinMode(SLOT1_IR_PIN, INPUT);
  pinMode(EXIT_IR_PIN, INPUT);

  // Only initialize Slot 2 pin if installed
  if (SLOT2_INSTALLED) {
    pinMode(SLOT2_IR_PIN, INPUT);
  }

  gateServo.attach(SERVO_PIN);
  gateServo.write(GATE_CLOSED_ANGLE);

  Wire.begin(4, 5); // SDA, SCL
  lcd.init();
  lcd.backlight();

  showMessage("Parking System", "Initializing...");
  delay(1500);
  lcd.clear();
}

void loop() {
  updateLCDRealtime();
  checkRFIDEntry();
  checkExitSensor();
}