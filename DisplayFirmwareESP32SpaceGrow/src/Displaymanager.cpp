#include "DisplayManager.h"

DisplayManager::DisplayManager() : currentTab(TAB_SENSORS), mainColor(COLOR_GREEN), touchPressed(false), lastTouchTime(0) {
    // Initialize sensor data
    for (int i = 0; i < SENSOR_COUNT; i++) {
        sensorData.values[i] = 0.0;
        sensorData.valid[i] = false;
    }
    sensorData.lastUpdate = 0;
    
    // Initialize system status
    systemStatus.mainDeviceConnected = false;
    systemStatus.wifiConnected = false;
    systemStatus.lastError = "";
    systemStatus.lastUpdate = 0;
}

void DisplayManager::begin() {
    tft.init();
    tft.setRotation(1);  // Landscape orientation
    tft.fillScreen(COLOR_BLACK);
    
    // Set text properties for terminal style
    tft.setTextColor(COLOR_WHITE, COLOR_BLACK);
    tft.setTextSize(1);
    
    Serial.printf("Display initialized: %dx%d\n", tft.width(), tft.height());
    
    // Draw initial UI
    drawBackground();
    drawTabs();
    drawTabContent();
}

void DisplayManager::update() {
    int16_t x, y;
    
    // Handle touch input with debouncing
    if (readTouch(x, y)) {
        if (!touchPressed && (millis() - lastTouchTime > 200)) {
            touchPressed = true;
            lastTouchTime = millis();
            handleTouch(x, y);
        }
    } else {
        touchPressed = false;
    }
    
    // Redraw tab content (data may have updated)
    drawTabContent();
}

bool DisplayManager::readTouch(int16_t& x, int16_t& y) {
    uint16_t touchX, touchY;
    bool touched = tft.getTouch(&touchX, &touchY);
    
    if (touched) {
        x = touchX;
        y = touchY;
        return true;
    }
    
    return false;
}

void DisplayManager::handleTouch(int16_t x, int16_t y) {
    // Tab bar area (top 40 pixels)
    if (y < 40) {
        int tabWidth = DISPLAY_WIDTH / TAB_COUNT;
        uint8_t newTab = x / tabWidth;
        
        if (newTab < TAB_COUNT && newTab != currentTab) {
            currentTab = newTab;
            drawTabs();
            drawTabContent();
        }
        return;
    }
    
    // Tab content area
    switch (currentTab) {
        case TAB_MANUAL:
            handleManualTabTouch(x, y);
            break;
        case TAB_SETTINGS:
            handleSettingsTabTouch(x, y);
            break;
    }
}

void DisplayManager::handleManualTabTouch(int16_t x, int16_t y) {
    // Manual control buttons (simplified layout)
    int buttonHeight = 50;
    int buttonSpacing = 10;
    int startY = 60;
    
    #ifdef DEVICE_TYPE_ENVIRONMENT
        // Two buttons: Lights and Spray
        if (y >= startY && y < startY + buttonHeight) {
            // Lights button
            // Send command via external callback (to be connected to UARTManager)
            Serial.println("Manual command: lights");
        } else if (y >= startY + buttonHeight + buttonSpacing && y < startY + 2 * buttonHeight + buttonSpacing) {
            // Spray button
            Serial.println("Manual command: spray");
        }
    #elif DEVICE_TYPE_LIQUID
        // Six buttons: 5 pumps + pH/EC check
        int buttonIndex = (y - startY) / (buttonHeight + buttonSpacing);
        if (buttonIndex >= 0 && buttonIndex < MANUAL_CONTROL_COUNT) {
            if (buttonIndex < 5) {
                Serial.printf("Manual command: pump %d\n", buttonIndex + 1);
            } else {
                Serial.println("Manual command: probe");
            }
        }
    #endif
}

