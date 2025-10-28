/*
 * MODULE 3 - LESSON 4: PWM - Pulse Width Modulation
 * 
 * What you'll learn:
 * - How PWM creates "analog" output from digital pins
 * - Controlling LED brightness, motor speed, servo position
 * - PWM frequency, duty cycle, and resolution concepts
 * - Real applications: dimming, motor control, audio generation
 * 
 * PWM = Pulse Width Modulation
 * Think of it like a light switch that turns on/off very fast:
 * - Fast switching creates the illusion of analog control
 * - 0% duty cycle = always OFF = 0V average
 * - 50% duty cycle = half ON, half OFF = 1.65V average  
 * - 100% duty cycle = always ON = 3.3V average
 * 
 * NOTE: This code runs on ESP32 with Arduino IDE
 */

// PWM pin definitions (ESP32 can do PWM on most GPIO pins)
#define LED_PIN         2   // Built-in LED
#define EXTERNAL_LED    4   // External LED
#define MOTOR_PIN       5   // DC motor control
#define SERVO_PIN       18  // Servo motor signal
#define BUZZER_PIN      19  // Buzzer for audio
#define RGB_RED_PIN     21  // Red LED in RGB LED
#define RGB_GREEN_PIN   22  // Green LED in RGB LED  
#define RGB_BLUE_PIN    23  // Blue LED in RGB LED

// PWM configuration
#define PWM_FREQUENCY   1000    // 1 kHz frequency
#define PWM_RESOLUTION  8       // 8-bit resolution (0-255)
#define PWM_CHANNEL_0   0       // PWM channel for LED
#define PWM_CHANNEL_1   1       // PWM channel for motor
#define PWM_CHANNEL_2   2       // PWM channel for servo
#define PWM_CHANNEL_3   3       // PWM channel for buzzer
#define PWM_CHANNEL_4   4       // PWM channel for RGB red
#define PWM_CHANNEL_5   5       // PWM channel for RGB green
#define PWM_CHANNEL_6   6       // PWM channel for RGB blue

// Servo control constants
#define SERVO_MIN_PULSE_MS  0.5   // 0.5ms pulse = 0 degrees
#define SERVO_MAX_PULSE_MS  2.5   // 2.5ms pulse = 180 degrees
#define SERVO_FREQUENCY     50    // 50 Hz = 20ms period

void setup() {
    Serial.begin(115200);
    while(!Serial) delay(10);
    
    Serial.println("=== ESP32 PWM Control Examples ===");
    Serial.println();
    
    // Configure PWM channels
    setupPWMChannels();
    
    Serial.println("PWM Configuration:");
    Serial.print("- Frequency: ");
    Serial.print(PWM_FREQUENCY);
    Serial.println(" Hz");
    Serial.print("- Resolution: ");
    Serial.print(PWM_RESOLUTION);
    Serial.print(" bits (0-");
    Serial.print((1 << PWM_RESOLUTION) - 1);
    Serial.println(")");
    Serial.println();
    
    delay(1000);
}

void loop() {
    Serial.println("=== PWM Examples Menu ===");
    Serial.println("Running all examples in sequence...");
    Serial.println();
    
    // Run PWM examples
    ledBrightnessControl();
    delay(2000);
    
    motorSpeedControl();
    delay(2000);
    
    servoPositionControl();
    delay(2000);
    
    audioToneGeneration();
    delay(2000);
    
    rgbLedControl();
    delay(2000);
    
    pwmEffectsDemo();
    delay(3000);
    
    Serial.println("All examples complete. Restarting in 5 seconds...");
    delay(5000);
}

/*
 * SETUP FUNCTIONS
 */
