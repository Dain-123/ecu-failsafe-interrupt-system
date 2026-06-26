# ecu-failsafe-interrupt-system
Automotive Sensor Diagnostic &amp; Safety Monitoring System
# Engine Control Unit (ECU) Fail-Safe System

An embedded firmware simulation showcasing safety-critical automotive systems using low-latency hardware interrupts.

## 🚀 Features
* **Real-Time Data Streams:** Processes filtered throttle adjustments and NTC temperature readings.
* **Hardware Interrupt Safety:** Employs a physical fallback mechanism utilizing a `FALLING` edge hardware interrupt on Digital Pin 2.
* **Instant Containment:** Bypasses standard application delays to execute an immediate fail-safe sequence upon error detection.

## 🛠️ Hardware Stack (Simulated)
* Arduino Uno Microcontroller
* Rotary Potentiometer (Throttle Input)
* NTC Thermistor Module (Engine Temperature)
* Slide Switch (Emergency Trip Toggle)
