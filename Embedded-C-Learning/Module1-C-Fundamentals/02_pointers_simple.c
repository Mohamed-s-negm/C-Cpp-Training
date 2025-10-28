/*
 * LESSON 2: Pointers - Your Key to Controlling Hardware
 * 
 * What you'll learn:
 * - What pointers really are (think of them as addresses)
 * - How to use pointers to control hardware directly
 * - Simple array handling without getting confused
 * 
 * Think of pointers like house addresses:
 * - You can tell someone your address so they can find your house
 * - Similarly, pointers tell you WHERE data lives in memory
 */

#include <stdio.h>
#include <stdint.h>

// Let's start SIMPLE - what is a pointer?
void what_is_a_pointer(void)
{
    printf("=== What is a Pointer? ===\n");
    
    uint8_t my_number = 42;        // A normal variable
    uint8_t *ptr = &my_number;     // A pointer that points to my_number
    
    printf("my_number = %d\n", my_number);
    printf("Address of my_number = %p\n", (void*)&my_number);
    printf("ptr points to address = %p\n", (void*)ptr);
    printf("Value at that address = %d\n", *ptr);
    
    // The * means "go to this address and get the value"
    // The & means "give me the address of this variable"
    
    printf("\nThink of it like:\n");
    printf("- my_number is like a house with the number 42 inside\n");
    printf("- ptr is like writing down the house address\n");
    printf("- *ptr is like going to that address and reading the number on the door\n");
}

// Simple example: Using pointers to change values
void changing_values_with_pointers(void)
{
    printf("\n=== Changing Values with Pointers ===\n");
    
    uint8_t led_brightness = 100;
    uint8_t *brightness_ptr = &led_brightness;
    
    printf("LED brightness starts at: %d\n", led_brightness);
    
    // Change the value using the pointer
    *brightness_ptr = 200;  // This changes led_brightness!
    
    printf("After changing through pointer: %d\n", led_brightness);
    
    // Why is this useful? Sometimes hardware registers work this way!
}

// Hardware example: Controlling an LED (simplified)
void hardware_pointer_example(void)
{
    printf("\n=== Hardware Control Example ===\n");
    
    // In real embedded systems, LEDs are controlled by writing to specific memory addresses
    // Let's simulate this:
    
    uint32_t fake_led_register = 0x00000000;  // Pretend this is hardware
    uint32_t *led_control = &fake_led_register; // Pointer to "hardware"
    
    printf("LED register starts as: 0x%08X (all LEDs off)\n", *led_control);
    
    // Turn on LED 0 (set bit 0)
    *led_control = *led_control | (1 << 0);  // Set bit 0 to 1
    printf("Turn on LED 0: 0x%08X\n", *led_control);
    
    // Turn on LED 3 (set bit 3)  
    *led_control = *led_control | (1 << 3);  // Set bit 3 to 1
    printf("Turn on LED 3: 0x%08X\n", *led_control);
    
    // This is how you actually control hardware in embedded systems!
}

// Arrays and pointers - keep it simple!
void simple_arrays_and_pointers(void)
{
    printf("\n=== Arrays and Pointers (Simple!) ===\n");
    
    // An array of sensor readings
    uint8_t sensor_data[5] = {10, 20, 30, 40, 50};
    
    // Two ways to access array elements:
    printf("Method 1 - Normal array access:\n");
    for (int i = 0; i < 5; i++) {
        printf("sensor_data[%d] = %d\n", i, sensor_data[i]);
    }
    
    printf("\nMethod 2 - Using pointer:\n");
    uint8_t *data_ptr = sensor_data;  // Point to first element
    for (int i = 0; i < 5; i++) {
        printf("*(data_ptr + %d) = %d\n", i, *(data_ptr + i));
    }
    
    // Both methods do the SAME thing! Arrays and pointers are closely related.
}

// Function that takes a pointer (very common in embedded C)
void update_sensor_value(uint8_t *sensor_ptr, uint8_t new_value)
{
    // This function can modify a variable from outside!
    *sensor_ptr = new_value;
    printf("Sensor updated to: %d\n", *sensor_ptr);
}

void functions_with_pointers(void)
{
    printf("\n=== Functions with Pointers ===\n");
    
    uint8_t temperature = 25;
    printf("Temperature before: %d\n", temperature);
    
    // Pass the ADDRESS of temperature to the function
    update_sensor_value(&temperature, 30);
    
    printf("Temperature after: %d\n", temperature);
    
    // The function changed our variable! This is very useful.
}

// Real-world example: Simple buffer for communication
void simple_buffer_example(void)
{
    printf("\n=== Simple Communication Buffer ===\n");
    
    // Imagine we're receiving data from a sensor over serial communication
    uint8_t receive_buffer[10] = {0};  // Buffer to store incoming data
    uint8_t buffer_index = 0;          // Where to put the next byte
    
    // Simulate receiving some bytes
    uint8_t incoming_data[] = {0xAA, 0xBB, 0xCC, 0xDD};
    
    printf("Receiving data into buffer:\n");
    for (int i = 0; i < 4; i++) {
        if (buffer_index < 10) {  // Make sure we don't overflow!
            receive_buffer[buffer_index] = incoming_data[i];
            printf("Received: 0x%02X, stored at position %d\n", 
                   incoming_data[i], buffer_index);
            buffer_index++;
        }
    }
    
    // Now let's read back what we stored
    printf("\nBuffer contents:\n");
    for (int i = 0; i < buffer_index; i++) {
        printf("buffer[%d] = 0x%02X\n", i, receive_buffer[i]);
    }
}

// NULL pointer safety (very important!)
void null_pointer_safety(void)
{
    printf("\n=== Pointer Safety ===\n");
    
    uint8_t *safe_ptr = NULL;  // Start with NULL (points to nothing)
    
    // ALWAYS check if pointer is valid before using it!
    if (safe_ptr != NULL) {
        printf("Pointer is safe to use\n");
        // Use the pointer here
    } else {
        printf("Pointer is NULL - don't use it!\n");
    }
    
    // This prevents crashes in your embedded system
    printf("Always check pointers before using them!\n");
}

int main(void)
{
    printf("Welcome to Pointers - Made Simple!\n");
    printf("===================================\n");
    
    // Start with basics
    what_is_a_pointer();
    
    // Show how to change values
    changing_values_with_pointers();
    
    // Hardware control example  
    hardware_pointer_example();
    
    // Arrays and pointers
    simple_arrays_and_pointers();
    
    // Functions with pointers
    functions_with_pointers();
    
    // Real buffer example
    simple_buffer_example();
    
    // Safety first!
    null_pointer_safety();
    
    printf("\n=== Key Points to Remember ===\n");
    printf("1. Pointer = address where data lives\n");
    printf("2. & gets the address of a variable\n");
    printf("3. * goes to the address and gets/sets the value\n");
    printf("4. Always check if pointer != NULL before using it\n");
    printf("5. Pointers let functions modify variables from outside\n");
    printf("6. Arrays and pointers work similarly\n");
    printf("7. Embedded systems use pointers to talk to hardware!\n");
    
    return 0;
}

/*
 * What did we learn?
 * 
 * 1. Pointers are just addresses - like house addresses
 * 2. & gets the address, * uses the address to get/set values
 * 3. Hardware registers are controlled using pointers to specific addresses
 * 4. Functions can modify outside variables using pointers
 * 5. Arrays and pointers are closely related
 * 6. Always check for NULL pointers to prevent crashes
 * 7. Pointers are essential for efficient embedded programming!
 * 
 * Next: We'll learn bit operations to control individual hardware pins!
 */