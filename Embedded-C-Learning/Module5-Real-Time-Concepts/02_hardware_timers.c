/*
 * Module 5.2: Hardware Timers - Precise Timing Control
 * 
 * What are Hardware Timers? Think of them as super-accurate stopwatches:
 * - They count independently of your main program
 * - They can interrupt your program at exact times
 * - Perfect for real-time tasks that need precise timing
 * - ESP32 has multiple timers you can use simultaneously
 * 
 * This example shows different timer techniques for embedded systems
 * Hardware needed: ESP32 (timers are built-in)
 */

#include <Arduino.h>

// ESP32 has 4 hardware timers (Timer 0-3)
// We'll use different timers for different tasks
hw_timer_t *timer0 = NULL;  // For LED blinking
hw_timer_t *timer1 = NULL;  // For sensor reading
hw_timer_t *timer2 = NULL;  // For data logging
hw_timer_t *timer3 = NULL;  // For system monitoring

// Pin definitions
#define LED_PIN       2   // Built-in LED on most ESP32 boards
#define SENSOR_PIN    A0  // Analog sensor pin
#define BUZZER_PIN    5   // Buzzer for alarms

// Global variables for timer-controlled tasks
volatile bool ledState = false;           // LED on/off state
volatile bool readSensorFlag = false;     // Time to read sensor?
volatile bool logDataFlag = false;        // Time to log data?
volatile bool checkSystemFlag = false;    // Time to check system?

// Data storage arrays (simple circular buffers)
#define BUFFER_SIZE 60
float sensorReadings[BUFFER_SIZE];        // Store last 60 readings
int bufferIndex = 0;                      // Current position in buffer
int totalReadings = 0;                    // Total readings taken

// System monitoring variables
unsigned long freeMemory = 0;
float cpuUsage = 0.0;
int alarmCount = 0;

// Timer interrupt functions (ISRs - Interrupt Service Routines)
// These run EXACTLY when the timer expires, interrupting whatever else is happening
// Keep these functions SHORT and SIMPLE!

// Timer 0 ISR: Blink LED every 500ms
void IRAM_ATTR timer0_ISR() {
    ledState = !ledState;                 // Toggle LED state
    digitalWrite(LED_PIN, ledState);      // Update LED immediately
}

// Timer 1 ISR: Set flag to read sensor every 1 second
void IRAM_ATTR timer1_ISR() {
    readSensorFlag = true;  // Just set a flag - do the work in main loop
}

// Timer 2 ISR: Set flag to log data every 10 seconds
void IRAM_ATTR timer2_ISR() {
    logDataFlag = true;     // Just set a flag - do the work in main loop
}

// Timer 3 ISR: Set flag to check system every 5 seconds
void IRAM_ATTR timer3_ISR() {
    checkSystemFlag = true; // Just set a flag - do the work in main loop
}

// Function to initialize all hardware timers
// Think of this as setting up multiple alarm clocks
void initializeTimers() {
    Serial.println("Setting up hardware timers...");
    
    // Timer 0: LED blink timer (500ms = 0.5 seconds)
    timer0 = timerBegin(0, 80, true);  // Timer 0, prescaler 80 (1MHz), count up
    timerAttachInterrupt(timer0, &timer0_ISR, true);  // Attach interrupt function
    timerAlarmWrite(timer0, 500000, true);  // 500,000 microseconds = 0.5 sec, auto-reload
    timerAlarmEnable(timer0);  // Start the timer
    Serial.println("✅ Timer 0: LED blink (500ms)");
    
    // Timer 1: Sensor reading timer (1 second)
    timer1 = timerBegin(1, 80, true);  // Timer 1, prescaler 80, count up
    timerAttachInterrupt(timer1, &timer1_ISR, true);
    timerAlarmWrite(timer1, 1000000, true);  // 1,000,000 microseconds = 1 sec
    timerAlarmEnable(timer1);
    Serial.println("✅ Timer 1: Sensor reading (1s)");
    
    // Timer 2: Data logging timer (10 seconds)
    timer2 = timerBegin(2, 80, true);  // Timer 2, prescaler 80, count up
    timerAttachInterrupt(timer2, &timer2_ISR, true);
    timerAlarmWrite(timer2, 10000000, true);  // 10,000,000 microseconds = 10 sec
    timerAlarmEnable(timer2);
    Serial.println("✅ Timer 2: Data logging (10s)");
    
    // Timer 3: System monitoring timer (5 seconds)
    timer3 = timerBegin(3, 80, true);  // Timer 3, prescaler 80, count up
    timerAttachInterrupt(timer3, &timer3_ISR, true);
    timerAlarmWrite(timer3, 5000000, true);  // 5,000,000 microseconds = 5 sec
    timerAlarmEnable(timer3);
    Serial.println("✅ Timer 3: System monitoring (5s)");
    
    Serial.println("All timers initialized and running!");
}

