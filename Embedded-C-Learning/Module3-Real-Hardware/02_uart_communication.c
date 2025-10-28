/*
 * MODULE 3 - LESSON 2: UART Communication - Talking to the World
 * 
 * What you'll learn:
 * - How to send and receive data over UART/Serial
 * - Communicating with sensors, GPS modules, other microcontrollers
 * - Data parsing and protocol handling
 * - Real-world serial communication examples
 * 
 * UART = Universal Asynchronous Receiver Transmitter
 * Think of it like a telephone line:
 * - TX (Transmit) = Your mouth (you talk)
 * - RX (Receive) = Your ear (you listen)
 * - Baud rate = How fast you talk (bits per second)
 * 
 * NOTE: This code runs on ESP32 with Arduino IDE
 */

#include <HardwareSerial.h>  // For additional UART ports

// ESP32 has 3 UART ports:
// UART0: USB connection (Serial) - used for programming and debug
// UART1: Available for use (Serial1) 
// UART2: Available for use (Serial2)

// Pin definitions for UART connections
#define GPS_RX_PIN      16  // ESP32 receives GPS data on GPIO 16
#define GPS_TX_PIN      17  // ESP32 sends commands to GPS on GPIO 17
#define SENSOR_RX_PIN   25  // ESP32 receives sensor data on GPIO 25
#define SENSOR_TX_PIN   26  // ESP32 sends commands to sensor on GPIO 26

// Create additional serial ports
HardwareSerial GPSSerial(1);     // Use UART1 for GPS
HardwareSerial SensorSerial(2);  // Use UART2 for sensor

// Data buffers for received messages
char gps_buffer[128];
char sensor_buffer[64];
char command_buffer[32];

void setup() {
    // Initialize main serial (USB connection to computer)
    Serial.begin(115200);
    while(!Serial) delay(10);  // Wait for serial to be ready
    
    Serial.println("=== ESP32 UART Communication Examples ===");
    Serial.println();
    
    // Initialize GPS serial port (9600 baud is common for GPS)
    GPSSerial.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
    Serial.println("GPS Serial initialized (9600 baud)");
    
    // Initialize sensor serial port (38400 baud for faster sensor)
    SensorSerial.begin(38400, SERIAL_8N1, SENSOR_RX_PIN, SENSOR_TX_PIN);
    Serial.println("Sensor Serial initialized (38400 baud)");
    
    Serial.println("Type commands in Serial Monitor:");
    Serial.println("  'gps' - Request GPS data");
    Serial.println("  'sensor' - Request sensor data");
    Serial.println("  'temp' - Get temperature reading");
    Serial.println("  'help' - Show this menu");
    Serial.println();
}

void loop() {
    // Check for commands from computer
    handleSerialCommands();
    
    // Check for incoming GPS data
    handleGPSData();
    
    // Check for incoming sensor data
    handleSensorData();
    
    // Send periodic status updates
    sendPeriodicUpdates();
    
    delay(100);  // Small delay to prevent overwhelming the system
}

/*
 * EXAMPLE 1: Handling Serial Commands from Computer
 */
void handleSerialCommands() {
    static uint8_t cmd_index = 0;
    
    // Read characters from USB serial
    while(Serial.available()) {
        char c = Serial.read();
        
        if(c == '\n' || c == '\r') {  // End of command
            command_buffer[cmd_index] = '\0';  // Null terminate
            
            if(cmd_index > 0) {  // If we got a command
                processCommand(command_buffer);
                cmd_index = 0;  // Reset for next command
            }
        }
        else if(cmd_index < sizeof(command_buffer) - 1) {
            command_buffer[cmd_index++] = c;
        }
    }
}

void processCommand(char* command) {
    Serial.print("Received command: ");
    Serial.println(command);
    
    if(strcmp(command, "help") == 0) {
        Serial.println("Available commands:");
        Serial.println("  gps    - Request GPS position");
        Serial.println("  sensor - Request all sensor data");
        Serial.println("  temp   - Get temperature only");
        Serial.println("  reset  - Reset system");
    }
    else if(strcmp(command, "gps") == 0) {
        requestGPSData();
    }
    else if(strcmp(command, "sensor") == 0) {
        requestSensorData();
    }
    else if(strcmp(command, "temp") == 0) {
        requestTemperature();
    }
    else if(strcmp(command, "reset") == 0) {
        Serial.println("Resetting system...");
        ESP.restart();
    }
    else {
        Serial.println("Unknown command. Type 'help' for available commands.");
    }
    
    Serial.println();
}

/*
 * EXAMPLE 2: GPS Communication (NMEA Protocol)
 */
void requestGPSData() {
    Serial.println("Requesting GPS data...");
    
    // Send command to GPS module (if it supports commands)
    GPSSerial.println("$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28");
    
    Serial.println("GPS request sent. Listening for response...");
}

