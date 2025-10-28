/*
 * Module 4.1: I2C Communication - Simple Sensor Reading
 * 
 * What is I2C? Think of it like a telephone party line:
 * - Only 2 wires needed: SDA (data) and SCL (clock)
 * - Multiple devices can share the same wires
 * - Each device has a unique address (like a phone number)
 * - One device (master) controls the conversation
 * 
 * This example shows how to read from common sensors using I2C
 * Hardware needed: ESP32 + I2C sensor (like BMP280, BME280, or MPU6050)
 */

#include <Arduino.h>
#include <Wire.h>  // I2C library for ESP32

// Common I2C sensor addresses (these are like phone numbers)
#define BMP280_ADDRESS    0x76  // Pressure/temperature sensor
#define BME280_ADDRESS    0x76  // Humidity/pressure/temp sensor  
#define MPU6050_ADDRESS   0x68  // Accelerometer/gyroscope
#define RTC_DS3231_ADDRESS 0x68 // Real-time clock

// I2C pins for ESP32 (you can change these)
#define SDA_PIN 21  // Data line (think: Serial Data line)
#define SCL_PIN 22  // Clock line (think: Serial Clock line)

// Simple function to scan for I2C devices
// This is like checking which phone numbers are active
void scanI2CDevices() {
    Serial.println("Scanning for I2C devices...");
    
    int devicesFound = 0;
    
    // Check all possible addresses (0 to 127)
    for (int address = 1; address < 127; address++) {
        Wire.beginTransmission(address);  // Try to call this address
        int error = Wire.endTransmission(); // See if anyone answers
        
        if (error == 0) {  // Device responded!
            Serial.print("Device found at address 0x");
            if (address < 16) Serial.print("0");  // Add leading zero
            Serial.print(address, HEX);
            Serial.print(" (");
            Serial.print(address);
            Serial.println(")");
            devicesFound++;
        }
    }
    
    if (devicesFound == 0) {
        Serial.println("No I2C devices found. Check wiring!");
    } else {
        Serial.print("Found ");
        Serial.print(devicesFound);
        Serial.println(" device(s)");
    }
}

// Simple function to read a byte from any I2C device
// Like asking a specific question to a specific person
uint8_t readByteFromDevice(uint8_t deviceAddress, uint8_t registerAddress) {
    Wire.beginTransmission(deviceAddress);  // Call the device
    Wire.write(registerAddress);            // Ask about specific register
    Wire.endTransmission(false);            // Don't hang up yet
    
    Wire.requestFrom(deviceAddress, (uint8_t)1);  // Ask for 1 byte answer
    
    if (Wire.available()) {
        return Wire.read();  // Get the answer
    }
    
    return 0;  // No answer received
}

// Example: Reading WHO_AM_I register (device identification)
// This is like asking "Who are you?" to make sure we're talking to the right device
void checkDeviceIdentity(uint8_t deviceAddress) {
    Serial.print("Checking device at address 0x");
    Serial.print(deviceAddress, HEX);
    Serial.print("... ");
    
    // Most sensors have a WHO_AM_I register at address 0x75
    uint8_t whoAmI = readByteFromDevice(deviceAddress, 0x75);
    
    Serial.print("WHO_AM_I register: 0x");
    Serial.println(whoAmI, HEX);
    
    // Common WHO_AM_I values:
    if (whoAmI == 0x58) {
        Serial.println("This might be a BMP280!");
    } else if (whoAmI == 0x60) {
        Serial.println("This might be a BME280!");
    } else if (whoAmI == 0x68) {
        Serial.println("This might be an MPU6050!");
    } else {
        Serial.println("Unknown device or different register address");
    }
}

// Simple temperature reading example (works with many sensors)
float readSimpleTemperature(uint8_t deviceAddress) {
    // This is a simplified example - real sensors need calibration
    // Read raw temperature data (usually 2 bytes)
    Wire.beginTransmission(deviceAddress);
    Wire.write(0x22);  // Temperature register (example address)
    Wire.endTransmission(false);
    
    Wire.requestFrom(deviceAddress, (uint8_t)2);  // Request 2 bytes
    
    if (Wire.available() >= 2) {
        uint16_t rawTemp = Wire.read() << 8;  // High byte first
        rawTemp |= Wire.read();               // Add low byte
        
        // Convert to temperature (this formula varies by sensor)
        float temperature = (float)rawTemp / 100.0;  // Example conversion
        return temperature;
    }
    
    return -999.0;  // Error value
}

void setup() {
    Serial.begin(115200);
    Serial.println("I2C Communication Example");
    Serial.println("========================");
    
    // Initialize I2C communication
    // Think of this as setting up the telephone system
    Wire.begin(SDA_PIN, SCL_PIN);  // ESP32 specific
    // For Arduino Uno, just use: Wire.begin();
    
    Wire.setClock(100000);  // Set speed to 100kHz (standard speed)
    // Higher speeds: 400kHz (fast), 1MHz (fast+)
    
    delay(1000);  // Wait for devices to wake up
    
    // Scan for devices
    scanI2CDevices();
    
    Serial.println("\nChecking common sensor addresses:");
    checkDeviceIdentity(BMP280_ADDRESS);
    checkDeviceIdentity(MPU6050_ADDRESS);
}

void loop() {
    Serial.println("\n--- I2C Reading Example ---");
    
    // Try to read temperature from different possible sensors
    float temp1 = readSimpleTemperature(BMP280_ADDRESS);
    float temp2 = readSimpleTemperature(MPU6050_ADDRESS);
    
    Serial.print("Temperature from 0x76: ");
    if (temp1 != -999.0) {
        Serial.print(temp1);
        Serial.println("°C");
    } else {
        Serial.println("No data");
    }
    
    Serial.print("Temperature from 0x68: ");
    if (temp2 != -999.0) {
        Serial.print(temp2);
        Serial.println("°C");
    } else {
        Serial.println("No data");
    }
    
    // Scan for devices periodically
    scanI2CDevices();
    
    delay(5000);  // Wait 5 seconds before next reading
}

/*
 * Hardware Setup for ESP32:
 * 
 * ESP32 Pin | I2C Function | Sensor Pin
 * ----------|-------------|------------
 * GPIO 21   | SDA (Data)  | SDA
 * GPIO 22   | SCL (Clock) | SCL  
 * 3.3V      | Power       | VCC
 * GND       | Ground      | GND
 * 
 * Important Notes:
 * 1. Most I2C sensors work with 3.3V (ESP32's voltage)
 * 2. Some sensors need pull-up resistors (4.7kΩ) on SDA and SCL
 * 3. Many breakout boards have built-in pull-ups
 * 4. Connect multiple sensors to the same SDA/SCL lines
 * 
 * Troubleshooting:
 * - No devices found? Check wiring and power
 * - Wrong readings? Check sensor datasheet for correct registers
 * - Communication errors? Try lower clock speed (10kHz)
 * - Multiple devices? Make sure each has unique address
 * 
 * Common I2C Sensors to Try:
 * - BMP280/BME280: Temperature, pressure, humidity
 * - MPU6050: Accelerometer and gyroscope  
 * - DS3231: Real-time clock
 * - OLED displays: SSD1306 (0x3C or 0x3D)
 * - EEPROM: 24C32 (0x50-0x57)
 */