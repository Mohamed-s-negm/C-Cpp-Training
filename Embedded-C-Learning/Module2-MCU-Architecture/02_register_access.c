/*
 * MODULE 2 - LESSON 2: Direct Register Access - Talking to Hardware
 * 
 * What you'll learn:
 * - How to control hardware by writing directly to registers
 * - Real ESP32 register addresses and what they do
 * - Why direct register access is faster than libraries
 * - Safe ways to read and write hardware registers
 * 
 * Think of registers like control panels:
 * - Each register controls a specific hardware function
 * - Writing to registers = pressing buttons on the control panel
 * - Reading from registers = checking indicator lights
 */

#include <stdio.h>
#include <stdint.h>

// Real ESP32 GPIO register addresses (these are the actual hardware addresses!)
#define GPIO_OUT_REG        ((volatile uint32_t*)0x60004004)   // Controls pin output levels
#define GPIO_ENABLE_REG     ((volatile uint32_t*)0x60004020)   // Enables pins as outputs
#define GPIO_IN_REG         ((volatile uint32_t*)0x6000400C)   // Reads pin input levels
#define GPIO_FUNC_OUT_SEL_CFG_REG ((volatile uint32_t*)0x60004530) // Pin function selection

// For simulation, we'll use variables instead of real hardware
volatile uint32_t simulated_gpio_out = 0;
volatile uint32_t simulated_gpio_enable = 0;
volatile uint32_t simulated_gpio_in = 0xAAAAAAAA;  // Simulate some input pattern

// Helper macros for safe register access
#define REG_READ(addr)          (*(volatile uint32_t*)(addr))
#define REG_WRITE(addr, val)    (*(volatile uint32_t*)(addr) = (val))
#define REG_SET_BITS(addr, mask) (*(volatile uint32_t*)(addr) |= (mask))
#define REG_CLR_BITS(addr, mask) (*(volatile uint32_t*)(addr) &= ~(mask))

// Basic register operations explained
void basic_register_operations(void)
{
    printf("=== Basic Register Operations ===\n");
    
    printf("Registers are just memory locations with special powers!\n");
    printf("When you write to them, hardware responds immediately.\n\n");
    
    // Show how to access a register
    printf("1. READING a register:\n");
    uint32_t current_value = simulated_gpio_in;  // Read current state
    printf("   GPIO_IN register value: 0x%08X\n", current_value);
    printf("   This tells us which pins are HIGH or LOW\n\n");
    
    printf("2. WRITING to a register:\n");
    simulated_gpio_out = 0x12345678;  // Write new value
    printf("   Wrote 0x%08X to GPIO_OUT register\n", simulated_gpio_out);
    printf("   This immediately changes pin output levels!\n\n");
    
    printf("3. SETTING specific bits (turn things ON):\n");
    uint32_t old_val = simulated_gpio_out;
    simulated_gpio_out |= (1 << 5);  // Set bit 5
    printf("   Before: 0x%08X, After: 0x%08X\n", old_val, simulated_gpio_out);
    printf("   Pin 5 just turned ON, others unchanged\n\n");
    
    printf("4. CLEARING specific bits (turn things OFF):\n");
    old_val = simulated_gpio_out;
    simulated_gpio_out &= ~(1 << 20);  // Clear bit 20
    printf("   Before: 0x%08X, After: 0x%08X\n", old_val, simulated_gpio_out);
    printf("   Pin 20 just turned OFF, others unchanged\n");
}

// Show real ESP32 GPIO control
void esp32_gpio_control_example(void)
{
    printf("\n=== ESP32 GPIO Control Example ===\n");
    
    printf("Let's configure GPIO pin 2 as an output and control an LED:\n\n");
    
    // Step 1: Enable pin as output
    printf("Step 1: Enable GPIO pin 2 as output\n");
    printf("Code: REG_SET_BITS(GPIO_ENABLE_REG, (1 << 2));\n");
    simulated_gpio_enable |= (1 << 2);
    printf("GPIO_ENABLE_REG now: 0x%08X (pin 2 enabled for output)\n\n", simulated_gpio_enable);
    
    // Step 2: Turn LED on
    printf("Step 2: Turn LED ON (set pin 2 HIGH)\n");
    printf("Code: REG_SET_BITS(GPIO_OUT_REG, (1 << 2));\n");
    simulated_gpio_out |= (1 << 2);
    printf("GPIO_OUT_REG now: 0x%08X (pin 2 is HIGH - LED ON!)\n\n", simulated_gpio_out);
    
    // Step 3: Turn LED off
    printf("Step 3: Turn LED OFF (set pin 2 LOW)\n");
    printf("Code: REG_CLR_BITS(GPIO_OUT_REG, (1 << 2));\n");
    simulated_gpio_out &= ~(1 << 2);
    printf("GPIO_OUT_REG now: 0x%08X (pin 2 is LOW - LED OFF!)\n\n", simulated_gpio_out);
    
    printf("That's it! You just controlled hardware directly!\n");
    printf("No libraries needed - just direct register access.\n");
}

