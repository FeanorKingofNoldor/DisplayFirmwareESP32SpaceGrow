#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include <LittleFS.h>
#include <ArduinoJson.h>
#include "DeviceConfig.h"

// Configuration structure
struct DisplayConfig {
    String wifiSSID;
    String wifiPassword;
    String deviceName;
    String userToken;
    uint16_t mainColor;         // COLOR_GREEN or COLOR_YELLOW
    bool registered;
    bool wifiConfigured;
};

class StorageManager {
private:
    DisplayConfig config;
    static const char* CONFIG_FILE_PATH;
    
    // File operations
    bool writeConfigFile();
    bool readConfigFile();
    void setDefaultConfig();
    
public:
    StorageManager();
    
    bool begin();
    
    // Configuration access
    const DisplayConfig& getConfig() const { return config; }
    
    // WiFi configuration
    void setWiFiCredentials(const String& ssid, const String& password);
    bool hasWiFiCredentials() const { return config.wifiConfigured; }
    
    // Device registration
    void setRegistrationData(const String& deviceName, const String& userToken);
    void setRegistered(bool registered);
    bool isRegistered() const { return config.registered; }
    
    // Display settings
    void setMainColor(uint16_t color);
    uint16_t getMainColor() const { return config.mainColor; }
    
    // File system utilities
    bool formatFileSystem();
    size_t getTotalSpace() const { return LittleFS.totalBytes(); }
    size_t getUsedSpace() const { return LittleFS.usedBytes(); }
};