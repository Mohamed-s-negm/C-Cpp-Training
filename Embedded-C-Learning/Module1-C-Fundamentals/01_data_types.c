/*
 * LESSON 1: Data Types for Embedded Systems
 * 
 * What you'll learn:
 * - Why we use specific data types in embedded systems
 * - How to save memory by choosing the right size
 * - What 'volatile' means and when to use it
 * 
 * Think of it like choosing the right size container:
 * - Don't use a big box for a small item (wastes space)
 * - Don't use a small box for a big item (won't fit)
 */

#include <stdio.h>
#include <stdint.h>  // This gives us exact-size data types

// Let's start with basic data types you know:
int normal_int = 10;        // Could be 16-bit or 32-bit (depends on system)
char normal_char = 'A';     // Always 8-bit, but can be signed or unsigned

// In embedded systems, we need to be EXACT about sizes:
uint8_t  my_byte = 200;     // Exactly 8 bits  (0 to 255)
uint16_t my_word = 50000;   // Exactly 16 bits (0 to 65,535)  
uint32_t my_long = 100000;  // Exactly 32 bits (0 to 4,294,967,295)

// Why exact sizes? Because we talk directly to hardware!
// Hardware doesn't care about "int" - it wants exact bit counts

// The 'volatile' keyword - VERY important for embedded!
volatile uint8_t button_pressed = 0;

/*
 * What does 'volatile' mean?
 * It tells the compiler: "This variable can change at ANY time"
 * 
 * Example: An interrupt can change button_pressed while your main 
 * program is running. Without 'volatile', the compiler might think
 * "I just checked this variable, it won't change" and optimize 
 * away future checks. With 'volatile', it always checks the real value.
 */

// Let's see how much memory different types use
void show_memory_usage(void) 
{
    printf("=== Memory Usage of Data Types ===\n");
    printf("uint8_t  uses %zu bytes (perfect for small numbers 0-255)\n", sizeof(uint8_t));
    printf("uint16_t uses %zu bytes (good for medium numbers 0-65,535)\n", sizeof(uint16_t));
    printf("uint32_t uses %zu bytes (big numbers, but uses more memory)\n", sizeof(uint32_t));
    printf("int      uses %zu bytes (size depends on your computer)\n", sizeof(int));
    
    printf("\nWhy this matters: If you have 1000 sensors, using uint8_t instead of int\n");
    printf("saves %zu bytes of precious memory!\n", 1000 * (sizeof(int) - sizeof(uint8_t)));
}

// Example: Storing sensor readings efficiently
void sensor_example(void)
{
    printf("\n=== Sensor Data Example ===\n");
    
    // Bad way (wastes memory):
    int temperature = 25;        // 25°C - we used 4 bytes for a small number!
    int humidity = 60;           // 60% - again, 4 bytes wasted!
    
    // Good way (saves memory):
    uint8_t temp_celsius = 25;   // Temperature: 0-100°C fits in 1 byte
    uint8_t humidity_percent = 60; // Humidity: 0-100% fits in 1 byte
    
    printf("Temperature: %d°C (using %zu bytes)\n", temp_celsius, sizeof(temp_celsius));
    printf("Humidity: %d%% (using %zu bytes)\n", humidity_percent, sizeof(humidity_percent));
    
    printf("We saved %zu bytes per sensor reading!\n", 
           (sizeof(temperature) + sizeof(humidity)) - (sizeof(temp_celsius) + sizeof(humidity_percent)));
}

// Example: When to use signed vs unsigned
void signed_vs_unsigned_example(void)
{
    printf("\n=== Signed vs Unsigned Example ===\n");
    
    // Unsigned: Only positive numbers (0 and up)
    uint8_t led_brightness = 200;  // LED brightness: 0-255 (never negative)
    
    // Signed: Can be positive or negative  
    int8_t temperature = -10;      // Temperature: can be below 0°C
    
    printf("LED Brightness: %u (unsigned - always positive)\n", led_brightness);
    printf("Temperature: %d°C (signed - can be negative)\n", temperature);
    
    // Show the ranges:
    printf("\nuint8_t range: 0 to %u\n", UINT8_MAX);
    printf("int8_t range: %d to %d\n", INT8_MIN, INT8_MAX);
}

// Example: Using volatile with hardware
void volatile_example(void)
{
    printf("\n=== Volatile Example ===\n");
    
    // Imagine this is connected to a physical button
    // The hardware can change this value at any time!
    volatile uint8_t button_state = 0;
    
    // Simulate button press (normally this would be done by hardware/interrupt)
    button_state = 1;
    
    // In real embedded code, you'd check the button like this:
    if (button_state == 1) {
        printf("Button is pressed!\n");
        // Do something when button is pressed
        button_state = 0;  // Clear the flag
    }
    
    printf("Without 'volatile', the compiler might optimize away repeated checks!\n");
}

// Simple example of choosing the right data type
void choose_right_type_example(void)
{
    printf("\n=== Choosing the Right Data Type ===\n");
    
    // Example 1: Counting loop iterations
    uint16_t loop_count;  // We might loop 1000 times, so uint8_t (max 255) isn't enough
    
    // Example 2: Store a percentage  
    uint8_t battery_percent = 85;  // 0-100% fits perfectly in uint8_t
    
    // Example 3: Store a large sensor value
    uint32_t distance_micrometers = 1500000;  // 1.5 meters in micrometers - needs uint32_t
    
    printf("Loop count uses %zu bytes (needs to count above 255)\n", sizeof(loop_count));
    printf("Battery %% uses %zu bytes (0-100 fits in 1 byte)\n", sizeof(battery_percent));
    printf("Distance uses %zu bytes (big numbers need more space)\n", sizeof(distance_micrometers));
}

int main(void) 
{
    printf("Welcome to Embedded C Data Types!\n");
    printf("=====================================\n");
    
    // Show how much memory different types use
    show_memory_usage();
    
    // Example with sensor data
    sensor_example();
    
    // When to use signed vs unsigned
    signed_vs_unsigned_example();
    
    // Why volatile is important
    volatile_example();
    
    // How to choose the right type
    choose_right_type_example();
    
    printf("\n=== Key Rules to Remember ===\n");
    printf("1. Use uint8_t for small positive numbers (0-255)\n");
    printf("2. Use int8_t when you need negative numbers (-128 to +127)\n");
    printf("3. Use uint16_t for medium numbers (0-65,535)\n");
    printf("4. Use uint32_t for big numbers\n");
    printf("5. Always use 'volatile' for variables that hardware can change\n");
    printf("6. Smaller data types = less memory used = better for embedded systems!\n");
    
    return 0;
}

/*
 * What did we learn?
 * 
 * 1. In embedded systems, memory is limited - choose small data types when possible
 * 2. uint8_t, uint16_t, uint32_t give us exact sizes (very important!)  
 * 3. 'volatile' tells compiler "this can change unexpectedly" - use with hardware
 * 4. Think about your data range: 0-255? Use uint8_t. Need negative? Use int8_t.
 * 5. Every byte saved matters when you have limited RAM!
 */