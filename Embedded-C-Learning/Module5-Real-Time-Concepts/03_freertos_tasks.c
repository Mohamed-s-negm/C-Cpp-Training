/*
 * Module 5.3: FreeRTOS Tasks - Multitasking on Embedded Systems
 * 
 * What is FreeRTOS? Think of it as a traffic controller for your programs:
 * - Allows multiple "tasks" (like mini-programs) to run "simultaneously"
 * - Each task has a priority (like VIP lanes on a highway)
 * - Tasks can communicate with each other safely
 * - Perfect for complex embedded systems with multiple responsibilities
 * 
 * ESP32 comes with FreeRTOS built-in!
 * This example shows multiple tasks working together
 */

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>

// Pin definitions
#define LED_RED_PIN     2   // Red LED
#define LED_GREEN_PIN   4   // Green LED
#define LED_BLUE_PIN    5   // Blue LED (or buzzer)
#define BUTTON_PIN      0   // Button input (GPIO 0 on many ESP32 boards)
#define SENSOR_PIN      A0  // Analog sensor

// Task handles (like ID cards for each task)
TaskHandle_t taskLEDHandle = NULL;
TaskHandle_t taskSensorHandle = NULL;
TaskHandle_t taskButtonHandle = NULL;
TaskHandle_t taskDisplayHandle = NULL;
TaskHandle_t taskWatchdogHandle = NULL;

// Communication between tasks
QueueHandle_t sensorDataQueue = NULL;     // Queue to send sensor data
QueueHandle_t buttonEventQueue = NULL;    // Queue to send button events
SemaphoreHandle_t serialMutex = NULL;     // Mutex to protect Serial output

// Shared data structure for sensor readings
typedef struct {
    float voltage;
    float temperature;
    int lightLevel;
    unsigned long timestamp;
} SensorData;

// Shared data structure for button events
typedef struct {
    bool pressed;
    bool released;
    unsigned long duration;
    unsigned long timestamp;
} ButtonEvent;

// Global system state (protected by tasks)
typedef struct {
    bool systemRunning;
    int ledMode;              // 0=off, 1=slow, 2=fast, 3=rainbow
    int sensorSamples;        // Total sensor readings
    int buttonPresses;        // Total button presses
    float averageTemp;        // Running average temperature
    bool alarmActive;         // System alarm state
} SystemState;

SystemState systemState = {
    .systemRunning = true,
    .ledMode = 1,
    .sensorSamples = 0,
    .buttonPresses = 0,
    .averageTemp = 0.0,
    .alarmActive = false
};

// Safe function to print to Serial (uses mutex)
// Think of mutex as a "talking stick" - only one task can use Serial at a time
void safePrint(const char* message) {
    if (xSemaphoreTake(serialMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        Serial.print(message);
        xSemaphoreGive(serialMutex);  // Give back the "talking stick"
    }
}

void safePrintln(const char* message) {
    if (xSemaphoreTake(serialMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        Serial.println(message);
        xSemaphoreGive(serialMutex);  // Give back the "talking stick"
    }
}

// Task 1: LED Control Task (Priority: 1 - Low)
// This task controls different LED patterns based on system state
void taskLEDControl(void *parameter) {
    safePrintln("🔥 LED Control Task Started");
    
    TickType_t lastWakeTime = xTaskGetTickCount();
    
    while (systemState.systemRunning) {
        switch (systemState.ledMode) {
            case 0:  // LEDs off
                digitalWrite(LED_RED_PIN, LOW);
                digitalWrite(LED_GREEN_PIN, LOW);
                digitalWrite(LED_BLUE_PIN, LOW);
                vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(1000));
                break;
                
            case 1:  // Slow blink green (normal operation)
                digitalWrite(LED_GREEN_PIN, HIGH);
                digitalWrite(LED_RED_PIN, LOW);
                digitalWrite(LED_BLUE_PIN, LOW);
                vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(500));
                
                digitalWrite(LED_GREEN_PIN, LOW);
                vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(500));
                break;
                
            case 2:  // Fast blink red (warning mode)
                digitalWrite(LED_RED_PIN, HIGH);
                digitalWrite(LED_GREEN_PIN, LOW);
                digitalWrite(LED_BLUE_PIN, LOW);
                vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(100));
                
                digitalWrite(LED_RED_PIN, LOW);
                vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(100));
                break;
                
            case 3:  // Rainbow mode (all colors cycling)
                // Red
                digitalWrite(LED_RED_PIN, HIGH);
                digitalWrite(LED_GREEN_PIN, LOW);
                digitalWrite(LED_BLUE_PIN, LOW);
                vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(200));
                
                // Green
                digitalWrite(LED_RED_PIN, LOW);
                digitalWrite(LED_GREEN_PIN, HIGH);
                digitalWrite(LED_BLUE_PIN, LOW);
                vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(200));
                
                // Blue
                digitalWrite(LED_RED_PIN, LOW);
                digitalWrite(LED_GREEN_PIN, LOW);
                digitalWrite(LED_BLUE_PIN, HIGH);
                vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(200));
                break;
        }
        
        // If alarm is active, override with rapid red flashing
        if (systemState.alarmActive) {
            digitalWrite(LED_RED_PIN, HIGH);
            digitalWrite(LED_GREEN_PIN, LOW);
            digitalWrite(LED_BLUE_PIN, LOW);
            vTaskDelay(pdMS_TO_TICKS(50));
            digitalWrite(LED_RED_PIN, LOW);
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
    
    safePrintln("🔥 LED Control Task Ended");
    vTaskDelete(NULL);  // Delete this task
}

