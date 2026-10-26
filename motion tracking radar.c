#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiS3.h> 

// --- 1. ตั้งค่า Wi-Fi ---
const char* ssid = "Aldnii";
const char* password = "0937719597n";

// --- 2. ตั้งค่า Discord Bot (Direct Message) ---
const char* botToken = "MTU2NDE2MTQ5NDc0MzEyNjA3Ng.GpxYO7.YUoyDnm_zzXStbel0RPzldoSf4MvHWXLbHuuF4";       
const char* dmChannelId = "1564163123085316187"; 

// --- 3. ตั้งค่า Google Sheets Web App ---
const char* sheetsHost = "script.google.com";
const char* sheetsPath = "/macros/s/AKfycbyGgKrs735scxnvegCuG1x0A0hHVYHL4maWfQ7a-j57Yr627lk06mLY5tPf7EscDzKQLA/exec";

// --- 4. ตั้งค่าขาอุปกรณ์ ---
const int trigPin = 11;
const int echoPin = 10;
const int redLed = 3;
const int greenLed = 4;
const int buzzer = 2;

LiquidCrystal_I2C lcd(0x27, 16, 2);
WiFiSSLClient client;

// --- 5. ตัวแปรระบบ ---
const int alertDistance = 150;           
const unsigned long checkInterval = 50; 
unsigned long lastCheckTime = 0;

unsigned long lastDiscordAlertTime = 0;
const unsigned long alertCooldown = 2000; 

unsigned long lastBlinkTime = 0;
bool blinkState = false;

enum RadarState { EMPTY, WARNING };
RadarState lastState = EMPTY;

// --- ตัวแปรสถานะ Wi-Fi ---
bool wifiConnected = false;
unsigned long lastWifiCheck = 0;
const unsigned long reconnectInterval = 5000; // พยายามเชื่อมต่อใหม่ทุกๆ 5 วินาที

// --- ฟังก์ชันบันทึกลง Google Sheets ---
void logToGoogleSheets(float distance) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiSSLClient sheetsClient;
    sheetsClient.setTimeout(2000); 
    
    if (sheetsClient.connect(sheetsHost, 443)) {
      String payload = "{\"distance\":\"" + String(distance, 1) + "\", \"status\":\"Detected\"}";
      
      sheetsClient.println(String("POST ") + sheetsPath + " HTTP/1.1");
      sheetsClient.println(String("Host: ") + sheetsHost);
      sheetsClient.println("Content-Type: application/json");
      sheetsClient.print("Content-Length: ");
      sheetsClient.println(payload.length());
      sheetsClient.println("Connection: close");
      sheetsClient.println();
      sheetsClient.println(payload);
      
      sheetsClient.stop();
      Serial.println("Logged to Google Sheets!");
    }
  }
}

// --- ฟังก์ชันส่ง Direct Message ---
void sendDiscordDM(String message) {
  if (WiFi.status() == WL_CONNECTED) {
    client.setTimeout(2000);
    
    if (client.connect("discord.com", 443)) {
      String payload = "{\"content\":\"" + message + "\"}";
      String discordPath = "/api/v10/channels/" + String(dmChannelId) + "/messages";
      
      client.println("POST " + discordPath + " HTTP/1.1");
      client.println("Host: discord.com");
      client.println("Authorization: Bot " + String(botToken)); 
      client.println("Content-Type: application/json");
      client.print("Content-Length: ");
      client.println(payload.length());
      client.println("Connection: close");
      client.println(); 
      client.println(payload); 
      
      client.stop(); 
      Serial.println("Discord DM Sent!");
    }
  }
}

float getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000); 
  if (duration == 0) return 999.0;
  return duration * 0.034 / 2.0;
}

void setNormalState() {
  digitalWrite(greenLed, HIGH);
  digitalWrite(redLed, LOW);
  digitalWrite(buzzer, LOW);
}

