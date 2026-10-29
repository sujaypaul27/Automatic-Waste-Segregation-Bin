/**
 * ============================================================================
 * Project: Automatic Waste Segregation Bin (Smart Dustbin)
 * File: automatic_waste_segregator.ino
 * Author: Sujay Paul
 * GitHub: https://github.com/sujaypaul27/Automatic-Waste-Segregation-Bin
 * Target Microcontroller: Arduino Uno / Nano (ATmega328P)
 * Description:
 *   An automated, sensor-driven waste segregation system that classifies
 *   incoming waste into three distinct categories:
 *     1. Metal Waste   (Inductive Proximity Sensor)
 *     2. Wet Waste     (Moisture / Soil Resistivity Sensor)
 *     3. Dry Waste     (IR Sensor Object Trigger + Absence of Metal & Moisture)
 *
 *   Actuation is handled by two servo motors:
 *     - Selector Servo: Rotates the discharge chute or carousel to target bin
 *     - Drop Flap Servo: Opens the trapdoor to release waste into the bin
 * ============================================================================
 */

#include <Servo.h>
#include "config.h"

#if ENABLE_LCD_I2C
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS, 16, 2);
#endif

// ==========================================
// ENUMS & STATE DEFINITIONS
// ==========================================
enum WasteType {
  WASTE_NONE = 0,
  WASTE_METAL,
  WASTE_WET,
  WASTE_DRY
};

enum SystemState {
  STATE_STANDBY = 0,
  STATE_WASTE_DETECTED,
  STATE_SAMPLING,
  STATE_CLASSIFYING,
  STATE_DISCHARGING,
  STATE_RESETTING
};

// ==========================================
// GLOBAL HARDWARE INSTANCES & VARIABLES
// ==========================================
Servo servoSelector;
Servo servoDropFlap;

SystemState currentState = STATE_STANDBY;
WasteType detectedWaste = WASTE_NONE;

int currentSelectorAngle = ANGLE_SELECTOR_NEUTRAL;
int currentFlapAngle = ANGLE_FLAP_CLOSED;

// ==========================================
// FUNCTION PROTOTYPES
// ==========================================
void initHardware();
void updateStatusLEDs(WasteType type, bool isBusy);
void beepBuzzer(int count, int durationMs, int pauseMs);
void moveServoSmooth(Servo &s, int &currentAngle, int targetAngle, int stepDelay);
int readMoistureAveraged(int samples);
WasteType classifyWaste();
void routeAndDropWaste(WasteType type);
void printBanner();
void updateDisplay(const char* line1, const char* line2);

// ==========================================
// ARDUINO SETUP
// ==========================================
void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000); // Wait for serial console on boards that require it

  printBanner();
  initHardware();

  #if ENABLE_LCD_I2C
  lcd.init();
  lcd.backlight();
  updateDisplay("SMART WASTE BIN", "SYSTEM READY");
  delay(1500);
  #endif

  // System ready indication
  beepBuzzer(2, 80, 80);
  Serial.println(F("[SYSTEM] Initialization complete. Standing by for waste..."));
  Serial.println(F("--------------------------------------------------"));
}

