/*
 * Module 4.4: WiFi and Bluetooth Communication
 * 
 * What is WiFi/Bluetooth? Think of them as wireless mail systems:
 * - WiFi: Like the internet postal system - connects to networks
 * - Bluetooth: Like a walkie-talkie - direct device-to-device communication
 * - ESP32 can do both at the same time!
 * 
 * This example shows simple WiFi web server and Bluetooth communication
 * Hardware needed: Just ESP32 (it has built-in WiFi and Bluetooth!)
 */

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <BluetoothSerial.h>

// WiFi credentials (change these to your network)
const char* ssid = "YourWiFiName";        // Replace with your WiFi name
const char* password = "YourWiFiPassword"; // Replace with your WiFi password

// Create objects for WiFi and Bluetooth
WebServer webServer(80);           // Web server on port 80 (standard web port)
BluetoothSerial bluetooth;         // Bluetooth serial communication

// Simple variables to share data between WiFi and Bluetooth
String lastBluetoothMessage = "No messages yet";
String lastWebCommand = "No commands yet";
int webVisitorCount = 0;
int bluetoothMessageCount = 0;

// Sensor simulation (pretend we have some sensors)
float temperature = 23.5;
int lightLevel = 512;
bool ledState = false;

// Function to initialize WiFi connection
// Think of this as connecting to the internet
void initializeWiFi() {
    Serial.println("Connecting to WiFi...");
    Serial.print("Network: ");
    Serial.println(ssid);
    
    WiFi.begin(ssid, password);
    
    // Wait for connection (like dialing up to the internet)
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println();
        Serial.println("WiFi connected successfully!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());  // This is like your house address on the internet
        Serial.print("Signal strength: ");
        Serial.print(WiFi.RSSI());
        Serial.println(" dBm");
    } else {
        Serial.println();
        Serial.println("WiFi connection failed!");
        Serial.println("Check your network name and password");
    }
}

// Function to initialize Bluetooth
// Think of this as turning on your walkie-talkie
void initializeBluetooth() {
    Serial.println("Starting Bluetooth...");
    
    if (bluetooth.begin("ESP32-Learning")) {  // This is the name other devices will see
        Serial.println("Bluetooth started successfully!");
        Serial.println("Device name: ESP32-Learning");
        Serial.println("You can now connect from your phone or computer");
    } else {
        Serial.println("Bluetooth failed to start!");
    }
}

// Function to create the main web page
// Think of this as creating a poster that people can read
String createWebPage() {
    String html = "<!DOCTYPE html><html><head>";
    html += "<title>ESP32 Learning Server</title>";
    html += "<style>body{font-family:Arial;margin:40px;background:#f0f0f0;}";
    html += ".container{background:white;padding:20px;border-radius:10px;box-shadow:0 2px 10px rgba(0,0,0,0.1);}";
    html += ".sensor{background:#e8f4fd;padding:15px;margin:10px 0;border-left:4px solid #2196F3;}";
    html += ".button{background:#4CAF50;color:white;padding:10px 20px;text-decoration:none;border-radius:5px;margin:5px;}";
    html += ".button:hover{background:#45a049;}";
    html += "</style></head><body>";
    
    html += "<div class='container'>";
    html += "<h1>🔧 ESP32 Learning Dashboard</h1>";
    html += "<p>Welcome to your ESP32 web server! Visitor #" + String(webVisitorCount) + "</p>";
    
    // Show sensor data
    html += "<div class='sensor'>";
    html += "<h3>📊 Sensor Readings</h3>";
    html += "<p>🌡️ Temperature: " + String(temperature) + "°C</p>";
    html += "<p>💡 Light Level: " + String(lightLevel) + "/1023</p>";
    html += "<p>🔆 LED Status: " + String(ledState ? "ON" : "OFF") + "</p>";
    html += "</div>";
    
    // Show communication status
    html += "<div class='sensor'>";
    html += "<h3>📡 Communication Status</h3>";
    html += "<p>📶 WiFi Signal: " + String(WiFi.RSSI()) + " dBm</p>";
    html += "<p>📧 Last Web Command: " + lastWebCommand + "</p>";
    html += "<p>📱 Last Bluetooth Message: " + lastBluetoothMessage + "</p>";
    html += "<p>💬 Bluetooth Messages: " + String(bluetoothMessageCount) + "</p>";
    html += "</div>";
    
    // Control buttons
    html += "<h3>🎛️ Controls</h3>";
    html += "<a href='/led_on' class='button'>💡 Turn LED ON</a>";
    html += "<a href='/led_off' class='button'>💤 Turn LED OFF</a>";
    html += "<a href='/refresh' class='button'>🔄 Refresh Data</a>";
    
    html += "<h3>📱 Try Bluetooth</h3>";
    html += "<p>Connect to 'ESP32-Learning' via Bluetooth and send messages!</p>";
    html += "<p>Try sending: 'LED ON', 'LED OFF', 'STATUS', or 'HELLO'</p>";
    
    html += "</div></body></html>";
    
    return html;
}