// Task 2: Sensor Reading Task (Priority: 2 - Medium)
// This task reads sensors and sends data to other tasks
void taskSensorReading(void *parameter) {
    safePrintln("📊 Sensor Reading Task Started");
    
    TickType_t lastWakeTime = xTaskGetTickCount();
    float tempSum = 0.0;
    
    while (systemState.systemRunning) {
        SensorData data;
        
        // Read analog sensor
        int rawValue = analogRead(SENSOR_PIN);
        data.voltage = (rawValue / 4095.0) * 3.3;
        
        // Convert to temperature (simulated sensor)
        // Assuming a temperature sensor that gives 0.01V per degree C
        data.temperature = (data.voltage * 100.0) - 50.0;  // Range: -50°C to +280°C
        
        // Simulate light level (could be from another sensor)
        data.lightLevel = rawValue;  // 0-4095
        
        data.timestamp = millis();
        
        // Update system statistics
        systemState.sensorSamples++;
        tempSum += data.temperature;
        systemState.averageTemp = tempSum / systemState.sensorSamples;
        
        // Check for alarm conditions
        if (data.temperature > 50.0 || data.temperature < -10.0) {
            systemState.alarmActive = true;
            systemState.ledMode = 2;  // Switch to warning mode
        } else {
            systemState.alarmActive = false;
            if (systemState.ledMode == 2) {
                systemState.ledMode = 1;  // Return to normal mode
            }
        }
        
        // Send data to display task via queue
        if (sensorDataQueue != NULL) {
            if (xQueueSend(sensorDataQueue, &data, 0) != pdTRUE) {
                safePrintln("⚠️  Sensor queue full!");
            }
        }
        
        // Read sensors every 2 seconds
        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(2000));
    }
    
    safePrintln("📊 Sensor Reading Task Ended");
    vTaskDelete(NULL);
}

