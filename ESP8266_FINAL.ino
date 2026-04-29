#define BLYNK_TEMPLATE_ID "YOUR_BLYNK_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "YOUR_BLYNK_TEMPLATE_NAME"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_AUTH_TOKEN"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// ================= WIFI =================
char ssid[] = "YOUR_WIFI_NAME";
char pass[] = "YOUR_WIFI_PASSWORD";

String GOOGLE_SCRIPT_URL = "YOUR_GOOGLE_APPS_SCRIPT_WEB_APP_URL";

// ================= PIN DEFINITIONS =================
#define SS_PIN         2    // D4 -> RC522 SDA
#define RST_PIN        0    // D3 -> RC522 RST
#define SLOT1_IR_PIN   16   // D0 -> Slot IR
#define SERVO_PIN      15   // D8 -> Servo
#define EXIT_IR_PIN    3    // RX -> Exit IR
#define SLOT2_IR_PIN   1    // TX -> Future Slot 2 IR

// ================= OBJECTS =================
MFRC522 rfid(SS_PIN, RST_PIN);
Servo gateServo;
LiquidCrystal_I2C lcd(0x27, 20, 2);
BlynkTimer timer;

// ================= SETTINGS =================
const int GATE_CLOSED_ANGLE = 0;
const int GATE_OPEN_ANGLE = 90;

const bool SLOT2_INSTALLED = false;
const int IR_DETECTED = LOW;

unsigned long lastRFIDTime = 0;
unsigned long lastExitTriggerTime = 0;

const unsigned long RFID_COOLDOWN = 2000;
const unsigned long EXIT_COOLDOWN = 2500;

// ================= REGISTERED USERS =================
struct RegisteredUser {
  byte uid[4];
  String name;
  String vehicle;
  String email;
};

// Replace demo UIDs/emails with your own while testing locally.
// Do not commit real user emails in public repos.
RegisteredUser users[] = {
  {{0xAB, 0xCD, 0xEF, 0x12}, "Demo User 1", "MP00AB0001", "user1@example.com"},
  {{0x11, 0x22, 0x33, 0x44}, "Demo User 2", "MP00AB0002", "user2@example.com"},
  {{0x55, 0x66, 0x77, 0x88}, "Demo User 3", "MP00AB0003", "user3@example.com"}
};

int totalUsers = sizeof(users) / sizeof(users[0]);

String lastEntryName = "Unknown";
String lastEntryVehicle = "Unknown";
String lastEntryUID = "Unknown";
String lastEntryEmail = "";

// ================= UID FUNCTIONS =================
bool compareUID(byte *cardUID, byte *validUID) {
  for (byte i = 0; i < 4; i++) {
    if (cardUID[i] != validUID[i]) return false;
  }
  return true;
}

int getUserIndex(byte *cardUID, byte uidSize) {
  if (uidSize != 4) return -1;

  for (int i = 0; i < totalUsers; i++) {
    if (compareUID(cardUID, users[i].uid)) return i;
  }

  return -1;
}

String uidToString(byte *uid, byte uidSize) {
  String uidStr = "";

  for (byte i = 0; i < uidSize; i++) {
    if (uid[i] < 0x10) uidStr += "0";
    uidStr += String(uid[i], HEX);
    if (i < uidSize - 1) uidStr += " ";
  }

  uidStr.toUpperCase();
  return uidStr;
}

// ================= PARKING FUNCTIONS =================
bool isSlot1Occupied() {
  return digitalRead(SLOT1_IR_PIN) == IR_DETECTED;
}

bool isSlot2Occupied() {
  if (!SLOT2_INSTALLED) return false;
  return digitalRead(SLOT2_IR_PIN) == IR_DETECTED;
}

bool isExitDetected() {
  return digitalRead(EXIT_IR_PIN) == IR_DETECTED;
}