void DisplayManager::handleSettingsTabTouch(int16_t x, int16_t y) {
    // Settings buttons (simplified - will be expanded later)
    int buttonHeight = 40;
    int buttonSpacing = 10;
    int startY = 60;
    
    int buttonIndex = (y - startY) / (buttonHeight + buttonSpacing);
    
    switch (buttonIndex) {
        case 0:
            Serial.println("Settings: WiFi setup");
            break;
        case 1:
            // Toggle color scheme
            mainColor = (mainColor == COLOR_GREEN) ? COLOR_YELLOW : COLOR_GREEN;
            Serial.printf("Color changed to: %s\n", (mainColor == COLOR_GREEN) ? "Green" : "Yellow");
            break;
        case 2:
            Serial.println("Settings: Device registration");
            break;
    }
}

void DisplayManager::drawBackground() {
    tft.fillScreen(COLOR_BLACK);
}

void DisplayManager::drawTabs() {
    int tabWidth = DISPLAY_WIDTH / TAB_COUNT;
    int tabHeight = 40;
    
    const char* tabNames[] = {"SENSORS", "MANUAL", "SETTINGS"};
    
    for (int i = 0; i < TAB_COUNT; i++) {
        int x = i * tabWidth;
        uint16_t bgColor = (i == currentTab) ? mainColor : COLOR_BLACK;
        uint16_t textColor = (i == currentTab) ? COLOR_BLACK : mainColor;
        
        // Draw tab background
        tft.fillRect(x, 0, tabWidth - 1, tabHeight, bgColor);
        tft.drawRect(x, 0, tabWidth, tabHeight, mainColor);
        
        // Draw tab text (centered)
        tft.setTextColor(textColor, bgColor);
        tft.setTextSize(2);
        int textWidth = strlen(tabNames[i]) * 12;  // Approximate width
        int textX = x + (tabWidth - textWidth) / 2;
        int textY = (tabHeight - 16) / 2;
        tft.setCursor(textX, textY);
        tft.print(tabNames[i]);
    }
}

void DisplayManager::drawTabContent() {
    // Clear content area (below tabs)
    tft.fillRect(0, 40, DISPLAY_WIDTH, DISPLAY_HEIGHT - 40, COLOR_BLACK);
    
    switch (currentTab) {
        case TAB_SENSORS:
            drawSensorsTab();
            break;
        case TAB_MANUAL:
            drawManualTab();
            break;
        case TAB_SETTINGS:
            drawSettingsTab();
            break;
    }
}

void DisplayManager::drawSensorsTab() {
    int startY = 60;
    int lineHeight = 30;
    
    // Draw title
    drawTerminalText(10, startY, "SENSOR READINGS:", mainColor);
    startY += lineHeight + 10;
    
    // Check if data is stale
    unsigned long dataAge = millis() - sensorData.lastUpdate;
    bool dataStale = (dataAge > UART_TIMEOUT_MS);
    
    if (dataStale && sensorData.lastUpdate > 0) {
        drawTerminalText(10, startY, "WARNING: DATA STALE", COLOR_RED);
        startY += lineHeight;
    }
    
    // Draw sensor values
    const char* sensorNames[] = {SENSOR_1_NAME, SENSOR_2_NAME, SENSOR_3_NAME};
    const char* sensorUnits[] = {SENSOR_1_UNIT, SENSOR_2_UNIT, SENSOR_3_UNIT};
    
    for (int i = 0; i < SENSOR_COUNT; i++) {
        String line = String(sensorNames[i]) + ": ";
        
        if (sensorData.valid[i] && !dataStale) {
            line += String(sensorData.values[i], 1) + " " + sensorUnits[i];
            drawTerminalText(10, startY, line, COLOR_WHITE);
        } else {
            line += "-- " + String(sensorUnits[i]);
            drawTerminalText(10, startY, line, COLOR_RED);
        }
        
        startY += lineHeight;
    }
    
    // Connection status
    startY += 10;
    drawTerminalText(10, startY, "STATUS:", mainColor);
    startY += lineHeight;
    
    String mainStatus = "Main Device: ";
    mainStatus += systemStatus.mainDeviceConnected ? "CONNECTED" : "DISCONNECTED";
    uint16_t mainStatusColor = systemStatus.mainDeviceConnected ? COLOR_WHITE : COLOR_RED;
    drawTerminalText(20, startY, mainStatus, mainStatusColor);
    startY += lineHeight;
    
    String wifiStatus = "WiFi: ";
    wifiStatus += systemStatus.wifiConnected ? "CONNECTED" : "DISCONNECTED";
    uint16_t wifiStatusColor = systemStatus.wifiConnected ? COLOR_WHITE : COLOR_RED;
    drawTerminalText(20, startY, wifiStatus, wifiStatusColor);
}

