/*
  Production Line Counter
  -----------------------
  Hardware:
    - IR beam-break sensor  → D2 (with interrupt)
    - Reset pushbutton      → D3 (with interrupt)
    - 16x2 LCD (I2C, 0x27) → SDA (A4), SCL (A5)

  Wiring:
    IR sensor  VCC → 5V | GND → GND | OUT → D2
    Button     one leg → D3, other leg → GND  (internal pull-up used)
    LCD I2C    VCC → 5V | GND → GND | SDA → A4 | SCL → A5
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ── Pin definitions ──────────────────────────────────────────────────────────
const uint8_t IR_PIN     = 2;   // INT0 – falling edge = item detected
const uint8_t BTN_PIN    = 3;   // INT1 – falling edge = reset

// ── LCD (address 0x27, 16 columns, 2 rows) ───────────────────────────────────
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ── Counters (volatile because changed in ISR) ───────────────────────────────
volatile unsigned long totalCount = 0;
volatile bool          resetFlag  = false;

// ── Rate calculation ─────────────────────────────────────────────────────────
unsigned long lastRateTime  = 0;
unsigned long lastRateCount = 0;
float         itemsPerMin   = 0.0;

const unsigned long RATE_INTERVAL_MS = 5000;  // recalculate every 5 s

// ── Debounce ─────────────────────────────────────────────────────────────────
volatile unsigned long lastIRTime  = 0;
const unsigned long    DEBOUNCE_MS = 50;       // ignore pulses faster than 50 ms

// ─────────────────────────────────────────────────────────────────────────────
void setup() {
  pinMode(IR_PIN,  INPUT_PULLUP);
  pinMode(BTN_PIN, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("  Line Counter  ");
  lcd.setCursor(0, 1);
  lcd.print("  Starting...   ");
  delay(1500);
  lcd.clear();

  // Attach interrupts
  attachInterrupt(digitalPinToInterrupt(IR_PIN),  onItemDetected, FALLING);
  attachInterrupt(digitalPinToInterrupt(BTN_PIN), onResetPressed, FALLING);

  lastRateTime = millis();
  Serial.begin(9600);
  Serial.println("Production Line Counter ready.");
}

// ─────────────────────────────────────────────────────────────────────────────
void loop() {
  // Handle reset (set by ISR, executed safely in main loop)
  if (resetFlag) {
    resetFlag    = false;
    totalCount   = 0;
    itemsPerMin  = 0.0;
    lastRateTime  = millis();
    lastRateCount = 0;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Counter reset!  ");
    delay(800);
    lcd.clear();
    Serial.println("Counter reset.");
  }

  // Recalculate items per minute every RATE_INTERVAL_MS
  unsigned long now = millis();
  if (now - lastRateTime >= RATE_INTERVAL_MS) {
    unsigned long delta = totalCount - lastRateCount;
    itemsPerMin   = (float)delta * (60000.0f / RATE_INTERVAL_MS);
    lastRateCount = totalCount;
    lastRateTime  = now;
  }

  // ── Update LCD ──────────────────────────────────────────────────────────────
  // Row 0: "Count: 000042"
  lcd.setCursor(0, 0);
  lcd.print("Count: ");
  char buf[10];
  sprintf(buf, "%06lu", totalCount);  // zero-padded 6-digit count
  lcd.print(buf);
  lcd.print("  ");

  // Row 1: "Rate:  12.3/min"
  lcd.setCursor(0, 1);
  lcd.print("Rate:  ");
  dtostrf(itemsPerMin, 5, 1, buf);   // 5 chars, 1 decimal place
  lcd.print(buf);
  lcd.print("/min ");

  // ── Serial monitor output ───────────────────────────────────────────────────
  Serial.print("Count=");
  Serial.print(totalCount);
  Serial.print("  Rate=");
  Serial.print(itemsPerMin, 1);
  Serial.println("/min");

  delay(200);  // refresh at ~5 Hz
}

// ─────────────────────────────────────────────────────────────────────────────
// ISR: called on falling edge of IR sensor output (beam broken = item passed)
void onItemDetected() {
  unsigned long t = millis();
  if (t - lastIRTime > DEBOUNCE_MS) {
    totalCount++;
    lastIRTime = t;
  }
}

// ISR: called when reset button is pressed
void onResetPressed() {
  resetFlag = true;
}