void setupPWMChannels() {
    Serial.println("Setting up PWM channels...");
    
    // Setup PWM channels with different configurations
    ledcSetup(PWM_CHANNEL_0, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcSetup(PWM_CHANNEL_1, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcSetup(PWM_CHANNEL_2, SERVO_FREQUENCY, 16);  // Higher resolution for servo
    ledcSetup(PWM_CHANNEL_3, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcSetup(PWM_CHANNEL_4, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcSetup(PWM_CHANNEL_5, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcSetup(PWM_CHANNEL_6, PWM_FREQUENCY, PWM_RESOLUTION);
    
    // Attach pins to PWM channels
    ledcAttachPin(LED_PIN, PWM_CHANNEL_0);
    ledcAttachPin(MOTOR_PIN, PWM_CHANNEL_1);
    ledcAttachPin(SERVO_PIN, PWM_CHANNEL_2);
    ledcAttachPin(BUZZER_PIN, PWM_CHANNEL_3);
    ledcAttachPin(RGB_RED_PIN, PWM_CHANNEL_4);
    ledcAttachPin(RGB_GREEN_PIN, PWM_CHANNEL_5);
    ledcAttachPin(RGB_BLUE_PIN, PWM_CHANNEL_6);
    
    Serial.println("PWM channels configured successfully!");
}

/*
 * EXAMPLE 1: LED Brightness Control
 */
void ledBrightnessControl() {
    Serial.println("--- LED Brightness Control ---");
    Serial.println("Demonstrating PWM duty cycle effects:");
    
    // Fade up from 0% to 100%
    Serial.println("Fading LED up (0% to 100%)...");
    for(int brightness = 0; brightness <= 255; brightness += 5) {
        ledcWrite(PWM_CHANNEL_0, brightness);
        
        float percentage = (brightness * 100.0) / 255.0;
        Serial.print("Brightness: ");
        Serial.print(percentage, 1);
        Serial.print("% (PWM value: ");
        Serial.print(brightness);
        Serial.println(")");
        
        delay(100);
    }
    
    delay(500);
    
    // Fade down from 100% to 0%
    Serial.println("Fading LED down (100% to 0%)...");
    for(int brightness = 255; brightness >= 0; brightness -= 5) {
        ledcWrite(PWM_CHANNEL_0, brightness);
        
        float percentage = (brightness * 100.0) / 255.0;
        Serial.print("Brightness: ");
        Serial.print(percentage, 1);
        Serial.println("%");
        
        delay(100);
    }
    
    Serial.println("LED brightness control complete!");
    Serial.println();
}

/*
 * EXAMPLE 2: Motor Speed Control
 */
void motorSpeedControl() {
    Serial.println("--- DC Motor Speed Control ---");
    Serial.println("Note: Connect motor through motor driver (not directly to ESP32!)");
    
    // Different speed levels
    int speed_levels[] = {0, 64, 128, 192, 255};  // 0%, 25%, 50%, 75%, 100%
    const char* speed_names[] = {"Stop", "Slow", "Medium", "Fast", "Maximum"};
    
    for(int i = 0; i < 5; i++) {
        int speed = speed_levels[i];
        float percentage = (speed * 100.0) / 255.0;
        
        Serial.print("Setting motor speed: ");
        Serial.print(speed_names[i]);
        Serial.print(" (");
        Serial.print(percentage, 0);
        Serial.print("% - PWM: ");
        Serial.print(speed);
        Serial.println(")");
        
        ledcWrite(PWM_CHANNEL_1, speed);
        delay(2000);
    }
    
    // Gradual acceleration and deceleration
    Serial.println("Demonstrating smooth acceleration...");
    for(int speed = 0; speed <= 255; speed += 3) {
        ledcWrite(PWM_CHANNEL_1, speed);
        delay(50);
    }
    
    delay(1000);
    
    Serial.println("Demonstrating smooth deceleration...");
    for(int speed = 255; speed >= 0; speed -= 3) {
        ledcWrite(PWM_CHANNEL_1, speed);
        delay(50);
    }
    
    Serial.println("Motor control complete!");
    Serial.println();
}

/*
 * EXAMPLE 3: Servo Position Control
 */
void servoPositionControl() {
    Serial.println("--- Servo Motor Position Control ---");
    Serial.println("Moving servo to different angles:");
    
    // Move to specific angles
    int angles[] = {0, 45, 90, 135, 180, 90, 0};
    
    for(int i = 0; i < 7; i++) {
        int angle = angles[i];
        
        Serial.print("Moving servo to ");
        Serial.print(angle);
        Serial.println(" degrees");
        
        setServoAngle(angle);
        delay(1000);
    }
    
    // Smooth sweep
    Serial.println("Smooth servo sweep (0° to 180° to 0°):");
    
    // Sweep from 0 to 180
    for(int angle = 0; angle <= 180; angle += 2) {
        setServoAngle(angle);
        delay(50);
    }
    
    delay(500);
    
    // Sweep from 180 to 0
    for(int angle = 180; angle >= 0; angle -= 2) {
        setServoAngle(angle);
        delay(50);
    }
    
    Serial.println("Servo control complete!");
    Serial.println();
}

void setServoAngle(int angle) {
    // Constrain angle to valid range
    if(angle < 0) angle = 0;
    if(angle > 180) angle = 180;
    
    // Calculate pulse width in microseconds
    // 0° = 500µs, 180° = 2500µs
    int pulse_width_us = map(angle, 0, 180, 500, 2500);
    
    // Convert to PWM value for 16-bit resolution at 50Hz
    // Period = 20ms = 20000µs
    int pwm_value = map(pulse_width_us, 0, 20000, 0, 65535);
    
    ledcWrite(PWM_CHANNEL_2, pwm_value);
}

/*
 * EXAMPLE 4: Audio Tone Generation
 */
void audioToneGeneration() {
    Serial.println("--- Audio Tone Generation ---");
    Serial.println("Playing musical scale using PWM...");
    
    // Musical notes (frequencies in Hz)
    struct Note {
        const char* name;
        int frequency;
    };
    
    Note scale[] = {
        {"C4", 262},
        {"D4", 294}, 
        {"E4", 330},
        {"F4", 349},
        {"G4", 392},
        {"A4", 440},
        {"B4", 494},
        {"C5", 523}
    };
    
    for(int i = 0; i < 8; i++) {
        Serial.print("Playing note: ");
        Serial.print(scale[i].name);
        Serial.print(" (");
        Serial.print(scale[i].frequency);
        Serial.println(" Hz)");
        
        // Setup PWM frequency for this note
        ledcSetup(PWM_CHANNEL_3, scale[i].frequency, 8);
        ledcAttachPin(BUZZER_PIN, PWM_CHANNEL_3);
        
        // 50% duty cycle for square wave audio
        ledcWrite(PWM_CHANNEL_3, 128);
        delay(500);
        
        // Stop the tone
        ledcWrite(PWM_CHANNEL_3, 0);
        delay(100);
    }
    
    // Play a simple melody
    Serial.println("Playing simple melody...");
    playMelody();
    
    Serial.println("Audio generation complete!");
    Serial.println();
}

void playMelody() {
    // Simple melody: "Twinkle, Twinkle, Little Star" (first line)
    int melody[] = {262, 262, 392, 392, 440, 440, 392};  // C C G G A A G
    int durations[] = {500, 500, 500, 500, 500, 500, 1000}; // Note lengths in ms
    
    for(int i = 0; i < 7; i++) {
        ledcSetup(PWM_CHANNEL_3, melody[i], 8);
        ledcAttachPin(BUZZER_PIN, PWM_CHANNEL_3);
        ledcWrite(PWM_CHANNEL_3, 128);  // 50% duty cycle
        
        delay(durations[i]);
        
        ledcWrite(PWM_CHANNEL_3, 0);  // Stop note
        delay(50);  // Brief pause between notes
    }
}

/*
 * EXAMPLE 5: RGB LED Color Control
 */
void rgbLedControl() {
    Serial.println("--- RGB LED Color Control ---");
    Serial.println("Demonstrating color mixing with PWM:");
    
    // Primary colors
    Serial.println("Primary colors:");
    setRGBColor(255, 0, 0);    // Red
    Serial.println("Red");
    delay(1000);
    
    setRGBColor(0, 255, 0);    // Green
    Serial.println("Green");
    delay(1000);
    
    setRGBColor(0, 0, 255);    // Blue
    Serial.println("Blue");
    delay(1000);
    
    // Secondary colors
    Serial.println("Secondary colors:");
    setRGBColor(255, 255, 0);  // Yellow (Red + Green)
    Serial.println("Yellow (Red + Green)");
    delay(1000);
    
    setRGBColor(255, 0, 255);  // Magenta (Red + Blue)
    Serial.println("Magenta (Red + Blue)");
    delay(1000);
    
    setRGBColor(0, 255, 255);  // Cyan (Green + Blue)
    Serial.println("Cyan (Green + Blue)");
    delay(1000);
    
    setRGBColor(255, 255, 255); // White (All colors)
    Serial.println("White (All colors)");
    delay(1000);
    
    // Color fade demo
    Serial.println("Color fading demonstration:");
    colorFadeDemo();
    
    setRGBColor(0, 0, 0);      // Off
    Serial.println("RGB LED control complete!");
    Serial.println();
}

void setRGBColor(int red, int green, int blue) {
    ledcWrite(PWM_CHANNEL_4, red);
    ledcWrite(PWM_CHANNEL_5, green);
    ledcWrite(PWM_CHANNEL_6, blue);
}

void colorFadeDemo() {
    Serial.println("Fading through rainbow colors...");
    
    // Rainbow color cycle
    for(int hue = 0; hue < 360; hue += 5) {
        int red, green, blue;
        hsvToRgb(hue, 255, 255, &red, &green, &blue);
        
        setRGBColor(red, green, blue);
        delay(50);
    }
}

// Convert HSV (Hue, Saturation, Value) to RGB
void hsvToRgb(int h, int s, int v, int* r, int* g, int* b) {
    int c = (v * s) / 255;
    int x = c * (60 - abs((h % 120) - 60)) / 60;
    int m = v - c;
    
    if(h < 60) {
        *r = c; *g = x; *b = 0;
    } else if(h < 120) {
        *r = x; *g = c; *b = 0;
    } else if(h < 180) {
        *r = 0; *g = c; *b = x;
    } else if(h < 240) {
        *r = 0; *g = x; *b = c;
    } else if(h < 300) {
        *r = x; *g = 0; *b = c;
    } else {
        *r = c; *g = 0; *b = x;
    }
    
    *r += m; *g += m; *b += m;
}

/*
 * EXAMPLE 6: PWM Effects and Patterns
 */
void pwmEffectsDemo() {
    Serial.println("--- PWM Effects Demo ---");
    
    // Breathing effect
    Serial.println("Breathing effect on LED...");
    for(int cycle = 0; cycle < 3; cycle++) {
        // Breathe in
        for(int brightness = 0; brightness <= 255; brightness += 3) {
            ledcWrite(PWM_CHANNEL_0, brightness);
            delay(20);
        }
        // Breathe out
        for(int brightness = 255; brightness >= 0; brightness -= 3) {
            ledcWrite(PWM_CHANNEL_0, brightness);
            delay(20);
        }
    }
    
    // Pulsing motor effect
    Serial.println("Pulsing motor effect...");
    for(int pulse = 0; pulse < 5; pulse++) {
        ledcWrite(PWM_CHANNEL_1, 200);  // Fast
        delay(200);
        ledcWrite(PWM_CHANNEL_1, 100);  // Slow
        delay(200);
        ledcWrite(PWM_CHANNEL_1, 0);    // Stop
        delay(300);
    }
    
    // RGB rainbow cycle
    Serial.println("RGB rainbow cycle...");
    for(int i = 0; i < 360; i += 10) {
        int r, g, b;
        hsvToRgb(i, 255, 128, &r, &g, &b);  // Half brightness
        setRGBColor(r, g, b);
        delay(100);
    }
    
    // Turn everything off
    ledcWrite(PWM_CHANNEL_0, 0);  // LED off
    ledcWrite(PWM_CHANNEL_1, 0);  // Motor off
    setRGBColor(0, 0, 0);         // RGB off
    
    Serial.println("PWM effects demo complete!");
    Serial.println();
}

/*
 * UTILITY FUNCTIONS
 */

// Function to set PWM duty cycle as percentage (0-100%)
void setPWMPercentage(int channel, float percentage) {
    if(percentage < 0) percentage = 0;
    if(percentage > 100) percentage = 100;
    
    int pwm_value = (percentage * 255) / 100;
    ledcWrite(channel, pwm_value);
}

// Function to create custom PWM waveform
void customPWMPattern(int channel, int pattern[], int length, int delay_ms) {
    for(int i = 0; i < length; i++) {
        ledcWrite(channel, pattern[i]);
        delay(delay_ms);
    }
}

/*
 * HARDWARE SETUP INSTRUCTIONS:
 * 
 * 1. LED (GPIO 2): Built-in LED, no external connection needed
 * 
 * 2. External LED (GPIO 4):
 *    - Long leg → 220Ω resistor → GPIO 4
 *    - Short leg → GND
 * 
 * 3. DC Motor (GPIO 5):
 *    - Use motor driver (L298N, L9110, etc.) - NEVER direct connection!
 *    - Motor driver PWM input → GPIO 5
 *    - Motor driver power: separate power supply
 * 
 * 4. Servo (GPIO 18):
 *    - Red wire → 5V (external power supply)
 *    - Brown/Black wire → GND
 *    - Orange/White wire → GPIO 18
 * 
 * 5. Buzzer (GPIO 19):
 *    - Active buzzer: + → GPIO 19, - → GND
 *    - Passive buzzer: either way (will work with PWM tones)
 * 
 * 6. RGB LED (GPIO 21, 22, 23):
 *    - Common cathode RGB LED
 *    - Red → 220Ω resistor → GPIO 21
 *    - Green → 220Ω resistor → GPIO 22  
 *    - Blue → 220Ω resistor → GPIO 23
 *    - Common cathode → GND
 * 
 * WARNING: Always use appropriate current-limiting resistors!
 * Motors need separate power supplies and driver circuits!
 */

/*
 * What did we learn?
 * 
 * 1. PWM creates analog-like output using fast digital switching
 * 2. Duty cycle (0-100%) controls the effective output voltage
 * 3. ESP32 has 16 independent PWM channels with configurable frequency
 * 4. PWM controls LED brightness, motor speed, servo position
 * 5. Different applications need different PWM frequencies
 * 6. Audio generation uses PWM at audible frequencies (20Hz-20kHz)
 * 7. RGB LEDs use 3 PWM channels for millions of color combinations
 * 8. Proper hardware design (drivers, resistors) is essential for safety
 * 
 * Next Module: Communication Protocols - I2C, SPI, and advanced UART!
 */