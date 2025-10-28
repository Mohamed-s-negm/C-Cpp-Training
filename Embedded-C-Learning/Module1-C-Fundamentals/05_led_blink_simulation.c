/*
 * LESSON 5: LED Blink - Your First Embedded Program!
 * 
 * What you'll learn:
 * - How to simulate controlling an LED with code
 * - Timing and delays in embedded systems
 * - Bringing together everything we learned
 * - How real LED control works (concept)
 * 
 * This is like the "Hello World" of embedded systems!
 * Instead of printing text, we blink an LED.
 */

#include <stdio.h>
#include <stdint.h>

// Simulate hardware registers (in real embedded systems, these are real addresses)
volatile uint8_t *GPIO_OUTPUT_REG = (volatile uint8_t*)0x12345678;  // Fake address
volatile uint8_t simulated_gpio_register = 0;                        // Our fake hardware

// LED pin definitions (which bit controls which LED)
#define LED_RED_PIN     0   // LED connected to pin 0 (bit 0)
#define LED_GREEN_PIN   1   // LED connected to pin 1 (bit 1)  
#define LED_BLUE_PIN    2   // LED connected to pin 2 (bit 2)

// Simple bit control macros (from our previous lesson)
#define LED_ON(pin)     (simulated_gpio_register |= (1 << (pin)))
#define LED_OFF(pin)    (simulated_gpio_register &= ~(1 << (pin)))
#define LED_TOGGLE(pin) (simulated_gpio_register ^= (1 << (pin)))
#define LED_STATE(pin)  ((simulated_gpio_register >> (pin)) & 1)

// Simple delay function (in real embedded systems, this would be more sophisticated)
void simple_delay(uint32_t milliseconds)
{
    // This is a VERY simplified delay
    // In real embedded systems, you'd use hardware timers
    
    printf("  [Waiting %d ms...]\n", (int)milliseconds);
    
    // Simulate delay with a simple loop
    // Note: This is not accurate timing, just for demonstration!
    for (uint32_t i = 0; i < milliseconds * 1000; i++) {
        // Do nothing - just waste time
        volatile uint32_t dummy = i; // volatile prevents optimization
    }
}

// Show current LED states in a nice format
void show_led_states(void)
{
    printf("LED States: ");
    printf("Red=%s ", LED_STATE(LED_RED_PIN) ? "ON " : "OFF");
    printf("Green=%s ", LED_STATE(LED_GREEN_PIN) ? "ON " : "OFF");
    printf("Blue=%s ", LED_STATE(LED_BLUE_PIN) ? "ON " : "OFF");
    printf("(Register: 0b");
    
    // Show binary representation
    for (int i = 7; i >= 0; i--) {
        printf("%d", (simulated_gpio_register >> i) & 1);
    }
    printf(")\n");
}

// Basic LED blink example
void basic_led_blink(void)
{
    printf("=== Basic LED Blink Example ===\n");
    
    // Start with all LEDs off
    simulated_gpio_register = 0;
    printf("Starting with all LEDs off:\n");
    show_led_states();
    
    // Blink the red LED 3 times
    printf("\nBlinking red LED 3 times:\n");
    for (int i = 0; i < 3; i++) {
        printf("Blink %d:\n", i + 1);
        
        // Turn LED on
        LED_ON(LED_RED_PIN);
        printf("  Red LED ON\n");
        show_led_states();
        simple_delay(500);  // Wait 500ms
        
        // Turn LED off
        LED_OFF(LED_RED_PIN);
        printf("  Red LED OFF\n");
        show_led_states();
        simple_delay(500);  // Wait 500ms
    }
}

// Multiple LED patterns
void led_patterns(void)
{
    printf("\n=== LED Patterns ===\n");
    
    // Pattern 1: Turn on LEDs one by one
    printf("Pattern 1: Sequential turn-on\n");
    simulated_gpio_register = 0;  // Start with all off
    
    LED_ON(LED_RED_PIN);
    printf("Red ON: ");
    show_led_states();
    simple_delay(300);
    
    LED_ON(LED_GREEN_PIN);
    printf("Green ON: ");
    show_led_states();
    simple_delay(300);
    
    LED_ON(LED_BLUE_PIN);
    printf("Blue ON: ");
    show_led_states();
    simple_delay(300);
    
    // Pattern 2: Turn off LEDs one by one
    printf("\nPattern 2: Sequential turn-off\n");
    
    LED_OFF(LED_RED_PIN);
    printf("Red OFF: ");
    show_led_states();
    simple_delay(300);
    
    LED_OFF(LED_GREEN_PIN);
    printf("Green OFF: ");
    show_led_states();
    simple_delay(300);
    
    LED_OFF(LED_BLUE_PIN);
    printf("Blue OFF: ");
    show_led_states();
    simple_delay(300);
}

// Advanced pattern: Traffic light simulation
void traffic_light_simulation(void)
{
    printf("\n=== Traffic Light Simulation ===\n");
    printf("Red = Stop, Green = Go, Blue = Caution\n\n");
    
    // Traffic light cycle
    const char* states[] = {"STOP", "CAUTION", "GO"};
    const uint8_t led_pins[] = {LED_RED_PIN, LED_BLUE_PIN, LED_GREEN_PIN};
    const uint32_t durations[] = {2000, 500, 2000};  // Time for each state
    
    for (int cycle = 0; cycle < 2; cycle++) {  // Run 2 complete cycles
        printf("Traffic Light Cycle %d:\n", cycle + 1);
        
        for (int state = 0; state < 3; state++) {
            // Turn off all LEDs
            simulated_gpio_register = 0;
            
            // Turn on current state LED
            LED_ON(led_pins[state]);
            
            printf("  %s - ", states[state]);
            show_led_states();
            simple_delay(durations[state]);
        }
        printf("\n");
    }
}

