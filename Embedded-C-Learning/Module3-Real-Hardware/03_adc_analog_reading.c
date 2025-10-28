/*
 * MODULE 3 - LESSON 3: ADC - Reading Analog Sensors
 * 
 * What you'll learn:
 * - How to read analog voltages (0V to 3.3V)
 * - Converting ADC values to real-world measurements
 * - Reading temperature, light, potentiometer, battery voltage
 * - ADC resolution, reference voltage, and calibration
 * 
 * ADC = Analog-to-Digital Converter
 * Think of it like a voltage meter that gives you numbers:
 * - Real world: 0V to 3.3V (continuous analog)
 * - ESP32 ADC: 0 to 4095 (discrete digital numbers)
 * - Your job: Convert numbers back to meaningful values
 * 
 * NOTE: This code runs on ESP32 with Arduino IDE
 */

// ESP32 ADC pins (only certain pins can read analog)
#define TEMP_SENSOR_PIN     A0   // GPIO 36 - Temperature sensor
#define LIGHT_SENSOR_PIN    A3   // GPIO 39 - Light sensor (LDR)
#define POTENTIOMETER_PIN   A6   // GPIO 34 - Potentiometer
#define BATTERY_VOLTAGE_PIN A7   // GPIO 35 - Battery voltage divider

// ADC characteristics
#define ADC_RESOLUTION      4096    // 12-bit ADC = 0 to 4095
#define ADC_REFERENCE_MV    3300    // 3.3V reference = 3300mV
#define ADC_MAX_VALUE       4095    // Maximum ADC reading

// Sensor calibration constants
#define TEMP_SENSOR_MV_PER_C    10  // TMP36: 10mV per °C
#define TEMP_SENSOR_OFFSET_C    50  // TMP36: 500mV at 0°C
#define VOLTAGE_DIVIDER_RATIO   2.0 // For battery voltage measurement

void setup() {
    Serial.begin(115200);
    while(!Serial) delay(10);
    
    Serial.println("=== ESP32 ADC - Analog Sensor Reading ===");
    Serial.println();
    
    // Configure ADC resolution (ESP32 supports 9-12 bits)
    analogReadResolution(12);  // Use full 12-bit resolution
    
    // Set ADC attenuation (affects input voltage range)
    // ADC_0db: 0-1.1V, ADC_2_5db: 0-1.5V, ADC_6db: 0-2.2V, ADC_11db: 0-3.3V
    analogSetPinAttenuation(TEMP_SENSOR_PIN, ADC_11db);     // 0-3.3V range
    analogSetPinAttenuation(LIGHT_SENSOR_PIN, ADC_11db);    // 0-3.3V range  
    analogSetPinAttenuation(POTENTIOMETER_PIN, ADC_11db);   // 0-3.3V range
    analogSetPinAttenuation(BATTERY_VOLTAGE_PIN, ADC_11db); // 0-3.3V range
    
    Serial.println("ADC Configuration:");
    Serial.println("- Resolution: 12 bits (0-4095)");
    Serial.println("- Reference: 3.3V");
    Serial.println("- Attenuation: 11dB (0-3.3V input range)");
    Serial.println();
    
    Serial.println("Sensor Connections:");
    Serial.println("- Temperature sensor (TMP36) on GPIO 36");
    Serial.println("- Light sensor (LDR + resistor) on GPIO 39");
    Serial.println("- Potentiometer on GPIO 34");
    Serial.println("- Battery voltage divider on GPIO 35");
    Serial.println();
    
    delay(1000);
}

void loop() {
    Serial.println("=== Sensor Reading Cycle ===");
    
    // Read all sensors
    readTemperatureSensor();
    delay(100);
    
    readLightSensor(); 
    delay(100);
    
    readPotentiometer();
    delay(100);
    
    readBatteryVoltage();
    delay(100);
    
    // Advanced examples
    demonstrateAveraging();
    delay(100);
    
    demonstrateCalibration();
    
    Serial.println();
    Serial.println("Waiting 5 seconds before next reading...");
    delay(5000);
}

