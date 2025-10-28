/*
 * MODULE 2 - LESSON 3: GPIO Control - Your Digital Fingers
 * 
 * What you'll learn:
 * - How to configure GPIO pins for different functions
 * - Pull-up and pull-down resistors (why buttons need them)
 * - Digital input and output in detail
 * - Common GPIO applications and patterns
 * 
 * Think of GPIO pins like your fingers:
 * - They can push things (output HIGH/LOW)
 * - They can feel things (input HIGH/LOW)
 * - They need to be "configured" for what they're doing
 * - Each finger/pin can do only one thing at a time
 */

#include <stdio.h>
#include <stdint.h>

// Simulated GPIO registers for ESP32
volatile uint32_t GPIO_OUT_REG = 0;         // Pin output levels (HIGH/LOW)
volatile uint32_t GPIO_ENABLE_REG = 0;      // Pin direction (input/output)
volatile uint32_t GPIO_IN_REG = 0x12345678; // Pin input readings
volatile uint32_t GPIO_PULLUP_REG = 0;      // Internal pull-up enable
volatile uint32_t GPIO_PULLDOWN_REG = 0;    // Internal pull-down enable

// Pin definitions for our examples
#define LED_PIN         2   // LED connected to pin 2
#define BUTTON_PIN      0   // Button connected to pin 0
#define BUZZER_PIN      4   // Buzzer connected to pin 4
#define SENSOR_PIN      5   // Digital sensor on pin 5

// GPIO configuration modes
typedef enum {
    GPIO_MODE_INPUT,        // Pin reads digital signals
    GPIO_MODE_OUTPUT,       // Pin outputs digital signals
    GPIO_MODE_INPUT_PULLUP, // Input with internal pull-up resistor
    GPIO_MODE_INPUT_PULLDOWN // Input with internal pull-down resistor
} gpio_mode_t;

// Simple functions to make GPIO control easier
void gpio_set_mode(uint8_t pin, gpio_mode_t mode)
{
    switch(mode) {
        case GPIO_MODE_OUTPUT:
            GPIO_ENABLE_REG |= (1 << pin);      // Enable as output
            GPIO_PULLUP_REG &= ~(1 << pin);     // Disable pull-up
            GPIO_PULLDOWN_REG &= ~(1 << pin);   // Disable pull-down
            printf("Pin %d configured as OUTPUT\n", pin);
            break;
            
        case GPIO_MODE_INPUT:
            GPIO_ENABLE_REG &= ~(1 << pin);     // Disable output (= input)
            GPIO_PULLUP_REG &= ~(1 << pin);     // Disable pull-up
            GPIO_PULLDOWN_REG &= ~(1 << pin);   // Disable pull-down
            printf("Pin %d configured as INPUT (floating)\n", pin);
            break;
            
        case GPIO_MODE_INPUT_PULLUP:
            GPIO_ENABLE_REG &= ~(1 << pin);     // Disable output (= input)
            GPIO_PULLUP_REG |= (1 << pin);      // Enable pull-up
            GPIO_PULLDOWN_REG &= ~(1 << pin);   // Disable pull-down
            printf("Pin %d configured as INPUT with PULL-UP\n", pin);
            break;
            
        case GPIO_MODE_INPUT_PULLDOWN:
            GPIO_ENABLE_REG &= ~(1 << pin);     // Disable output (= input)
            GPIO_PULLUP_REG &= ~(1 << pin);     // Disable pull-up
            GPIO_PULLDOWN_REG |= (1 << pin);    // Enable pull-down
            printf("Pin %d configured as INPUT with PULL-DOWN\n", pin);
            break;
    }
}

void gpio_write(uint8_t pin, uint8_t level)
{
    if (level) {
        GPIO_OUT_REG |= (1 << pin);   // Set pin HIGH (3.3V)
        printf("Pin %d set to HIGH (3.3V)\n", pin);
    } else {
        GPIO_OUT_REG &= ~(1 << pin);  // Set pin LOW (0V)
        printf("Pin %d set to LOW (0V)\n", pin);
    }
}