// Demonstrate different timing patterns
void timing_patterns(void)
{
    printf("\n=== Different Timing Patterns ===\n");
    
    // Fast blink
    printf("Fast blink (5 times):\n");
    for (int i = 0; i < 5; i++) {
        LED_TOGGLE(LED_GREEN_PIN);
        show_led_states();
        simple_delay(100);  // Very fast
    }
    
    // Slow blink  
    printf("\nSlow blink (3 times):\n");
    LED_OFF(LED_GREEN_PIN);  // Make sure it's off
    for (int i = 0; i < 3; i++) {
        LED_TOGGLE(LED_GREEN_PIN);
        show_led_states();
        simple_delay(1000);  // Slow
    }
    
    // Heartbeat pattern (double blink)
    printf("\nHeartbeat pattern (3 heartbeats):\n");
    LED_OFF(LED_GREEN_PIN);
    for (int i = 0; i < 3; i++) {
        // First beat
        LED_ON(LED_GREEN_PIN);
        simple_delay(100);
        LED_OFF(LED_GREEN_PIN);
        simple_delay(100);
        
        // Second beat  
        LED_ON(LED_GREEN_PIN);
        simple_delay(100);
        LED_OFF(LED_GREEN_PIN);
        simple_delay(800);  // Long pause
        
        printf("  Heartbeat %d\n", i + 1);
    }
}

// Simulate real embedded system behavior
void embedded_system_simulation(void)
{
    printf("\n=== Embedded System Simulation ===\n");
    
    // Simulate system status with LEDs
    uint8_t system_ready = 0;
    uint8_t sensor_ok = 0;
    uint8_t communication_active = 0;
    
    printf("System starting up...\n");
    
    // Step 1: System initialization
    printf("1. Initializing system...\n");
    simple_delay(500);
    system_ready = 1;
    LED_ON(LED_RED_PIN);  // Red = System Ready
    printf("   System ready! ");
    show_led_states();
    
    // Step 2: Sensor check
    printf("2. Checking sensors...\n");
    simple_delay(300);
    sensor_ok = 1;
    LED_ON(LED_GREEN_PIN);  // Green = Sensors OK
    printf("   Sensors OK! ");
    show_led_states();
    
    // Step 3: Communication test
    printf("3. Testing communication...\n");
    simple_delay(400);
    communication_active = 1;
    LED_ON(LED_BLUE_PIN);  // Blue = Communication Active
    printf("   Communication active! ");
    show_led_states();
    
    // Step 4: Normal operation (blink blue to show activity)
    printf("4. Normal operation - blinking blue for activity\n");
    for (int i = 0; i < 5; i++) {
        LED_TOGGLE(LED_BLUE_PIN);
        printf("   Activity blink %d: ", i + 1);
        show_led_states();
        simple_delay(200);
    }
    
    printf("System running normally!\n");
}

// Show how this relates to real embedded systems
void real_world_explanation(void)
{
    printf("\n=== How This Works in Real Embedded Systems ===\n");
    
    printf("In this simulation:\n");
    printf("- We used a variable 'simulated_gpio_register' to represent hardware\n");
    printf("- We used bit operations to control individual LEDs\n");
    printf("- We used delays to create timing\n\n");
    
    printf("In a REAL embedded system (like ESP32):\n");
    printf("- GPIO_OUTPUT_REG would be a real hardware address (like 0x60004004)\n");
    printf("- Writing to that address directly controls physical pins\n");
    printf("- LEDs connected to those pins would actually light up!\n");
    printf("- Delays would use hardware timers for accurate timing\n\n");
    
    printf("The concepts are exactly the same:\n");
    printf("1. Use pointers to access hardware registers\n");
    printf("2. Use bit operations to control individual pins\n");
    printf("3. Use timing to create patterns\n");
    printf("4. Use status LEDs to show system state\n");
}

int main(void)
{
    printf("Welcome to LED Blink - Your First Embedded Program!\n");
    printf("==================================================\n");
    
    // Basic LED control
    basic_led_blink();
    
    // Show different patterns
    led_patterns();
    
    // Traffic light example
    traffic_light_simulation();
    
    // Different timing patterns
    timing_patterns();
    
    // Complete system simulation
    embedded_system_simulation();
    
    // Explain real-world connection
    real_world_explanation();
    
    printf("\n=== What You Learned ===\n");
    printf("1. LED control uses bit operations on hardware registers\n");
    printf("2. Timing creates patterns and sequences\n");
    printf("3. LEDs can show system status and activity\n");
    printf("4. The same concepts work on real hardware\n");
    printf("5. Embedded programming combines all our lessons:\n");
    printf("   - Data types (uint8_t for register values)\n");
    printf("   - Pointers (to access hardware registers)\n");
    printf("   - Bit operations (to control individual pins)\n");
    printf("   - Memory layout (volatile for hardware registers)\n");
    
    printf("\n🎉 Congratulations! You've learned the fundamentals of embedded C!\n");
    printf("Next: We'll learn about real microcontroller architecture!\n");
    
    return 0;
}

/*
 * What did we learn?
 * 
 * 1. LED blink is the "Hello World" of embedded systems
 * 2. Hardware registers are controlled using pointers and bit operations
 * 3. Timing creates patterns and sequences
 * 4. LEDs can indicate system status (ready, error, activity, etc.)
 * 5. Real embedded systems work exactly like this simulation
 * 6. All our previous lessons come together in practical applications
 * 
 * You now understand:
 * ✓ Data types for embedded systems
 * ✓ Pointers for hardware access
 * ✓ Bit operations for pin control
 * ✓ Memory layout and volatile
 * ✓ Putting it all together in a real program
 * 
 * Ready for the next module: Microcontroller Architecture!
 */