/*
 * EXAMPLE 1: Temperature Sensor (TMP36)
 */
void readTemperatureSensor() {
    Serial.println("--- Temperature Sensor (TMP36) ---");
    
    // Read raw ADC value
    int raw_adc = analogRead(TEMP_SENSOR_PIN);
    Serial.print("Raw ADC: ");
    Serial.println(raw_adc);
    
    // Convert to voltage (mV)
    float voltage_mv = (raw_adc * ADC_REFERENCE_MV) / ADC_MAX_VALUE;
    Serial.print("Voltage: ");
    Serial.print(voltage_mv);
    Serial.println(" mV");
    
    // Convert to temperature (TMP36 formula)
    // TMP36: 500mV at 0°C, 10mV per °C
    float temperature_c = (voltage_mv - 500.0) / 10.0;
    Serial.print("Temperature: ");
    Serial.print(temperature_c);
    Serial.println(" °C");
    
    // Convert to Fahrenheit
    float temperature_f = (temperature_c * 9.0 / 5.0) + 32.0;
    Serial.print("Temperature: ");
    Serial.print(temperature_f);
    Serial.println(" °F");
    
    // Sanity check
    if(temperature_c < -40 || temperature_c > 125) {
        Serial.println("WARNING: Temperature reading out of sensor range!");
    }
    
    Serial.println();
}

/*
 * EXAMPLE 2: Light Sensor (LDR - Light Dependent Resistor)
 */
void readLightSensor() {
    Serial.println("--- Light Sensor (LDR) ---");
    
    // Read raw ADC value
    int raw_adc = analogRead(LIGHT_SENSOR_PIN);
    Serial.print("Raw ADC: ");
    Serial.println(raw_adc);
    
    // Convert to voltage
    float voltage_v = (raw_adc * 3.3) / ADC_MAX_VALUE;
    Serial.print("Voltage: ");
    Serial.print(voltage_v);
    Serial.println(" V");
    
    // Convert to light percentage (0% = dark, 100% = bright)
    // This assumes LDR is in voltage divider with pull-down resistor
    float light_percentage = (raw_adc * 100.0) / ADC_MAX_VALUE;
    Serial.print("Light level: ");
    Serial.print(light_percentage);
    Serial.println(" %");
    
    // Classify light levels
    if(light_percentage > 80) {
        Serial.println("Condition: Very bright");
    } else if(light_percentage > 60) {
        Serial.println("Condition: Bright");
    } else if(light_percentage > 40) {
        Serial.println("Condition: Medium");
    } else if(light_percentage > 20) {
        Serial.println("Condition: Dim");
    } else {
        Serial.println("Condition: Dark");
    }
    
    Serial.println();
}

/*
 * EXAMPLE 3: Potentiometer (Variable Resistor)
 */
void readPotentiometer() {
    Serial.println("--- Potentiometer ---");
    
    // Read raw ADC value
    int raw_adc = analogRead(POTENTIOMETER_PIN);
    Serial.print("Raw ADC: ");
    Serial.println(raw_adc);
    
    // Convert to voltage
    float voltage_v = (raw_adc * 3.3) / ADC_MAX_VALUE;
    Serial.print("Voltage: ");
    Serial.print(voltage_v);
    Serial.println(" V");
    
    // Convert to percentage (0% = minimum, 100% = maximum)
    float position_percentage = (raw_adc * 100.0) / ADC_MAX_VALUE;
    Serial.print("Position: ");
    Serial.print(position_percentage);
    Serial.println(" %");
    
    // Map to custom range (example: 0-255 for PWM)
    int pwm_value = map(raw_adc, 0, ADC_MAX_VALUE, 0, 255);
    Serial.print("PWM equivalent: ");
    Serial.println(pwm_value);
    
    // Map to angle (example: 0-180 degrees for servo)
    int servo_angle = map(raw_adc, 0, ADC_MAX_VALUE, 0, 180);
    Serial.print("Servo angle: ");
    Serial.print(servo_angle);
    Serial.println("°");
    
    Serial.println();
}

