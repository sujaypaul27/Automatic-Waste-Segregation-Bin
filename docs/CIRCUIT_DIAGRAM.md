# Circuit Diagram & Hardware Wiring Guide

This document details the complete schematic pinout, power distribution strategy, and wiring specifications for the **Automatic Waste Segregation Bin** system.

---

## 1. Electrical Overview & Pinout Table

### Microcontroller: Arduino Uno (ATmega328P) / Arduino Nano

| Component | Component Pin / Wire | Arduino Pin | Logic Level / Type | Description |
| :--- | :--- | :--- | :--- | :--- |
| **IR Proximity Sensor** | VCC | 5V | Power | Powers sensor module |
| | GND | GND | Ground | Common system ground |
| | OUT | D2 | Digital Input | Detects presence of waste in intake hopper |
| **Inductive Metal Sensor** (LJ12A3-4-Z/BX) | Brown Wire (VCC) | 6V - 12V (VIN/Batt) | Power | High-voltage supply for proximity field |
| | Blue Wire (GND) | GND | Ground | Common system ground |
| | Black Wire (Signal) | D3 (via divider) | Digital Input | Pull-down / Voltage divider to 5V max |
| **Moisture / Raindrop Sensor** | VCC | 5V | Power | Powers comparator board |
| | GND | GND | Ground | Common system ground |
| | A0 (Analog Out) | A0 | Analog Input | Continuous conductivity / moisture level |
| | D0 (Digital Out) | D4 (Optional) | Digital Input | Onboard potentiometer threshold trigger |
| **Selector Servo Motor (MG995/SG90)** | Red (VCC) | 5V - 6V (Ext Power) | Power | Servo motor power |
| | Brown/Black (GND) | GND | Ground | Common ground |
| | Orange/Yellow (Signal) | D9 | PWM Output | Rotates chute/carousel to dry/wet/metal |
| **Drop Flap Servo Motor (SG90)** | Red (VCC) | 5V - 6V (Ext Power) | Power | Trapdoor servo power |
| | Brown/Black (GND) | GND | Ground | Common ground |
| | Orange/Yellow (Signal) | D10 | PWM Output | Opens/closes intake chamber trapdoor |
| **Piezo Buzzer** | Positive (+) | D6 | Digital Output | Audio tones for states & alerts |
| | Negative (-) | GND | Ground | Common ground |
| **Status / Metal LED** | Anode (+) via 220Ω | D5 | Digital Output | Metal waste indicator |
| **Status / Wet LED** | Anode (+) via 220Ω | D7 | Digital Output | Wet waste indicator |
| **Status / Dry LED** | Anode (+) via 220Ω | D8 | Digital Output | Dry waste indicator |

---

## 2. Power Distribution & Regulation Architecture

```
                  +--------------------------------+
                  |  2x 18650 Li-ion Battery Pack  |
                  |       (7.4V - 8.4V Nominal)    |
                  +---------------+----------------+
                                  |
               +------------------+-------------------+
               |                                      |
               v                                      v
     +-------------------+                  +-------------------+
     | Arduino Vin Pin   |                  | LM2596 / Buck     |
     | (Onboard 5V Reg)  |                  | Step-Down (5V 3A) |
     +---------+---------+                  +---------+---------+
               |                                      |
               v                                      v
      +-----------------+                    +-----------------+
      | Sensors & Logic |                    |   Servo Motors  |
      | - IR Sensor     |                    | - Selector Servo|
      | - Moisture Sens |                    | - Flap Servo    |
      | - Buzzer & LEDs |                    +--------+--------+
      +--------+--------+                             |
               |                                      |
               +------------------+-------------------+
                                  |
                         COMMON GROUND (GND)
```

> [!IMPORTANT]
> **Common Ground Rule**: Always connect the negative (-) terminal of the external battery, step-down regulator, and Arduino GND together. Without a shared common ground, PWM control signals will flicker and behave erratically.

---

## 3. Inductive Proximity Sensor Interfacing Note

The **LJ12A3-4-Z/BX** inductive proximity sensor is an NPN normally open (NO) sensor rated for 6V–36V:
- When operating with an 8.4V / 12V supply, the black signal wire will output supply voltage when high if improperly interfaced.
- To safely connect to the 5V Arduino input pin without damaging the ATmega328P microcontroller:
  1. Use a **Voltage Divider** (10kΩ and 15kΩ resistors) to step down the signal to ~4.5V, OR
  2. Use a diode in reverse bias (cathode to sensor, anode to pin D3) with `INPUT_PULLUP` enabled on Arduino pin D3. When metal is detected, the sensor pulls the pin to GND (LOW).

---

## 4. Mechanical Alignment & Angles

- **Bin 1 (Dry Waste)**: Servo position set to `30°`.
- **Bin 2 (Wet Waste)**: Servo position set to `90°`.
- **Bin 3 (Metal Waste)**: Servo position set to `150°`.
- **Chamber Trapdoor Flap**: Closed at `0°`, Open at `95°`.
