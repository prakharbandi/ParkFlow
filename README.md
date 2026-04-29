# 🚗 SmartPark RFID — IoT-Based Parking Access & Monitoring System

SmartPark RFID is an IoT-enabled parking entry system built using ESP8266 (NodeMCU), RFID authentication, IR sensors, servo gate automation, Blynk dashboard monitoring, Google Sheets logging, and automated email alerts.

---

## 📌 Features

- RFID-based vehicle authentication
- Automatic gate control using servo motor
- Real-time parking slot availability detection
- Blynk IoT dashboard monitoring
- Manual gate override via mobile app
- Google Sheets entry logging
- Automated email notifications
- LCD display for local parking status
- Access denied detection for unauthorized RFID cards

---

## 🧠 System Architecture

RFID Card Scan  
↓  
ESP8266 NodeMCU  
↓  
Check Slot Availability  
↓  
Servo Gate Control  
↓  
Blynk Dashboard Update  
↓  
Google Sheets Log Entry  
↓  
Email Notification Sent  

---

## 🛠️ Hardware Components

| Component | Quantity |
|----------|----------|
| NodeMCU ESP8266 | 1 |
| RC522 RFID Module | 1 |
| IR Sensors | 2 |
| Servo Motor | 1 |
| I2C LCD Display | 1 |
| Jumper Wires | As required |
| External 5V Supply | 1 |

---

## 🔌 Hardware Connections

### NodeMCU ESP8266 Pin Mapping

| Component | Module Pin | NodeMCU Pin | GPIO |
|-----------|------------|-------------|------|
| RC522 SDA | SDA | D4 | GPIO2 |
| RC522 RST | RST | D3 | GPIO0 |
| RC522 SCK | SCK | D5 | GPIO14 |
| RC522 MOSI | MOSI | D7 | GPIO13 |
| RC522 MISO | MISO | D6 | GPIO12 |
| LCD SDA | SDA | D2 | GPIO4 |
| LCD SCL | SCL | D1 | GPIO5 |
| Slot IR Sensor | OUT | D0 | GPIO16 |
| Exit IR Sensor | OUT | RX | GPIO3 |
| Servo Motor | Signal | D8 | GPIO15 |

⚠️ Servo motor must be powered using an external 5V supply. Connect GND with NodeMCU GND.

---

## 📱 Blynk Dashboard Setup

| Virtual Pin | Widget | Purpose |
|-------------|--------|---------|
| V0 | Value Display | Cars Parked |
| V1 | Value Display | Slots Available |
| V2 | Value Display | Total Slots |
| V3 | Label | Gate Status |
| V4 | Button | Manual Gate Control |

---

## ☁️ Google Sheets Logging

Each entry event is stored automatically in Google Sheets:

Date | Time | UID | Name | Vehicle No | Action

Example:

12-Apr-2026 | 08:14 PM | AB CD EF 12 | Prakhar Jain | AB1234 | ENTRY

---

## 📧 Email Alert System

Whenever an event occurs:

- ENTRY
- EXIT
- ACCESS DENIED
- PARKING FULL
- MANUAL GATE OPEN

An automated email notification is sent to the administrator and vehicle owner.

---

## ⚙️ Software Stack

- ESP8266 Arduino Core
- MFRC522 RFID Library
- Servo Library
- LiquidCrystal_I2C Library
- Blynk IoT Platform
- Google Apps Script Webhook
- Google Sheets Cloud Logging

---

## 🚀 Installation Guide

### Step 1 — Install Libraries

Install from Arduino Library Manager:

- MFRC522
- Servo
- LiquidCrystal_I2C
- Blynk
- ESP8266WiFi
- ESP8266HTTPClient

---

### Step 2 — Configure Credentials

Replace placeholders inside code:

```
YOUR_WIFI_NAME
YOUR_WIFI_PASSWORD
YOUR_BLYNK_AUTH_TOKEN
YOUR_GOOGLE_SCRIPT_WEB_APP_URL
```

---

### Step 3 — Upload Code

Select board:

```
NodeMCU 1.0 (ESP8266) (32 PIN)
```

Upload firmware.

---

## 🔐 Security Note

Sensitive credentials are not included in this repository.

Before running project:

- Create your own Blynk Auth Token
- Deploy your own Apps Script Web App
- Create your own Google Sheet

---

## 📊 Future Improvements

- Multi-slot expansion support
- Firebase database integration
- Web admin dashboard
- Automatic RFID registration
- License plate recognition integration

---

## 👨‍💻 Author

Prakhar Bandi Jain  
B.Tech Electronics & Communication Engineering  
IET DAVV, Indore

---

⭐ If you found this useful, consider starring the repository.