/*
 * EXAMPLE 4: Battery Voltage Monitoring
 */
void readBatteryVoltage() {
    Serial.println("--- Battery Voltage Monitor ---");
    
    // Read raw ADC value
    int raw_adc = analogRead(BATTERY_VOLTAGE_PIN);
    Serial.print("Raw ADC: ");
    Serial.println(raw_adc);
    
    // Convert to voltage at ADC pin
    float adc_voltage = (raw_adc * 3.3) / ADC_MAX_VALUE;
    Serial.print("ADC pin voltage: ");
    Serial.print(adc_voltage);
    Serial.println(" V");
    
    // Calculate actual battery voltage (accounting for voltage divider)
    // If using 2:1 voltage divider (two equal resistors)
    float battery_voltage = adc_voltage * VOLTAGE_DIVIDER_RATIO;
    Serial.print("Battery voltage: ");
    Serial.print(battery_voltage);
    Serial.println(" V");
    
    // Calculate battery percentage (example for 3.7V Li-ion)
    float battery_percentage = ((battery_voltage - 3.0) / (4.2 - 3.0)) * 100.0;
    if(battery_percentage > 100) battery_percentage = 100;
    if(battery_percentage < 0) battery_percentage = 0;
    
    Serial.print("Battery level: ");
    Serial.print(battery_percentage);
    Serial.println(" %");
    
    // Battery status
    if(battery_percentage > 75) {
        Serial.println("Status: Battery Good");
    } else if(battery_percentage > 25) {
        Serial.println("Status: Battery Medium");
    } else if(battery_percentage > 10) {
        Serial.println("Status: Battery Low");
    } else {
        Serial.println("Status: Battery Critical!");
    }
    
    Serial.println();
}

/*
 * EXAMPLE 5: Noise Reduction Through Averaging
 */
void demonstrateAveraging() {
    Serial.println("--- Noise Reduction Example ---");
    
    const int num_samples = 10;
    int total = 0;
    
    Serial.print("Taking ");
    Serial.print(num_samples);
    Serial.println(" samples of temperature sensor:");
    
    // Take multiple readings
    for(int i = 0; i < num_samples; i++) {
        int reading = analogRead(TEMP_SENSOR_PIN);
        total += reading;
        
        Serial.print("Sample ");
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.println(reading);
        
        delay(50);  // Small delay between samples
    }
    
    // Calculate average
    float average = total / (float)num_samples;
    Serial.print("Average: ");
    Serial.println(average);
    
    // Convert averaged reading to temperature
    float voltage_mv = (average * ADC_REFERENCE_MV) / ADC_MAX_VALUE;
    float temperature_c = (voltage_mv - 500.0) / 10.0;
    
    Serial.print("Averaged temperature: ");
    Serial.print(temperature_c);
    Serial.println(" °C");
    
    Serial.println("Note: Averaging reduces noise and gives more stable readings.");
    Serial.println();
}

/*
 * EXAMPLE 6: Sensor Calibration
 */
void demonstrateCalibration() {
    Serial.println("--- Sensor Calibration Example ---");
    
    // Read raw potentiometer value
    int raw_reading = analogRead(POTENTIOMETER_PIN);
    
    Serial.print("Raw potentiometer reading: ");
    Serial.println(raw_reading);
    
    // Method 1: Simple linear mapping
    float simple_percentage = (raw_reading * 100.0) / ADC_MAX_VALUE;
    Serial.print("Simple mapping: ");
    Serial.print(simple_percentage);
    Serial.println(" %");
    
    // Method 2: Calibrated mapping (account for real min/max values)
    // In reality, potentiometer might not reach exact 0 and 4095
    const int calibrated_min = 50;    // Actual minimum reading
    const int calibrated_max = 4000;  // Actual maximum reading
    
    float calibrated_percentage = ((raw_reading - calibrated_min) * 100.0) / 
                                 (calibrated_max - calibrated_min);
    
    // Constrain to 0-100%
    if(calibrated_percentage < 0) calibrated_percentage = 0;
    if(calibrated_percentage > 100) calibrated_percentage = 100;
    
    Serial.print("Calibrated mapping: ");
    Serial.print(calibrated_percentage);
    Serial.println(" %");
    
    Serial.println("Note: Calibration accounts for real-world sensor limitations.");
    Serial.println();
}

