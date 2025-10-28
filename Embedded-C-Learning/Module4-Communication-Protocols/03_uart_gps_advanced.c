/*
 * Module 4.3: Advanced UART - GPS and Sensor Data Processing
 * 
 * What is UART? Think of it like sending letters back and forth:
 * - Only 2 wires: TX (transmit) and RX (receive)
 * - Serial communication (one bit at a time)
 * - Both devices must agree on speed (baud rate)
 * - Great for GPS modules, sensors, and device-to-device communication
 * 
 * This example shows advanced UART techniques for GPS and sensor data
 * Hardware needed: ESP32 + GPS module (like NEO-6M) or sensor with UART
 */

#include <Arduino.h>
#include <HardwareSerial.h>

// ESP32 has 3 hardware serial ports (UART0, UART1, UART2)
// UART0 is used for USB communication (Serial)
// We'll use UART1 for GPS and UART2 for sensor
HardwareSerial gpsSerial(1);    // GPS on UART1
HardwareSerial sensorSerial(2); // Sensor on UART2

// Pin definitions for ESP32
#define GPS_RX_PIN      16    // ESP32 receives GPS data here
#define GPS_TX_PIN      17    // ESP32 sends commands to GPS here
#define SENSOR_RX_PIN   25    // ESP32 receives sensor data here  
#define SENSOR_TX_PIN   26    // ESP32 sends commands to sensor here

// GPS data structure
// Think of this as a form to fill out with GPS information
struct GPSData {
    bool valid;           // Is the GPS data good?
    float latitude;       // Where are we? (North/South)
    float longitude;      // Where are we? (East/West)  
    float altitude;       // How high are we?
    int satellites;       // How many satellites can we see?
    float speed;          // How fast are we moving?
    char timeString[20];  // What time is it?
};

GPSData currentGPS = {false, 0, 0, 0, 0, 0, "00:00:00"};

// Buffer for incoming data (like a mailbox for messages)
String gpsBuffer = "";
String sensorBuffer = "";

// Function to initialize UART communication
// Think of this as setting up the mail system
void initializeUART() {
    Serial.println("Setting up UART communication...");
    
    // Start GPS communication (9600 baud is standard for most GPS modules)
    gpsSerial.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
    Serial.println("GPS UART started at 9600 baud");
    
    // Start sensor communication (you can use different speeds)
    sensorSerial.begin(115200, SERIAL_8N1, SENSOR_RX_PIN, SENSOR_TX_PIN);
    Serial.println("Sensor UART started at 115200 baud");
    
    delay(1000);  // Wait for everything to settle
    
    // Send initialization commands to GPS
    gpsSerial.println("$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28");  // Set output
    delay(100);
    gpsSerial.println("$PMTK220,1000*1F");  // Set update rate to 1Hz
    
    Serial.println("UART initialization complete!");
}

