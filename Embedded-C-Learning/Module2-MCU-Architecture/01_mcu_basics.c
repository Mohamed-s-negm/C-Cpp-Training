/*
 * MODULE 2 - LESSON 1: Understanding Microcontroller Architecture
 * 
 * What you'll learn:
 * - What's inside a microcontroller (the ESP32 brain!)
 * - How CPU, memory, and peripherals work together
 * - Why understanding architecture helps you write better code
 * - The difference between Flash, SRAM, and registers
 * 
 * Think of a microcontroller like a mini computer:
 * - CPU = the brain that runs your code
 * - Flash = permanent storage (like a hard drive)
 * - SRAM = temporary working memory (like RAM)
 * - Peripherals = arms and legs that interact with the world
 */

#include <stdio.h>
#include <stdint.h>

// Let's simulate the main parts of a microcontroller
typedef struct {
    uint32_t program_counter;    // Which instruction we're executing
    uint32_t stack_pointer;      // Where we are in the stack
    uint32_t registers[16];      // CPU working registers
    uint8_t  flags;             // Status flags (zero, carry, etc.)
} cpu_state_t;

typedef struct {
    uint8_t flash_memory[1024];  // Program storage (1KB simulation)
    uint8_t sram_memory[256];    // Data memory (256 bytes simulation)
    uint32_t peripheral_regs[32]; // Hardware control registers
} mcu_memory_t;

// Our simulated microcontroller
cpu_state_t cpu;
mcu_memory_t memory;

// ESP32 has different memory regions - let's learn about them
void explain_memory_types(void)
{
    printf("=== Microcontroller Memory Types ===\n");
    
    printf("1. FLASH MEMORY (Program Storage):\n");
    printf("   - Stores your C code after compilation\n");
    printf("   - Non-volatile (keeps data when power off)\n");
    printf("   - Read-only during normal operation\n");
    printf("   - ESP32 has 4MB of Flash\n");
    printf("   - Like a book with your program written in it\n\n");
    
    printf("2. SRAM (Static RAM - Working Memory):\n");
    printf("   - Stores variables while program runs\n");
    printf("   - Volatile (loses data when power off)\n");
    printf("   - Fast access for CPU\n");
    printf("   - ESP32 has ~520KB of SRAM\n");
    printf("   - Like your desk where you work with papers\n\n");
    
    printf("3. REGISTERS (Hardware Control):\n");
    printf("   - Special memory locations that control hardware\n");
    printf("   - Writing to them changes how hardware behaves\n");
    printf("   - Reading from them tells you hardware status\n");
    printf("   - Very fast access\n");
    printf("   - Like switches and indicators on a control panel\n\n");
    
    // Show memory sizes in perspective
    printf("Memory size comparison (ESP32):\n");
    printf("Flash:  4,194,304 bytes (4MB) - Your program lives here\n");
    printf("SRAM:     520,192 bytes (520KB) - Your variables live here\n");
    printf("Registers:    ~1000 locations - Hardware controls live here\n");
}

// Simulate how the CPU executes instructions
void simulate_cpu_execution(void)
{
    printf("\n=== How the CPU Executes Your Code ===\n");
    
    // Initialize CPU state
    cpu.program_counter = 0x1000;  // Start address of our program
    cpu.stack_pointer = 0x2000;    // Start of stack area
    
    printf("CPU starts execution:\n");
    printf("Program Counter: 0x%04X (points to next instruction)\n", cpu.program_counter);
    printf("Stack Pointer:   0x%04X (points to stack top)\n", cpu.stack_pointer);
    
    // Simulate executing a few instructions
    const char* instructions[] = {
        "LOAD R1, #10",      // Load number 10 into register 1
        "LOAD R2, #20",      // Load number 20 into register 2
        "ADD R3, R1, R2",    // Add R1 and R2, store in R3
        "STORE R3, 0x300"    // Store result in memory location 0x300
    };
    
    printf("\nExecuting instructions step by step:\n");
    for (int i = 0; i < 4; i++) {
        printf("Step %d: PC=0x%04X, Instruction: %s\n", 
               i+1, cpu.program_counter, instructions[i]);
        
        // Simulate the instruction execution
        switch(i) {
            case 0: cpu.registers[1] = 10; break;
            case 1: cpu.registers[2] = 20; break;
            case 2: cpu.registers[3] = cpu.registers[1] + cpu.registers[2]; break;
            case 3: memory.sram_memory[0x300-0x200] = cpu.registers[3]; break;
        }
        
        cpu.program_counter += 4;  // Move to next instruction (4 bytes per instruction)
        
        if (i == 2) {
            printf("         Result: R3 = %d\n", cpu.registers[3]);
        }
    }
    
    printf("Final result stored in memory: %d\n", memory.sram_memory[0x300-0x200]);
    printf("\nThis is how YOUR C code gets executed by the CPU!\n");
}