// Multiple pin control example
void multiple_pin_control(void)
{
    printf("\n=== Controlling Multiple Pins at Once ===\n");
    
    // Setup multiple pins as outputs (pins 2, 4, 5)
    uint32_t output_pins = (1 << 2) | (1 << 4) | (1 << 5);
    printf("Setting up pins 2, 4, and 5 as outputs...\n");
    printf("Pin mask: 0x%08X\n", output_pins);
    simulated_gpio_enable |= output_pins;
    printf("GPIO_ENABLE_REG: 0x%08X\n\n", simulated_gpio_enable);
    
    // Turn on all three LEDs at once
    printf("Turn ON all three LEDs simultaneously:\n");
    simulated_gpio_out |= output_pins;
    printf("GPIO_OUT_REG: 0x%08X (all three LEDs ON!)\n\n", simulated_gpio_out);
    
    // Create blinking pattern
    printf("Creating a blinking pattern:\n");
    uint32_t patterns[] = {
        0x00000004,  // Only pin 2 (binary: 000100)
        0x00000010,  // Only pin 4 (binary: 010000) 
        0x00000020,  // Only pin 5 (binary: 100000)
        0x00000034   // All three  (binary: 110100)
    };
    
    for (int i = 0; i < 4; i++) {
        // Clear all our pins first
        simulated_gpio_out &= ~output_pins;
        // Set new pattern
        simulated_gpio_out |= patterns[i];
        
        printf("Pattern %d: GPIO = 0x%08X - ", i+1, simulated_gpio_out);
        printf("Pin2=%s Pin4=%s Pin5=%s\n",
               (patterns[i] & (1<<2)) ? "ON " : "OFF",
               (patterns[i] & (1<<4)) ? "ON " : "OFF", 
               (patterns[i] & (1<<5)) ? "ON " : "OFF");
    }
}

// Reading inputs from buttons/sensors
void reading_input_pins(void)
{
    printf("\n=== Reading Input Pins ===\n");
    
    // Simulate button connected to pin 0
    printf("Reading button state on pin 0:\n");
    printf("GPIO_IN_REG value: 0x%08X\n", simulated_gpio_in);
    
    uint8_t button_pressed = (simulated_gpio_in >> 0) & 1;  // Extract bit 0
    printf("Button on pin 0: %s\n", button_pressed ? "PRESSED" : "Not pressed");
    
    // Check multiple inputs at once
    printf("\nChecking multiple input pins:\n");
    for (int pin = 0; pin < 8; pin++) {
        uint8_t pin_state = (simulated_gpio_in >> pin) & 1;
        printf("Pin %d: %s\n", pin, pin_state ? "HIGH" : "LOW");
    }
    
    // Practical example: reading sensor values
    printf("\nPractical example - Reading sensor array:\n");
    uint32_t sensor_mask = 0xFF;  // Check pins 0-7
    uint32_t sensor_readings = simulated_gpio_in & sensor_mask;
    printf("Sensor readings (pins 0-7): 0x%02X\n", (uint8_t)sensor_readings);
    
    // Convert to individual sensor states
    printf("Individual sensors:\n");
    for (int i = 0; i < 8; i++) {
        if (sensor_readings & (1 << i)) {
            printf("  Sensor %d: DETECTED\n", i);
        }
    }
}