// Function to parse GPS NMEA sentences
// NMEA is like a standard format for GPS messages
// Think of it as a template that all GPS devices use
bool parseGPSData(String nmeaSentence) {
    // We're looking for GPGGA sentences (Global Positioning System Fix Data)
    if (!nmeaSentence.startsWith("$GPGGA") && !nmeaSentence.startsWith("$GNGGA")) {
        return false;  // Not the sentence we want
    }
    
    // Split the sentence by commas (it's like a CSV line)
    int commaIndex[15];  // GPGGA has about 15 fields
    int commaCount = 0;
    
    // Find all the commas
    for (int i = 0; i < nmeaSentence.length() && commaCount < 15; i++) {
        if (nmeaSentence.charAt(i) == ',') {
            commaIndex[commaCount] = i;
            commaCount++;
        }
    }
    
    if (commaCount < 10) return false;  // Not enough data
    
    // Extract time (field 1)
    String timeStr = nmeaSentence.substring(commaIndex[0] + 1, commaIndex[1]);
    if (timeStr.length() >= 6) {
        // Convert HHMMSS to HH:MM:SS
        snprintf(currentGPS.timeString, sizeof(currentGPS.timeString), 
                "%c%c:%c%c:%c%c", 
                timeStr[0], timeStr[1], timeStr[2], timeStr[3], timeStr[4], timeStr[5]);
    }
    
    // Extract latitude (fields 2 and 3)
    String latStr = nmeaSentence.substring(commaIndex[1] + 1, commaIndex[2]);
    String latDir = nmeaSentence.substring(commaIndex[2] + 1, commaIndex[3]);
    
    if (latStr.length() > 0) {
        float lat = latStr.toFloat();
        // Convert from DDMM.MMMMM to DD.DDDDDD
        int degrees = (int)(lat / 100);
        float minutes = lat - (degrees * 100);
        currentGPS.latitude = degrees + (minutes / 60.0);
        if (latDir == "S") currentGPS.latitude = -currentGPS.latitude;
    }
    
    // Extract longitude (fields 4 and 5)
    String lonStr = nmeaSentence.substring(commaIndex[3] + 1, commaIndex[4]);
    String lonDir = nmeaSentence.substring(commaIndex[4] + 1, commaIndex[5]);
    
    if (lonStr.length() > 0) {
        float lon = lonStr.toFloat();
        // Convert from DDDMM.MMMMM to DDD.DDDDDD
        int degrees = (int)(lon / 100);
        float minutes = lon - (degrees * 100);
        currentGPS.longitude = degrees + (minutes / 60.0);
        if (lonDir == "W") currentGPS.longitude = -currentGPS.longitude;
    }
    
    // Extract fix quality and satellites (fields 6 and 7)
    String fixStr = nmeaSentence.substring(commaIndex[5] + 1, commaIndex[6]);
    String satStr = nmeaSentence.substring(commaIndex[6] + 1, commaIndex[7]);
    
    int fixQuality = fixStr.toInt();
    currentGPS.valid = (fixQuality > 0);  // 0 = no fix, 1+ = valid fix
    currentGPS.satellites = satStr.toInt();
    
    // Extract altitude (field 9)
    String altStr = nmeaSentence.substring(commaIndex[8] + 1, commaIndex[9]);
    if (altStr.length() > 0) {
        currentGPS.altitude = altStr.toFloat();
    }
    
    return currentGPS.valid;
}

// Function to read and process GPS data
// Like checking the mailbox for GPS messages
void processGPSData() {
    while (gpsSerial.available()) {
        char c = gpsSerial.read();
        
        if (c == '\n') {  // End of sentence
            if (gpsBuffer.length() > 0) {
                // Try to parse this sentence
                if (parseGPSData(gpsBuffer)) {
                    Serial.println("GPS data updated!");
                }
                gpsBuffer = "";  // Clear for next sentence
            }
        } else if (c != '\r') {  // Ignore carriage returns
            gpsBuffer += c;
        }
        
        // Prevent buffer overflow
        if (gpsBuffer.length() > 200) {
            gpsBuffer = "";
        }
    }
}

// Function to display GPS information in a friendly way
void displayGPSInfo() {
    Serial.println("\n=== GPS Information ===");
    
    if (currentGPS.valid) {
        Serial.println("GPS Status: VALID FIX");
        Serial.print("Time: ");
        Serial.println(currentGPS.timeString);
        
        Serial.print("Location: ");
        Serial.print(currentGPS.latitude, 6);
        Serial.print(", ");
        Serial.println(currentGPS.longitude, 6);
        
        Serial.print("Altitude: ");
        Serial.print(currentGPS.altitude);
        Serial.println(" meters");
        
        Serial.print("Satellites: ");
        Serial.println(currentGPS.satellites);
        
        // Fun fact: calculate approximate distance from equator
        float distFromEquator = abs(currentGPS.latitude) * 111.32;  // km per degree
        Serial.print("Distance from equator: ~");
        Serial.print(distFromEquator);
        Serial.println(" km");
        
    } else {
        Serial.println("GPS Status: NO FIX");
        Serial.print("Satellites visible: ");
        Serial.println(currentGPS.satellites);
        Serial.println("Waiting for GPS lock...");
    }
    
    Serial.println("=====================");
}

// Function to send commands to a sensor via UART
// Like asking a sensor specific questions
void sendSensorCommand(String command) {
    Serial.print("Sending to sensor: ");
    Serial.println(command);
    
    sensorSerial.println(command);  // Send command
    
    // Wait for response
    unsigned long startTime = millis();
    while (millis() - startTime < 1000) {  // Wait up to 1 second
        if (sensorSerial.available()) {
            String response = sensorSerial.readString();
            Serial.print("Sensor response: ");
            Serial.println(response);
            break;
        }
    }
}

// Function to create a simple sensor protocol
// This shows how to design your own communication protocol
void demonstrateSensorProtocol() {
    Serial.println("\n--- Sensor Protocol Demo ---");
    
    // Simple protocol: Send command, expect response
    // Format: "CMD:parameter\n" -> "RSP:value\n"
    
    // Example commands you might send to a smart sensor:
    sendSensorCommand("READ:TEMP");      // Read temperature
    delay(100);
    sendSensorCommand("READ:HUMID");     // Read humidity  
    delay(100);
    sendSensorCommand("SET:RATE:5");     // Set update rate to 5 Hz
    delay(100);
    sendSensorCommand("GET:STATUS");     // Get sensor status
    delay(100);
}