// Task 3: Button Handler Task (Priority: 3 - High)
// This task monitors button presses and sends events
void taskButtonHandler(void *parameter) {
    safePrintln("🔘 Button Handler Task Started");
    
    bool lastButtonState = HIGH;  // Assuming pull-up resistor
    unsigned long pressStartTime = 0;
    TickType_t lastWakeTime = xTaskGetTickCount();
    
    while (systemState.systemRunning) {
        bool currentButtonState = digitalRead(BUTTON_PIN);
        ButtonEvent event = {false, false, 0, millis()};
        bool sendEvent = false;
        
        // Button press detected (HIGH to LOW transition)
        if (lastButtonState == HIGH && currentButtonState == LOW) {
            pressStartTime = millis();
            event.pressed = true;
            sendEvent = true;
            systemState.buttonPresses++;
            
            // Cycle through LED modes on button press
            systemState.ledMode = (systemState.ledMode + 1) % 4;
            
            safePrintln("🔘 Button Pressed!");
        }
        
        // Button release detected (LOW to HIGH transition)
        if (lastButtonState == LOW && currentButtonState == HIGH) {
            event.released = true;
            event.duration = millis() - pressStartTime;
            sendEvent = true;
            
            safePrint("🔘 Button Released! Duration: ");
            if (xSemaphoreTake(serialMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
                Serial.print(event.duration);
                Serial.println("ms");
                xSemaphoreGive(serialMutex);
            }
            
            // Long press (>2 seconds) triggers special action
            if (event.duration > 2000) {
                systemState.alarmActive = !systemState.alarmActive;
                safePrintln("🔘 Long press - Toggled alarm!");
            }
        }
        
        // Send button event to other tasks
        if (sendEvent && buttonEventQueue != NULL) {
            if (xQueueSend(buttonEventQueue, &event, 0) != pdTRUE) {
                safePrintln("⚠️  Button queue full!");
            }
        }
        
        lastButtonState = currentButtonState;
        
        // Check button every 50ms for responsive feel
        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(50));
    }
    
    safePrintln("🔘 Button Handler Task Ended");
    vTaskDelete(NULL);
}

// Task 4: Display Task (Priority: 1 - Low)
// This task receives data from other tasks and displays information
void taskDisplay(void *parameter) {
    safePrintln("📺 Display Task Started");
    
    SensorData sensorData;
    ButtonEvent buttonEvent;
    TickType_t lastWakeTime = xTaskGetTickCount();
    
    while (systemState.systemRunning) {
        bool displayUpdate = false;
        
        // Check for new sensor data
        if (xQueueReceive(sensorDataQueue, &sensorData, 0) == pdTRUE) {
            if (xSemaphoreTake(serialMutex, pdMS_TO_TICKS(200)) == pdTRUE) {
                Serial.println("\n📊 === SENSOR UPDATE ===");
                Serial.print("Voltage: ");
                Serial.print(sensorData.voltage, 3);
                Serial.println("V");
                Serial.print("Temperature: ");
                Serial.print(sensorData.temperature, 1);
                Serial.println("°C");
                Serial.print("Light Level: ");
                Serial.print(sensorData.lightLevel);
                Serial.println("/4095");
                Serial.print("Timestamp: ");
                Serial.println(sensorData.timestamp);
                xSemaphoreGive(serialMutex);
            }
            displayUpdate = true;
        }
        
        // Check for button events
        if (xQueueReceive(buttonEventQueue, &buttonEvent, 0) == pdTRUE) {
            if (xSemaphoreTake(serialMutex, pdMS_TO_TICKS(200)) == pdTRUE) {
                Serial.println("\n🔘 === BUTTON EVENT ===");
                if (buttonEvent.pressed) Serial.println("Action: PRESSED");
                if (buttonEvent.released) {
                    Serial.println("Action: RELEASED");
                    Serial.print("Duration: ");
                    Serial.print(buttonEvent.duration);
                    Serial.println("ms");
                }
                Serial.print("Timestamp: ");
                Serial.println(buttonEvent.timestamp);
                xSemaphoreGive(serialMutex);
            }
            displayUpdate = true;
        }
        
        // Display system status every 10 seconds
        static unsigned long lastStatusTime = 0;
        if (millis() - lastStatusTime > 10000 || displayUpdate) {
            if (xSemaphoreTake(serialMutex, pdMS_TO_TICKS(500)) == pdTRUE) {
                Serial.println("\n🖥️  === SYSTEM STATUS ===");
                Serial.print("LED Mode: ");
                switch (systemState.ledMode) {
                    case 0: Serial.println("OFF"); break;
                    case 1: Serial.println("NORMAL (Green)"); break;
                    case 2: Serial.println("WARNING (Red)"); break;
                    case 3: Serial.println("RAINBOW"); break;
                }
                Serial.print("Sensor Samples: ");
                Serial.println(systemState.sensorSamples);
                Serial.print("Button Presses: ");
                Serial.println(systemState.buttonPresses);
                Serial.print("Average Temperature: ");
                Serial.print(systemState.averageTemp, 1);
                Serial.println("°C");
                Serial.print("Alarm Active: ");
                Serial.println(systemState.alarmActive ? "YES" : "NO");
                Serial.print("Free Heap: ");
                Serial.print(ESP.getFreeHeap());
                Serial.println(" bytes");
                Serial.println("========================\n");
                xSemaphoreGive(serialMutex);
            }
            lastStatusTime = millis();
        }
        
        // Display task runs every 1 second
        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(1000));
    }
    
    safePrintln("📺 Display Task Ended");
    vTaskDelete(NULL);
}