// Function to read sensor and store in buffer
// This is called when Timer 1 sets the flag
void readAndStoreSensor() {
    // Read analog sensor (0-4095 on ESP32)
    int rawValue = analogRead(SENSOR_PIN);
    
    // Convert to voltage (ESP32 ADC reference is 3.3V)
    float voltage = (rawValue / 4095.0) * 3.3;
    
    // Store in circular buffer
    sensorReadings[bufferIndex] = voltage;
    bufferIndex = (bufferIndex + 1) % BUFFER_SIZE;  // Wrap around at end
    totalReadings++;
    
    Serial.print("📊 Sensor reading #");
    Serial.print(totalReadings);
    Serial.print(": ");
    Serial.print(voltage, 3);
    Serial.print("V (raw: ");
    Serial.print(rawValue);
    Serial.println(")");
    
    // Simple alarm: if voltage is too high or too low
    if (voltage > 2.8 || voltage < 0.5) {
        alarmCount++;
        Serial.println("⚠️  WARNING: Sensor value out of range!");
        
        // Sound buzzer briefly (non-blocking)
        digitalWrite(BUZZER_PIN, HIGH);
        delay(50);  // Very short beep
        digitalWrite(BUZZER_PIN, LOW);
    }
}

// Function to calculate statistics from sensor buffer
// This shows how to process data collected by timers
void calculateSensorStats(float *average, float *minimum, float *maximum) {
    if (totalReadings == 0) {
        *average = *minimum = *maximum = 0.0;
        return;
    }
    
    // Determine how many readings we actually have
    int readingsToUse = (totalReadings < BUFFER_SIZE) ? totalReadings : BUFFER_SIZE;
    
    float sum = 0.0;
    float min = sensorReadings[0];
    float max = sensorReadings[0];
    
    for (int i = 0; i < readingsToUse; i++) {
        float value = sensorReadings[i];
        sum += value;
        if (value < min) min = value;
        if (value > max) max = value;
    }
    
    *average = sum / readingsToUse;
    *minimum = min;
    *maximum = max;
}

// Function to log data summary
// This is called when Timer 2 sets the flag
void logDataSummary() {
    Serial.println("\n📈 === Data Logging Summary ===");
    
    float average, minimum, maximum;
    calculateSensorStats(&average, &minimum, &maximum);
    
    Serial.print("Total readings: ");
    Serial.println(totalReadings);
    
    Serial.print("Average voltage: ");
    Serial.print(average, 3);
    Serial.println("V");
    
    Serial.print("Minimum voltage: ");
    Serial.print(minimum, 3);
    Serial.println("V");
    
    Serial.print("Maximum voltage: ");
    Serial.print(maximum, 3);
    Serial.println("V");
    
    Serial.print("Voltage range: ");
    Serial.print(maximum - minimum, 3);
    Serial.println("V");
    
    Serial.print("Alarm count: ");
    Serial.println(alarmCount);
    
    // Calculate data rate
    float dataRate = (float)totalReadings / (millis() / 1000.0);
    Serial.print("Data rate: ");
    Serial.print(dataRate, 2);
    Serial.println(" readings/second");
    
    Serial.println("==============================\n");
}

// Function to check system health
// This is called when Timer 3 sets the flag
void checkSystemHealth() {
    Serial.println("🔍 System Health Check:");
    
    // Check free memory (approximate)
    freeMemory = ESP.getFreeHeap();
    Serial.print("Free memory: ");
    Serial.print(freeMemory);
    Serial.println(" bytes");
    
    // Check uptime
    unsigned long uptimeSeconds = millis() / 1000;
    unsigned long uptimeMinutes = uptimeSeconds / 60;
    unsigned long uptimeHours = uptimeMinutes / 60;
    
    Serial.print("Uptime: ");
    Serial.print(uptimeHours);
    Serial.print("h ");
    Serial.print(uptimeMinutes % 60);
    Serial.print("m ");
    Serial.print(uptimeSeconds % 60);
    Serial.println("s");
    
    // Simple CPU usage estimate (very rough)
    static unsigned long lastCheckTime = 0;
    static unsigned long lastLoopCount = 0;
    static unsigned long loopCount = 0;
    
    loopCount++;
    
    if (lastCheckTime != 0) {
        unsigned long timeDiff = millis() - lastCheckTime;
        unsigned long loopDiff = loopCount - lastLoopCount;
        cpuUsage = 100.0 - (loopDiff * 10.0 / timeDiff);  // Very rough estimate
        if (cpuUsage < 0) cpuUsage = 0;
        if (cpuUsage > 100) cpuUsage = 100;
        
        Serial.print("Estimated CPU usage: ");
        Serial.print(cpuUsage, 1);
        Serial.println("%");
    }
    
    lastCheckTime = millis();
    lastLoopCount = loopCount;
    
    // Check if any alarms occurred
    if (alarmCount > 0) {
        Serial.print("⚠️  Total alarms: ");
        Serial.println(alarmCount);
    } else {
        Serial.println("✅ No alarms");
    }
    
    // Memory warning
    if (freeMemory < 10000) {  // Less than 10KB free
        Serial.println("⚠️  WARNING: Low memory!");
    }
    
    Serial.println();
}