// Function to demonstrate data logging over UART
// Like keeping a diary of sensor readings
void logDataOverUART() {
    // Create a data packet with timestamp
    String dataPacket = "DATA,";
    dataPacket += millis();              // Timestamp
    dataPacket += ",";
    dataPacket += currentGPS.latitude;   // GPS latitude
    dataPacket += ",";
    dataPacket += currentGPS.longitude;  // GPS longitude
    dataPacket += ",";
    dataPacket += currentGPS.altitude;   // GPS altitude
    dataPacket += ",";
    dataPacket += analogRead(A0);        // Some sensor value
    
    // Send to both UART ports
    Serial.println("Logging data packet:");
    Serial.println(dataPacket);
    
    // You could send this to another device or data logger
    sensorSerial.println(dataPacket);
}

void setup() {
    Serial.begin(115200);
    Serial.println("Advanced UART Communication Example");
    Serial.println("===================================");
    
    // Initialize UART communication
    initializeUART();
    
    Serial.println("\nWaiting for GPS fix...");
    Serial.println("This may take 30 seconds to several minutes outdoors");
    Serial.println("GPS may not work indoors - try near a window");
}

void loop() {
    // Process incoming GPS data
    processGPSData();
    
    // Display GPS info every 10 seconds
    static unsigned long lastGPSDisplay = 0;
    if (millis() - lastGPSDisplay > 10000) {
        displayGPSInfo();
        lastGPSDisplay = millis();
    }
    
    // Demonstrate sensor protocol every 30 seconds
    static unsigned long lastSensorDemo = 0;
    if (millis() - lastSensorDemo > 30000) {
        demonstrateSensorProtocol();
        lastSensorDemo = millis();
    }
    
    // Log data every 60 seconds
    static unsigned long lastDataLog = 0;
    if (millis() - lastDataLog > 60000) {
        logDataOverUART();
        lastDataLog = millis();
    }
    
    // Small delay to prevent overwhelming the system
    delay(100);
}

/*
 * Hardware Setup for ESP32:
 * 
 * GPS Module (NEO-6M, NEO-8M):
 * ESP32 Pin | GPS Pin  | Function
 * ----------|----------|----------
 * GPIO 16   | TX       | ESP32 receives GPS data
 * GPIO 17   | RX       | ESP32 sends commands to GPS
 * 3.3V      | VCC      | Power (some need 5V)
 * GND       | GND      | Ground
 * 
 * Additional Sensor (optional):
 * ESP32 Pin | Sensor Pin | Function
 * ----------|------------|----------
 * GPIO 25   | TX         | ESP32 receives sensor data
 * GPIO 26   | RX         | ESP32 sends commands to sensor
 * 3.3V      | VCC        | Power
 * GND       | GND        | Ground
 * 
 * GPS Module Notes:
 * 1. Most GPS modules output NMEA sentences at 9600 baud
 * 2. First fix can take 30 seconds to several minutes
 * 3. Works best outdoors with clear sky view
 * 4. May not work indoors - try near windows
 * 5. Some modules need 5V power, others work with 3.3V
 * 
 * UART Communication Tips:
 * 1. Both devices must use same baud rate
 * 2. Use proper voltage levels (3.3V for ESP32)
 * 3. Cross connections: ESP32 TX -> Device RX, ESP32 RX -> Device TX
 * 4. Add pull-up resistors if communication is unreliable
 * 5. Use hardware serial when possible (more reliable than software serial)
 * 
 * Troubleshooting:
 * - No GPS data? Check baud rate and wiring
 * - Garbled data? Verify voltage levels and connections
 * - GPS not getting fix? Try outdoors with clear sky
 * - UART errors? Add delays between transmissions
 * - Buffer overflow? Process data more frequently
 * 
 * NMEA Sentence Types:
 * - GPGGA: Global Positioning System Fix Data
 * - GPRMC: Recommended Minimum Course data
 * - GPGSV: Satellites in view
 * - GPGSA: GPS DOP and active satellites
 * 
 * Protocol Design Tips:
 * 1. Use clear command/response format
 * 2. Add checksums for critical data
 * 3. Include timeouts for reliability
 * 4. Use delimiters (commas, semicolons)
 * 5. Plan for error handling
 */