// Task 5: Watchdog Task (Priority: 4 - Highest)
// This task monitors system health and can reset if needed
void taskWatchdog(void *parameter) {
    safePrintln("🐕 Watchdog Task Started");
    
    TickType_t lastWakeTime = xTaskGetTickCount();
    unsigned long lastSensorTime = 0;
    unsigned long lastButtonTime = 0;
    
    while (systemState.systemRunning) {
        bool systemHealthy = true;
        
        // Check if sensor task is responding
        if (systemState.sensorSamples > 0) {
            lastSensorTime = millis();
        } else if (millis() - lastSensorTime > 10000) {  // 10 seconds timeout
            safePrintln("🐕 WARNING: Sensor task not responding!");
            systemHealthy = false;
        }
        
        // Check memory levels
        uint32_t freeHeap = ESP.getFreeHeap();
        if (freeHeap < 10000) {  // Less than 10KB free
            safePrintln("🐕 WARNING: Low memory!");
            systemHealthy = false;
        }
        
        // Check task stack usage (advanced monitoring)
        UBaseType_t ledStackHighWater = uxTaskGetStackHighWaterMark(taskLEDHandle);
        if (ledStackHighWater < 100) {  // Less than 100 words free
            safePrintln("🐕 WARNING: LED task stack low!");
            systemHealthy = false;
        }
        
        // If system is unhealthy for too long, could trigger reset
        static int unhealthyCount = 0;
        if (!systemHealthy) {
            unhealthyCount++;
            if (unhealthyCount > 5) {  // 5 consecutive bad checks
                safePrintln("🐕 CRITICAL: System reset required!");
                // In a real system, you might call ESP.restart() here
                // For demo, just set alarm
                systemState.alarmActive = true;
                unhealthyCount = 0;
            }
        } else {
            unhealthyCount = 0;  // Reset counter if system is healthy
        }
        
        // Watchdog runs every 2 seconds
        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(2000));
    }
    
    safePrintln("🐕 Watchdog Task Ended");
    vTaskDelete(NULL);
}

void setup() {
    Serial.begin(115200);
    Serial.println("FreeRTOS Multitasking Example");
    Serial.println("=============================");
    
    // Set up pins
    pinMode(LED_RED_PIN, OUTPUT);
    pinMode(LED_GREEN_PIN, OUTPUT);
    pinMode(LED_BLUE_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);  // Use internal pull-up resistor
    pinMode(SENSOR_PIN, INPUT);
    
    // Create communication objects
    sensorDataQueue = xQueueCreate(5, sizeof(SensorData));      // Queue for 5 sensor readings
    buttonEventQueue = xQueueCreate(3, sizeof(ButtonEvent));    // Queue for 3 button events
    serialMutex = xSemaphoreCreateMutex();                      // Mutex for Serial protection
    
    if (sensorDataQueue == NULL || buttonEventQueue == NULL || serialMutex == NULL) {
        Serial.println("❌ Failed to create communication objects!");
        return;
    }
    
    Serial.println("✅ Communication objects created");
    
    // Create tasks with different priorities
    // Higher numbers = higher priority
    
    xTaskCreate(
        taskLEDControl,      // Function to run
        "LED Control",       // Task name (for debugging)
        2048,               // Stack size (words)
        NULL,               // Parameters to pass
        1,                  // Priority (1 = low)
        &taskLEDHandle      // Task handle
    );
    
    xTaskCreate(
        taskSensorReading,
        "Sensor Reading",
        2048,
        NULL,
        2,                  // Priority (2 = medium)
        &taskSensorHandle
    );
    
    xTaskCreate(
        taskButtonHandler,
        "Button Handler",
        2048,
        NULL,
        3,                  // Priority (3 = high)
        &taskButtonHandle
    );
    
    xTaskCreate(
        taskDisplay,
        "Display",
        3072,               // Larger stack for Serial operations
        NULL,
        1,                  // Priority (1 = low)
        &taskDisplayHandle
    );
    
    xTaskCreate(
        taskWatchdog,
        "Watchdog",
        2048,
        NULL,
        4,                  // Priority (4 = highest)
        &taskWatchdogHandle
    );
    
    Serial.println("✅ All tasks created and running!");
    Serial.println("\n🚀 System Status:");
    Serial.println("- Press button to change LED modes");
    Serial.println("- Hold button >2s to toggle alarm");
    Serial.println("- Touch analog pin (A0) to change sensor readings");
    Serial.println("- Watch the multitasking magic happen!\n");
    
    // The setup() function ends here, but the tasks keep running!
}

