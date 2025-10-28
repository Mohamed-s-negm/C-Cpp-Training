/*
 * LESSON 3: Bit Operations - Controlling Individual Pins and Flags
 * 
 * What you'll learn:
 * - How to turn individual LEDs on/off using bits
 * - How to read button states from individual pins
 * - Why bit operations are essential in embedded systems
 * 
 * Think of bits like light switches:
 * - Each bit is like a switch that can be ON (1) or OFF (0)  
 * - You can control each switch individually
 * - 8 switches = 8 bits = 1 byte
 */

#include <stdio.h>
#include <stdint.h>

// Let's start with understanding bits in a simple way
void understanding_bits(void)
{
    printf("=== Understanding Bits ===\n");
    
    uint8_t my_byte = 0;  // Start with all bits OFF (00000000)
    
    printf("Starting value: %d (in decimal)\n", my_byte);
    printf("In binary, this looks like: 00000000\n");
    printf("Each position is a bit: 76543210 (bit numbers)\n\n");
    
    // Let's set bit 0 to 1
    my_byte = my_byte | (1 << 0);  // This turns ON bit 0
    printf("After setting bit 0: %d\n", my_byte);
    printf("In binary: 00000001\n\n");
    
    // Set bit 3
    my_byte = my_byte | (1 << 3);  // This turns ON bit 3  
    printf("After setting bit 3: %d\n", my_byte);
    printf("In binary: 00001001\n\n");
    
    printf("Now bits 0 and 3 are ON, others are OFF\n");
}

// Simple macros to make bit operations easier to understand
#define SET_BIT(reg, bit)    ((reg) |= (1 << (bit)))   // Turn bit ON
#define CLEAR_BIT(reg, bit)  ((reg) &= ~(1 << (bit)))  // Turn bit OFF  
#define TOGGLE_BIT(reg, bit) ((reg) ^= (1 << (bit)))   // Flip bit ON/OFF
#define READ_BIT(reg, bit)   (((reg) >> (bit)) & 1)    // Check if bit is ON

// Example: Controlling LEDs using bits
void led_control_example(void)
{
    printf("\n=== LED Control Example ===\n");
    
    // Imagine we have 8 LEDs connected to 8 pins
    uint8_t led_register = 0;  // All LEDs start OFF
    
    printf("All LEDs OFF: 0x%02X\n", led_register);
    
    // Turn ON LED 0 (connected to bit 0)
    SET_BIT(led_register, 0);
    printf("LED 0 ON: 0x%02X\n", led_register);
    
    // Turn ON LED 2  
    SET_BIT(led_register, 2);
    printf("LED 0 and 2 ON: 0x%02X\n", led_register);
    
    // Turn ON LED 7
    SET_BIT(led_register, 7);
    printf("LEDs 0, 2, and 7 ON: 0x%02X\n", led_register);
    
    // Turn OFF LED 0
    CLEAR_BIT(led_register, 0);
    printf("LED 0 OFF, LEDs 2 and 7 still ON: 0x%02X\n", led_register);
    
    // Toggle LED 2 (if ON, turn OFF; if OFF, turn ON)
    TOGGLE_BIT(led_register, 2);
    printf("After toggling LED 2: 0x%02X\n", led_register);
}

// Example: Reading button states
void button_reading_example(void)
{
    printf("\n=== Button Reading Example ===\n");
    
    // Simulate reading from a hardware register
    uint8_t button_register = 0b00100101;  // Buttons 0, 2, and 5 are pressed
    
    printf("Button register value: 0x%02X\n", button_register);
    printf("In binary: ");
    
    // Print each bit
    for (int i = 7; i >= 0; i--) {
        printf("%d", READ_BIT(button_register, i));
    }
    printf(" (bit positions 76543210)\n");
    
    // Check individual button states
    printf("\nButton states:\n");
    for (int i = 0; i < 8; i++) {
        if (READ_BIT(button_register, i)) {
            printf("Button %d: PRESSED\n", i);
        } else {
            printf("Button %d: not pressed\n", i);
        }
    }
}

// Example: Working with multiple bits at once
void multiple_bits_example(void)
{
    printf("\n=== Working with Multiple Bits ===\n");
    
    uint8_t control_register = 0;
    
    // Set multiple bits at once (turn on LEDs 0, 1, 2)
    control_register |= 0b00000111;  // This is the same as (1<<0)|(1<<1)|(1<<2)
    printf("LEDs 0,1,2 ON: 0x%02X\n", control_register);
    
    // Clear multiple bits at once (turn off LEDs 1,2)
    control_register &= ~0b00000110;  // Clear bits 1 and 2
    printf("Only LED 0 ON: 0x%02X\n", control_register);
    
    // Check if any of several bits are set
    uint8_t error_bits = 0b11110000;  // Bits 4,5,6,7 represent different errors
    if (control_register & error_bits) {
        printf("No errors detected\n");
    } else {
        printf("Some error detected in bits 4-7\n");
    }
}

