/*
 * LESSON 4: Memory Layout - Where Your Variables Live
 * 
 * What you'll learn:
 * - Where different types of variables are stored in memory
 * - Why this matters for embedded systems
 * - How to use memory efficiently
 * - The difference between stack, global, and constant memory
 * 
 * Think of memory like different rooms in a house:
 * - Some rooms for temporary stuff (stack)
 * - Some rooms for permanent stuff (global variables)
 * - Some rooms for things that never change (constants)
 */

#include <stdio.h>
#include <stdint.h>

// GLOBAL VARIABLES - Live in a special area of memory
uint8_t global_counter = 0;        // This lives in RAM, initialized to 0
uint8_t global_sensor_data[10];    // Array in global memory (automatically set to 0)

// CONSTANT DATA - Lives in a read-only area (usually Flash memory)
const uint8_t firmware_version[] = "v1.0.0";  // Can't be changed, saves RAM
const uint16_t sensor_calibration_values[] = {100, 200, 300, 400, 500};

// STATIC VARIABLES - Like global, but only visible in this file
static uint8_t static_error_count = 0;

void show_memory_basics(void)
{
    printf("=== Memory Layout Basics ===\n");
    
    // LOCAL VARIABLES - Live on the "stack" (temporary memory)
    uint8_t local_variable = 42;      // Created when function starts
    uint8_t local_array[5] = {1,2,3,4,5}; // Also on stack
    
    printf("Local variable: %d\n", local_variable);
    
    // Show addresses to see where things live
    printf("\nMemory Addresses (these show WHERE variables live):\n");
    printf("Global counter address:    %p\n", (void*)&global_counter);
    printf("Local variable address:    %p\n", (void*)&local_variable);
    printf("Constant data address:     %p\n", (void*)firmware_version);
    
    // When this function ends, local_variable and local_array disappear!
    // But global_counter stays around forever
}

// Example showing the difference between global and local
void global_vs_local_example(void)
{
    printf("\n=== Global vs Local Variables ===\n");
    
    static uint8_t function_call_count = 0;  // Static = remembers value between calls
    uint8_t local_temp = 100;                // Local = reset each time
    
    function_call_count++;  // This keeps counting up
    local_temp++;           // This always becomes 101
    
    printf("Function called %d times\n", function_call_count);
    printf("Local temp (always 101): %d\n", local_temp);
    
    // The global counter can be accessed from anywhere
    global_counter += 10;
    printf("Global counter is now: %d\n", global_counter);
}

// Example: Using constants efficiently  
void constants_example(void)
{
    printf("\n=== Using Constants Efficiently ===\n");
    
    // Constants don't use RAM - they're stored in Flash memory
    printf("Firmware version: %s\n", firmware_version);
    
    // Calibration values for sensors (these never change)
    printf("Sensor calibration values:\n");
    for (int i = 0; i < 5; i++) {
        printf("Sensor %d calibration: %d\n", i, sensor_calibration_values[i]);
    }
    
    // Why use const?
    // 1. Saves RAM (stored in Flash instead)
    // 2. Prevents accidental changes
    // 3. Compiler can optimize better
    
    printf("These constants don't use precious RAM!\n");
}

// Example: Stack overflow risk (very important in embedded!)
void stack_example(void)
{
    printf("\n=== Stack Memory Example ===\n");
    
    // This is OK - small array
    uint8_t small_buffer[10] = {0};
    
    // This might be dangerous in embedded systems - large array on stack!
    // uint8_t huge_buffer[1000] = {0};  // Don't do this!
    
    printf("Small buffer size: %zu bytes (OK for stack)\n", sizeof(small_buffer));
    // printf("Huge buffer size: %zu bytes (BAD for stack!)\n", sizeof(huge_buffer));
    
    printf("Stack is limited in embedded systems - keep local arrays small!\n");
    printf("For big arrays, use global variables instead.\n");
}