uint8_t gpio_read(uint8_t pin)
{
    uint8_t level = (GPIO_IN_REG >> pin) & 1;
    printf("Pin %d reads: %s\n", pin, level ? "HIGH" : "LOW");
    return level;
}

// Basic LED control example
void basic_led_control(void)
{
    printf("=== Basic LED Control ===\n");
    
    // Step 1: Configure LED pin as output
    printf("Step 1: Configure pin %d for LED output\n", LED_PIN);
    gpio_set_mode(LED_PIN, GPIO_MODE_OUTPUT);
    
    printf("LED pin registers after config:\n");
    printf("  GPIO_ENABLE_REG: 0x%08X (bit %d = %d)\n", 
           GPIO_ENABLE_REG, LED_PIN, (GPIO_ENABLE_REG >> LED_PIN) & 1);
    
    // Step 2: Turn LED on and off
    printf("\nStep 2: Control the LED\n");
    gpio_write(LED_PIN, 1);  // LED ON
    printf("LED should be glowing now!\n");
    
    gpio_write(LED_PIN, 0);  // LED OFF
    printf("LED should be dark now!\n");
    
    printf("Current GPIO_OUT_REG: 0x%08X\n", GPIO_OUT_REG);
}

// Button reading with pull-up resistors
void button_reading_example(void)
{
    printf("\n=== Button Reading with Pull-up ===\n");
    
    printf("Why do buttons need pull-up resistors?\n");
    printf("- Button pressed: pin connects to GND (0V) = LOW\n");
    printf("- Button released: pin connects to... nothing! = ??? (floating)\n");
    printf("- Pull-up resistor: pulls pin to 3.3V when not pressed = HIGH\n");
    printf("- Result: Button pressed = LOW, Button released = HIGH\n\n");
    
    // Configure button pin with pull-up
    printf("Configuring button pin %d with internal pull-up:\n", BUTTON_PIN);
    gpio_set_mode(BUTTON_PIN, GPIO_MODE_INPUT_PULLUP);
    
    printf("Button pin registers after config:\n");
    printf("  GPIO_ENABLE_REG: 0x%08X (bit %d = %d - input mode)\n", 
           GPIO_ENABLE_REG, BUTTON_PIN, (GPIO_ENABLE_REG >> BUTTON_PIN) & 1);
    printf("  GPIO_PULLUP_REG: 0x%08X (bit %d = %d - pull-up enabled)\n",
           GPIO_PULLUP_REG, BUTTON_PIN, (GPIO_PULLUP_REG >> BUTTON_PIN) & 1);
    
    // Simulate button states
    printf("\nSimulating button presses:\n");
    
    // Simulate button not pressed (pull-up pulls to HIGH)
    GPIO_IN_REG |= (1 << BUTTON_PIN);  // Simulate HIGH
    uint8_t button_state = gpio_read(BUTTON_PIN);
    printf("Button not pressed, pin reads: %s ✓\n", button_state ? "HIGH" : "LOW");
    
    // Simulate button pressed (connected to GND)
    GPIO_IN_REG &= ~(1 << BUTTON_PIN); // Simulate LOW
    button_state = gpio_read(BUTTON_PIN);
    printf("Button pressed, pin reads: %s ✓\n", button_state ? "HIGH" : "LOW");
}