void handleGPSData() {
    static uint8_t gps_index = 0;
    
    while(GPSSerial.available()) {
        char c = GPSSerial.read();
        
        if(c == '\n') {  // End of GPS sentence
            gps_buffer[gps_index] = '\0';
            
            if(gps_index > 0) {
                parseGPSData(gps_buffer);
                gps_index = 0;
            }
        }
        else if(gps_index < sizeof(gps_buffer) - 1) {
            gps_buffer[gps_index++] = c;
        }
    }
}

void parseGPSData(char* gps_sentence) {
    Serial.print("GPS: ");
    Serial.println(gps_sentence);
    
    // Simple NMEA sentence parsing
    if(strncmp(gps_sentence, "$GPGGA", 6) == 0) {
        Serial.println("  → This is position data (GPGGA)");
        parseGPGGA(gps_sentence);
    }
    else if(strncmp(gps_sentence, "$GPRMC", 6) == 0) {
        Serial.println("  → This is recommended minimum data (GPRMC)");
        parseGPRMC(gps_sentence);
    }
    else {
        Serial.println("  → Other GPS sentence");
    }
}

void parseGPGGA(char* sentence) {
    // Example GPGGA: $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
    // This is a simplified parser - real GPS parsing is more complex
    
    char* token;
    uint8_t field = 0;
    
    token = strtok(sentence, ",");
    while(token != NULL && field < 10) {
        switch(field) {
            case 1:  // Time
                Serial.print("  Time: ");
                Serial.println(token);
                break;
            case 2:  // Latitude
                Serial.print("  Latitude: ");
                Serial.println(token);
                break;
            case 4:  // Longitude  
                Serial.print("  Longitude: ");
                Serial.println(token);
                break;
            case 6:  // Fix quality
                Serial.print("  GPS Fix: ");
                Serial.println(token);
                break;
        }
        field++;
        token = strtok(NULL, ",");
    }
}

void parseGPRMC(char* sentence) {
    Serial.println("  → Speed and course data available in this sentence");
    // Implementation similar to GPGGA parsing
}

/*
 * EXAMPLE 3: Sensor Communication (Custom Protocol)
 */
void requestSensorData() {
    Serial.println("Requesting sensor data...");
    
    // Send command to sensor (custom protocol)
    SensorSerial.println("READ_ALL");
    
    Serial.println("Sensor request sent. Waiting for response...");
}

void requestTemperature() {
    Serial.println("Requesting temperature only...");
    
    // Send specific temperature request
    SensorSerial.println("READ_TEMP");
}

void handleSensorData() {
    static uint8_t sensor_index = 0;
    
    while(SensorSerial.available()) {
        char c = SensorSerial.read();
        
        if(c == '\n') {  // End of sensor message
            sensor_buffer[sensor_index] = '\0';
            
            if(sensor_index > 0) {
                parseSensorData(sensor_buffer);
                sensor_index = 0;
            }
        }
        else if(sensor_index < sizeof(sensor_buffer) - 1) {
            sensor_buffer[sensor_index++] = c;
        }
    }
}

void parseSensorData(char* sensor_message) {
    Serial.print("Sensor: ");
    Serial.println(sensor_message);
    
    // Parse different sensor message types
    if(strncmp(sensor_message, "TEMP:", 5) == 0) {
        float temperature = atof(&sensor_message[5]);
        Serial.print("  → Temperature: ");
        Serial.print(temperature);
        Serial.println("°C");
    }
    else if(strncmp(sensor_message, "HUMIDITY:", 9) == 0) {
        float humidity = atof(&sensor_message[9]);
        Serial.print("  → Humidity: ");
        Serial.print(humidity);
        Serial.println("%");
    }
    else if(strncmp(sensor_message, "PRESSURE:", 9) == 0) {
        float pressure = atof(&sensor_message[9]);
        Serial.print("  → Pressure: ");
        Serial.print(pressure);
        Serial.println(" hPa");
    }
    else if(strncmp(sensor_message, "ALL:", 4) == 0) {
        parseAllSensorData(&sensor_message[4]);
    }
    else {
        Serial.println("  → Unknown sensor message format");
    }
}

void parseAllSensorData(char* data) {
    // Example format: "25.3,60.1,1013.2" (temp,humidity,pressure)
    Serial.println("  → Parsing all sensor data:");
    
    char* temp_str = strtok(data, ",");
    char* humid_str = strtok(NULL, ",");
    char* pressure_str = strtok(NULL, ",");
    
    if(temp_str) {
        Serial.print("    Temperature: ");
        Serial.print(temp_str);
        Serial.println("°C");
    }
    
    if(humid_str) {
        Serial.print("    Humidity: ");
        Serial.print(humid_str);
        Serial.println("%");
    }
    
    if(pressure_str) {
        Serial.print("    Pressure: ");
        Serial.print(pressure_str);
        Serial.println(" hPa");
    }
}

/*
 * EXAMPLE 4: Periodic Status Updates
 */
