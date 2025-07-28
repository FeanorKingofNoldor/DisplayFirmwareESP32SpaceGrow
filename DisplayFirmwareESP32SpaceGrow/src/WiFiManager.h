#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include <ArduinoJson.h>
#include "DeviceConfig.h"

// Network credentials structure
struct NetworkCredentials {
    String ssid;
    String password;
    bool valid;
};

// Registration data structure
struct RegistrationData {
    String deviceName;
    String userToken;
    bool registered;
};

class WiFiManager {
private:
    NetworkCredentials credentials;
    RegistrationData registration;
    
    unsigned long lastConnectionAttempt;
    unsigned long lastScanTime;
    int connectionAttempts;
    
    // Connection management
    static const unsigned long CONNECTION_TIMEOUT = 15000;
    static const unsigned long RECONNECT_INTERVAL = 30000;
    static const int MAX_CONNECTION_ATTEMPTS = 3;
    
    // Network operations
    bool connectToNetwork();
    void handleDisconnection();
    
    // Device registration
    bool registerWithServer();
    
public:
    WiFiManager();
    
    void begin();
    void handleConnection();
    
    // Network scanning for display
    int scanNetworks();
    String getScannedSSID(int index);
    int getScannedRSSI(int index);
    
    // Credential management
    void setCredentials(const String& ssid, const String& password);
    bool hasValidCredentials() const { return credentials.valid; }
    
    // Registration management
    void setRegistrationData(const String& deviceName, const String& userToken);
    bool isRegistered() const { return registration.registered; }
    
    // Status
    bool isConnected() const { return WiFi.status() == WL_CONNECTED; }
    String getLocalIP() const { return WiFi.localIP().toString(); }
};