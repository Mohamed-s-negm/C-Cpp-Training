/*
 * MODULE 2 - LESSON 4: Interrupts - Instant Response to Events
 * 
 * What you'll learn:
 * - What interrupts are and why they're essential
 * - How to write Interrupt Service Routines (ISRs)
 * - Different types of interrupts and when to use them
 * - Best practices for interrupt programming
 * 
 * Think of interrupts like urgent phone calls:
 * - You're doing something (main program running)
 * - Phone rings (interrupt occurs)  
 * - You stop what you're doing (save context)
 * - Answer the phone (run ISR)
 * - Hang up and continue what you were doing (restore context)
 */

#include <stdio.h>
#include <stdint.h>

// Simulate interrupt system
volatile uint8_t interrupt_pending = 0;
volatile uint8_t button_pressed_flag = 0;
volatile uint32_t timer_overflow_count = 0;
volatile uint8_t emergency_stop_flag = 0;

// Interrupt types
#define BUTTON_INTERRUPT    0
#define TIMER_INTERRUPT     1  
#define EMERGENCY_INTERRUPT 2

// What is an interrupt? Basic explanation
void what_are_interrupts(void)
{
    printf("=== What Are Interrupts? ===\n");
    
    printf("Without interrupts (polling method):\n");
    printf("while(1) {\n");
    printf("    if(button_pressed()) handle_button();  // Check constantly\n");
    printf("    if(timer_expired()) handle_timer();    // Check constantly\n");
    printf("    if(sensor_ready()) read_sensor();      // Check constantly\n");
    printf("    do_main_work();                        // Finally do real work\n");
    printf("}\n");
    printf("Problems: Wastes CPU time, might miss fast events\n\n");
    
    printf("With interrupts (event-driven method):\n");
    printf("// Setup interrupts once\n");
    printf("setup_button_interrupt();\n");
    printf("setup_timer_interrupt();\n");
    printf("setup_sensor_interrupt();\n\n");
    
    printf("// Main program focuses on main work\n");
    printf("while(1) {\n");
    printf("    do_main_work();  // CPU does useful work\n");
    printf("    // Interrupts handle events automatically!\n");
    printf("}\n\n");
    
    printf("// Interrupt Service Routines (ISRs) - run when events happen\n");
    printf("void button_isr() { /* Handle button instantly */ }\n");
    printf("void timer_isr()  { /* Handle timer instantly */ }\n");
    printf("void sensor_isr() { /* Handle sensor instantly */ }\n\n");
    
    printf("Benefits: Fast response, efficient CPU use, don't miss events\n");
}

// Simulated ISR functions (real ISRs are called by hardware)
void button_isr(void)
{
    // This function would be called automatically when button is pressed
    printf("    [ISR] Button interrupt! Button was pressed.\n");
    button_pressed_flag = 1;  // Set flag for main program to handle
    // ISR should be SHORT and FAST!
}

void timer_isr(void)
{
    // This function would be called automatically when timer overflows
    printf("    [ISR] Timer interrupt! Timer overflow occurred.\n");
    timer_overflow_count++;   // Just increment counter
    // ISR should be SHORT and FAST!
}

void emergency_isr(void)
{
    // This function would be called for urgent safety events
    printf("    [ISR] EMERGENCY! Safety system activated!\n");
    emergency_stop_flag = 1;  // Set critical safety flag
    // ISR should be SHORT and FAST!
}

// Simulate interrupt handling
void simulate_interrupt_system(void)
{
    printf("\n=== Interrupt System Simulation ===\n");
    
    printf("Setting up interrupt system...\n");
    printf("- Button interrupt: Enabled\n");
    printf("- Timer interrupt: Enabled\n"); 
    printf("- Emergency interrupt: Enabled\n\n");
    
    printf("Main program running normally...\n");
    
    // Simulate main program loop with interrupts
    for (int cycle = 0; cycle < 5; cycle++) {
        printf("\nMain program cycle %d: Doing important calculations...\n", cycle + 1);
        
        // Simulate different interrupt events
        switch (cycle) {
            case 1:
                printf("** INTERRUPT EVENT: Button pressed! **\n");
                button_isr();  // Simulate interrupt call
                break;
                
            case 3:
                printf("** INTERRUPT EVENT: Timer overflow! **\n");
                timer_isr();   // Simulate interrupt call
                break;
                
            case 4:
                printf("** INTERRUPT EVENT: Emergency detected! **\n");
                emergency_isr(); // Simulate interrupt call
                break;
        }
        
        // Main program checks flags set by ISRs
        if (button_pressed_flag) {
            printf("Main: Handling button press (flag was set by ISR)\n");
            button_pressed_flag = 0;  // Clear flag
        }
        
        if (timer_overflow_count > 0) {
            printf("Main: Timer overflowed %d times\n", (int)timer_overflow_count);
            // Don't clear counter, just note it
        }
        
        if (emergency_stop_flag) {
            printf("Main: EMERGENCY STOP! Shutting down safely...\n");
            emergency_stop_flag = 0;  // Clear flag after handling
        }
        
        printf("Main: Continuing normal operation...\n");
    }
}