// Function to handle web page requests
// Think of this as answering the door when someone visits your house
void handleWebRoot() {
    webVisitorCount++;  // Count visitors
    Serial.println("Web page requested by: " + webServer.client().remoteIP().toString());
    
    String page = createWebPage();
    webServer.send(200, "text/html", page);  // Send the web page
}

// Function to handle LED ON command from web
void handleLEDOn() {
    ledState = true;
    lastWebCommand = "LED turned ON via web";
    Serial.println("LED turned ON via web interface");
    
    // Redirect back to main page
    webServer.sendHeader("Location", "/");
    webServer.send(303);  // Redirect response
}

// Function to handle LED OFF command from web
void handleLEDOff() {
    ledState = false;
    lastWebCommand = "LED turned OFF via web";
    Serial.println("LED turned OFF via web interface");
    
    // Redirect back to main page
    webServer.sendHeader("Location", "/");
    webServer.send(303);  // Redirect response
}

// Function to handle refresh command
void handleRefresh() {
    // Simulate changing sensor readings
    temperature = 20.0 + random(0, 100) / 10.0;  // 20.0 to 30.0°C
    lightLevel = random(0, 1024);                  // 0 to 1023
    
    lastWebCommand = "Data refreshed";
    Serial.println("Sensor data refreshed via web");
    
    // Redirect back to main page
    webServer.sendHeader("Location", "/");
    webServer.send(303);  // Redirect response
}

// Function to set up web server routes
// Think of this as making a map of what happens when people visit different pages
void setupWebServer() {
    webServer.on("/", handleWebRoot);          // Main page
    webServer.on("/led_on", handleLEDOn);      // LED on command
    webServer.on("/led_off", handleLEDOff);    // LED off command
    webServer.on("/refresh", handleRefresh);    // Refresh data
    
    // Handle page not found
    webServer.onNotFound([]() {
        webServer.send(404, "text/plain", "Page not found! Try going to the main page.");
    });
    
    webServer.begin();  // Start the server
    Serial.println("Web server started!");
}

// Function to process Bluetooth messages
// Think of this as listening to your walkie-talkie
void processBluetoothMessages() {
    if (bluetooth.available()) {
        String message = bluetooth.readString();
        message.trim();  // Remove extra spaces and newlines
        
        bluetoothMessageCount++;
        lastBluetoothMessage = message;
        
        Serial.print("Bluetooth message received: ");
        Serial.println(message);
        
        // Respond to different commands
        if (message.equalsIgnoreCase("LED ON")) {
            ledState = true;
            bluetooth.println("LED turned ON! ✅");
            
        } else if (message.equalsIgnoreCase("LED OFF")) {
            ledState = false;
            bluetooth.println("LED turned OFF! ❌");
            
        } else if (message.equalsIgnoreCase("STATUS")) {
            bluetooth.println("📊 ESP32 Status Report:");
            bluetooth.println("🌡️ Temperature: " + String(temperature) + "°C");
            bluetooth.println("💡 Light: " + String(lightLevel) + "/1023");
            bluetooth.println("🔆 LED: " + String(ledState ? "ON" : "OFF"));
            bluetooth.println("📶 WiFi: " + String(WiFi.RSSI()) + " dBm");
            
        } else if (message.equalsIgnoreCase("HELLO")) {
            bluetooth.println("👋 Hello! I'm your ESP32!");
            bluetooth.println("Try these commands:");
            bluetooth.println("• LED ON / LED OFF");
            bluetooth.println("• STATUS");
            bluetooth.println("• SENSORS");
            
        } else if (message.equalsIgnoreCase("SENSORS")) {
            // Simulate new sensor readings
            temperature = 20.0 + random(0, 100) / 10.0;
            lightLevel = random(0, 1024);
            bluetooth.println("📊 Fresh sensor readings:");
            bluetooth.println("🌡️ " + String(temperature) + "°C");
            bluetooth.println("💡 " + String(lightLevel) + "/1023");
            
        } else {
            bluetooth.println("❓ Unknown command: " + message);
            bluetooth.println("Try: LED ON, LED OFF, STATUS, or HELLO");
        }
    }
}