void DisplayManager::drawManualTab() {
    int startY = 60;
    int buttonHeight = 50;
    int buttonSpacing = 10;
    int buttonWidth = DISPLAY_WIDTH - 40;
    
    // Draw title
    drawTerminalText(10, startY, "MANUAL CONTROLS:", mainColor);
    startY += 40;
    
    #ifdef DEVICE_TYPE_ENVIRONMENT
        drawButton(20, startY, buttonWidth, buttonHeight, MANUAL_1_NAME);
        startY += buttonHeight + buttonSpacing;
        drawButton(20, startY, buttonWidth, buttonHeight, MANUAL_2_NAME);
    #elif DEVICE_TYPE_LIQUID
        const char* controlNames[] = {
            MANUAL_1_NAME, MANUAL_2_NAME, MANUAL_3_NAME,
            MANUAL_4_NAME, MANUAL_5_NAME, MANUAL_6_NAME
        };
        
        for (int i = 0; i < MANUAL_CONTROL_COUNT; i++) {
            drawButton(20, startY, buttonWidth, buttonHeight, controlNames[i]);
            startY += buttonHeight + buttonSpacing;
        }
    #endif
}

void DisplayManager::drawSettingsTab() {
    int startY = 60;
    int buttonHeight = 40;
    int buttonSpacing = 10;
    int buttonWidth = DISPLAY_WIDTH - 40;
    
    drawTerminalText(10, startY, "SETTINGS:", mainColor);
    startY += 40;
    
    drawButton(20, startY, buttonWidth, buttonHeight, "WiFi Setup");
    startY += buttonHeight + buttonSpacing;
    
    String colorText = "Color: ";
    colorText += (mainColor == COLOR_GREEN) ? "GREEN" : "YELLOW";
    drawButton(20, startY, buttonWidth, buttonHeight, colorText);
    startY += buttonHeight + buttonSpacing;
    
    drawButton(20, startY, buttonWidth, buttonHeight, "Device Registration");
}

void DisplayManager::drawTerminalText(int16_t x, int16_t y, const String& text, uint16_t color) {
    tft.setTextColor(color, COLOR_BLACK);
    tft.setTextSize(2);
    tft.setCursor(x, y);
    tft.print(text);
}

void DisplayManager::drawButton(int16_t x, int16_t y, int16_t w, int16_t h, const String& text, bool pressed) {
    uint16_t bgColor = pressed ? mainColor : COLOR_BLACK;
    uint16_t borderColor = mainColor;
    uint16_t textColor = pressed ? COLOR_BLACK : mainColor;
    
    // Draw button background and border
    tft.fillRect(x, y, w, h, bgColor);
    tft.drawRect(x, y, w, h, borderColor);
    
    // Draw button text (centered)
    tft.setTextColor(textColor, bgColor);
    tft.setTextSize(2);
    int textWidth = text.length() * 12;  // Approximate width
    int textX = x + (w - textWidth) / 2;
    int textY = y + (h - 16) / 2;
    tft.setCursor(textX, textY);
    tft.print(text);
}

void DisplayManager::updateSensorData(const SensorData& data) {
    sensorData = data;
}

void DisplayManager::updateSystemStatus(const SystemStatus& status) {
    systemStatus = status;
}

void DisplayManager::setMainColor(uint16_t color) {
    mainColor = color;
}