/**
 * @file config.h
 * @brief Hardware Pin Mapping, Thresholds, and Calibration Constants
 * @project Automatic Waste Segregation Bin (Smart Dustbin)
 * @author Sujay Paul (https://github.com/sujaypaul27)
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ==========================================
// 1. PIN DEFINITIONS
// ==========================================

// Sensor Pins
#define PIN_IR_SENSOR         2     // Digital input: IR proximity sensor (detects waste entry)
#define PIN_METAL_SENSOR      3     // Digital input: Inductive proximity sensor (detects metallic waste)
#define PIN_MOISTURE_ANALOG   A0    // Analog input: Soil moisture / Rain sensor analog out
#define PIN_MOISTURE_DIGITAL  4     // Digital input: Moisture sensor digital threshold output (optional)

// Actuator Pins (PWM capable pins on Arduino Uno: 3, 5, 6, 9, 10, 11)
#define PIN_SERVO_SELECTOR    9     // Servo 1: Rotates/directs chute or carousel to bin position
#define PIN_SERVO_DROP_FLAP   10    // Servo 2: Opens & closes trapdoor / drop flap in upper chamber

// Indicator Pins
#define PIN_BUZZER            6     // Piezo buzzer for audio feedback
#define PIN_LED_METAL         5     // LED indicator for Metal waste
#define PIN_LED_WET           7     // LED indicator for Wet waste
#define PIN_LED_DRY           8     // LED indicator for Dry waste
#define PIN_LED_STATUS        13    // Onboard / status LED (idle / processing)

// ==========================================
// 2. SERVO ANGLE CONFIGURATIONS (DEGREES)
// ==========================================

// Selector Servo (Directs waste into one of 3 bins)
#define ANGLE_SELECTOR_NEUTRAL  90    // Center / rest position
#define ANGLE_BIN_DRY           30    // Angle aligned with Dry waste compartment
#define ANGLE_BIN_WET           90    // Angle aligned with Wet waste compartment
#define ANGLE_BIN_METAL         150   // Angle aligned with Metal waste compartment

// Drop Flap Servo (Trap door)
#define ANGLE_FLAP_CLOSED       0     // Trapdoor closed (holding waste for sensing)
#define ANGLE_FLAP_OPEN         95    // Trapdoor open (dropping waste into selected bin)

// Servo Speed / Delay
#define SERVO_STEP_DELAY_MS     15    // Smooth servo rotation delay per degree
#define FLAP_DISCHARGE_HOLD_MS  1200  // Time to hold flap open for waste to fall completely

// ==========================================
// 3. SENSOR CALIBRATION & THRESHOLDS
// ==========================================

// Moisture Sensor Threshold (0 to 1023)
// Typically, lower values = more moisture (or vice-versa depending on sensor board).
// Default standard: Analog read < 700 indicates wet waste.
#define MOISTURE_WET_THRESHOLD  650   // Readings below this value qualify as WET waste
#define MOISTURE_SAMPLE_COUNT   10    // Number of analog samples for noise rejection

// Active Logic Levels
#define IR_OBJECT_DETECTED      LOW   // Standard active-low IR obstacle sensor
#define METAL_DETECTED          LOW   // Standard inductive proximity NPN NO output (active low)

// System Timings (Milliseconds)
#define SENSING_SETTLE_DELAY_MS 1500  // Delay after IR triggers to allow waste to settle before reading
#define SYSTEM_COOLDOWN_MS      1000  // Delay after dumping before accepting new waste

// ==========================================
// 4. OPTIONAL HARDWARE MODULES
// ==========================================
#define ENABLE_LCD_I2C          0     // Set to 1 if using 16x2 I2C LCD Display (0x27 or 0x3F)
#define LCD_I2C_ADDRESS         0x27

#endif // CONFIG_H