// Example: Efficient memory usage patterns
void memory_efficiency_tips(void)
{
    printf("\n=== Memory Efficiency Tips ===\n");
    
    // TIP 1: Use the smallest data type possible
    uint8_t small_counter = 0;     // 1 byte - perfect for counting 0-255
    // uint32_t big_counter = 0;   // 4 bytes - waste for small counts
    
    // TIP 2: Group related data together
    struct sensor_data {
        uint8_t temperature;    // 1 byte
        uint8_t humidity;       // 1 byte  
        uint16_t pressure;      // 2 bytes
    } my_sensor;               // Total: 4 bytes
    
    // TIP 3: Use bit fields for flags
    struct status_flags {
        uint8_t led_on : 1;        // Only 1 bit
        uint8_t button_pressed : 1; // Only 1 bit
        uint8_t error_code : 4;     // 4 bits (16 possible errors)
        uint8_t reserved : 2;       // 2 unused bits
    } status;                      // Total: 1 byte for 8 different values!
    
    printf("Sensor data size: %zu bytes\n", sizeof(my_sensor));
    printf("Status flags size: %zu bytes\n", sizeof(status));
    
    // TIP 4: Use arrays instead of individual variables
    uint8_t led_states[8] = {0};   // Better than led1, led2, led3, etc.
    
    printf("8 LED states in %zu bytes\n", sizeof(led_states));
}

// Example: What happens when memory runs out
void memory_limits_example(void)
{
    printf("\n=== Memory Limits in Embedded Systems ===\n");
    
    // In embedded systems, you might have very limited RAM
    // Example: Arduino Uno has only 2KB of RAM!
    
    printf("Imagine you have only 2048 bytes of RAM total:\n");
    
    // Calculate memory usage
    size_t global_usage = sizeof(global_counter) + sizeof(global_sensor_data);
    size_t constants_usage = 0;  // Constants don't count - they're in Flash
    
    printf("Global variables use: %zu bytes\n", global_usage);
    printf("Constants use: 0 bytes of RAM (they're in Flash)\n");
    printf("Remaining RAM: ~%zu bytes\n", 2048 - global_usage - 200); // 200 for system use
    
    printf("\nThis is why we:\n");
    printf("1. Choose small data types\n");
    printf("2. Use const for fixed data\n");
    printf("3. Avoid large local arrays\n");
    printf("4. Pack data efficiently\n");
}

// Example: volatile keyword for hardware registers
volatile uint8_t hardware_register = 0;  // Hardware can change this anytime!

void volatile_memory_example(void)
{
    printf("\n=== Volatile Memory Example ===\n");
    
    // Simulate hardware changing the register
    hardware_register = 0x55;
    
    printf("Hardware register: 0x%02X\n", hardware_register);
    
    // In real embedded systems:
    // - Interrupt handlers can change volatile variables
    // - Hardware registers can change values unexpectedly  
    // - volatile tells compiler "always read the real value"
    
    printf("Without 'volatile', compiler might cache old values!\n");
    printf("With 'volatile', compiler always reads fresh value from memory.\n");
}

int main(void)
{
    printf("Welcome to Memory Layout - Made Simple!\n");
    printf("======================================\n");
    
    // Show where variables live
    show_memory_basics();
    
    // Call the function multiple times to see static vs local
    printf("\nCalling function 3 times to show static vs local:\n");
    for (int i = 0; i < 3; i++) {
        global_vs_local_example();
    }
    
    // Show constants
    constants_example();
    
    // Stack considerations
    stack_example();
    
    // Efficiency tips
    memory_efficiency_tips();
    
    // Memory limits
    memory_limits_example();
    
    // Volatile example
    volatile_memory_example();
    
    printf("\n=== Key Points to Remember ===\n");
    printf("1. Local variables (in functions) use stack - keep them small\n");
    printf("2. Global variables use RAM - they stay around forever\n");
    printf("3. const variables use Flash, not RAM - saves memory\n");
    printf("4. static variables remember values between function calls\n");
    printf("5. volatile tells compiler 'this can change unexpectedly'\n");
    printf("6. Embedded systems have very limited RAM - every byte counts!\n");
    printf("7. Use smallest data types possible\n");
    printf("8. Group related data in structs\n");
    
    return 0;
}

/*
 * What did we learn?
 * 
 * 1. Memory is divided into different areas (stack, global, constants)
 * 2. Local variables are temporary and use stack (limited space)
 * 3. Global variables are permanent and use RAM 
 * 4. Constants use Flash memory, not RAM (saves space)
 * 5. static variables are like global but with limited visibility
 * 6. volatile prevents compiler from caching hardware register values
 * 7. Embedded systems have very limited memory - efficiency matters!
 * 8. Choose data types carefully to minimize memory usage
 * 
 * Next: We'll put it all together and blink an LED!
 */