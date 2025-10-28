/*
 * MODULE 3 - LESSON 1: ESP32 GPIO - Real Hardware Examples
 * 
 * What you'll learn:
 * - How to use ESP32 GPIO pins in real projects
 * - Setting up toolchain (Arduino IDE basics)
 * - Real code that runs on ESP32 hardware
 * - Common pin configurations and connections
 * 
 * This is where theory meets reality!
 * From now on, code examples can run on actual ESP32 hardware.
 * 
 * NOTE: This code is written for Arduino IDE with ESP32 board support.
 * To use this: File → Examples → Copy this code → Upload to ESP32
 */

#include <stdio.h>

// ESP32 Pin Definitions (these are the real pin numbers!)
#define LED_BUILTIN     2   // ESP32 has built-in LED on GPIO 2
#define BUTTON_PIN      0   // GPIO 0 (also BOOT button on most ESP32 boards)
#define EXTERNAL_LED    4   // GPIO 4 for external LED
#define BUZZER_PIN      5   // GPIO 5 for buzzer/speaker
#define SENSOR_PIN      18  // GPIO 18 for digital sensor

// Function prototypes (Arduino IDE needs these)
void setup();
void loop();
void basicLedControl();
void buttonControl();
void multipleOutputs();
void inputOutputCombo();

/*
 * ARDUINO SETUP FUNCTION
 * This runs ONCE when ESP32 starts up
 * Use it to configure pins and initialize hardware
 */
void setup() {
    // Initialize serial communication (for debugging)
    Serial.begin(115200);  // 115200 baud rate
    
    // Wait a moment for serial to initialize
    delay(1000);
    
    Serial.println("=== ESP32 GPIO Examples Starting ===");
    Serial.println("Open Serial Monitor to see output!");
    
    // Configure pins
    pinMode(LED_BUILTIN, OUTPUT);   // Built-in LED as output
    pinMode(EXTERNAL_LED, OUTPUT);  // External LED as output  
    pinMode(BUZZER_PIN, OUTPUT);    // Buzzer as output
    pinMode(BUTTON_PIN, INPUT_PULLUP); // Button with internal pull-up
    pinMode(SENSOR_PIN, INPUT);     // Sensor as input
    
    Serial.println("GPIO pins configured successfully!");
    Serial.println();
}

/*
 * ARDUINO LOOP FUNCTION  
 * This runs CONTINUOUSLY after setup() completes
 * Put your main program logic here
 */
void loop() {
    Serial.println("Choose an example:");
    Serial.println("1. Basic LED Control");
    Serial.println("2. Button Control");
    Serial.println("3. Multiple Outputs");
    Serial.println("4. Input/Output Combination");
    Serial.println();
    
    // Run each example with delays between them
    basicLedControl();
    delay(2000);
    
    buttonControl();
    delay(2000);
    
    multipleOutputs();  
    delay(2000);
    
    inputOutputCombo();
    delay(5000);  // Longer delay before repeating
}

/*
 * EXAMPLE 1: Basic LED Control
 * The "Hello World" of embedded systems!
 */
void basicLedControl() {
    Serial.println("=== Example 1: Basic LED Control ===");
    
    // Turn on built-in LED
    digitalWrite(LED_BUILTIN, HIGH);  // HIGH = 3.3V = LED ON
    Serial.println("Built-in LED ON");
    delay(500);
    
    // Turn off built-in LED
    digitalWrite(LED_BUILTIN, LOW);   // LOW = 0V = LED OFF
    Serial.println("Built-in LED OFF");
    delay(500);
    
    // Blink pattern
    Serial.println("Blinking pattern (3 times):");
    for(int i = 0; i < 3; i++) {
        digitalWrite(LED_BUILTIN, HIGH);
        Serial.print("Blink ");
        Serial.print(i + 1);
        Serial.println(" - ON");
        delay(200);
        
        digitalWrite(LED_BUILTIN, LOW);
        Serial.println("         OFF");
        delay(200);
    }
    
    Serial.println("LED control example complete!");
    Serial.println();
}

/*
 * EXAMPLE 2: Button Control
 * Read button state and respond to presses
 */
