#include "UARTManager.h"

UARTManager::UARTManager() : serial(&Serial2), lastSensorRequest(0), lastStatusRequest(0), lastResponse(0), displayManager(nullptr) {
}

void UARTManager::begin() {
    serial->begin(UART_BAUD_RATE, SERIAL_8N1, 16, 17);  // RX=16, TX=17 for ESP32-S3
    serial->setTimeout(100);  // 100ms timeout for serial reads
    
    Serial.println("UART Manager initialized");
}

void UARTManager::processMessages() {
    unsigned long currentTime = millis();
    
    // Send periodic requests
    if (currentTime - lastSensorRequest >= SENSOR_REQUEST_INTERVAL) {
        requestSensorData();
        lastSensorRequest = currentTime;
    }
    
    if (currentTime - lastStatusRequest >= STATUS_REQUEST_INTERVAL) {
        requestStatus();
        lastStatusRequest = currentTime;
    }
    
    // Process incoming messages
    while (serial->available()) {
        String message = serial->readStringUntil('\n');
        message.trim();
        
        if (message.length() > 0) {
            processIncomingMessage(message);
            lastResponse = currentTime;
        }
    }
    
    // Update connection status based on last response time
    if (displayManager) {
        SystemStatus status;
        status.mainDeviceConnected = isMainDeviceConnected();
        status.wifiConnected = WiFi.status() == WL_CONNECTED;
        status.lastUpdate = currentTime;
        status.lastError = "";
        
        displayManager->updateSystemStatus(status);
    }
}

void UARTManager::processIncomingMessage(const String& message) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, message);
    
    if (error) {
        Serial.printf("JSON parse error: %s\n", error.c_str());
        return;
    }
    
    // Check if this is sensor data or status data
    #ifdef DEVICE_TYPE_ENVIRONMENT
        if (doc.containsKey("temp") || doc.containsKey("humidity") || doc.containsKey("air_pressure")) {
            parseSensorData(doc);
        }
    #elif DEVICE_TYPE_LIQUID
        if (doc.containsKey("ph") || doc.containsKey("ec") || doc.containsKey("water_temp")) {
            parseSensorData(doc);
        }
    #endif
    
    if (doc.containsKey("status")) {
        parseStatusData(doc);
    }
}

void UARTManager::parseSensorData(JsonDocument& doc) {
    if (!displayManager) return;
    
    SensorData data;
    data.lastUpdate = millis();
    
    #ifdef DEVICE_TYPE_ENVIRONMENT
        // Environment sensors: temperature, humidity, air_pressure
        data.values[0] = doc["temp"] | 0.0;
        data.values[1] = doc["humidity"] | 0.0;
        data.values[2] = doc["air_pressure"] | 0.0;
        
        data.valid[0] = doc.containsKey("temp");
        data.valid[1] = doc.containsKey("humidity");
        data.valid[2] = doc.containsKey("air_pressure");
        
    #elif DEVICE_TYPE_LIQUID
        // Liquid sensors: ph, ec, water_temp
        data.values[0] = doc["ph"] | 0.0;
        data.values[1] = doc["ec"] | 0.0;
        data.values[2] = doc["water_temp"] | 0.0;
        
        data.valid[0] = doc.containsKey("ph");
        data.valid[1] = doc.containsKey("ec");
        data.valid[2] = doc.containsKey("water_temp");
    #endif
    
    displayManager->updateSensorData(data);
    
    Serial.println("Sensor data updated");
}

void UARTManager::parseStatusData(JsonDocument& doc) {
    if (!displayManager) return;
    
    SystemStatus status;
    status.mainDeviceConnected = true;  // We received a response
    status.wifiConnected = doc["wifi_connected"] | false;
    status.lastUpdate = millis();
    
    if (doc.containsKey("error")) {
        status.lastError = doc["error"].as<String>();
    } else {
        status.lastError = "";
    }
    
    displayManager->updateSystemStatus(status);
}

void UARTManager::sendCommand(const String& cmd) {
    JsonDocument doc;
    doc["cmd"] = cmd;
    
    String message;
    serializeJson(doc, message);
    
    serial->println(message);
    Serial.printf("Sent command: %s\n", message.c_str());
}

void UARTManager::sendCommand(const String& cmd, int value) {
    JsonDocument doc;
    doc["cmd"] = cmd;
    
    #ifdef DEVICE_TYPE_LIQUID
        if (cmd == "manual_pump") {
            doc["pump"] = value;
        }
    #endif
    
    String message;
    serializeJson(doc, message);
    
    serial->println(message);
    Serial.printf("Sent command: %s\n", message.c_str());
}

void UARTManager::requestSensorData() {
    sendCommand("get_sensors");
}

void UARTManager::requestStatus() {
    sendCommand("get_status");
}

void UARTManager::sendManualCommand(const String& command) {
    #ifdef DEVICE_TYPE_ENVIRONMENT
        if (command == "lights") {
            sendCommand("manual_lights");
        } else if (command == "spray") {
            sendCommand("manual_spray");
        }
    #elif DEVICE_TYPE_LIQUID
        if (command == "probe") {
            sendCommand("manual_probe");
        }
    #endif
}

void UARTManager::sendManualCommand(const String& command, int pumpNumber) {
    #ifdef DEVICE_TYPE_LIQUID
        if (command == "pump" && pumpNumber >= 1 && pumpNumber <= 5) {
            sendCommand("manual_pump", pumpNumber);
        }
    #endif
}

bool UARTManager::isMainDeviceConnected() const {
    unsigned long timeSinceLastResponse = millis() - lastResponse;
    return (timeSinceLastResponse < UART_TIMEOUT_MS) && (lastResponse > 0);
}