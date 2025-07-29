#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <TFT_eSPI.h>
#include "DeviceConfig.h"

// Sensor data structure
struct SensorData {
    float values[SENSOR_COUNT];
    bool valid[SENSOR_COUNT];
    unsigned long lastUpdate;
};

// System status structure
struct SystemStatus {
    bool mainDeviceConnected;
    bool wifiConnected;
    String lastError;
    unsigned long lastUpdate;
};

class DisplayManager {
private:
    TFT_eSPI tft;
    
    // UI state
    uint8_t currentTab;
    uint16_t mainColor;        // Green or Yellow
    bool touchPressed;
    unsigned long lastTouchTime;
    
    // Data
    SensorData sensorData;
    SystemStatus systemStatus;
    
    // Touch handling
    bool readTouch(int16_t& x, int16_t& y);
    void handleTouch(int16_t x, int16_t y);
    void handleManualTabTouch(int16_t x, int16_t y);
    void handleSettingsTabTouch(int16_t x, int16_t y);
    
    // UI rendering
    void drawBackground();
    void drawTabs();
    void drawTabContent();
    void drawSensorsTab();
    void drawManualTab();
    void drawSettingsTab();
    
    // Terminal-style helpers
    void drawTerminalText(int16_t x, int16_t y, const String& text, uint16_t color = COLOR_WHITE);
    void drawButton(int16_t x, int16_t y, int16_t w, int16_t h, const String& text, bool pressed = false);
    
public:
    DisplayManager();
    
    void begin();
    void update();
    
    // Data updates from other components
    void updateSensorData(const SensorData& data);
    void updateSystemStatus(const SystemStatus& status);
    void setMainColor(uint16_t color);
    
    // UI state
    uint8_t getCurrentTab() const { return currentTab; }
};