void buttonControl() {
    Serial.println("=== Example 2: Button Control ===");
    Serial.println("Press BOOT button on ESP32 for 5 seconds...");
    
    uint32_t start_time = millis();  // Get current time in milliseconds
    uint8_t button_presses = 0;
    uint8_t last_button_state = HIGH;  // Button not pressed (pull-up)
    
    // Monitor button for 5 seconds
    while(millis() - start_time < 5000) {
        uint8_t current_button_state = digitalRead(BUTTON_PIN);
        
        // Detect button press (HIGH to LOW transition)
        if(last_button_state == HIGH && current_button_state == LOW) {
            button_presses++;
            Serial.print("Button pressed! Count: ");
            Serial.println(button_presses);
            
            // Flash LED to confirm button press
            digitalWrite(LED_BUILTIN, HIGH);
            delay(100);
            digitalWrite(LED_BUILTIN, LOW);
        }
        
        last_button_state = current_button_state;
        delay(50);  // Small delay to debounce button
    }
    
    Serial.print("Total button presses detected: ");
    Serial.println(button_presses);
    Serial.println("Button control example complete!");
    Serial.println();
}

/*
 * EXAMPLE 3: Multiple Outputs
 * Control several outputs in patterns
 */
void multipleOutputs() {
    Serial.println("=== Example 3: Multiple Outputs ===");
    
    // Turn on outputs one by one
    Serial.println("Turning on outputs sequentially:");
    
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("  Built-in LED ON");
    delay(300);
    
    digitalWrite(EXTERNAL_LED, HIGH);  
    Serial.println("  External LED ON");
    delay(300);
    
    digitalWrite(BUZZER_PIN, HIGH);
    Serial.println("  Buzzer ON");
    delay(300);
    
    // Turn off outputs one by one
    Serial.println("Turning off outputs sequentially:");
    
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("  Built-in LED OFF");
    delay(300);
    
    digitalWrite(EXTERNAL_LED, LOW);
    Serial.println("  External LED OFF");  
    delay(300);
    
    digitalWrite(BUZZER_PIN, LOW);
    Serial.println("  Buzzer OFF");
    delay(300);
    
    // Pattern: Alternating LEDs
    Serial.println("Alternating pattern (3 cycles):");
    for(int i = 0; i < 3; i++) {
        // Pattern A
        digitalWrite(LED_BUILTIN, HIGH);
        digitalWrite(EXTERNAL_LED, LOW);
        Serial.println("  Pattern A: Built-in ON, External OFF");
        delay(200);
        
        // Pattern B  
        digitalWrite(LED_BUILTIN, LOW);
        digitalWrite(EXTERNAL_LED, HIGH);
        Serial.println("  Pattern B: Built-in OFF, External ON");
        delay(200);
    }
    
    // All off
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(EXTERNAL_LED, LOW);
    Serial.println("Multiple outputs example complete!");
    Serial.println();
}

/*
 * EXAMPLE 4: Input/Output Combination
 * Read inputs and control outputs based on the readings
 */
void inputOutputCombo() {
    Serial.println("=== Example 4: Input/Output Combination ===");
    Serial.println("Interactive mode for 10 seconds:");
    Serial.println("- Press BOOT button to control LED");
    Serial.println("- Sensor input affects buzzer");
    
    uint32_t start_time = millis();
    uint8_t led_state = LOW;
    uint8_t last_button_state = HIGH;
    
    while(millis() - start_time < 10000) {  // Run for 10 seconds
        // Read inputs
        uint8_t button_state = digitalRead(BUTTON_PIN);
        uint8_t sensor_state = digitalRead(SENSOR_PIN);
        
        // Button controls LED (toggle on press)
        if(last_button_state == HIGH && button_state == LOW) {
            led_state = !led_state;  // Toggle LED state
            digitalWrite(LED_BUILTIN, led_state);
            
            Serial.print("Button pressed! LED is now ");
            Serial.println(led_state ? "ON" : "OFF");
        }
        
        // Sensor controls buzzer (direct connection)
        digitalWrite(BUZZER_PIN, sensor_state);
        
        // Show status every 2 seconds
        static uint32_t last_status_time = 0;
        if(millis() - last_status_time > 2000) {
            Serial.print("Status: LED=");
            Serial.print(led_state ? "ON" : "OFF");
            Serial.print(", Button=");
            Serial.print(button_state ? "Released" : "Pressed");
            Serial.print(", Sensor=");
            Serial.print(sensor_state ? "HIGH" : "LOW");
            Serial.print(", Buzzer=");
            Serial.println(sensor_state ? "ON" : "OFF");
            
            last_status_time = millis();
        }
        
        last_button_state = button_state;
        delay(50);  // Small delay for stability
    }
    
    // Turn everything off
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(EXTERNAL_LED, LOW); 
    digitalWrite(BUZZER_PIN, LOW);
    
    Serial.println("Input/Output combination example complete!");
    Serial.println();
}

