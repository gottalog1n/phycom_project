# 📡 Motion Tracking Radar 

**Physical Computing Project 2026 — School of Information Technology, KMITL**
**บทคัดย่อ**

ระบบเฝ้าระวังและตรวจจับสิ่งแปลกปลอมในพื้นที่ระยะไกล (Motion Tracking Radar) สร้างขึ้นเพื่อแก้ปัญหาการพลาดการรับรู้ต่อสภาพแวดล้อมขณะที่ผู้ใช้งานไม่อยู่ในบริเวณนั้น โดยระบบจะตรวจจับวัตถุ แจ้งเตือนหน้างาน และส่งข้อมูลแจ้งเตือนออนไลน์แบบเรียลไทม์

---

## 🔗 ลิงก์ที่เกี่ยวข้อง (Project Links)
- **🌐 Live Dashboard:** [ดูหน้าเว็บไซต์แสดงผลแบบเรียลไทม์](https://gottalog1n.github.io/phycom_project/)
- **🎬 YouTube Demo:** [ดูคลิปวิดีโอสาธิตการทำงาน](YOUR_YOUTUBE_LINK)
- **⚡ Tinkercad:** [ดูแผนผังวงจรและการจำลองบน Tinkercad](https://www.tinkercad.com/things/2wPerupvwLj-motion-tracking-radar?sharecode=Cfw9TqBLlTuAfajSBgSiyv99cDoRF2JYGbCNHqPSH_I)

---

## ✨ ฟีเจอร์หลัก (Key Features)
- **Smart Detection:** ตรวจจับวัตถุในระยะ < 150 ซม. โดยมีการนับซ้ำเพื่อยืนยัน (Consecutive Trigger Filter) ป้องกันการแจ้งเตือนผิดพลาด (False Alarm)
- **Local Alerts:** แจ้งเตือนผู้บุกรุกผ่านเสียง Buzzer, ไฟ LED สีแดงกะพริบ และหน้าจอ LCD 
- **IoT Notification:** ส่งข้อความแจ้งเตือนอัตโนมัติเข้า **Discord** ทันทีที่พบผู้บุกรุก
- **Data Logging:** บันทึกประวัติการตรวจจับ (ระยะห่าง, วันเวลา, สถานะ) ลงใน **Google Sheets** โดยอัตโนมัติ
- **Auto-Reconnect:** หากเชื่อมต่อ Wi-Fi ไม่สำเร็จหรือเน็ตหลุด ระบบจะระงับการแจ้งเตือนออนไลน์ชั่วคราว และพยายามเชื่อมต่อใหม่ให้อัตโนมัติ (ระบบหน้างานยังทำงานปกติ)
- **Real-time Dashboard:** มีหน้าเว็บ HTML ที่ใช้ JSONP ดึงข้อมูลจาก Google Sheets มาแสดงผลแบบสดๆ ทุก 5 วินาที (Bypass CORS)

---

## Poster
[View Motion Tracking Radar PDF](<Motion Tracking Radar.pdf>)
![Motion Tracking Radar](<Motion Tracking Radar.png>)

## 🛠 ฮาร์ดแวร์และการต่อวงจร (Hardware & Pinout)

| อุปกรณ์ (Component) | พินที่เชื่อมต่อ (Pin Assignment) | หน้าที่การทำงาน |
| :--- | :--- | :--- |
| **Ultrasonic Sensor (HC-SR04)** | Trig: `Pin 11`, Echo: `Pin 10` | ตรวจจับและวัดระยะห่างวัตถุ |
| **LiquidCrystal I2C (16x2)** | `SDA`, `SCL` (Address `0x27`) | แสดงสถานะพื้นที่ Empty / Warning และสถานะ Wi-Fi |
| **Active Buzzer** | `Pin 2` | ส่งสัญญาณเสียงเตือนภัย |
| **Red LED** | `Pin 3` | ไฟกะพริบเตือนภัย |
| **Green LED** | `Pin 4` | ไฟสถานะสแตนด์บาย (พื้นที่ปลอดภัย) |

*(หมายเหตุ: ใช้บอร์ดไมโครคอนโทรลเลอร์ที่มีโมดูล Wi-Fi เช่น Arduino UNO R4 WiFi / WiFiS3)*

---

## 💻 การติดตั้งและใช้งาน (Setup & Installation)

### 1. ไลบรารีที่ต้องใช้ (Dependencies)
ตรวจสอบให้แน่ใจว่าติดตั้งไลบรารีเหล่านี้ใน Arduino IDE แล้ว:
- `Wire.h` (มีในตัว)
- `LiquidCrystal_I2C` (สำหรับจอ LCD)
- `WiFiS3` (สำหรับการเชื่อมต่อเครือข่าย)

### 2. การตั้งค่าตัวแปรในโค้ด (Configuration)
ก่อนอัปโหลดโค้ดลงบอร์ด ให้แก้ไขตัวแปรส่วนหัวในไฟล์ `.ino` ดังนี้:

```cpp
// 1. ตั้งค่า Wi-Fi ของคุณ
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// 2. ตั้งค่า Discord Bot (Bot Token และ Channel ID)
const char* botToken = "YOUR_DISCORD_BOT_TOKEN";       
const char* dmChannelId = "YOUR_CHANNEL_ID"; 

// 3. ตั้งค่า Google Sheets (URL จาก Google Apps Script)
const char* sheetsPath = "/macros/s/YOUR_SCRIPT_ID/exec";
