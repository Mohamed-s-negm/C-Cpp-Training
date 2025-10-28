/*
 * Module 4.2: SPI Communication - SD Card and Display Control
 * 
 * What is SPI? Think of it like a dedicated phone line with multiple extensions:
 * - 4 wires: MOSI (Master Out), MISO (Master In), SCK (Clock), CS (Chip Select)
 * - Master controls everything (ESP32 is master)
 * - Each slave device has its own CS line (like different extensions)
 * - Faster than I2C but uses more pins
 * 
 * This example shows SD card reading and simple display control
 * Hardware needed: ESP32 + SD card module + SPI display (optional)
 */

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

// SPI pin definitions for ESP32
#define SCK_PIN   18  // Serial Clock (like a metronome)
#define MISO_PIN  19  // Master In, Slave Out (data coming to ESP32)
#define MOSI_PIN  23  // Master Out, Slave In (data going from ESP32)
#define CS_SD     5   // Chip Select for SD card
#define CS_DISPLAY 2  // Chip Select for display (if you have one)

// Simple variables to track our data
bool sdCardReady = false;
int fileCount = 0;

// Function to initialize SPI communication
// Think of this as setting up the phone system
void initializeSPI() {
    Serial.println("Setting up SPI communication...");
    
    // Start SPI with default settings
    SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN);  // ESP32 specific pins
    // For Arduino Uno, just use: SPI.begin();
    
    // Set SPI settings (like dialing rules)
    SPI.setFrequency(1000000);    // 1MHz - start slow for reliability
    SPI.setDataMode(SPI_MODE0);   // Clock polarity and phase
    SPI.setBitOrder(MSBFIRST);    // Most significant bit first
    
    Serial.println("SPI ready!");
}

// Function to initialize SD card
// Think of this as connecting to a filing cabinet
bool initializeSDCard() {
    Serial.print("Initializing SD card... ");
    
    // Try to begin SD card communication
    if (!SD.begin(CS_SD)) {
        Serial.println("FAILED!");
        Serial.println("Check SD card and wiring");
        return false;
    }
    
    Serial.println("SUCCESS!");
    
    // Get SD card information (like checking the filing cabinet size)
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);  // Convert to MB
    Serial.print("SD Card Size: ");
    Serial.print(cardSize);
    Serial.println(" MB");
    
    return true;
}

// Function to list files on SD card
// Like looking through the filing cabinet
void listSDCardFiles() {
    Serial.println("\nFiles on SD card:");
    Serial.println("=================");
    
    File root = SD.open("/");  // Open root directory
    fileCount = 0;
    
    while (true) {
        File entry = root.openNextFile();
        if (!entry) {
            break;  // No more files
        }
        
        fileCount++;
        
        // Print file information
        Serial.print(fileCount);
        Serial.print(". ");
        Serial.print(entry.name());
        
        if (entry.isDirectory()) {
            Serial.println(" (Folder)");
        } else {
            Serial.print(" - ");
            Serial.print(entry.size());
            Serial.println(" bytes");
        }
        
        entry.close();
    }
    
    root.close();
    
    if (fileCount == 0) {
        Serial.println("No files found");
    } else {
        Serial.print("Total files: ");
        Serial.println(fileCount);
    }
}

// Function to create a simple test file
// Like putting a document in the filing cabinet
void createTestFile() {
    Serial.println("\nCreating test file...");
    
    File testFile = SD.open("/test.txt", FILE_WRITE);
    
    if (testFile) {
        // Write some simple data
        testFile.println("ESP32 SPI Test File");
        testFile.println("==================");
        testFile.print("Created at: ");
        testFile.println(millis());  // Time since ESP32 started
        testFile.println("This file was created using SPI communication!");
        
        testFile.close();
        Serial.println("Test file created successfully!");
    } else {
        Serial.println("Error creating test file");
    }
}

// Function to read a file from SD card
// Like taking a document out of the filing cabinet to read
void readFile(const char* filename) {
    Serial.print("\nReading file: ");
    Serial.println(filename);
    Serial.println("====================");
    
    File file = SD.open(filename);
    
    if (file) {
        // Read and print each line
        while (file.available()) {
            String line = file.readStringUntil('\n');  // Read until newline
            Serial.println(line);
        }
        
        file.close();
    } else {
        Serial.println("Error opening file");
    }
}

// Simple function to send data to SPI display (if you have one)
// Think of this as writing on a whiteboard
void sendToDisplay(uint8_t data) {
    // Select the display (pick up the right phone)
    digitalWrite(CS_DISPLAY, LOW);
    
    // Send the data
    SPI.transfer(data);
    
    // Deselect the display (hang up the phone)
    digitalWrite(CS_DISPLAY, HIGH);
}

