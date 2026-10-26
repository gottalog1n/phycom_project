#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Ultrasonic sensor settings
const int trigPin = 11;
const int echoPin = 10;

// LED and buzzer (active buzzer controlled via digitalWrite)
const int redLed = 3;
const int greenLed = 4;
const int buzzer = 2;

// Initialize the LCD (16x2). Check the I2C address (commonly 0x27 or 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Settings
const int alertDistance = 150;           // Alarm triggers if distance is less than 150cm
const unsigned long checkInterval = 60;  // Measurement interval (ms)
unsigned long lastCheckTime = 0;

// Variable to count consecutive detections under threshold
int triggerCount = 0;

// Enum to record state (for updating LCD only when state changes)
enum RadarState { EMPTY, WARNING };
RadarState lastState = EMPTY;

void setup() {
  // Initialize ultrasonic sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Initialize LED and buzzer pins
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(buzzer, OUTPUT);

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();

  // Set initial status: LCD shows "Area is Empty", green LED on
  lcdSetEmpty();
  setNormalState();
}

void loop() {
  unsigned long currentMillis = millis();

  // Perform distance measurement at regular intervals
  if (currentMillis - lastCheckTime >= checkInterval) {
    lastCheckTime = currentMillis;

    float distance = getDistance();

    if (distance > 0 && distance < alertDistance) {
      triggerCount++;  // Count consecutive detections under threshold
    } else {
      triggerCount = 0; // Reset counter if measurement is safe
    }

    // Trigger alarm only if detected 5 times consecutively
    if (triggerCount >= 5) {
      if (lastState != WARNING) {
        lastState = WARNING;
        lcdSetWarning();
      }
      alertMode();
      triggerCount = 0; // Reset counter after alarm mode
    } else {
      if (lastState != EMPTY) {
        lastState = EMPTY;
        lcdSetEmpty();
        setNormalState();
      }
    }
  }
}

// Measure distance using the ultrasonic sensor (returns distance in cm)
float getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);  // Wait for a maximum of 30ms
  if (duration == 0) return 999.0;                // Timeout or out of range
  float distance = duration * 0.034 / 2;          // Calculate distance (cm)
  return distance;
}

// Alarm mode: if an object is detected, continuously alarm until cleared
void alertMode() {
  // Turn off green LED while in alarm mode
  digitalWrite(greenLed, LOW);

  // Keep alarming until the object is removed
  while (getDistance() < alertDistance) {
    // Flash red LED and activate buzzer
    digitalWrite(redLed, HIGH);
    digitalWrite(buzzer, HIGH);
    delay(100);
    digitalWrite(redLed, LOW);
    digitalWrite(buzzer, LOW);
    delay(100);
  }

  // When safe, exit alarm mode and update status
  lcdSetEmpty();
  setNormalState();
  lastState = EMPTY;
}

// Set normal status: green LED on, red LED and buzzer off
void setNormalState() {
  digitalWrite(greenLed, HIGH);
  digitalWrite(redLed, LOW);
  digitalWrite(buzzer, LOW);
}

// LCD display "Area is Empty" centered on the first line
void lcdSetEmpty() {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Area is Empty");
  lcd.setCursor(0, 1);
  lcd.print("                ");
}

// LCD display warning message: "WARNING!!" and "Foreign Body"
void lcdSetWarning() {
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("WARNING!!");
  lcd.setCursor(2, 1);
  lcd.print("Foreign Body");
}