void lcdSetEmpty() {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Area is Empty");
  lcd.setCursor(0, 1);
  lcd.print("                ");
}

void lcdSetWarning() {
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("WARNING!!");
  lcd.setCursor(2, 1);
  lcd.print("Foreign Body");
}

void setup() {
  Serial.begin(115200);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(buzzer, OUTPUT);

  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi");
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  wifiConnected = true;
  Serial.println("\nWiFi Connected!");
  lcdSetEmpty();
  setNormalState();
}

void loop() {
  unsigned long currentMillis = millis();

  // ---------------------------------------------------------
  // 1. ระบบเช็คและ Reconnect Wi-Fi
  // ---------------------------------------------------------
  if (WiFi.status() != WL_CONNECTED) {
    
    // ทำงานครั้งแรกที่ตรวจพบว่าเน็ตหลุด
    if (wifiConnected) {
      wifiConnected = false;
      Serial.println("⚠️ Alert: Wi-Fi Disconnected! Stopping radar...");
      
      // ปิดระบบตรวจจับทั้งหมด (ไฟและเสียง)
      digitalWrite(greenLed, LOW);
      digitalWrite(redLed, LOW);
      digitalWrite(buzzer, LOW);
      
      // แสดงข้อความกำลังเชื่อมต่อใหม่
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Wi-Fi Lost!");
      lcd.setCursor(0, 1);
      lcd.print("Reconnecting...");
    }

    // พยายามเชื่อมต่อใหม่ทุกๆ 5 วินาที
    if (currentMillis - lastWifiCheck >= reconnectInterval) {
      lastWifiCheck = currentMillis;
      Serial.println("Attempting to reconnect Wi-Fi...");
      WiFi.begin(ssid, password); // สั่งเชื่อมต่อใหม่
    }
    
    return; // สำคัญ: ออกจาก loop() ไปเลย ไม่ต้องรันคำสั่งเซนเซอร์ด้านล่างจนกว่าเน็ตจะมา
  }

  // หากเพิ่งกลับมาเชื่อมต่อ Wi-Fi สำเร็จ
  if (!wifiConnected) {
    wifiConnected = true;
    Serial.println("✅ Wi-Fi Reconnected! Resuming radar...");
    
    // รีเซ็ตหน้าจอและเปิดไฟสถานะการทำงานปกติกลับมา
    lastState = EMPTY;
    lcdSetEmpty();
    setNormalState();
  }

  // ---------------------------------------------------------
  // 2. การทำงานของ Radar (จะทำงานเฉพาะตอนมี Wi-Fi เท่านั้น)
  // ---------------------------------------------------------
  if (currentMillis - lastCheckTime >= checkInterval) {
    lastCheckTime = currentMillis;
    float distance = getDistance();

    if (distance > 0 && distance < alertDistance) {
      
      if (lastState != WARNING) {
        lastState = WARNING;
        lcdSetWarning();
        digitalWrite(greenLed, LOW);
      }

      // ล็อกดาวน์เรื่องการส่งสัญญาณเตือน
      if (currentMillis - lastDiscordAlertTime >= alertCooldown) {
        String alertText = "🚨 **แจ้งเตือนความปลอดภัย:** ตรวจพบวัตถุในระยะ **" + String(distance, 1) + " cm**!";
        
        logToGoogleSheets(distance);
        sendDiscordDM(alertText);
        
        lastDiscordAlertTime = millis();
      }

      // ไฟแดงและเสียงกะพริบแจ้งเตือนบุกรุก
      if (currentMillis - lastBlinkTime >= 100) {
        blinkState = !blinkState;
        digitalWrite(redLed, blinkState);
        digitalWrite(buzzer, blinkState);
        lastBlinkTime = currentMillis;
      }

    } 
    else {
      if (lastState != EMPTY) {
        lastState = EMPTY;
        lcdSetEmpty();
        setNormalState();
      }
    }
  }
}
