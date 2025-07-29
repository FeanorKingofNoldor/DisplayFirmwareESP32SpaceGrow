#include "WiFiManager.h"
#include <HTTPClient.h>

WiFiManager::WiFiManager() : lastConnectionAttempt(0), lastScanTime(0), connectionAttempts(0) {
    credentials.valid = false;
    registration.registered = false;
}

void WiFiManager::begin() {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    
    Serial.println("WiFi Manager initialized");
}

void WiFiManager::handleConnection() {
    unsigned long currentTime = millis();
    
    if (!credentials.valid) {
        // No credentials configured
        return;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        // Already connected, check registration status
        if (!registration.registered && !registration.userToken.isEmpty()) {
            if (registerWithServer()) {
                registration.registered = true;
                Serial.println("Device registered successfully");
            }
        }
        return;
    }
    
    // Handle disconnection
    if (WiFi.status() != WL_CONNECTED && (currentTime - lastConnectionAttempt >= RECONNECT_INTERVAL)) {
        if (connectionAttempts < MAX_CONNECTION_ATTEMPTS) {
            connectToNetwork();
            lastConnectionAttempt = currentTime;
            connectionAttempts++;
        } else {
            // Reset attempts after extended delay
            if (currentTime - lastConnectionAttempt >= RECONNECT_INTERVAL * 3) {
                connectionAttempts = 0;
            }
        }
    }
}

bool WiFiManager::connectToNetwork() {
    if (!credentials.valid) {
        return false;
    }
    
    Serial.printf("Connecting to WiFi: %s\n", credentials.ssid.c_str());
    
    WiFi.begin(credentials.ssid.c_str(), credentials.password.c_str());
    
    // Wait for connection with timeout
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - startTime < CONNECTION_TIMEOUT)) {
        delay(500);
        Serial.print(".");
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.printf("\nWiFi connected! IP: %s\n", WiFi.localIP().toString().c_str());
        connectionAttempts = 0;
        return true;
    } else {
        Serial.println("\nWiFi connection failed");
        return false;
    }
}

void WiFiManager::handleDisconnection() {
    Serial.println("WiFi disconnected");
    WiFi.disconnect();
}

int WiFiManager::scanNetworks() {
    unsigned long currentTime = millis();
    
    // Don't scan too frequently
    if (currentTime - lastScanTime < 10000) {  // 10 second minimum between scans
        return WiFi.scanComplete();
    }
    
    Serial.println("Scanning for WiFi networks...");
    WiFi.scanDelete();  // Clear previous scan results
    
    int networkCount = WiFi.scanNetworks();
    lastScanTime = currentTime;
    
    if (networkCount > 0) {
        Serial.printf("Found %d networks:\n", networkCount);
        for (int i = 0; i < networkCount; i++) {
            Serial.printf("  %d: %s (%d dBm)\n", i, WiFi.SSID(i).c_str(), WiFi.RSSI(i));
        }
    } else {
        Serial.println("No networks found");
    }
    
    return networkCount;
}

String WiFiManager::getScannedSSID(int index) {
    if (index >= 0 && index < WiFi.scanComplete()) {
        return WiFi.SSID(index);
    }
    return "";
}

int WiFiManager::getScannedRSSI(int index) {
    if (index >= 0 && index < WiFi.scanComplete()) {
        return WiFi.RSSI(index);
    }
    return -999;
}

void WiFiManager::setCredentials(const String& ssid, const String& password) {
    credentials.ssid = ssid;
    credentials.password = password;
    credentials.valid = !ssid.isEmpty();
    
    if (credentials.valid) {
        Serial.printf("WiFi credentials set for: %s\n", ssid.c_str());
        connectionAttempts = 0;  // Reset connection attempts
    }
}

void WiFiManager::setRegistrationData(const String& deviceName, const String& userToken) {
    registration.deviceName = deviceName;
    registration.userToken = userToken;
    registration.registered = false;  // Will be set to true after successful registration
    
    Serial.printf("Registration data set - Device: %s\n", deviceName.c_str());
}

bool WiFiManager::registerWithServer() {
    if (!isConnected() || registration.userToken.isEmpty()) {
        return false;
    }
    
    HTTPClient http;
    
    // This URL would be configurable in a real implementation
    String serverUrl = "https://api.aeroponic.com/devices/register";
    
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + registration.userToken);
    
    JsonDocument doc;
    doc["device_name"] = registration.deviceName;
    doc["device_type"] = DEVICE_TYPE_STR;
    doc["user_token"] = registration.userToken;
    doc["firmware_version"] = "1.0.0";
    doc["hardware_info"] = "ESP32-S3 4.3\" Display";
    
    String payload;
    serializeJson(doc, payload);
    
    Serial.printf("Registering device with server...\n");
    int httpCode = http.POST(payload);
    
    if (httpCode == 200) {
        String response = http.getString();
        Serial.printf("Registration successful: %s\n", response.c_str());
        http.end();
        return true;
    } else {
        Serial.printf("Registration failed: HTTP %d\n", httpCode);
        http.end();
        return false;
    }
}