// Show how peripherals are controlled through registers
void explain_peripheral_registers(void)
{
    printf("\n=== Peripheral Control Through Registers ===\n");
    
    // Simulate some common peripheral registers
    #define GPIO_OUTPUT_REG     0x60004004
    #define GPIO_ENABLE_REG     0x60004020
    #define TIMER_CONTROL_REG   0x3FF5F000
    #define UART_DATA_REG       0x60000000
    
    printf("Common ESP32 peripheral registers:\n");
    printf("GPIO Output:    0x%08X - Controls which pins are high/low\n", GPIO_OUTPUT_REG);
    printf("GPIO Enable:    0x%08X - Enables pins as outputs\n", GPIO_ENABLE_REG);
    printf("Timer Control:  0x%08X - Controls hardware timers\n", TIMER_CONTROL_REG);
    printf("UART Data:      0x%08X - Sends/receives serial data\n", UART_DATA_REG);
    
    printf("\nHow register control works:\n");
    printf("1. CPU writes value to register address\n");
    printf("2. Hardware sees the new value\n");
    printf("3. Hardware changes its behavior accordingly\n");
    printf("4. Real world changes happen (LED turns on, motor spins, etc.)\n");
    
    // Simulate writing to a register
    uint32_t simulated_gpio_reg = 0x00000000;
    printf("\nExample: Controlling GPIO register\n");
    printf("Initial GPIO state: 0x%08X (all pins off)\n", simulated_gpio_reg);
    
    simulated_gpio_reg |= (1 << 2);  // Turn on pin 2
    printf("After setting pin 2: 0x%08X (pin 2 LED turns on!)\n", simulated_gpio_reg);
    
    simulated_gpio_reg |= (1 << 5);  // Turn on pin 5
    printf("After setting pin 5: 0x%08X (pin 5 LED also on!)\n", simulated_gpio_reg);
}

// Explain the boot process - what happens when ESP32 starts
void explain_boot_process(void)
{
    printf("\n=== What Happens When ESP32 Boots Up ===\n");
    
    printf("1. POWER ON:\n");
    printf("   - Electricity flows to the chip\n");
    printf("   - Internal circuits stabilize\n");
    printf("   - Clock starts running\n\n");
    
    printf("2. ROM BOOTLOADER RUNS:\n");
    printf("   - Small program burned into ROM starts\n");
    printf("   - Checks for valid program in Flash\n");
    printf("   - Sets up basic hardware\n\n");
    
    printf("3. LOAD YOUR PROGRAM:\n");
    printf("   - Copies your code from Flash to faster memory\n");
    printf("   - Sets up stack and heap areas\n");
    printf("   - Initializes global variables\n\n");
    
    printf("4. CALL main():\n");
    printf("   - Jumps to your main() function\n");
    printf("   - Your code starts running!\n");
    printf("   - From here, it's all your program\n\n");
    
    printf("This entire process happens in milliseconds!\n");
}