int getTotalSlots() {
  return SLOT2_INSTALLED ? 2 : 1;
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

// ================= LCD =================
void showMessage(String line1, String line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

void updateLCDRealtime() {
  lcd.setCursor(0, 0);
  lcd.print("Avail: ");
  lcd.print(getAvailableSlots());
  lcd.print("/");
  lcd.print(getTotalSlots());
  lcd.print("   ");

  lcd.setCursor(0, 1);
  lcd.print("Tap entry card ");
}

// ================= URL ENCODER =================
String urlEncode(String str) {
  str.replace(" ", "%20");
  str.replace("#", "%23");
  str.replace("&", "%26");
  str.replace("@", "%40");
  return str;
}

// ================= GOOGLE SHEETS LOGGER =================
void sendLogToSheet(String uid, String name, String vehicle, String email, String action) {
  if (WiFi.status() != WL_CONNECTED) return;

  std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
  client->setInsecure();

  HTTPClient https;

  String url = GOOGLE_SCRIPT_URL;
  url += "?uid=" + urlEncode(uid);
  url += "&name=" + urlEncode(name);
  url += "&vehicle=" + urlEncode(vehicle);
  url += "&action=" + urlEncode(action);
  url += "&email=" + urlEncode(email);

  if (https.begin(*client, url)) {
    https.GET();
    https.end();
  }
}

// ================= GATE =================
void openGate() {
  Blynk.virtualWrite(V3, "Open");

  gateServo.write(GATE_OPEN_ANGLE);
  delay(1500);

  gateServo.write(GATE_CLOSED_ANGLE);
  delay(500);

  Blynk.virtualWrite(V3, "Closed");
}

// ================= BLYNK =================
void updateBlynk() {
  Blynk.virtualWrite(V0, getOccupiedSlots());
  Blynk.virtualWrite(V1, getAvailableSlots());
  Blynk.virtualWrite(V2, getTotalSlots());
}

BLYNK_WRITE(V4) {
  int value = param.asInt();

  if (value == 1) {
    showMessage("Manual Open", "Blynk Control");

    openGate();

    sendLogToSheet(
      "MANUAL",
      "Admin Override",
      "NA",
      "",
      "MANUAL GATE OPEN"
    );

    delay(1000);
    Blynk.virtualWrite(V4, 0);
  }
}

// ================= RFID ENTRY =================
void checkRFIDEntry() {
  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  if (millis() - lastRFIDTime < RFID_COOLDOWN) {
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    return;
  }

  lastRFIDTime = millis();

  String scannedUID = uidToString(rfid.uid.uidByte, rfid.uid.size);
  int userIndex = getUserIndex(rfid.uid.uidByte, rfid.uid.size);

  if (userIndex == -1) {
    showMessage("Access Denied", "Unknown Card");

    sendLogToSheet(
      scannedUID,
      "Unknown",
      "Unknown",
      "",
      "ACCESS DENIED"
    );

    delay(1500);
  } else {
    String name = users[userIndex].name;
    String vehicle = users[userIndex].vehicle;
    String email = users[userIndex].email;

    if (!isAnySlotAvailable()) {
      showMessage("Valid RFID", "Parking Full");

      sendLogToSheet(
        scannedUID,
        name,
        vehicle,
        email,
        "PARKING FULL"
      );

      delay(1500);
    } else {
      showMessage("Valid RFID", "Gate Opening");

      lastEntryName = name;
      lastEntryVehicle = vehicle;
      lastEntryUID = scannedUID;
      lastEntryEmail = email;

      openGate();

      sendLogToSheet(
        scannedUID,
        name,
        vehicle,
        email,
        "ENTRY"
      );

      showMessage("Vehicle Entered", name);
      delay(1000);
    }
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

// ================= EXIT SENSOR =================
void checkExitSensor() {
  if (millis() - lastExitTriggerTime < EXIT_COOLDOWN) return;

  if (isExitDetected()) {
    lastExitTriggerTime = millis();

    showMessage("Exit Detected", "Gate Opening");

    openGate();

    sendLogToSheet(
      lastEntryUID,
      lastEntryName,
      lastEntryVehicle,
      lastEntryEmail,
      "EXIT"
    );

    showMessage("Thank You", "");
    delay(1000);
  }
}

// ================= SETUP =================
void setup() {
  WiFi.begin(ssid, pass);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  SPI.begin();
  rfid.PCD_Init();

  pinMode(SLOT1_IR_PIN, INPUT);
  pinMode(EXIT_IR_PIN, INPUT);

  if (SLOT2_INSTALLED) {
    pinMode(SLOT2_IR_PIN, INPUT);
  }

  gateServo.attach(SERVO_PIN);
  gateServo.write(GATE_CLOSED_ANGLE);

  Wire.begin(4, 5);
  lcd.init();
  lcd.backlight();

  showMessage("SmartPark RFID", "Starting...");
  delay(1500);
  lcd.clear();

  Blynk.virtualWrite(V3, "Closed");

  timer.setInterval(1000L, updateBlynk);
}

// ================= LOOP =================
void loop() {
  Blynk.run();
  timer.run();

  updateLCDRealtime();
  checkRFIDEntry();
  checkExitSensor();
}