// LED and button together - interactive control
void led_button_interactive(void)
{
    printf("\n=== LED + Button Interactive Control ===\n");
    
    // Setup hardware
    printf("Setting up LED and button:\n");
    gpio_set_mode(LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_mode(BUTTON_PIN, GPIO_MODE_INPUT_PULLUP);
    
    // Initial states
    gpio_write(LED_PIN, 0);  // LED starts OFF
    
    printf("\nSimulating button press sequence:\n");
    
    // Sequence of button presses and LED responses
    struct {
        uint8_t button_pressed;
        const char* description;
    } sequence[] = {
        {0, "Button released"},
        {1, "Button pressed - turn LED ON"},
        {0, "Button released - LED stays ON"}, 
        {1, "Button pressed again - turn LED OFF"},
        {0, "Button released - LED stays OFF"}
    };
    
    uint8_t led_state = 0;  // Track LED state
    
    for (int i = 0; i < 5; i++) {
        printf("\nStep %d: %s\n", i+1, sequence[i].description);
        
        // Simulate button input
        if (sequence[i].button_pressed) {
            GPIO_IN_REG &= ~(1 << BUTTON_PIN);  // Button pressed = LOW
        } else {
            GPIO_IN_REG |= (1 << BUTTON_PIN);   // Button released = HIGH  
        }
        
        // Read button state
        uint8_t current_button = gpio_read(BUTTON_PIN);
        
        // Simple button logic: toggle LED when button is pressed
        if (current_button == 0 && sequence[i].button_pressed) {  // Button press detected
            led_state = !led_state;  // Toggle LED state
            gpio_write(LED_PIN, led_state);
            printf("Button press detected! LED toggled to %s\n", led_state ? "ON" : "OFF");
        }
        
        printf("Current states: Button=%s, LED=%s\n", 
               current_button ? "Released" : "Pressed",
               led_state ? "ON" : "OFF");
    }
}

// Multiple GPIO pins working together
void multiple_pin_example(void)
{
    printf("\n=== Multiple GPIO Pins Working Together ===\n");
    
    // Setup multiple pins
    printf("Setting up multiple pins:\n");
    gpio_set_mode(LED_PIN, GPIO_MODE_OUTPUT);      // LED output
    gpio_set_mode(BUZZER_PIN, GPIO_MODE_OUTPUT);   // Buzzer output
    gpio_set_mode(BUTTON_PIN, GPIO_MODE_INPUT_PULLUP); // Button input
    gpio_set_mode(SENSOR_PIN, GPIO_MODE_INPUT);    // Sensor input
    
    printf("\nPin configuration summary:\n");
    printf("Pin %d (LED): Output\n", LED_PIN);
    printf("Pin %d (BUZZER): Output\n", BUZZER_PIN);
    printf("Pin %d (BUTTON): Input with pull-up\n", BUTTON_PIN);
    printf("Pin %d (SENSOR): Input (floating)\n", SENSOR_PIN);
    
    // Create alarm system logic
    printf("\nSimulating a simple alarm system:\n");
    
    // Simulate sensor triggered
    GPIO_IN_REG |= (1 << SENSOR_PIN);  // Sensor detects something (HIGH)
    printf("1. Motion sensor triggered!\n");
    gpio_read(SENSOR_PIN);
    
    // Turn on LED and buzzer
    printf("2. Activating alarm (LED + Buzzer):\n");
    gpio_write(LED_PIN, 1);    // LED ON
    gpio_write(BUZZER_PIN, 1); // Buzzer ON
    
    printf("3. Alarm is active! LED blinking, buzzer sounding...\n");
    
    // Simulate button press to turn off alarm
    GPIO_IN_REG &= ~(1 << BUTTON_PIN); // Button pressed
    printf("4. Button pressed to turn off alarm:\n");
    gpio_read(BUTTON_PIN);
    
    gpio_write(LED_PIN, 0);    // LED OFF
    gpio_write(BUZZER_PIN, 0); // Buzzer OFF
    printf("5. Alarm deactivated!\n");
    
    printf("\nFinal GPIO register states:\n");
    printf("GPIO_OUT_REG: 0x%08X\n", GPIO_OUT_REG);
    printf("GPIO_ENABLE_REG: 0x%08X\n", GPIO_ENABLE_REG);
    printf("GPIO_PULLUP_REG: 0x%08X\n", GPIO_PULLUP_REG);
}

// Explain common GPIO problems and solutions
void gpio_troubleshooting(void)
{
    printf("\n=== Common GPIO Problems and Solutions ===\n");
    
    printf("1. PROBLEM: Button readings are random/noisy\n");
    printf("   CAUSE: No pull-up/pull-down resistor\n");
    printf("   SOLUTION: Enable internal pull-up for buttons\n");
    printf("   CODE: gpio_set_mode(pin, GPIO_MODE_INPUT_PULLUP);\n\n");
    
    printf("2. PROBLEM: LED won't turn on\n");
    printf("   CAUSES:\n");
    printf("   - Pin not configured as output\n");
    printf("   - Pin set to LOW instead of HIGH\n");
    printf("   - LED connected backwards\n");
    printf("   SOLUTIONS: Check pin config, check voltage level, check wiring\n\n");
    
    printf("3. PROBLEM: Multiple pins interfering with each other\n");
    printf("   CAUSE: Writing to entire register instead of individual bits\n");
    printf("   WRONG: GPIO_OUT_REG = (1 << 2);  // This clears all other pins!\n");
    printf("   RIGHT: GPIO_OUT_REG |= (1 << 2); // This only sets pin 2\n\n");
    
    printf("4. PROBLEM: GPIO state changes unexpectedly\n");
    printf("   CAUSES:\n");
    printf("   - Forgot 'volatile' keyword\n");
    printf("   - Race condition with interrupts\n");
    printf("   - Hardware malfunction\n");
    printf("   SOLUTIONS: Use volatile, disable interrupts during critical sections\n\n");
    
    printf("5. PROBLEM: Pin doesn't work as expected\n");
    printf("   CAUSES:\n");
    printf("   - Pin has special function (I2C, SPI, etc.)\n");
    printf("   - Pin is input-only or output-only\n");
    printf("   - Pin is being used by bootloader\n");
    printf("   SOLUTION: Check ESP32 pinout diagram and datasheet\n");
}

int main(void)
{
    printf("Welcome to GPIO Control!\n");
    printf("=======================\n");
    
    // Basic LED control
    basic_led_control();
    
    // Button reading
    button_reading_example();
    
    // Interactive LED + button
    led_button_interactive();
    
    // Multiple pins working together
    multiple_pin_example();
    
    // Troubleshooting guide
    gpio_troubleshooting();
    
    printf("\n=== GPIO Best Practices ===\n");
    printf("1. Always configure pin mode before using\n");
    printf("2. Use pull-up resistors for buttons and switches\n");
    printf("3. Use bit operations to modify individual pins\n");
    printf("4. Check ESP32 pinout for special pin functions\n");
    printf("5. Use meaningful names for pin definitions\n");
    printf("6. Document your pin assignments clearly\n");
    printf("7. Test each pin individually before combining\n");
    
    printf("\n=== Key Concepts Learned ===\n");
    printf("✓ GPIO pins can be inputs or outputs\n");
    printf("✓ Pull-up resistors prevent floating inputs\n");
    printf("✓ Digital signals are either HIGH (3.3V) or LOW (0V)\n");
    printf("✓ Register bits control individual pin functions\n");
    printf("✓ Multiple pins can work together for complex functions\n");
    printf("✓ Proper configuration prevents common problems\n");
    
    return 0;
}

/*
 * What did we learn?
 * 
 * 1. GPIO pins are your interface to the physical world
 * 2. Pins must be configured before use (input vs output)
 * 3. Pull-up/pull-down resistors prevent floating inputs  
 * 4. Digital signals are binary: HIGH (3.3V) or LOW (0V)
 * 5. Bit operations let you control individual pins safely
 * 6. Multiple pins can work together for complex applications
 * 7. Common problems have known solutions and workarounds
 * 8. Proper planning and testing prevents GPIO issues
 * 
 * Next: Interrupts and ISR design - responding to events instantly!
 */