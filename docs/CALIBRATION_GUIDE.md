# Sensor Calibration & Tuning Guide

Achieving consistent waste classification depends heavily on calibrating the moisture threshold and fine-tuning servo angles to match the physical geometry of your custom prototype bin.

Follow this step-by-step guide to calibrate your system.

---

## 1. Moisture Sensor Calibration:

The moisture sensor measures electrical conductivity between two conductive probes. High moisture content (water, fruit juices, food scraps) increases conductivity, resulting in a lower analog resistance reading.

### Step-by-Step Calibration:

1. Flash the `src/sensor_calibration/sensor_calibration.ino` sketch onto your Arduino.
2. Open the **Arduino Serial Monitor** at **9600 baud**.
3. **Dry Baseline Test**:
   - Place dry items (paper, dry cardboard, dry plastic bottle caps) on the sensing bed.
   - Record the analog readings over 10 seconds.
   - *Typical Dry Reading:* `850 – 1023`.
4. **Wet Waste Test**:
   - Place wet/organic items (sliced tomato, orange peel, damp sponge, wet paper napkin) on the sensing bed.
   - Record the analog readings over 10 seconds.
   - *Typical Wet Reading:* `200 – 600`.
5. **Set the Decision Boundary**:
   - Compute the midpoint threshold:
     $$\text{Threshold} = \frac{\text{Min Dry Reading} + \text{Max Wet Reading}}{2}$$
   - Open `src/automatic_waste_segregator/config.h` and update:
     ```cpp
     #define MOISTURE_WET_THRESHOLD 650 // Adjust based on your calculated midpoint
     ```

---

## 2. Inductive Metal Sensor Calibration

1. Mount the LJ12A3-4-Z/BX sensor so its detection face is within **4mm to 8mm** of the intake chamber platform.
2. Test metallic items (aluminum foil, soda cans, steel bottle caps, paper clips).
3. The LED on the rear of the sensor should illuminate bright red/yellow whenever metal enters the proximity zone, pulling pin `D3` LOW.
4. If the sensor triggers on non-metallic items, adjust its distance or check for electromagnetic interference from nearby unshielded servo wires.

---

## 3. Servo Angle Alignment

Because custom 3D-printed, cardboard, or PVC prototypes vary slightly in dimensions, you should verify that each servo angle corresponds exactly to the center of each bin.

In `src/automatic_waste_segregator/config.h`:
```cpp
// Selector Servo (Directs waste into one of 3 bins)
#define ANGLE_SELECTOR_NEUTRAL  90    // Center / rest position
#define ANGLE_BIN_DRY           30    // Adjust until aligned with Dry waste bin
#define ANGLE_BIN_WET           90    // Adjust until aligned with Wet waste bin
#define ANGLE_BIN_METAL         150   // Adjust until aligned with Metal waste bin

// Drop Flap Servo (Trap door)
#define ANGLE_FLAP_CLOSED       0     // Trapdoor firmly closed
#define ANGLE_FLAP_OPEN         95    // Flap drops steep enough for waste to slide down
```

### Pro Tip for Servo Jitter & Power Drops:
- Servos draw high stall current (up to 1A each). If the Arduino resets when a servo moves, ensure the servos are powered by an independent **5V 2A regulator** or separate battery rail, sharing a common ground with the Arduino.
