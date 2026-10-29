/**
 * ============================================================================
 * Project: Automatic Waste Segregation Bin
 * File: sensor_calibration.ino
 * Author: Sujay Paul
 * Description:
 *   Diagnostic and calibration tool to benchmark sensors:
 *     - Proximity Inductive Metal Sensor
 *     - IR Obstacle Sensor
 *     - Moisture / Rain Sensor
 *
 *   Use this sketch to establish baseline threshold values in your config.h
 *   for dry materials (paper, plastic) vs wet materials (fruits, food waste).
 * ============================================================================
 */

#define PIN_IR_SENSOR         2
#define PIN_METAL_SENSOR      3
#define PIN_MOISTURE_ANALOG   A0
#define PIN_MOISTURE_DIGITAL  4
#define PIN_BUZZER            6

void setup() {
  Serial.begin(9600);
  pinMode(PIN_IR_SENSOR, INPUT);
  pinMode(PIN_METAL_SENSOR, INPUT_PULLUP);
  pinMode(PIN_MOISTURE_DIGITAL, INPUT);
  pinMode(PIN_MOISTURE_ANALOG, INPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  Serial.println(F("=================================================="));
  Serial.println(F("    WASTE SEGREGATOR - SENSOR CALIBRATION TOOL    "));
  Serial.println(F("=================================================="));
  Serial.println(F("Place items on sensing platform to observe live readings.\n"));
  delay(1000);
}

void loop() {
  int irVal = digitalRead(PIN_IR_SENSOR);
  int metalVal = digitalRead(PIN_METAL_SENSOR);
  int moistureAnalog = analogRead(PIN_MOISTURE_ANALOG);
  int moistureDigital = digitalRead(PIN_MOISTURE_DIGITAL);

  Serial.println(F("----------- SENSOR SNAPSHOT -----------"));
  Serial.print(F("[IR Proximity]       : "));
  Serial.print(irVal == LOW ? F("OBJECT PRESENT (LOW)") : F("CLEAR (HIGH)"));
  Serial.println();

  Serial.print(F("[Metal Inductive]   : "));
  Serial.print(metalVal == LOW ? F("METAL DETECTED (LOW)") : F("NO METAL (HIGH)"));
  Serial.println();

  Serial.print(F("[Moisture Analog]    : "));
  Serial.print(moistureAnalog);
  Serial.print(F(" (0=Wet, 1023=Dry) | Digital Out: "));
  Serial.println(moistureDigital);

  // Quick evaluation prediction
  Serial.print(F("--> PREDICTED CLASS : "));
  if (metalVal == LOW) {
    Serial.println(F(">> METAL <<"));
    digitalWrite(PIN_BUZZER, HIGH); delay(50); digitalWrite(PIN_BUZZER, LOW);
  } else if (moistureAnalog < 650) {
    Serial.println(F(">> WET WASTE <<"));
    digitalWrite(PIN_BUZZER, HIGH); delay(100); digitalWrite(PIN_BUZZER, LOW);
  } else if (irVal == LOW) {
    Serial.println(F(">> DRY WASTE <<"));
  } else {
    Serial.println(F("No waste placed."));
  }

  Serial.println();
  delay(1000);
}