// ==========================================
// MAIN LOOP (FINITE STATE MACHINE)
// ==========================================
void loop() {
  switch (currentState) {

    // ----------------------------------------------------
    // STATE 1: STANDBY - Wait for waste in the entry funnel
    // ----------------------------------------------------
    case STATE_STANDBY: {
      updateStatusLEDs(WASTE_NONE, false);

      // Check if IR proximity sensor detects an object
      int irState = digitalRead(PIN_IR_SENSOR);
      if (irState == IR_OBJECT_DETECTED) {
        Serial.println(F("\n[EVENT] Waste item detected in entry hopper!"));
        beepBuzzer(1, 100, 50);

        #if ENABLE_LCD_I2C
        updateDisplay("ITEM DETECTED", "SETTLING...");
        #endif

        currentState = STATE_WASTE_DETECTED;
      }
      break;
    }

    // ----------------------------------------------------
    // STATE 2: WASTE DETECTED - Allow item to settle
    // ----------------------------------------------------
    case STATE_WASTE_DETECTED: {
      updateStatusLEDs(WASTE_NONE, true);
      Serial.print(F("[INFO] Waiting for item to settle on sensor bed ("));
      Serial.print(SENSING_SETTLE_DELAY_MS);
      Serial.println(F(" ms)..."));

      delay(SENSING_SETTLE_DELAY_MS);
      currentState = STATE_SAMPLING;
      break;
    }

    // ----------------------------------------------------
    // STATE 3: SAMPLING & CLASSIFICATION
    // ----------------------------------------------------
    case STATE_SAMPLING: {
      #if ENABLE_LCD_I2C
      updateDisplay("ANALYZING...", "READING SENSORS");
      #endif

      detectedWaste = classifyWaste();
      currentState = STATE_CLASSIFYING;
      break;
    }

    // ----------------------------------------------------
    // STATE 4: CLASSIFYING - Announce result & set indicators
    // ----------------------------------------------------
    case STATE_CLASSIFYING: {
      updateStatusLEDs(detectedWaste, true);

      switch (detectedWaste) {
        case WASTE_METAL:
          Serial.println(F("[CLASSIFICATION] Result: >>> METAL WASTE <<<"));
          #if ENABLE_LCD_I2C
          updateDisplay("TYPE: METAL", "DIVERTING...");
          #endif
          beepBuzzer(3, 120, 80);
          break;

        case WASTE_WET:
          Serial.println(F("[CLASSIFICATION] Result: >>> WET / ORGANIC WASTE <<<"));
          #if ENABLE_LCD_I2C
          updateDisplay("TYPE: WET WASTE", "DIVERTING...");
          #endif
          beepBuzzer(2, 120, 80);
          break;

        case WASTE_DRY:
          Serial.println(F("[CLASSIFICATION] Result: >>> DRY / RECYCLABLE WASTE <<<"));
          #if ENABLE_LCD_I2C
          updateDisplay("TYPE: DRY WASTE", "DIVERTING...");
          #endif
          beepBuzzer(1, 200, 50);
          break;

        default:
          Serial.println(F("[WARNING] Unknown waste category. Defaulting to Dry."));
          detectedWaste = WASTE_DRY;
          break;
      }

      currentState = STATE_DISCHARGING;
      break;
    }

    // ----------------------------------------------------
    // STATE 5: DISCHARGING - Position chute & open trapdoor
    // ----------------------------------------------------
    case STATE_DISCHARGING: {
      routeAndDropWaste(detectedWaste);
      currentState = STATE_RESETTING;
      break;
    }

    // ----------------------------------------------------
    // STATE 6: RESETTING - Return to neutral & cooldown
    // ----------------------------------------------------
    case STATE_RESETTING: {
      Serial.println(F("[RESET] Returning servos to standby position..."));

      // Ensure flap is closed
      moveServoSmooth(servoDropFlap, currentFlapAngle, ANGLE_FLAP_CLOSED, SERVO_STEP_DELAY_MS);

      // Return selector to neutral
      moveServoSmooth(servoSelector, currentSelectorAngle, ANGLE_SELECTOR_NEUTRAL, SERVO_STEP_DELAY_MS);

      updateStatusLEDs(WASTE_NONE, false);

      #if ENABLE_LCD_I2C
      updateDisplay("SORT COMPLETED", "SYSTEM READY");
      #endif

      Serial.print(F("[INFO] System cooling down ("));
      Serial.print(SYSTEM_COOLDOWN_MS);
      Serial.println(F(" ms)...\n"));

      delay(SYSTEM_COOLDOWN_MS);

      currentState = STATE_STANDBY;
      break;
    }
  }

  // Small scheduler delay
  delay(30);
}

// ==========================================
// HARDWARE INITIALIZATION
// ==========================================
void initHardware() {
  // Pin modes
  pinMode(PIN_IR_SENSOR, INPUT);
  pinMode(PIN_METAL_SENSOR, INPUT_PULLUP);
  pinMode(PIN_MOISTURE_DIGITAL, INPUT);
  pinMode(PIN_MOISTURE_ANALOG, INPUT);

  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);

  pinMode(PIN_LED_METAL, OUTPUT);
  pinMode(PIN_LED_WET, OUTPUT);
  pinMode(PIN_LED_DRY, OUTPUT);
  pinMode(PIN_LED_STATUS, OUTPUT);

  digitalWrite(PIN_LED_METAL, LOW);
  digitalWrite(PIN_LED_WET, LOW);
  digitalWrite(PIN_LED_DRY, LOW);
  digitalWrite(PIN_LED_STATUS, HIGH);

  // Attach servos
  servoSelector.attach(PIN_SERVO_SELECTOR);
  servoDropFlap.attach(PIN_SERVO_DROP_FLAP);

  // Set initial home positions
  currentSelectorAngle = ANGLE_SELECTOR_NEUTRAL;
  currentFlapAngle = ANGLE_FLAP_CLOSED;
  servoSelector.write(currentSelectorAngle);
  servoDropFlap.write(currentFlapAngle);
  delay(500);
}