// Real-world example: Configuring a simple peripheral
void peripheral_config_example(void)
{
    printf("\n=== Peripheral Configuration Example ===\n");
    
    // Imagine configuring a UART (serial communication)
    uint8_t uart_config = 0;
    
    printf("UART config starts as: 0x%02X (everything disabled)\n", uart_config);
    
    // Enable UART (bit 0)
    SET_BIT(uart_config, 0);
    printf("UART enabled: 0x%02X\n", uart_config);
    
    // Enable transmit (bit 1)
    SET_BIT(uart_config, 1);
    printf("Transmit enabled: 0x%02X\n", uart_config);
    
    // Enable receive (bit 2)  
    SET_BIT(uart_config, 2);
    printf("Receive enabled: 0x%02X\n", uart_config);
    
    // Set baud rate bits (bits 4-5: 00=9600, 01=19200, 10=38400, 11=115200)
    // Let's set to 01 for 19200 baud
    CLEAR_BIT(uart_config, 4);  // Make sure bit 4 is 0
    SET_BIT(uart_config, 5);    // Set bit 5 to 1
    printf("Baud rate set to 19200: 0x%02X\n", uart_config);
    
    // Check current settings
    printf("\nCurrent UART settings:\n");
    printf("UART enabled: %s\n", READ_BIT(uart_config, 0) ? "YES" : "NO");
    printf("Transmit enabled: %s\n", READ_BIT(uart_config, 1) ? "YES" : "NO");  
    printf("Receive enabled: %s\n", READ_BIT(uart_config, 2) ? "YES" : "NO");
    
    // Extract baud rate setting (2-bit field in bits 4-5)
    uint8_t baud_setting = (uart_config >> 4) & 0b11;  // Get bits 4-5
    const char* baud_rates[] = {"9600", "19200", "38400", "115200"};
    printf("Baud rate: %s\n", baud_rates[baud_setting]);
}

// Example: Status flags (very common in embedded systems)
void status_flags_example(void)
{
    printf("\n=== Status Flags Example ===\n");
    
    uint8_t system_status = 0;
    
    // Define what each bit means (makes code easier to read)
    #define SYSTEM_READY_BIT    0
    #define SENSOR_OK_BIT       1  
    #define COMM_ACTIVE_BIT     2
    #define LOW_BATTERY_BIT     3
    #define ERROR_BIT           4
    
    // System starts up
    SET_BIT(system_status, SYSTEM_READY_BIT);
    SET_BIT(system_status, SENSOR_OK_BIT);
    printf("System ready, sensor OK: 0x%02X\n", system_status);
    
    // Communication becomes active
    SET_BIT(system_status, COMM_ACTIVE_BIT);
    printf("Communication active: 0x%02X\n", system_status);
    
    // Battery gets low
    SET_BIT(system_status, LOW_BATTERY_BIT);
    printf("Low battery warning: 0x%02X\n", system_status);
    
    // Check system health
    printf("\nSystem Health Check:\n");
    if (READ_BIT(system_status, SYSTEM_READY_BIT)) {
        printf("✓ System is ready\n");
    }
    if (READ_BIT(system_status, SENSOR_OK_BIT)) {
        printf("✓ Sensors working\n");
    }
    if (READ_BIT(system_status, LOW_BATTERY_BIT)) {
        printf("⚠ Low battery!\n");
    }
    if (READ_BIT(system_status, ERROR_BIT)) {
        printf("✗ System error!\n");
    } else {
        printf("✓ No errors\n");
    }
}

int main(void)
{
    printf("Welcome to Bit Operations - Made Simple!\n");
    printf("========================================\n");
    
    // Start with understanding what bits are
    understanding_bits();
    
    // Control LEDs with bits
    led_control_example();
    
    // Read button states
    button_reading_example();
    
    // Work with multiple bits
    multiple_bits_example();
    
    // Real peripheral configuration
    peripheral_config_example();
    
    // Status flags
    status_flags_example();
    
    printf("\n=== Key Points to Remember ===\n");
    printf("1. Each bit is like a switch: 1=ON, 0=OFF\n");
    printf("2. Use | to turn bits ON (SET)\n");
    printf("3. Use & with ~ to turn bits OFF (CLEAR)\n");
    printf("4. Use ^ to flip bits (TOGGLE)\n");
    printf("5. Use >> and & to read individual bits\n");
    printf("6. Bit operations let you control hardware pins individually\n");
    printf("7. Status flags use bits to store multiple true/false values efficiently\n");
    
    return 0;
}

/*
 * What did we learn?
 * 
 * 1. Bits are the fundamental unit of digital control
 * 2. Each bit can represent an ON/OFF state (LED, button, flag, etc.)
 * 3. Bit operations (|, &, ^, ~, <<, >>) let us control individual bits
 * 4. Hardware peripherals are configured using bit operations
 * 5. Status flags pack multiple boolean values into a single byte
 * 6. Using #define makes bit positions easier to understand
 * 7. Bit operations are fast and memory-efficient
 * 
 * Next: We'll learn about memory layout and how to blink an LED!
 */