/*
 * UTILITY FUNCTIONS
 */

// Function to convert ADC reading to voltage
float adcToVoltage(int adc_reading) {
    return (adc_reading * 3.3) / ADC_MAX_VALUE;
}

// Function to take multiple samples and return average
float readADCAverage(uint8_t pin, uint8_t samples) {
    long total = 0;
    
    for(uint8_t i = 0; i < samples; i++) {
        total += analogRead(pin);
        delay(10);
    }
    
    return total / (float)samples;
}

// Function to map value with custom constraints
float mapFloat(float value, float in_min, float in_max, float out_min, float out_max) {
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/*
 * ADC TROUBLESHOOTING GUIDE
 */
void printADCTroubleshootingGuide() {
    Serial.println("=== ADC Troubleshooting Guide ===");
    Serial.println();
    
    Serial.println("Problem: Readings are noisy/unstable");
    Serial.println("Solutions:");
    Serial.println("1. Take multiple samples and average them");
    Serial.println("2. Add capacitor across sensor (100nF - 1µF)");
    Serial.println("3. Use shorter wires to sensor");
    Serial.println("4. Keep analog wires away from digital switching signals");
    Serial.println();
    
    Serial.println("Problem: Readings don't reach full range");
    Serial.println("Solutions:");
    Serial.println("1. Check sensor power supply voltage");
    Serial.println("2. Verify ADC attenuation setting");
    Serial.println("3. Calibrate min/max values in software");
    Serial.println("4. Check for voltage dividers in circuit");
    Serial.println();
    
    Serial.println("Problem: Readings are completely wrong");
    Serial.println("Solutions:");
    Serial.println("1. Verify pin connections and pin numbers");
    Serial.println("2. Check sensor datasheet for correct formula");
    Serial.println("3. Measure actual voltages with multimeter");
    Serial.println("4. Verify ADC reference voltage setting");
    Serial.println();
}

/*
 * HARDWARE SETUP INSTRUCTIONS:
 * 
 * 1. Temperature Sensor (TMP36) on GPIO 36:
 *    - Pin 1 (left) → 3.3V
 *    - Pin 2 (middle) → GPIO 36
 *    - Pin 3 (right) → GND
 * 
 * 2. Light Sensor (LDR) on GPIO 39:
 *    - LDR one side → 3.3V
 *    - LDR other side → GPIO 39 AND 10kΩ resistor to GND
 *    - Creates voltage divider: bright = high voltage, dark = low voltage
 * 
 * 3. Potentiometer on GPIO 34:
 *    - Left pin → GND
 *    - Right pin → 3.3V
 *    - Middle pin (wiper) → GPIO 34
 * 
 * 4. Battery Voltage Monitor on GPIO 35:
 *    - Battery + → 10kΩ resistor → GPIO 35
 *    - GPIO 35 → 10kΩ resistor → GND
 *    - Creates 2:1 voltage divider for batteries up to 6.6V
 * 
 * WARNING: Never exceed 3.3V on any ESP32 pin!
 * Use voltage dividers for higher voltages.
 */

/*
 * What did we learn?
 * 
 * 1. ADC converts analog voltages (0-3.3V) to digital numbers (0-4095)
 * 2. Different sensors require different conversion formulas
 * 3. Voltage dividers let you measure higher voltages safely
 * 4. Averaging multiple readings reduces noise
 * 5. Calibration improves accuracy for real-world sensors
 * 6. ESP32 ADC has configurable resolution and attenuation
 * 7. Proper wiring and grounding are crucial for clean readings
 * 8. Always verify readings with known test conditions
 * 
 * Next: PWM - Controlling motors, LEDs, servos with analog-like output!
 */