void loop() {
    // In FreeRTOS systems, the main loop often does very little
    // All the work is done by tasks!
    
    // We could add some main loop work here if needed
    // But for this demo, we'll just let the tasks do everything
    
    delay(1000);  // Small delay to prevent watchdog timeout
    
    // Optional: Monitor system status in main loop
    static int loopCount = 0;
    loopCount++;
    
    if (loopCount % 30 == 0) {  // Every 30 seconds
        if (xSemaphoreTake(serialMutex, pdMS_TO_TICKS(200)) == pdTRUE) {
            Serial.print("🔄 Main loop cycle: ");
            Serial.println(loopCount);
            xSemaphoreGive(serialMutex);
        }
    }
}

/*
 * FreeRTOS Concepts Explained:
 * 
 * 1. TASKS: Independent pieces of code that run "simultaneously"
 *    - Each task has its own stack memory
 *    - Tasks are scheduled by priority and time-slicing
 *    - Higher priority tasks run before lower priority ones
 * 
 * 2. PRIORITIES: Determine which task runs when
 *    - 0 = Idle task (lowest)
 *    - 1-4 = Application tasks (higher = more important)
 *    - Critical tasks should have higher priority
 * 
 * 3. QUEUES: Safe way to pass data between tasks
 *    - FIFO (First In, First Out) buffer
 *    - Thread-safe (no corruption if multiple tasks use it)
 *    - Can block if queue is full or empty
 * 
 * 4. SEMAPHORES/MUTEXES: Control access to shared resources
 *    - Mutex = "Mutual Exclusion" (only one task can use resource)
 *    - Prevents corruption of shared data (like Serial output)
 *    - Essential for thread-safe programming
 * 
 * 5. TASK DELAYS: Let other tasks run
 *    - vTaskDelay() = relative delay
 *    - vTaskDelayUntil() = absolute delay (better for precise timing)
 *    - Tasks yield CPU time during delays
 * 
 * Task Design Best Practices:
 * ✅ Each task should have ONE main responsibility
 * ✅ Use queues/semaphores for communication
 * ✅ Don't use global variables without protection
 * ✅ Include delays so other tasks can run
 * ✅ Handle task cleanup properly
 * ✅ Monitor stack usage to prevent overflow
 * 
 * When to Use FreeRTOS:
 * - Multiple independent functions needed
 * - Different timing requirements
 * - Complex user interfaces
 * - Real-time responses required
 * - System needs to handle multiple sensors/actuators
 * - Need separation of concerns
 * 
 * ESP32 FreeRTOS Features:
 * - Dual-core support (tasks can run on either core)
 * - Built-in WiFi/Bluetooth tasks
 * - Arduino functions are task-safe
 * - Plenty of RAM for multiple tasks
 * - Hardware acceleration for context switching
 * 
 * Memory Considerations:
 * - Each task needs its own stack (usually 1-4KB)
 * - Queues and semaphores use heap memory
 * - Monitor free heap with ESP.getFreeHeap()
 * - Use uxTaskGetStackHighWaterMark() to check stack usage
 * 
 * Debugging Tips:
 * 1. Use task names for easier identification
 * 2. Print from one task at a time (use mutex)
 * 3. Monitor task states with uxTaskGetSystemState()
 * 4. Check stack high water marks regularly
 * 5. Use watchdog tasks for system health monitoring
 * 6. Test priority inversion scenarios
 * 7. Verify queue sizes are adequate
 */