// Function to demonstrate timer control
// Shows how to start/stop timers during runtime
void demonstrateTimerControl() {
    Serial.println("🎛️  Timer Control Demo:");
    
    // Pause all timers for 2 seconds
    Serial.println("Pausing all timers for 2 seconds...");
    timerAlarmDisable(timer0);
    timerAlarmDisable(timer1);
    timerAlarmDisable(timer2);
    timerAlarmDisable(timer3);
    
    delay(2000);
    
    Serial.println("Resuming all timers...");
    timerAlarmEnable(timer0);
    timerAlarmEnable(timer1);
    timerAlarmEnable(timer2);
    timerAlarmEnable(timer3);
    
    // Change timer 0 frequency temporarily
    Serial.println("Speeding up LED blink for 5 seconds...");
    timerAlarmWrite(timer0, 100000, true);  // 100ms = very fast blink
    
    delay(5000);
    
    Serial.println("Returning LED to normal speed...");
    timerAlarmWrite(timer0, 500000, true);  // Back to 500ms
}

void setup() {
    Serial.begin(115200);
    Serial.println("Hardware Timer Control Example");
    Serial.println("==============================");
    
    // Set up pins
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(SENSOR_PIN, INPUT);
    
    // Initialize all timers
    initializeTimers();
    
    // Initialize sensor buffer with zeros
    for (int i = 0; i < BUFFER_SIZE; i++) {
        sensorReadings[i] = 0.0;
    }
    
    Serial.println("\n🚀 System running! Watch the timers work:");
    Serial.println("- LED should blink every 0.5 seconds");
    Serial.println("- Sensor readings every 1 second");
    Serial.println("- Data summary every 10 seconds");
    Serial.println("- System check every 5 seconds");
    Serial.println("\nTry touching the analog pin (A0) to change readings!\n");
}

void loop() {
    // Check all timer flags and handle them
    // This is the main "event loop" pattern for timer-based systems
    
    // Handle sensor reading (triggered by Timer 1)
    if (readSensorFlag) {
        readSensorFlag = false;  // Clear the flag
        readAndStoreSensor();
    }
    
    // Handle data logging (triggered by Timer 2)
    if (logDataFlag) {
        logDataFlag = false;     // Clear the flag
        logDataSummary();
    }
    
    // Handle system monitoring (triggered by Timer 3)
    if (checkSystemFlag) {
        checkSystemFlag = false; // Clear the flag
        checkSystemHealth();
    }
    
    // Demonstrate timer control every 60 seconds
    static unsigned long lastDemo = 0;
    if (millis() - lastDemo > 60000) {  // Every 60 seconds
        demonstrateTimerControl();
        lastDemo = millis();
    }
    
    // Main loop can do other tasks here
    // The beauty of hardware timers is that they don't interfere with this!
    
    // Small delay to prevent overwhelming the serial output
    delay(10);
}

/*
 * Hardware Timer Concepts Explained:
 * 
 * 1. PRESCALER: Divides the main clock frequency
 *    - ESP32 runs at 80MHz
 *    - Prescaler of 80 gives us 1MHz timer frequency
 *    - This means each timer tick = 1 microsecond
 * 
 * 2. TIMER VALUE: How many ticks before interrupt
 *    - 1,000,000 ticks = 1 second at 1MHz
 *    - 500,000 ticks = 0.5 seconds
 *    - More ticks = longer delay
 * 
 * 3. AUTO-RELOAD: Timer restarts automatically
 *    - true = timer repeats continuously
 *    - false = timer fires once and stops
 * 
 * 4. ISR (Interrupt Service Routine):
 *    - Function that runs when timer expires
 *    - Interrupts whatever else is happening
 *    - Should be SHORT and FAST
 *    - Use IRAM_ATTR for ESP32 (stores in fast memory)
 * 
 * Timer Advantages:
 * ✅ Perfect timing accuracy
 * ✅ Independent of main program
 * ✅ Can handle multiple precise tasks
 * ✅ Real-time response guaranteed
 * ✅ No drift over time
 * 
 * Timer Best Practices:
 * 1. Keep ISRs short and simple
 * 2. Don't use Serial.print() in ISRs (use flags instead)
 * 3. Don't use delay() in ISRs
 * 4. Use volatile variables for ISR communication
 * 5. Clear flags promptly in main loop
 * 6. Consider interrupt priorities for critical tasks
 * 
 * Real-World Applications:
 * - PWM signal generation
 * - Precise sensor sampling
 * - Motor control timing
 * - Communication protocols
 * - Real-time audio/video processing
 * - Safety system monitoring
 * - Data acquisition systems
 * 
 * Common Timer Calculations:
 * - 1ms = 1,000 microseconds
 * - 1s = 1,000,000 microseconds
 * - For 80MHz with prescaler 80: 1 tick = 1 microsecond
 * - Timer value = desired delay in microseconds
 * 
 * Troubleshooting:
 * - Timer not working? Check prescaler and timer value
 * - System crashes? ISR might be too complex
 * - Timing drift? Use hardware timers, not software delays
 * - Multiple timers interfering? Check interrupt priorities
 * - Memory issues? ISRs should use IRAM_ATTR on ESP32
 */