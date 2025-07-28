#ifndef UART_MANAGER_H
#define UART_MANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "DeviceConfig.h"
#include "DisplayManager.h"

class UARTManager {
private:
    HardwareSerial* serial;
    unsigned long lastSensorRequest;
    unsigned long lastStatusRequest;
    unsigned long lastResponse;
    
    // Request intervals
    static const unsigned long SENSOR_REQUEST_INTERVAL = 2000;  // 2 seconds
    static const unsigned long STATUS_REQUEST_INTERVAL = 5000;  // 5 seconds
    
    // JSON processing
    void processIncomingMessage(const String& message);
    void sendCommand(const String& cmd);
    void sendCommand(const String& cmd, int value);
    
    // Data parsing
    void parseSensorData(JsonDocument& doc);
    void parseStatusData(JsonDocument& doc);
    
    // External references
    DisplayManager* displayManager;
    
public:
    UARTManager();
    
    void begin();
    void processMessages();
    void setDisplayManager(DisplayManager* dm) { displayManager = dm; }
    
    // Command sending
    void requestSensorData();
    void requestStatus();
    void sendManualCommand(const String& command);
    void sendManualCommand(const String& command, int pumpNumber);
    
    // Connection status
    bool isMainDeviceConnected() const;
};