// Different interrupt trigger types
void interrupt_trigger_types(void)
{
    printf("\n=== Different Interrupt Trigger Types ===\n");
    
    printf("1. EDGE TRIGGERED INTERRUPTS:\n");
    printf("   - Rising Edge: Triggers when signal goes LOW → HIGH\n");
    printf("   - Falling Edge: Triggers when signal goes HIGH → LOW\n");
    printf("   - Both Edges: Triggers on any change\n");
    printf("   Use for: Buttons, switches, pulse counting\n\n");
    
    printf("2. LEVEL TRIGGERED INTERRUPTS:\n");
    printf("   - High Level: Triggers while signal is HIGH\n");
    printf("   - Low Level: Triggers while signal is LOW\n");
    printf("   Use for: Urgent conditions that need constant attention\n\n");
    
    printf("3. TIMER INTERRUPTS:\n");
    printf("   - Periodic: Triggers at regular intervals\n");
    printf("   - One-shot: Triggers once after a delay\n");
    printf("   Use for: Real-time tasks, timeouts, periodic sampling\n\n");
    
    // Simulate different trigger types
    printf("Simulation of edge-triggered button interrupt:\n");
    uint8_t button_state = 0;
    
    // Rising edge detection
    printf("Button: LOW → HIGH (Rising edge detected!)\n");
    button_isr();
    
    printf("Button: HIGH (no interrupt, already high)\n");
    printf("Button: HIGH (no interrupt, still high)\n");
    
    printf("Button: HIGH → LOW (Falling edge, but we only watch rising)\n");
    printf("Button: LOW (no interrupt)\n");
    
    printf("Button: LOW → HIGH (Rising edge detected again!)\n");
    button_isr();
}

// ISR best practices and rules
void isr_best_practices(void)
{
    printf("\n=== ISR Best Practices and Rules ===\n");
    
    printf("RULE 1: Keep ISRs SHORT and FAST\n");
    printf("Good ISR:\n");
    printf("void button_isr() {\n");
    printf("    button_flag = 1;     // Just set a flag\n");
    printf("    button_count++;      // Maybe count events\n");
    printf("}  // Total time: microseconds\n\n");
    
    printf("Bad ISR:\n");
    printf("void button_isr() {\n");
    printf("    printf(\"Button!\");     // NEVER do I/O in ISR!\n");
    printf("    delay(100);           // NEVER delay in ISR!\n");
    printf("    complex_calculation(); // NEVER do heavy work in ISR!\n");
    printf("}  // Total time: milliseconds (TOO LONG!)\n\n");
    
    printf("RULE 2: Don't call functions that can block\n");
    printf("AVOID in ISRs:\n");
    printf("- printf(), scanf() (I/O functions)\n");
    printf("- malloc(), free() (memory allocation)\n");
    printf("- delay(), sleep() (timing functions)\n");
    printf("- Complex calculations\n");
    printf("- Non-reentrant functions\n\n");
    
    printf("RULE 3: Use volatile for shared variables\n");
    printf("volatile uint8_t interrupt_flag = 0;  // ALWAYS volatile!\n");
    printf("Why? ISR and main program both access it\n");
    printf("Compiler needs to know it can change anytime\n\n");
    
    printf("RULE 4: Use flags to communicate with main program\n");
    printf("ISR sets flags → Main program checks flags → Main program does work\n");
    printf("This keeps ISRs fast and main program flexible\n\n");
    
    printf("RULE 5: Be careful with shared resources\n");
    printf("If both ISR and main program use same variable:\n");
    printf("- Make it volatile\n");
    printf("- Consider disabling interrupts during critical sections\n");
    printf("- Use atomic operations when possible\n");
}