// Show different register types and their purposes
void different_register_types(void)
{
    printf("\n=== Different Types of Registers ===\n");
    
    printf("1. OUTPUT REGISTERS (Write-only or Read/Write):\n");
    printf("   - GPIO_OUT: Controls pin voltage levels\n");
    printf("   - TIMER_LOAD: Sets timer start value\n");
    printf("   - UART_TX: Sends data over serial\n");
    printf("   Purpose: Tell hardware what to do\n\n");
    
    printf("2. INPUT REGISTERS (Read-only):\n");
    printf("   - GPIO_IN: Reads current pin states\n");
    printf("   - ADC_DATA: Gets analog sensor readings\n");
    printf("   - UART_RX: Receives serial data\n");
    printf("   Purpose: Get information from hardware\n\n");
    
    printf("3. CONTROL REGISTERS (Read/Write):\n");
    printf("   - GPIO_ENABLE: Enable/disable pin functions\n");
    printf("   - TIMER_CTRL: Start/stop/configure timers\n");
    printf("   - UART_CTRL: Configure communication settings\n");
    printf("   Purpose: Configure how hardware behaves\n\n");
    
    printf("4. STATUS REGISTERS (Read-only):\n");
    printf("   - TIMER_STATUS: Is timer running? Overflowed?\n");
    printf("   - UART_STATUS: Is data ready? Buffer full?\n");
    printf("   - INT_STATUS: Which interrupts are pending?\n");
    printf("   Purpose: Check current hardware state\n");
}

// Safe register access patterns
void safe_register_access(void)
{
    printf("\n=== Safe Register Access Patterns ===\n");
    
    printf("1. Always use 'volatile' for hardware registers:\n");
    printf("   volatile uint32_t* gpio_reg = (volatile uint32_t*)0x60004004;\n");
    printf("   Reason: Hardware can change register values anytime!\n\n");
    
    printf("2. Read-Modify-Write for single bits:\n");
    printf("   // WRONG: reg = reg | (1 << 5);  // Might lose changes from hardware\n");
    printf("   // RIGHT:\n");
    printf("   uint32_t temp = *reg;           // Read current value\n");
    printf("   temp |= (1 << 5);              // Modify our copy\n");
    printf("   *reg = temp;                    // Write back atomically\n\n");
    
    printf("3. Use macros for common operations:\n");
    printf("   #define SET_BIT(reg, bit)   ((reg) |= (1 << (bit)))\n");
    printf("   #define CLR_BIT(reg, bit)   ((reg) &= ~(1 << (bit)))\n");
    printf("   Makes code more readable and less error-prone!\n\n");
    
    printf("4. Check register documentation:\n");
    printf("   - Some registers are write-only (reading gives undefined results)\n");
    printf("   - Some registers clear when read (reading changes state!)\n");
    printf("   - Some registers have reserved bits (don't touch them!)\n\n");
    
    // Demonstrate safe bit manipulation
    uint32_t demo_reg = 0x12345678;
    printf("Demo: Safe bit manipulation\n");
    printf("Original register: 0x%08X\n", demo_reg);
    
    // Safe way to set bit 10 without affecting others
    demo_reg |= (1 << 10);
    printf("After setting bit 10: 0x%08X\n", demo_reg);
    
    // Safe way to clear bit 8 without affecting others  
    demo_reg &= ~(1 << 8);
    printf("After clearing bit 8: 0x%08X\n", demo_reg);
}

int main(void)
{
    printf("Welcome to Direct Register Access!\n");
    printf("=================================\n");
    
    // Basic register operations
    basic_register_operations();
    
    // ESP32 GPIO example
    esp32_gpio_control_example();
    
    // Multiple pin control
    multiple_pin_control();
    
    // Reading inputs
    reading_input_pins();
    
    // Different register types
    different_register_types();
    
    // Safe access patterns
    safe_register_access();
    
    printf("\n=== Why Direct Register Access? ===\n");
    printf("1. SPEED: No library overhead - direct hardware control\n");
    printf("2. CONTROL: You decide exactly what happens and when\n");
    printf("3. UNDERSTANDING: You learn how hardware really works\n");
    printf("4. EFFICIENCY: Smaller code size, less memory usage\n");
    printf("5. FLEXIBILITY: Can do things libraries might not support\n");
    
    printf("\n=== Key Rules to Remember ===\n");
    printf("1. Always use 'volatile' for hardware register pointers\n");
    printf("2. Read the register documentation before using\n");
    printf("3. Use bit operations to modify single bits safely\n");
    printf("4. Check if register is read-only, write-only, or read/write\n");
    printf("5. Some registers have side effects when read/written\n");
    printf("6. Use meaningful names and macros for register addresses\n");
    
    return 0;
}

/*
 * What did we learn?
 * 
 * 1. Registers are memory-mapped hardware control interfaces
 * 2. Writing to registers immediately affects hardware behavior  
 * 3. Reading from registers tells you current hardware state
 * 4. Different register types serve different purposes
 * 5. Direct register access is faster than using libraries
 * 6. Always use volatile for hardware register pointers
 * 7. Bit manipulation lets you control individual hardware features
 * 8. Safe access patterns prevent race conditions and errors
 * 
 * Next: GPIO control and digital I/O in detail!
 */