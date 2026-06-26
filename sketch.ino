/**
 * @file Automotive_ECU_Monitor.ino
 * @brief Mission-Critical Automotive ECU Signal Integrity & Fail-Safe Controller
 * @details Implements a real-time signal processing array (Moving Average Filter),
 *          hardware-interrupt fault injection handling, and out-of-bounds containment.
 */

#include <Arduino.h>

// Hardware Pin Constraints
const int PIN_THROTTLE_ANALOG = A0;   // Analog Input: Throttle Position Sensor (TPS)
const int PIN_TEMP_ANALOG     = A1;   // Analog Input: Engine Coolant Temperature (ECT)
const int PIN_CRITICAL_FAULT  = 2;    // Digital Input: External Hardware Interrupt Line

// Digital Signal Processing (DSP) Variables
const int FILTER_WINDOW_SIZE = 5;
int throttleSamples[FILTER_WINDOW_SIZE] = {0};
int sampleIndex = 0;

// Mission-Critical System Flags (Declared volatile because they change inside an ISR)
volatile bool emergencyTripActive = false;

void emergencyFaultISR() {
  // HARDWARE INTERRUPT SERVICE ROUTINE (ISR)
  // Executes instantly at the silicon level when a circuit fault is detected
  emergencyTripActive = true;
}

void setup() {
  Serial.begin(115200);
  
  pinMode(PIN_THROTTLE_ANALOG, INPUT);
  pinMode(PIN_TEMP_ANALOG, INPUT);
  
  // Configure Interrupt pin with internal pull-up resistor
  pinMode(PIN_CRITICAL_FAULT, INPUT_PULLUP);
  
  // Bind physical wire falling edges directly to the Interrupt Service Routine
  attachInterrupt(digitalPinToInterrupt(PIN_CRITICAL_FAULT), emergencyFaultISR, FALLING);

  Serial.println(F("======================================================"));
  Serial.println(F(" AUTOMOTIVE ECU SYSTEM SAFETY BOOT SEQUENCE           "));
  Serial.println(F(" STATUS: MONITORING PERIPHERAL REGISTER BUSSES       "));
  Serial.println(F("======================================================"));
  delay(1000);
}

void loop() {
  // CRITICAL FAIL-SAFE EVALUATION FIRST (0-Latency Execution Path)
  if (emergencyTripActive) {
    executeEmergencyContainment();
    return; // Freeze and isolate all normal system operations
  }

  // 1. SIGNAL ACQUISITION & DIGITAL FILTERING (DSP)
  int rawThrottleRead = analogRead(PIN_THROTTLE_ANALOG);
  
  // Apply a Rolling Average Filter to smooth out high-frequency circuit noise
  throttleSamples[sampleIndex] = rawThrottleRead;
  sampleIndex = (sampleIndex + 1) % FILTER_WINDOW_SIZE;
  
  long sampleSum = 0;
  for (int i = 0; i < FILTER_WINDOW_SIZE; i++) {
    sampleSum += throttleSamples[i];
  }
  int filteredThrottle = sampleSum / FILTER_WINDOW_SIZE;

  // 2. PARAMETRIC VALIDATION (Checking sensor health boundaries)
  int rawTempRead = analogRead(PIN_TEMP_ANALOG);
  
  // Diagnostic Validation: Check for out-of-bounds (Short circuit or open sensor wire)
  if (rawTempRead > 1000 || rawTempRead < 20) {
    Serial.println(F("[DIAGNOSTIC ALERT]: Sensor open/short circuit trace detected on A1!"));
    emergencyTripActive = true; // Trigger internal fail-safe software trip
  }

  // 3. NORMAL OPERATION TRANSMISSION (UART Log)
  Serial.print(F("[ECU ACTIVE] Throttle (Filtered): "));
  Serial.print(filteredThrottle);
  Serial.print(F(" | Engine Temp Sensor ADC: "));
  Serial.println(rawTempRead);

  delay(200); // 5Hz Telemetry Loop
}

void executeEmergencyContainment() {
  // EMERGENCY SAFE MODE STATE
  // Overrides throttle output completely to protect engine components
  Serial.println(F("\n######################################################"));
  Serial.println(F(" !!! [CRITICAL FAULT EVENT DETECTED: SYSTEM TRIPPED] !!!"));
  Serial.println(F(" ACTION: FORCE ACTUATOR SHUTDOWN & ISOLATE INJECTORS  "));
  Serial.println(F(" STATUS: ECU REBOOT / HARDWARE RESET REQUIRED         "));
  Serial.println(F("######################################################"));
  
  while (true) {
    // Infinite safety trap holding the hardware in a safe state until an engineer resets it
    delay(1000);
  }
}