// Function to send periodic updates via Bluetooth
void sendBluetoothUpdates() {
    static unsigned long lastUpdate = 0;
    
    if (millis() - lastUpdate > 30000) {  // Every 30 seconds
        if (bluetooth.hasClient()) {  // Only if someone is connected
            bluetooth.println("📡 Periodic Update:");
            bluetooth.println("⏰ Uptime: " + String(millis() / 1000) + " seconds");
            bluetooth.println("👥 Web visitors: " + String(webVisitorCount));
            bluetooth.println("💬 BT messages: " + String(bluetoothMessageCount));
        }
        lastUpdate = millis();
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("ESP32 WiFi & Bluetooth Communication");
    Serial.println("====================================");
    
    // Initialize communications
    initializeWiFi();
    initializeBluetooth();
    
    // Set up web server only if WiFi is connected
    if (WiFi.status() == WL_CONNECTED) {
        setupWebServer();
        Serial.println("\n🌐 Web Interface Ready!");
        Serial.print("Visit: http://");
        Serial.println(WiFi.localIP());
    }
    
    Serial.println("\n📱 Bluetooth Ready!");
    Serial.println("Connect to 'ESP32-Learning' and send messages");
    
    Serial.println("\n🚀 System ready! Try both web and Bluetooth interfaces");
}

void loop() {
    // Handle web server requests
    if (WiFi.status() == WL_CONNECTED) {
        webServer.handleClient();  // Process any web requests
    }
    
    // Handle Bluetooth messages
    processBluetoothMessages();
    
    // Send periodic Bluetooth updates
    sendBluetoothUpdates();
    
    // Simulate sensor readings changing over time
    static unsigned long lastSensorUpdate = 0;
    if (millis() - lastSensorUpdate > 5000) {  // Every 5 seconds
        temperature += (random(-10, 11) / 10.0);  // Small random change
        if (temperature < 15.0) temperature = 15.0;
        if (temperature > 35.0) temperature = 35.0;
        
        lightLevel += random(-50, 51);  // Small random change
        if (lightLevel < 0) lightLevel = 0;
        if (lightLevel > 1023) lightLevel = 1023;
        
        lastSensorUpdate = millis();
    }
    
    delay(10);  // Small delay to prevent overwhelming the system
}

/*
 * Setup Instructions:
 * 
 * 1. WiFi Setup:
 *    - Change 'ssid' and 'password' to your WiFi network
 *    - Upload code and check Serial Monitor for IP address
 *    - Open web browser and go to the IP address shown
 * 
 * 2. Bluetooth Setup:
 *    - On your phone/computer, look for 'ESP32-Learning' in Bluetooth devices
 *    - Connect to it (no pairing code needed)
 *    - Use a Bluetooth terminal app to send messages
 * 
 * Recommended Bluetooth Apps:
 * - Android: "Serial Bluetooth Terminal"
 * - iOS: "Bluetooth Terminal"
 * - Windows: "Bluetooth LE Explorer"
 * - Linux/Mac: Use built-in terminal with bluetoothctl
 * 
 * Web Interface Features:
 * ✅ Real-time sensor display
 * ✅ LED control buttons
 * ✅ Visitor counter
 * ✅ Communication status
 * ✅ Responsive design
 * 
 * Bluetooth Commands to Try:
 * - "LED ON" / "LED OFF" - Control LED
 * - "STATUS" - Get system status
 * - "HELLO" - Get help message
 * - "SENSORS" - Get fresh sensor readings
 * 
 * Troubleshooting:
 * 
 * WiFi Issues:
 * - Double-check network name and password
 * - Make sure ESP32 is within range
 * - Some networks block device communication
 * - Try 2.4GHz network (ESP32 doesn't support 5GHz)
 * 
 * Bluetooth Issues:
 * - Make sure Bluetooth is enabled on your device
 * - ESP32 uses Bluetooth Classic, not BLE
 * - Clear Bluetooth cache if connection fails
 * - Try unpairing and reconnecting
 * 
 * Web Page Issues:
 * - Check if firewall is blocking connections
 * - Try accessing from same network only
 * - Use IP address, not device name
 * - Refresh page if it seems stuck
 * 
 * Advanced Features You Could Add:
 * 1. Password protection for web interface
 * 2. Data logging to SD card
 * 3. Email notifications
 * 4. Mobile app integration
 * 5. Voice commands via Bluetooth
 * 6. Real sensor connections (temperature, humidity, etc.)
 * 7. Remote firmware updates
 * 8. Multiple user support
 * 
 * Security Notes:
 * - This is for learning - don't use on public networks without security
 * - Add authentication for real projects
 * - Use HTTPS for sensitive data
 * - Be careful with Bluetooth - anyone nearby can connect
 */