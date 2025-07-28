#include "StorageManager.h"

const char* StorageManager::CONFIG_FILE_PATH = "/config.json";

StorageManager::StorageManager() {
    setDefaultConfig();
}

bool StorageManager::begin() {
    if (!LittleFS.begin(true)) {  // format on failure
        Serial.println("LittleFS mount failed, formatting...");
        if (!LittleFS.format()) {
            Serial.println("LittleFS format failed");
            return false;
        }
        if (!LittleFS.begin()) {
            Serial.println("LittleFS mount failed after format");
            return false;
        }
    }
    
    Serial.println("LittleFS mounted successfully");
    
    // Load existing configuration or create default
    if (!readConfigFile()) {
        Serial.println("No valid config found, creating default");
        writeConfigFile();
    }
    
    return true;
}

void StorageManager::setDefaultConfig() {
    config.wifiSSID = "";
    config.wifiPassword = "";
    config.deviceName = DEVICE_NAME;
    config.userToken = "";
    config.mainColor = COLOR_GREEN;  // Default to classic green
    config.registered = false;
    config.wifiConfigured = false;
}

bool StorageManager::readConfigFile() {
    if (!LittleFS.exists(CONFIG_FILE_PATH)) {
        return false;
    }
    
    File file = LittleFS.open(CONFIG_FILE_PATH, "r");
    if (!file) {
        Serial.println("Failed to open config file for reading");
        return false;
    }
    
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();
    
    if (error) {
        Serial.printf("Config JSON parse error: %s\n", error.c_str());
        return false;
    }
    
    // Load configuration values
    config.wifiSSID = doc["wifi_ssid"] | "";
    config.wifiPassword = doc["wifi_password"] | "";
    config.deviceName = doc["device_name"] | DEVICE_NAME;
    config.userToken = doc["user_token"] | "";
    config.mainColor = doc["main_color"] | COLOR_GREEN;
    config.registered = doc["registered"] | false;
    config.wifiConfigured = doc["wifi_configured"] | false;
    
    Serial.println("Configuration loaded successfully");
    return true;
}

bool StorageManager::writeConfigFile() {
    JsonDocument doc;
    
    doc["wifi_ssid"] = config.wifiSSID;
    doc["wifi_password"] = config.wifiPassword;
    doc["device_name"] = config.deviceName;
    doc["user_token"] = config.userToken;
    doc["main_color"] = config.mainColor;
    doc["registered"] = config.registered;
    doc["wifi_configured"] = config.wifiConfigured;
    
    File file = LittleFS.open(CONFIG_FILE_PATH, "w");
    if (!file) {
        Serial.println("Failed to open config file for writing");
        return false;
    }
    
    if (serializeJson(doc, file) == 0) {
        Serial.println("Failed to write config file");
        file.close();
        return false;
    }
    
    file.close();
    Serial.println("Configuration saved successfully");
    return true;
}

void StorageManager::setWiFiCredentials(const String& ssid, const String& password) {
    config.wifiSSID = ssid;
    config.wifiPassword = password;
    config.wifiConfigured = !ssid.isEmpty();
    writeConfigFile();
}

void StorageManager::setRegistrationData(const String& deviceName, const String& userToken) {
    config.deviceName = deviceName;
    config.userToken = userToken;
    writeConfigFile();
}

void StorageManager::setRegistered(bool registered) {
    config.registered = registered;
    writeConfigFile();
}

void StorageManager::setMainColor(uint16_t color) {
    config.mainColor = color;
    writeConfigFile();
}

bool StorageManager::formatFileSystem() {
    Serial.println("Formatting file system...");
    return LittleFS.format();
}