#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "DeviceConfig.h"
#include "DisplayManager.h"
#include "UARTManager.h"
#include "WiFiManager.h"
#include "StorageManager.h"

// Task handles
TaskHandle_t displayTaskHandle = nullptr;
TaskHandle_t uartTaskHandle = nullptr;
TaskHandle_t wifiTaskHandle = nullptr;

// Global managers
DisplayManager* displayManager = nullptr;
UARTManager* uartManager = nullptr;
WiFiManager* wifiManager = nullptr;
StorageManager* storageManager = nullptr;

// Task priorities from reference patterns
#define PRIORITY_HIGH       15
#define PRIORITY_MEDIUM     10
#define PRIORITY_LOW        5

#define STACK_SIZE_NORMAL   4096
#define STACK_SIZE_MINIMAL  2048

// Display task - handles UI updates and touch input
void displayTask(void* pvParameters) {
    displayManager = new DisplayManager();
    
    // Load color scheme from storage
    uint16_t savedColor = storageManager->getMainColor();
    displayManager->setMainColor(savedColor);
    
    displayManager->begin();
    
    while (true) {
        displayManager->update();
        vTaskDelay(pdMS_TO_TICKS(50)); // 20 FPS update rate
    }
}

// UART communication task - handles JSON protocol with main device
void uartTask(void* pvParameters) {
    uartManager = new UARTManager();
    uartManager->setDisplayManager(displayManager);
    uartManager->begin();
    
    while (true) {
        uartManager->processMessages();
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// WiFi management task - handles network connections and OTA
void wifiTask(void* pvParameters) {
    wifiManager = new WiFiManager();
    
    // Load WiFi credentials from storage
    const DisplayConfig& config = storageManager->getConfig();
    if (config.wifiConfigured) {
        wifiManager->setCredentials(config.wifiSSID, config.wifiPassword);
    }
    
    // Load registration data
    if (!config.userToken.isEmpty()) {
        wifiManager->setRegistrationData(config.deviceName, config.userToken);
    }
    
    wifiManager->begin();
    
    while (true) {
        wifiManager->handleConnection();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void setup() {
    Serial.begin(115200);
    
    // Initialize storage first
    storageManager = new StorageManager();
    storageManager->begin();
    
    // Print device type for debugging
    #ifdef DEVICE_TYPE_ENVIRONMENT
        Serial.println("AeroDisplay ESP32 - Environment Mode");
    #elif DEVICE_TYPE_LIQUID
        Serial.println("AeroDisplay ESP32 - Liquid Mode");
    #else
        Serial.println("ERROR: Device type not defined");
        while(1) delay(1000);
    #endif
    
    // Create FreeRTOS tasks - no core assignment, let scheduler handle
    xTaskCreate(
        displayTask,
        "DisplayTask",
        STACK_SIZE_NORMAL,
        nullptr,
        PRIORITY_HIGH,
        &displayTaskHandle
    );
    
    xTaskCreate(
        uartTask,
        "UARTTask",
        STACK_SIZE_NORMAL,
        nullptr,
        PRIORITY_MEDIUM,
        &uartTaskHandle
    );
    
    xTaskCreate(
        wifiTask,
        "WiFiTask",
        STACK_SIZE_MINIMAL,
        nullptr,
        PRIORITY_LOW,
        &wifiTaskHandle
    );
    
    Serial.println("AeroDisplay tasks started");
}

void loop() {
    // FreeRTOS handles everything, keep loop empty
    vTaskDelay(portMAX_DELAY);
}