// Real-world interrupt examples
void real_world_examples(void)
{
    printf("\n=== Real-World Interrupt Examples ===\n");
    
    printf("1. SAFETY STOP BUTTON:\n");
    printf("   - Emergency stop must work INSTANTLY\n");
    printf("   - Can't wait for main program to check\n");
    printf("   - High priority interrupt\n");
    printf("   ISR: Set emergency_stop flag immediately\n\n");
    
    printf("2. SERIAL DATA RECEPTION:\n");
    printf("   - Data arrives at any time\n");
    printf("   - Must be read before next byte arrives\n");
    printf("   - Buffer overflow if too slow\n");
    printf("   ISR: Read byte, store in buffer\n\n");
    
    printf("3. ENCODER COUNTING:\n");
    printf("   - Motor encoder pulses come fast\n");
    printf("   - Miss a pulse = wrong position\n");
    printf("   - Edge-triggered interrupt\n");
    printf("   ISR: Increment/decrement position counter\n\n");
    
    printf("4. PERIODIC SENSOR READING:\n");
    printf("   - Need regular temperature measurements\n");
    printf("   - Every 1 second exactly\n");
    printf("   - Timer interrupt\n");
    printf("   ISR: Set 'time_to_read_sensor' flag\n\n");
    
    printf("5. WATCHDOG TIMER:\n");
    printf("   - System must 'kick' watchdog regularly\n");
    printf("   - If main program hangs, watchdog resets system\n");
    printf("   - Safety feature\n");
    printf("   ISR: Reset entire microcontroller\n");
}

// Interrupt priorities and nesting
void interrupt_priorities(void)
{
    printf("\n=== Interrupt Priorities and Nesting ===\n");
    
    printf("Some interrupts are more important than others:\n\n");
    
    printf("PRIORITY 1 (HIGHEST): Safety and critical systems\n");
    printf("- Emergency stop buttons\n");
    printf("- Overcurrent protection\n");
    printf("- Watchdog timer\n");
    printf("- System faults\n\n");
    
    printf("PRIORITY 2 (HIGH): Time-critical communication\n");
    printf("- UART receive (before buffer overflow)\n");
    printf("- SPI data ready\n");
    printf("- High-speed pulse counting\n\n");
    
    printf("PRIORITY 3 (MEDIUM): Regular timing\n");
    printf("- System tick (1ms timer)\n");
    printf("- Periodic sensor reading\n");
    printf("- PWM updates\n\n");
    
    printf("PRIORITY 4 (LOW): User interface\n");
    printf("- Button presses\n");
    printf("- Rotary encoder\n");
    printf("- Display updates\n\n");
    
    printf("INTERRUPT NESTING:\n");
    printf("Higher priority interrupts can interrupt lower priority ones\n");
    printf("Example: Emergency stop can interrupt button handler\n");
    printf("But button handler cannot interrupt emergency stop\n");
}

int main(void)
{
    printf("Welcome to Interrupts and ISRs!\n");
    printf("===============================\n");
    
    // Explain what interrupts are
    what_are_interrupts();
    
    // Simulate interrupt system
    simulate_interrupt_system();
    
    // Different trigger types
    interrupt_trigger_types();
    
    // Best practices
    isr_best_practices();
    
    // Real examples
    real_world_examples();
    
    // Priorities
    interrupt_priorities();
    
    printf("\n=== Key Takeaways ===\n");
    printf("1. Interrupts provide instant response to events\n");
    printf("2. ISRs must be SHORT and FAST\n");
    printf("3. Use volatile for variables shared between ISR and main\n");
    printf("4. ISRs set flags, main program does the heavy work\n");
    printf("5. Different interrupt types serve different purposes\n");
    printf("6. Interrupt priorities ensure critical events are handled first\n");
    printf("7. Proper interrupt design makes systems responsive and efficient\n");
    
    printf("\n=== Common ISR Pattern ===\n");
    printf("// In ISR (keep it simple!):\n");
    printf("void some_isr(void) {\n");
    printf("    event_flag = 1;        // Set flag\n");
    printf("    event_count++;         // Update counter\n");
    printf("    // That's it! No complex work here.\n");
    printf("}\n\n");
    
    printf("// In main program (do the work):\n");
    printf("while(1) {\n");
    printf("    if(event_flag) {\n");
    printf("        event_flag = 0;    // Clear flag\n");
    printf("        handle_event();    // Do complex work here\n");
    printf("    }\n");
    printf("    // Continue main program\n");
    printf("}\n");
    
    return 0;
}

/*
 * What did we learn?
 * 
 * 1. Interrupts provide immediate response to hardware events
 * 2. ISRs (Interrupt Service Routines) handle interrupt events
 * 3. ISRs must be fast - just set flags and return quickly
 * 4. Main program checks flags and does the actual work
 * 5. volatile keyword is essential for interrupt-shared variables
 * 6. Different interrupt types (edge, level, timer) serve different needs
 * 7. Interrupt priorities ensure critical events get handled first
 * 8. Proper interrupt design makes embedded systems responsive and efficient
 * 
 * Next Module: Real Hardware - GPIO, UART, ADC, PWM on ESP32!
 */