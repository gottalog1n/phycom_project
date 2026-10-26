# 🚨 Real-Time Ultrasonic Security Radar (with Discord Alerts)

![Arduino](https://img.shields.io/badge/-Arduino-00979D?style=for-the-badge&logo=Arduino&logoColor=white)
![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Discord API](https://img.shields.io/badge/Discord%20Webhook-5865F2?style=for-the-badge&logo=discord&logoColor=white)

A high-performance, real-time security system built on the **Arduino Uno R4 WiFi**. It uses an HC-SR04 ultrasonic sensor to detect intruders and instantly sends a notification to a Discord server via Webhooks, without blocking the main processor loop.

> **🎥 [Insert a link to a GIF or YouTube video showing the radar working here]**

---

## ✨ Features

- **⚡ Real-Time Discord Alerts:** Sends instant HTTP POST requests to a Discord Webhook.
- **⏱️ Non-Blocking Architecture:** Uses `millis()` instead of `delay()` to ensure the siren, LEDs, and network requests run simultaneously without freezing the board.
- **📟 I2C LCD Integration:** Real-time status display ("Area is Empty" / "WARNING!!").
- **🔊 Visual & Audio Alarms:** Triggers a red LED and an active buzzer instantly upon detection.
- **🛡️ Rate-Limit Protection:** Built-in cooldown timer prevents Discord API bans (Error 429).

---

## 🛠️ Hardware Requirements

- **Arduino Uno R4 WiFi** (or ESP32/ESP8266 with minor code adjustments)
- **HC-SR04** Ultrasonic Distance Sensor
- **16x2 LCD Display** (with I2C Backpack)
- **1x Red LED** + 220Ω Resistor
- **1x Green LED** + 220Ω Resistor
- **1x Active Buzzer**
- Breadboard & Jumper Wires

---

## 🔌 Wiring Diagram

| Component | Pin on Component | Pin on Uno R4 WiFi |
| :--- | :--- | :--- |
| **HC-SR04** | VCC | 5V |
| | GND | GND |
| | Trig | Pin 11 |
| | Echo | Pin 10 |
| **I2C LCD** | VCC | 5V |
| | GND | GND |
| | SDA | SDA (A4) |
| | SCL | SCL (A5) |
| **Red LED** | Anode (+) | Pin 3 |
| **Green LED** | Anode (+) | Pin 4 |
| **Buzzer** | Positive (+) | Pin 2 |

---

## 🚀 Installation & Setup

### 1. Arduino IDE Setup
1. Install the **Arduino UNO R4 Boards** package from the Boards Manager.
2. Go to **Sketch > Include Library > Manage Libraries** and install:
   - `LiquidCrystal_I2C` (by Frank de Brabander)

### 2. Discord Webhook Setup
1. Go to your Discord Server Settings > Integrations > Webhooks.
2. Click **New Webhook**, name it, and copy the **Webhook URL**.
3. *Note:* The URL looks like this: `https://discord.com/api/webhooks/YOUR_ID/YOUR_TOKEN`.

### 3. Code Configuration
Open the `.ino` file and update the following lines with your credentials:

```cpp
// --- 1. Wi-Fi Settings ---
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// --- 2. Discord Webhook Settings ---
const char* discordHost = "discord.com";
const char* discordPath = "/api/webhooks/YOUR_ID/YOUR_TOKEN"; // Remove "[https://discord.com](https://discord.com)"