// Show clock system basics
void explain_clock_system(void)
{
    printf("\n=== Clock System - The Heartbeat of Your MCU ===\n");
    
    printf("Every microcontroller needs a clock - like a heartbeat:\n");
    printf("- Each tick, CPU can execute one instruction\n");
    printf("- Faster clock = more instructions per second\n");
    printf("- But faster clock = more power consumption\n\n");
    
    printf("ESP32 clock options:\n");
    printf("- Internal RC: ~8 MHz (built-in, not very accurate)\n");
    printf("- External Crystal: 40 MHz (accurate, common choice)\n");
    printf("- PLL (Phase Locked Loop): up to 240 MHz (very fast!)\n\n");
    
    // Simulate different clock speeds
    uint32_t instructions_per_second[] = {8000000, 40000000, 240000000};
    const char* clock_names[] = {"8 MHz RC", "40 MHz Crystal", "240 MHz PLL"};
    
    printf("Instructions per second at different speeds:\n");
    for (int i = 0; i < 3; i++) {
        printf("%s: %d instructions/second\n", clock_names[i], (int)instructions_per_second[i]);
        
        // Calculate how long a simple delay loop takes
        uint32_t loop_cycles = 1000000;  // 1 million loop iterations
        float time_seconds = (float)loop_cycles / instructions_per_second[i];
        printf("  1M loop iterations take: %.3f seconds\n", time_seconds);
    }
    
    printf("\nFaster isn't always better - consider power consumption!\n");
}

// Memory mapping explanation
void explain_memory_mapping(void)
{
    printf("\n=== Memory Mapping - Where Everything Lives ===\n");
    
    printf("ESP32 memory map (simplified):\n");
    printf("0x00000000 - 0x3F3FFFFF: External Flash (your program)\n");
    printf("0x3F400000 - 0x3F7FFFFF: External RAM (if added)\n");
    printf("0x3FF80000 - 0x3FFFFFFF: Internal SRAM (variables)\n");
    printf("0x60000000 - 0x600FFFFF: Peripheral registers\n");
    printf("0x40000000 - 0x4000FFFF: ROM (bootloader)\n\n");
    
    printf("What this means for your code:\n");
    printf("- Your compiled program lives in Flash (0x00000000+)\n");
    printf("- Your variables live in SRAM (0x3FF80000+)\n");
    printf("- Hardware registers live in peripheral area (0x60000000+)\n");
    printf("- You access hardware by writing to specific addresses\n\n");
    
    // Show how this looks in code
    printf("In your C code:\n");
    printf("uint8_t my_variable = 10;  // Lives in SRAM area\n");
    printf("const char msg[] = \"Hi\";   // Lives in Flash area\n");
    printf("volatile uint32_t* gpio = (uint32_t*)0x60004004;  // Points to hardware\n");
}

int main(void)
{
    printf("Welcome to Microcontroller Architecture!\n");
    printf("=======================================\n");
    
    // Explain the different types of memory
    explain_memory_types();
    
    // Show how CPU executes code
    simulate_cpu_execution();
    
    // Explain peripheral registers
    explain_peripheral_registers();
    
    // Boot process
    explain_boot_process();
    
    // Clock system
    explain_clock_system();
    
    // Memory mapping
    explain_memory_mapping();
    
    printf("\n=== Key Takeaways ===\n");
    printf("1. MCU = CPU + Flash + SRAM + Peripherals all on one chip\n");
    printf("2. Your code lives in Flash, variables live in SRAM\n");
    printf("3. Hardware is controlled by writing to register addresses\n");
    printf("4. CPU executes instructions one by one, driven by clock\n");
    printf("5. Boot process loads and starts your program automatically\n");
    printf("6. Memory mapping determines where everything lives\n");
    printf("7. Understanding architecture helps you write efficient code!\n");
    
    return 0;
}

/*
 * What did we learn?
 * 
 * 1. Microcontrollers integrate CPU, memory, and peripherals on one chip
 * 2. Flash stores your program, SRAM stores your variables
 * 3. Registers are special memory locations that control hardware
 * 4. CPU executes instructions sequentially, controlled by clock
 * 5. Boot process automatically starts your program when powered on
 * 6. Memory mapping defines where different types of data live
 * 7. This knowledge helps you understand how your C code becomes hardware action
 * 
 * Next: Direct register access and GPIO control!
 */