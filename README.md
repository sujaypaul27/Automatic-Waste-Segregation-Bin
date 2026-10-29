# 🗑️ Automatic Waste Segregation Bin (Smart Dustbin)

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Arduino Compatible](https://img.shields.io/badge/Platform-Arduino%20Uno%20%2F%20Nano-00979C.svg?logo=arduino)](https://www.arduino.cc/)
[![Language: C++](https://img.shields.io/badge/Language-C%2B%2B%20%2F%20Embedded%20C-00599C.svg?logo=c%2B%2B)](https://isocpp.org/)
[![Status: Functional Prototype](https://img.shields.io/badge/Status-Functional%20Prototype-success.svg)]()
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg)](CONTRIBUTING.md)

> An automated, sensor-driven embedded waste management system that detects, classifies, and segregates municipal and domestic waste into **Dry**, **Wet (Organic)**, and **Metallic** categories right at the point of disposal using sensor fusion and precision servo actuation.

---

## 📸 Physical Prototype Showcase

<div align="center">
  <img src="assets/project_prototype.png" alt="Automatic Waste Segregation Bin Hardware Prototype" width="850px" style="border-radius: 10px; box-shadow: 0 4px 15px rgba(0,0,0,0.2);">
  <p><em>Figure 1: Working 3-Compartment Smart Segregator Prototype featuring Intake Hopper, Proximity Sensors, Dual Servos, and Arduino Uno.</em></p>
</div>

---

## 🌟 Key Highlights & Engineering Features

- **Multi-Sensor Fusion**: Combines **Inductive Proximity**, **Resistive Moisture**, and **Infrared (IR)** detection for rapid, reliable material classification.
- **Hierarchical Priority Classification**: Intelligently prioritizes metal detection before moisture analysis to avoid misclassifying wet metallic cans.
- **Dual-Servo Automated Routing**:
  - **Selector Servo**: Rotates the discharge chute or carousel to align with the correct target compartment.
  - **Drop Flap Servo**: Actuates the chamber trapdoor to release waste cleanly under gravity.
- **Robust Finite State Machine (FSM)**: Architecture prevents race conditions, jitter, and unexpected resets.
- **Analog Noise Filtering**: 10-sample moving average filter on moisture readings eliminates false positives from static charge and erratic surface contact.
- **Audio-Visual Feedback**: Dynamic buzzer tone sequences and multi-color LED indicators signal system states and real-time classification decisions.
- **Interview & Resume Ready**: Includes complete schematics, calibration tools, and interview discussion scripts.

---

## ❓ Problem Statement & Motivation

According to municipal solid waste studies, **over 70% of recyclable materials end up in landfills** due to source cross-contamination. 

1. **Hazardous Manual Sorting**: Workers manually hand-pick through mixed waste, exposing themselves to pathogen infections, sharp metals, and toxic chemical leachates.
2. **Loss of Recyclability**: Organic wet waste ruins dry paper and cardboard, making recycling economically unviable.
3. **The Solution**: An autonomous, touchless segregation bin installed in public spaces, offices, and households that separates waste **immediately at entry** before cross-contamination can occur.

---

## ⚙️ How It Works (Working Principle)

```mermaid
flowchart TD
    A([Waste Placed in Entry Hopper]) --> B[IR Sensor Detects Object]
    B --> C[Wait 1.5s Settling Time]
    C --> D{Inductive Metal Sensor Active?}
    
    D -- YES --> E[Classify as: METAL WASTE]
    D -- NO --> F{Moisture Analog < Wet Threshold?}
    
    F -- YES --> G[Classify as: WET / ORGANIC WASTE]
    F -- NO --> H[Classify as: DRY / RECYCLABLE WASTE]
    
    E --> I[Selector Servo Rotates to Metal Bin - 150°]
    G --> J[Selector Servo Rotates to Wet Bin - 90°]
    H --> K[Selector Servo Rotates to Dry Bin - 30°]
    
    I --> L[Drop Flap Servo Opens Trapdoor - 95°]
    J --> L
    K --> L
    
    L --> M[Waste Drops into Designated Bin]
    M --> N[Flap Closes & Selector Returns to Neutral]
    N --> O([System Ready for Next Cycle])
```

---

## 🛠️ Hardware Components & Bill of Materials (BOM)

| Component | Specification | Quantity | Primary Function |
| :--- | :--- | :---: | :--- |
| **Microcontroller** | Arduino Uno R3 / ATmega328P | 1 | Executes FSM logic, reads sensors, and drives PWM servos |
| **Metal Sensor** | LJ12A3-4-Z/BX Inductive Proximity (NPN NO) | 1 | Detects ferrous and non-ferrous metallic objects (range: 4–8mm) |
| **Moisture Sensor** | Resistive Soil/Raindrop Conductivity Probe | 1 | Measures water/moisture content to identify wet organic waste |
| **Object Sensor** | Active-Low IR Obstacle Proximity Module | 1 | Detects placement of waste in intake hopper to initiate cycle |
| **Chute Actuator** | MG995 / TowerPro SG90 High-Torque Servo | 1 | Rotates the diverter chute to Dry (30°), Wet (90°), or Metal (150°) |
| **Flap Actuator** | TowerPro SG90 Micro Servo | 1 | Opens and closes the trapdoor beneath the sensing chamber |
| **Audio Feedback** | 5V Piezo Buzzer | 1 | Emits distinct diagnostic and classification confirmation beeps |
| **Visual Indicators**| 5mm LEDs (Green, Yellow, Red, Blue) + 220Ω Resistors | 4 | Real-time status and category identification |
| **Power Supply** | 2x 18650 Li-ion Batteries (7.4V) + LM2596 Buck Step-Down | 1 | High-current dual-rail supply for Arduino logic and servo rails |
| **Chassis / Frame** | PVC Pipe Arm, Cylindrical Hoppers, Wooden Baseboard | 1 | Structural framework and waste holding compartments |

---

## 🔌 Pin Mapping & Wiring Overview

| Sensor / Module | Signal Pin | Arduino Pin | Description |
| :--- | :--- | :--- | :--- |
| **IR Proximity Sensor** | OUT | `D2` | Waste entry trigger |
| **Inductive Metal Sensor** | Black (Signal) | `D3` | Metal detection (NPN Open Collector / Divider) |
| **Moisture Sensor** | A0 (Analog) | `A0` | Conductivity reading (0 = Wet, 1023 = Dry) |
| **Moisture Sensor** | D0 (Digital) | `D4` | Hardware threshold trigger (optional) |
| **Selector Servo** | PWM Signal | `D9` | 3-way chute alignment |
| **Drop Flap Servo** | PWM Signal | `D10` | Hopper trapdoor actuator |
| **Piezo Buzzer** | Positive (+) | `D6` | System audio alerts |
| **Metal Indicator LED**| Anode (+) | `D5` | Lights up when metal is sorted |
| **Wet Indicator LED** | Anode (+) | `D7` | Lights up when organic wet waste is sorted |
| **Dry Indicator LED** | Anode (+) | `D8` | Lights up when dry recyclable is sorted |

> 📖 **Full Schematic & Power Isolation Details:** See [docs/CIRCUIT_DIAGRAM.md](docs/CIRCUIT_DIAGRAM.md)

---

## 📂 Repository Structure

```
Automatic-Waste-Segregation-Bin/
│
├── assets/
│   └── project_prototype.png          # Visual photograph of physical hardware build
│
├── docs/
│   ├── CIRCUIT_DIAGRAM.md             # Complete wiring tables, power distribution & isolation
│   ├── CALIBRATION_GUIDE.md           # Sensor threshold tuning and servo angle calibration
│   ├── INTERVIEW_PREP.md              # Technical Q&A, talking points & resume bullets
│   └── IOT_EXPANSION_GUIDE.md         # ESP32 / Blynk cloud telemetry & smart city integration
│
├── src/
│   ├── automatic_waste_segregator/
│   │   ├── automatic_waste_segregator.ino  # Production firmware with Finite State Machine
│   │   └── config.h                        # Pin mappings, servo angles, and threshold constants
│   │
│   └── sensor_calibration/
│       └── sensor_calibration.ino          # Diagnostic benchmark tool for live sensor logging
│
├── .gitignore
├── CONTRIBUTING.md
├── LICENSE
└── README.md
```

---

## 🚀 Quick Start Guide

### 1. Prerequisites
- [Arduino IDE](https://www.arduino.cc/en/software) (version 1.8.x or 2.x)
- Arduino Uno / Nano connected via USB cable
- Standard `Servo.h` library (bundled with Arduino IDE)

### 2. Installation & Uploading
1. Clone this repository:
   ```bash
   git clone https://github.com/sujaypaul27/Automatic-Waste-Segregation-Bin.git
   cd Automatic-Waste-Segregation-Bin
   ```
2. Open `src/automatic_waste_segregator/automatic_waste_segregator.ino` in Arduino IDE.
3. Select your board: **Tools > Board > Arduino Uno** (or Nano).
4. Select the matching Port: **Tools > Port > COMx**.
5. Click **Upload** (Ctrl + U).

### 3. Verification & Live Monitoring
1. Open the Serial Monitor (**Tools > Serial Monitor**) at **9600 baud**.
2. Place a waste item (e.g. tomato slice, aluminum can, paper sheet) into the upper hopper.
3. Observe live sensor logging, classification decisions, and servo actuation.

---

## 🎯 Interview & Resume Talking Points

Putting this project on your resume? Here is how to speak about it with authority:

- **State Machine Architecture**: *"I structured the firmware as a Finite State Machine (FSM) rather than blocking procedural code, allowing clean error recovery, servo speed damping, and noise filtering."*
- **Hierarchical Classification Rationale**: *"I programmed the logic to check metal before moisture. A wet metallic beverage can contains moisture, but its high recycling and non-compostable properties require it to be routed to the metal bin."*
- **Power Management**: *"Servos produce sudden current spikes up to 1A. I engineered an isolated power distribution scheme with a common reference ground, preventing Arduino brownouts."*

> 💼 **Comprehensive Interview Guide with 10+ Technical Q&As:** See [docs/INTERVIEW_PREP.md](docs/INTERVIEW_PREP.md)

---

## 🔮 Future Roadmap & Upgrades

- [ ] **Capacitive Proximity Sensor**: Distinguish between plastics and non-plastic dry waste.
- [ ] **Ultrasonic Bin Fill-Level Telemetry**: Alert municipal services when individual bins reach 90% capacity.
- [ ] **Edge AI Vision (ESP32-CAM)**: Lightweight neural network model to classify complex recyclables (PET, e-waste, glass).
- [ ] **Solar Power Harvesting**: Add solar panel and charge controller for standalone public outdoor operation.

---

## 👨‍💻 Author

**Sujay Paul**
- GitHub: [@sujaypaul27](https://github.com/sujaypaul27)
- Repository: [Automatic-Waste-Segregation-Bin](https://github.com/sujaypaul27/Automatic-Waste-Segregation-Bin)

---

## 📄 License

This project is licensed under the [MIT License](LICENSE) - feel free to use, modify, and build upon it for academic and open-source projects.