/*
 * ADDITIONAL HELPER FUNCTIONS
 * These show common GPIO patterns you'll use often
 */

// Function: Blink LED a specific number of times
void blinkLED(uint8_t pin, uint8_t times, uint16_t delay_ms) {
    for(uint8_t i = 0; i < times; i++) {
        digitalWrite(pin, HIGH);
        delay(delay_ms);
        digitalWrite(pin, LOW);
        delay(delay_ms);
    }
}

// Function: Wait for button press with timeout
bool waitForButtonPress(uint8_t pin, uint32_t timeout_ms) {
    uint32_t start_time = millis();
    
    while(millis() - start_time < timeout_ms) {
        if(digitalRead(pin) == LOW) {  // Button pressed (assuming pull-up)
            return true;
        }
        delay(10);
    }
    
    return false;  // Timeout occurred
}

// Function: Read multiple digital inputs into a byte
uint8_t readInputBank(uint8_t pins[], uint8_t count) {
    uint8_t result = 0;
    
    for(uint8_t i = 0; i < count; i++) {
        if(digitalRead(pins[i])) {
            result |= (1 << i);  // Set bit i if pin is HIGH
        }
    }
    
    return result;
}

/*
 * HARDWARE SETUP INSTRUCTIONS:
 * 
 * For these examples to work, connect:
 * 
 * 1. Built-in LED (GPIO 2): Already connected on most ESP32 boards
 * 
 * 2. External LED (GPIO 4):
 *    - Long leg (anode) → 220Ω resistor → GPIO 4
 *    - Short leg (cathode) → GND
 * 
 * 3. Buzzer (GPIO 5):
 *    - Positive → GPIO 5
 *    - Negative → GND
 *    
 * 4. Button (GPIO 0): Usually built-in as BOOT button
 *    - If external: One side → GPIO 0, Other side → GND
 *    - Internal pull-up resistor is enabled in code
 * 
 * 5. Digital Sensor (GPIO 18):
 *    - Signal → GPIO 18
 *    - VCC → 3.3V, GND → GND
 *    - Or just connect GPIO 18 to 3.3V/GND for testing
 */

/*
 * ARDUINO IDE SETUP INSTRUCTIONS:
 * 
 * 1. Install ESP32 board support:
 *    - File → Preferences
 *    - Additional Board Manager URLs: 
 *      https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
 *    - Tools → Board → Boards Manager → Search "ESP32" → Install
 * 
 * 2. Select your ESP32 board:
 *    - Tools → Board → ESP32 Arduino → ESP32 Dev Module (or your specific board)
 * 
 * 3. Select COM port:
 *    - Tools → Port → (select your ESP32's port)
 * 
 * 4. Upload code:
 *    - Click Upload button (→)
 *    - Open Serial Monitor to see output (Ctrl+Shift+M)
 * 
 * 5. If upload fails:
 *    - Hold BOOT button while clicking Upload
 *    - Release BOOT button when "Connecting..." appears
 */

/*
 * What did we learn?
 * 
 * 1. Real ESP32 GPIO control using Arduino IDE
 * 2. pinMode() configures pins as INPUT or OUTPUT
 * 3. digitalWrite() sets output pins HIGH (3.3V) or LOW (0V)
 * 4. digitalRead() reads input pin states
 * 5. INPUT_PULLUP enables internal pull-up resistors
 * 6. Serial.print() sends debug messages to computer
 * 7. delay() creates timing for patterns and debouncing
 * 8. Real hardware connections and setup procedures
 * 
 * Next: UART communication - talking to sensors and computers!
 */