// Function to write sensor data to SD card
// Like keeping a logbook of measurements
void logSensorData() {
    // Simulate some sensor readings
    float temperature = 23.5 + (random(-50, 50) / 10.0);  // Random temp around 23.5°C
    int humidity = 45 + random(-10, 10);                   // Random humidity around 45%
    int lightLevel = random(0, 1024);                      // Random light level
    
    Serial.println("\nLogging sensor data...");
    
    File dataFile = SD.open("/sensors.csv", FILE_APPEND);  // Append to file
    
    if (dataFile) {
        // Write CSV format data (easy to open in Excel)
        dataFile.print(millis());        // Time
        dataFile.print(",");
        dataFile.print(temperature);     // Temperature
        dataFile.print(",");
        dataFile.print(humidity);        // Humidity
        dataFile.print(",");
        dataFile.println(lightLevel);    // Light level
        
        dataFile.close();
        
        Serial.print("Logged: T=");
        Serial.print(temperature);
        Serial.print("°C, H=");
        Serial.print(humidity);
        Serial.print("%, Light=");
        Serial.println(lightLevel);
    } else {
        Serial.println("Error opening data file");
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("SPI Communication Example");
    Serial.println("=========================");
    
    // Set up chip select pins as outputs
    pinMode(CS_SD, OUTPUT);
    pinMode(CS_DISPLAY, OUTPUT);
    
    // Make sure devices are not selected initially
    digitalWrite(CS_SD, HIGH);      // SD card not selected
    digitalWrite(CS_DISPLAY, HIGH); // Display not selected
    
    // Initialize SPI
    initializeSPI();
    
    // Try to initialize SD card
    sdCardReady = initializeSDCard();
    
    if (sdCardReady) {
        // List existing files
        listSDCardFiles();
        
        // Create a test file
        createTestFile();
        
        // Read the test file back
        readFile("/test.txt");
        
        // Create CSV header for sensor data
        File dataFile = SD.open("/sensors.csv", FILE_WRITE);
        if (dataFile && dataFile.size() == 0) {  // If file is empty
            dataFile.println("Time,Temperature,Humidity,Light");  // CSV header
            dataFile.close();
        }
    }
}

void loop() {
    if (sdCardReady) {
        Serial.println("\n--- SPI Operations ---");
        
        // Log some sensor data every 10 seconds
        logSensorData();
        
        // Show updated file list every 30 seconds
        static unsigned long lastListTime = 0;
        if (millis() - lastListTime > 30000) {
            listSDCardFiles();
            lastListTime = millis();
        }
        
        // If you have an SPI display, send some data to it
        static uint8_t displayData = 0;
        sendToDisplay(displayData++);
        
        delay(10000);  // Wait 10 seconds
    } else {
        Serial.println("SD card not ready - check wiring!");
        delay(5000);
        
        // Try to reinitialize
        sdCardReady = initializeSDCard();
    }
}

/*
 * Hardware Setup for ESP32:
 * 
 * ESP32 Pin | SPI Function | SD Card Pin | Display Pin
 * ----------|-------------|-------------|------------
 * GPIO 18   | SCK (Clock) | CLK         | SCK
 * GPIO 19   | MISO        | DO          | (not used)
 * GPIO 23   | MOSI        | DI          | SDA/MOSI
 * GPIO 5    | CS (SD)     | CS          | (not connected)
 * GPIO 2    | CS (Display)| (not conn.) | CS
 * 3.3V      | Power       | VCC         | VCC
 * GND       | Ground      | GND         | GND
 * 
 * SD Card Module Wiring:
 * - Many SD modules work with 3.3V (perfect for ESP32)
 * - Some need 5V - check your module specifications
 * - SD card must be formatted as FAT32
 * 
 * SPI Display (like ST7735, ILI9341):
 * - Most small color displays use SPI
 * - Need additional pins for DC (data/command) and RST (reset)
 * - Check display library for exact pin requirements
 * 
 * Important Notes:
 * 1. SPI is faster than I2C (up to several MHz)
 * 2. Each device needs its own CS (Chip Select) pin
 * 3. All devices share SCK, MOSI, and MISO lines
 * 4. Always set CS HIGH when not using a device
 * 5. SD cards can be picky about timing - start with low speeds
 * 
 * Troubleshooting:
 * - SD card not detected? Check power (3.3V vs 5V)
 * - File operations fail? Format SD card as FAT32
 * - SPI errors? Try lower frequency (100kHz for testing)
 * - Multiple devices interfering? Check CS pin control
 * - Data corruption? Add delays between operations
 * 
 * File System Tips:
 * - Use short filenames (8.3 format: FILENAME.TXT)
 * - Always close files after use
 * - Check if file opened successfully before writing
 * - Use FILE_APPEND to add data to existing files
 * - CSV format is great for data logging
 */