void sendPeriodicUpdates() {
    static uint32_t last_update = 0;
    const uint32_t UPDATE_INTERVAL = 30000;  // 30 seconds
    
    if(millis() - last_update >= UPDATE_INTERVAL) {
        Serial.println("=== Periodic Status Update ===");
        Serial.print("System uptime: ");
        Serial.print(millis() / 1000);
        Serial.println(" seconds");
        
        Serial.print("Free heap: ");
        Serial.print(ESP.getFreeHeap());
        Serial.println(" bytes");
        
        // Send status to connected devices
        GPSSerial.println("$PMTK301,2*2E");  // Example GPS status request
        SensorSerial.println("STATUS");        // Request sensor status
        
        Serial.println("Status update complete.");
        Serial.println();
        
        last_update = millis();
    }
}

/*
 * EXAMPLE 5: Data Logging Over UART
 */
void logDataToSerial() {
    // Create CSV-format data log
    Serial.print(millis());
    Serial.print(",");
    
    // Simulate sensor readings
    float temperature = 25.0 + (random(-50, 50) / 10.0);
    float humidity = 60.0 + (random(-200, 200) / 10.0);
    
    Serial.print(temperature);
    Serial.print(",");
    Serial.print(humidity);
    Serial.println();
}

/*
 * EXAMPLE 6: Binary Data Communication
 */
void sendBinaryData() {
    // Sometimes you need to send binary data (not text)
    uint8_t binary_packet[] = {
        0xAA,           // Start marker
        0x05,           // Data length
        0x12, 0x34,     // Temperature (big-endian)
        0x56, 0x78,     // Humidity (big-endian)
        0x9A,           // Checksum
        0x55            // End marker
    };
    
    Serial.println("Sending binary packet:");
    for(uint8_t i = 0; i < sizeof(binary_packet); i++) {
        Serial.print("0x");
        if(binary_packet[i] < 0x10) Serial.print("0");
        Serial.print(binary_packet[i], HEX);
        Serial.print(" ");
        
        // Actually send the byte
        SensorSerial.write(binary_packet[i]);
    }
    Serial.println();
}

/*
 * UART TROUBLESHOOTING GUIDE
 */
void printUARTTroubleshootingGuide() {
    Serial.println("=== UART Troubleshooting Guide ===");
    Serial.println();
    
    Serial.println("Problem: No data received");
    Serial.println("Solutions:");
    Serial.println("1. Check wiring: TX → RX, RX → TX, GND → GND");
    Serial.println("2. Check baud rates match on both devices");
    Serial.println("3. Check voltage levels (3.3V vs 5V)");
    Serial.println("4. Verify pin assignments in code");
    Serial.println();
    
    Serial.println("Problem: Garbled data");
    Serial.println("Solutions:");
    Serial.println("1. Wrong baud rate - try common rates: 9600, 38400, 115200");
    Serial.println("2. Voltage level mismatch - use level shifter");
    Serial.println("3. Electrical interference - use shorter wires, twisted pairs");
    Serial.println("4. Check data format: 8N1, 8E1, etc.");
    Serial.println();
    
    Serial.println("Problem: Data loss or corruption");
    Serial.println("Solutions:");
    Serial.println("1. Add flow control (RTS/CTS)");
    Serial.println("2. Increase buffer sizes");
    Serial.println("3. Process received data faster");
    Serial.println("4. Add checksums for error detection");
    Serial.println();
}

/*
 * HARDWARE SETUP INSTRUCTIONS:
 * 
 * 1. GPS Module Connection:
 *    - GPS VCC → ESP32 3.3V
 *    - GPS GND → ESP32 GND  
 *    - GPS TX → ESP32 GPIO 16 (RX)
 *    - GPS RX → ESP32 GPIO 17 (TX)
 * 
 * 2. Sensor Module Connection:
 *    - Sensor VCC → ESP32 3.3V
 *    - Sensor GND → ESP32 GND
 *    - Sensor TX → ESP32 GPIO 25 (RX)
 *    - Sensor RX → ESP32 GPIO 26 (TX)
 * 
 * 3. USB Connection:
 *    - Automatically connected when ESP32 is plugged into computer
 *    - Used for programming and Serial Monitor
 * 
 * NOTE: Always connect GND between devices!
 * Never connect 5V device directly to ESP32 (3.3V) pins!
 */

/*
 * What did we learn?
 * 
 * 1. UART enables communication between devices
 * 2. ESP32 has multiple UART ports (Serial, Serial1, Serial2)
 * 3. Different devices use different baud rates and protocols
 * 4. Data can be text-based (ASCII) or binary
 * 5. Parsing received data requires careful string/buffer handling
 * 6. Real protocols like NMEA (GPS) have specific formats
 * 7. Error handling and troubleshooting are essential
 * 8. Proper wiring and voltage levels are critical
 * 
 * Next: ADC - Reading analog sensors like temperature, light, etc.!
 */