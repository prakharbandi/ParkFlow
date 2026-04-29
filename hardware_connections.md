# Hardware Connections

## NodeMCU ESP8266 Pin Mapping

| Component | Module Pin | NodeMCU Pin | GPIO |
|-----------|------------|-------------|------|
| RC522 RFID | SDA | D4 | GPIO2 |
| RC522 RFID | RST | D3 | GPIO0 |
| RC522 RFID | SCK | D5 | GPIO14 |
| RC522 RFID | MOSI | D7 | GPIO13 |
| RC522 RFID | MISO | D6 | GPIO12 |
| LCD | SDA | D2 | GPIO4 |
| LCD | SCL | D1 | GPIO5 |
| Slot IR Sensor | OUT | D0 | GPIO16 |
| Exit IR Sensor | OUT | RX | GPIO3 |
| Servo Motor | Signal | D8 | GPIO15 |

## Important Notes

- Power RC522 using 3.3V only
- Servo requires external 5V supply
- Connect external supply GND with NodeMCU GND