// ==========================================
// SENSOR CLASSIFICATION LOGIC
// ==========================================
WasteType classifyWaste() {
  // 1. Check Inductive Metal Sensor
  // Note: NPN Inductive proximity sensors typically pull LOW when metal is present
  int metalVal = digitalRead(PIN_METAL_SENSOR);
  Serial.print(F("[SENSOR] Inductive Metal Sensor Pin State: "));
  Serial.println(metalVal == METAL_DETECTED ? F("ACTIVE (Metal Found)") : F("INACTIVE (No Metal)"));

  if (metalVal == METAL_DETECTED) {
    return WASTE_METAL;
  }

  // 2. Check Moisture Sensor
  int moistureAvg = readMoistureAveraged(MOISTURE_SAMPLE_COUNT);
  Serial.print(F("[SENSOR] Moisture Sensor Analog Value: "));
  Serial.print(moistureAvg);
  Serial.print(F(" (Wet Threshold <= "));
  Serial.print(MOISTURE_WET_THRESHOLD);
  Serial.println(F(")"));

  // Check against threshold
  if (moistureAvg <= MOISTURE_WET_THRESHOLD) {
    return WASTE_WET;
  }

  // 3. Fallback: If neither metal nor wet, classify as DRY waste
  return WASTE_DRY;
}

// ==========================================
// AVERAGED MOISTURE READING (NOISE FILTER)
// ==========================================
int readMoistureAveraged(int samples) {
  long sum = 0;
  for (int i = 0; i < samples; i++) {
    sum += analogRead(PIN_MOISTURE_ANALOG);
    delay(15);
  }
  return (int)(sum / samples);
}

// ==========================================
// SERVO ROUTING & DISCHARGE ACTION
// ==========================================
void routeAndDropWaste(WasteType type) {
  int targetSelectorAngle = ANGLE_SELECTOR_NEUTRAL;

  switch (type) {
    case WASTE_DRY:
      targetSelectorAngle = ANGLE_BIN_DRY;
      break;
    case WASTE_WET:
      targetSelectorAngle = ANGLE_BIN_WET;
      break;
    case WASTE_METAL:
      targetSelectorAngle = ANGLE_BIN_METAL;
      break;
    default:
      targetSelectorAngle = ANGLE_BIN_DRY;
      break;
  }

  Serial.print(F("[ACTUATOR] Rotating selector chute to "));
  Serial.print(targetSelectorAngle);
  Serial.println(F(" degrees..."));

  // Step 1: Align chute / carousel to target bin
  moveServoSmooth(servoSelector, currentSelectorAngle, targetSelectorAngle, SERVO_STEP_DELAY_MS);
  delay(300); // Allow physical momentum to damp

  // Step 2: Open trap door / drop flap
  Serial.println(F("[ACTUATOR] Opening drop flap to release waste..."));
  moveServoSmooth(servoDropFlap, currentFlapAngle, ANGLE_FLAP_OPEN, SERVO_STEP_DELAY_MS / 2);

  // Step 3: Hold open to allow item to drop cleanly
  delay(FLAP_DISCHARGE_HOLD_MS);

  // Step 4: Close drop flap
  Serial.println(F("[ACTUATOR] Closing drop flap..."));
  moveServoSmooth(servoDropFlap, currentFlapAngle, ANGLE_FLAP_CLOSED, SERVO_STEP_DELAY_MS / 2);
  delay(200);
}

// ==========================================
// SMOOTH SERVO MOTION HELPER
// ==========================================
void moveServoSmooth(Servo &s, int &currentAngle, int targetAngle, int stepDelay) {
  if (currentAngle < targetAngle) {
    for (int pos = currentAngle; pos <= targetAngle; pos++) {
      s.write(pos);
      delay(stepDelay);
    }
  } else if (currentAngle > targetAngle) {
    for (int pos = currentAngle; pos >= targetAngle; pos--) {
      s.write(pos);
      delay(stepDelay);
    }
  }
  currentAngle = targetAngle;
}

// ==========================================
// LED & BUZZER HELPERS
// ==========================================
void updateStatusLEDs(WasteType type, bool isBusy) {
  digitalWrite(PIN_LED_STATUS, isBusy ? HIGH : LOW);
  digitalWrite(PIN_LED_METAL, (type == WASTE_METAL) ? HIGH : LOW);
  digitalWrite(PIN_LED_WET, (type == WASTE_WET) ? HIGH : LOW);
  digitalWrite(PIN_LED_DRY, (type == WASTE_DRY) ? HIGH : LOW);
}

void beepBuzzer(int count, int durationMs, int pauseMs) {
  for (int i = 0; i < count; i++) {
    digitalWrite(PIN_BUZZER, HIGH);
    delay(durationMs);
    digitalWrite(PIN_BUZZER, LOW);
    if (i < count - 1) {
      delay(pauseMs);
    }
  }
}

void updateDisplay(const char* line1, const char* line2) {
  #if ENABLE_LCD_I2C
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
  #else
  (void)line1;
  (void)line2;
  #endif
}

void printBanner() {
  Serial.println(F("=================================================="));
  Serial.println(F("     AUTOMATIC WASTE SEGREGATION BIN SYSTEM       "));
  Serial.println(F("       Firmware v1.0.0 | Sujay Paul               "));
  Serial.println(F("=================================================="));
}
