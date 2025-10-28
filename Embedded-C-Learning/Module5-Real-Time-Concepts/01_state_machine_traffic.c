/*
 * Module 5.1: Finite State Machines (FSM) - Simple Traffic Light
 * 
 * What is a State Machine? Think of it like a flowchart that controls behavior:
 * - Current STATE: Where are we now? (like Red Light, Green Light)
 * - EVENTS: What happened? (like Timer expired, Button pressed)
 * - TRANSITIONS: What do we do next? (like Red -> Green -> Yellow -> Red)
 * 
 * State machines make embedded systems predictable and easy to debug
 * This example shows a simple traffic light controller
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

// Define the different states our traffic light can be in
// Think of these as different "moods" the traffic light can have
typedef enum {
    STATE_RED,           // Cars stop, pedestrians can cross
    STATE_GREEN,         // Cars go, pedestrians wait
    STATE_YELLOW,        // Cars prepare to stop
    STATE_RED_YELLOW,    // Some countries use this transition
    STATE_PEDESTRIAN,    // Special pedestrian crossing phase
    STATE_ERROR         // Something went wrong
} TrafficLightState;

// Define events that can happen
// Think of these as "things that cause changes"
typedef enum {
    EVENT_TIMER_EXPIRED,    // Time to change state
    EVENT_BUTTON_PRESSED,   // Pedestrian button pressed
    EVENT_EMERGENCY,        // Emergency vehicle coming
    EVENT_SENSOR_TRIGGERED, // Car sensor activated
    EVENT_RESET            // System reset
} TrafficLightEvent;

// Structure to hold our traffic light system information
// Think of this as the "brain" of our traffic light
typedef struct {
    TrafficLightState currentState;      // Where are we now?
    TrafficLightState previousState;     // Where did we come from?
    uint32_t stateStartTime;            // When did we enter this state?
    uint32_t stateDuration;             // How long should we stay here?
    bool pedestrianRequested;           // Did someone press the button?
    bool emergencyMode;                 // Are we in emergency mode?
    uint32_t totalCycles;               // How many complete cycles?
    bool errorFlag;                     // Is there an error?
} TrafficLightSystem;

// Create our traffic light system
TrafficLightSystem trafficLight = {
    .currentState = STATE_RED,
    .previousState = STATE_RED,
    .stateStartTime = 0,
    .stateDuration = 0,
    .pedestrianRequested = false,
    .emergencyMode = false,
    .totalCycles = 0,
    .errorFlag = false
};

// Simulated hardware functions (in real hardware, these would control actual LEDs)
void turnOnRedLight() {
    printf("🔴 RED light ON\n");
}

void turnOffRedLight() {
    printf("⚫ RED light OFF\n");
}

void turnOnGreenLight() {
    printf("🟢 GREEN light ON\n");
}

void turnOffGreenLight() {
    printf("⚫ GREEN light OFF\n");
}

void turnOnYellowLight() {
    printf("🟡 YELLOW light ON\n");
}

void turnOffYellowLight() {
    printf("⚫ YELLOW light OFF\n");
}

void turnOnPedestrianSignal() {
    printf("🚶 WALK signal ON\n");
}

void turnOffPedestrianSignal() {
    printf("🚫 DON'T WALK signal ON\n");
}

void soundEmergencyAlarm() {
    printf("🚨 EMERGENCY ALARM!\n");
}

// Function to get current time (milliseconds)
// In real embedded systems, this would come from a hardware timer
uint32_t getCurrentTime() {
    static uint32_t simulatedTime = 0;
    simulatedTime += 1000;  // Simulate 1 second passing each call
    return simulatedTime;
}

// Function to turn off all lights (safety first!)
void turnOffAllLights() {
    turnOffRedLight();
    turnOffGreenLight();
    turnOffYellowLight();
    turnOffPedestrianSignal();
}

// Function to enter a new state
// Think of this as "changing the mood" of the traffic light
void enterState(TrafficLightState newState) {
    // Remember where we came from
    trafficLight.previousState = trafficLight.currentState;
    trafficLight.currentState = newState;
    trafficLight.stateStartTime = getCurrentTime();
    
    printf("\n--- State Change ---\n");
    printf("From: ");
    
    // Print previous state name
    switch (trafficLight.previousState) {
        case STATE_RED:         printf("RED"); break;
        case STATE_GREEN:       printf("GREEN"); break;
        case STATE_YELLOW:      printf("YELLOW"); break;
        case STATE_RED_YELLOW:  printf("RED+YELLOW"); break;
        case STATE_PEDESTRIAN:  printf("PEDESTRIAN"); break;
        case STATE_ERROR:       printf("ERROR"); break;
    }
    
    printf(" → To: ");
    
    // Print new state name and set up the lights
    switch (newState) {
        case STATE_RED:
            printf("RED\n");
            turnOffAllLights();
            turnOnRedLight();
            turnOnPedestrianSignal();  // Pedestrians can cross
            trafficLight.stateDuration = 10000;  // 10 seconds
            break;
            
        case STATE_GREEN:
            printf("GREEN\n");
            turnOffAllLights();
            turnOnGreenLight();
            turnOffPedestrianSignal();  // Pedestrians must wait
            trafficLight.stateDuration = 15000;  // 15 seconds
            break;
            
        case STATE_YELLOW:
            printf("YELLOW\n");
            turnOffAllLights();
            turnOnYellowLight();
            turnOffPedestrianSignal();  // Pedestrians must wait
            trafficLight.stateDuration = 3000;   // 3 seconds
            break;
            
        case STATE_RED_YELLOW:
            printf("RED+YELLOW (Prepare to go)\n");
            turnOffAllLights();
            turnOnRedLight();
            turnOnYellowLight();
            turnOffPedestrianSignal();
            trafficLight.stateDuration = 2000;   // 2 seconds
            break;
            
        case STATE_PEDESTRIAN:
            printf("PEDESTRIAN CROSSING\n");
            turnOffAllLights();
            turnOnRedLight();
            turnOnPedestrianSignal();
            trafficLight.stateDuration = 20000;  // 20 seconds for crossing
            trafficLight.pedestrianRequested = false;  // Request fulfilled
            break;
            
        case STATE_ERROR:
            printf("ERROR - Flashing Red\n");
            turnOffAllLights();
            soundEmergencyAlarm();
            trafficLight.stateDuration = 1000;   // 1 second flashing
            trafficLight.errorFlag = true;
            break;
    }
    
    printf("Duration: %d seconds\n", trafficLight.stateDuration / 1000);
}

// Function to handle events (things that happen to our state machine)
// Think of this as "reacting to the world around us"
void handleEvent(TrafficLightEvent event) {
    printf("\n⚡ Event: ");
    
    switch (event) {
        case EVENT_TIMER_EXPIRED:
            printf("Timer Expired\n");
            
            // What happens next depends on what state we're in
            switch (trafficLight.currentState) {
                case STATE_RED:
                    if (trafficLight.pedestrianRequested) {
                        enterState(STATE_PEDESTRIAN);  // Give pedestrians time
                    } else {
                        enterState(STATE_RED_YELLOW);  // Normal sequence
                    }
                    break;
                    
                case STATE_RED_YELLOW:
                    enterState(STATE_GREEN);
                    break;
                    
                case STATE_GREEN:
                    enterState(STATE_YELLOW);
                    break;
                    
                case STATE_YELLOW:
                    enterState(STATE_RED);
                    trafficLight.totalCycles++;  // Count complete cycles
                    break;
                    
                case STATE_PEDESTRIAN:
                    enterState(STATE_RED_YELLOW);
                    break;
                    
                case STATE_ERROR:
                    // Flash red light
                    static bool redFlashing = false;
                    if (redFlashing) {
                        turnOffRedLight();
                        redFlashing = false;
                    } else {
                        turnOnRedLight();
                        redFlashing = true;
                    }
                    // Stay in error state until reset
                    break;
            }
            break;
            
        case EVENT_BUTTON_PRESSED:
            printf("Pedestrian Button Pressed\n");
            trafficLight.pedestrianRequested = true;
            
            // If we're in green state and button is pressed, go to yellow sooner
            if (trafficLight.currentState == STATE_GREEN) {
                uint32_t currentTime = getCurrentTime();
                uint32_t timeInState = currentTime - trafficLight.stateStartTime;
                
                // If we've been green for at least 5 seconds, start changing
                if (timeInState > 5000) {
                    enterState(STATE_YELLOW);
                }
            }
            break;
            
        case EVENT_EMERGENCY:
            printf("Emergency Vehicle Detected\n");
            trafficLight.emergencyMode = true;
            enterState(STATE_GREEN);  // Clear the way immediately
            break;
            
        case EVENT_SENSOR_TRIGGERED:
            printf("Car Sensor Triggered\n");
            // Could extend green time if cars are still coming
            if (trafficLight.currentState == STATE_GREEN) {
                trafficLight.stateDuration += 5000;  // Add 5 seconds
            }
            break;
            
        case EVENT_RESET:
            printf("System Reset\n");
            trafficLight.emergencyMode = false;
            trafficLight.errorFlag = false;
            trafficLight.pedestrianRequested = false;
            enterState(STATE_RED);  // Always start with red for safety
            break;
    }
}

// Function to check if it's time to change states
// Think of this as "checking the clock"
bool isTimeToChangeState() {
    uint32_t currentTime = getCurrentTime();
    uint32_t timeInState = currentTime - trafficLight.stateStartTime;
    
    return (timeInState >= trafficLight.stateDuration);
}

// Function to display system status
// Think of this as a "dashboard" showing what's happening
void displayStatus() {
    printf("\n📊 Traffic Light Status:\n");
    printf("Current State: ");
    
    switch (trafficLight.currentState) {
        case STATE_RED:         printf("🔴 RED"); break;
        case STATE_GREEN:       printf("🟢 GREEN"); break;
        case STATE_YELLOW:      printf("🟡 YELLOW"); break;
        case STATE_RED_YELLOW:  printf("🔴🟡 RED+YELLOW"); break;
        case STATE_PEDESTRIAN:  printf("🚶 PEDESTRIAN"); break;
        case STATE_ERROR:       printf("❌ ERROR"); break;
    }
    
    uint32_t currentTime = getCurrentTime();
    uint32_t timeInState = currentTime - trafficLight.stateStartTime;
    uint32_t timeRemaining = trafficLight.stateDuration - timeInState;
    
    printf(" (Time remaining: %d seconds)\n", timeRemaining / 1000);
    printf("Total cycles completed: %d\n", trafficLight.totalCycles);
    printf("Pedestrian requested: %s\n", trafficLight.pedestrianRequested ? "Yes" : "No");
    printf("Emergency mode: %s\n", trafficLight.emergencyMode ? "Yes" : "No");
    printf("Error flag: %s\n", trafficLight.errorFlag ? "Yes" : "No");
}

// Function to run one cycle of the state machine
// This is the "heartbeat" of our system
void runStateMachine() {
    // Check if it's time to change states
    if (isTimeToChangeState()) {
        handleEvent(EVENT_TIMER_EXPIRED);
    }
    
    // Display current status
    displayStatus();
}

// Function to simulate external events happening
// In real systems, these would come from hardware (buttons, sensors, etc.)
void simulateEvents() {
    static int eventCounter = 0;
    eventCounter++;
    
    // Simulate different events at different times
    if (eventCounter == 5) {
        handleEvent(EVENT_BUTTON_PRESSED);
    } else if (eventCounter == 15) {
        handleEvent(EVENT_SENSOR_TRIGGERED);
    } else if (eventCounter == 25) {
        handleEvent(EVENT_EMERGENCY);
    } else if (eventCounter == 30) {
        handleEvent(EVENT_RESET);
        eventCounter = 0;  // Reset the simulation
    }
}

int main() {
    printf("🚦 Traffic Light State Machine Demo\n");
    printf("===================================\n");
    
    // Initialize the system (start in safe state)
    enterState(STATE_RED);
    
    printf("\nStarting traffic light simulation...\n");
    printf("Watch how the state machine handles different events!\n");
    
    // Run the simulation for a while
    for (int i = 0; i < 50; i++) {  // 50 cycles = about 50 simulated seconds
        printf("\n--- Cycle %d ---\n", i + 1);
        
        // Run the state machine
        runStateMachine();
        
        // Simulate some events happening
        simulateEvents();
        
        // Small delay to make it readable
        printf("(Simulating 1 second delay...)\n");
    }
    
    printf("\n🎯 State Machine Demo Complete!\n");
    printf("Total traffic cycles: %d\n", trafficLight.totalCycles);
    
    return 0;
}

/*
 * Key State Machine Concepts Demonstrated:
 * 
 * 1. STATES: Clear definition of what each state means
 *    - Each state has specific behavior (which lights are on)
 *    - Each state has a duration (how long to stay there)
 * 
 * 2. EVENTS: Things that can cause state changes
 *    - Timer expiration (automatic progression)
 *    - Button press (user input)
 *    - Sensor trigger (external input)
 *    - Emergency (priority event)
 * 
 * 3. TRANSITIONS: Rules for moving between states
 *    - Each state knows what comes next
 *    - Events can override normal progression
 *    - Safety is always considered (start with RED)
 * 
 * 4. STATE DATA: Information that persists during states
 *    - Time tracking
 *    - Event flags
 *    - System status
 * 
 * Real-World Applications:
 * - Traffic lights (this example)
 * - Washing machine controllers
 * - Elevator control systems
 * - Robot behavior control
 * - Communication protocols
 * - User interface controls
 * - Security system states
 * 
 * Advantages of State Machines:
 * ✅ Predictable behavior
 * ✅ Easy to debug
 * ✅ Clear code structure
 * ✅ Easy to test each state
 * ✅ Easy to add new states/events
 * ✅ Prevents impossible conditions
 * 
 * When to Use State Machines:
 * - System has clearly defined modes/phases
 * - Behavior changes based on current situation
 * - Need to handle various events differently
 * - Safety is critical (like traffic control)
 * - System has complex timing requirements
 * 
 * Implementation Tips:
 * 1. Start simple - add complexity gradually
 * 2. Always have a safe default state
 * 3. Handle unexpected events gracefully
 * 4. Use enums for states and events (type safety)
 * 5. Keep state entry/exit code in one place
 * 6. Log state changes for debugging
 * 7. Test all possible transitions
 * 8. Consider timeout events for stuck states
 */