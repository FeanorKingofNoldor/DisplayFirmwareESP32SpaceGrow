# AeroDisplay ESP32 - Universal Display Firmware

Shared display firmware for AeroEnv and AeroLiquid aeroponic monitoring devices.

## Hardware

- **ESP32-S3** development board
- **4.3" TFT Display** (480x320) with resistive touch
- **UART connection** to main device (Environment or Liquid controller)
- **WiFi capability** for setup and OTA updates

## Device Types

Single codebase supports two device configurations:

### Environment Display
- **Sensors:** Temperature (°C), Humidity (%), Air Pressure (PSI)
- **Manual Controls:** Light Toggle, Spray Cycle
- **Build:** `display_environment`

### Liquid Display  
- **Sensors:** pH Level (pH), EC Level (mS/cm), Water Temperature (°C)
- **Manual Controls:** Pump Controls (1-5), pH/EC Check
- **Build:** `display_liquid`

## Build Instructions

### Environment Display
```bash
pio run -e display_environment
pio run -e display_environment -t upload
```

### Liquid Display
```bash
pio run -e display_liquid  
pio run -e display_liquid -t upload
```

## User Interface

**Matrix Terminal Theme:**
- Classic terminal aesthetic with monospace fonts
- Selectable colors: Green or Yellow (main), Red (errors), Blue (info)
- Three-tab navigation: Sensors → Manual → Settings

**Tab Structure:**
1. **SENSORS** - Current readings from main device
2. **MANUAL** - Device-specific control buttons
3. **SETTINGS** - WiFi setup, color scheme, device registration

## Communication Protocol

**UART JSON Protocol** (115200 baud):

### Requests TO Main Device:
```json
{"cmd": "get_sensors"}
{"cmd": "get_status"}
{"cmd": "manual_lights"}        // Environment only
{"cmd": "manual_spray"}         // Environment only  
{"cmd": "manual_pump", "pump": 1}  // Liquid only (pumps 1-5)
{"cmd": "manual_probe"}         // Liquid only
```

### Responses FROM Main Device:
```json
// Environment sensors
{"temp": 23.5, "humidity": 65.2, "air_pressure": 45.3}

// Liquid sensors  
{"ph": 6.2, "ec": 1.8, "water_temp": 22.1}

// Status
{"status": "ok", "wifi_connected": true}
```

## Setup Flow

1. **WiFi Network Scanning** - Display available networks on screen
2. **Touch-based Selection** - Select network and enter password
3. **Device Registration** - Enter activation token from email
4. **Normal Operation** - Display sensor data and provide manual controls

## Architecture

**FreeRTOS Tasks:**
- **DisplayTask** (High Priority) - UI updates and touch input
- **UARTTask** (Medium Priority) - Communication with main device
- **WiFiTask** (Low Priority) - Network management and OTA

**Manager Classes:**
- **DisplayManager** - TFT display and touch interface
- **UARTManager** - JSON protocol communication
- **WiFiManager** - Network connection and device registration
- **StorageManager** - Configuration persistence (LittleFS)

## Configuration

Device-specific settings in `DeviceConfig.h`:
- Sensor names and units
- Manual control commands
- Display layout parameters
- Communication timeouts

## Features

- **Graceful Degradation** - Shows last known values if main device disconnects
- **Touch Debouncing** - Reliable button presses
- **Data Timeout** - Visual indication when sensor data is stale
- **Color Customization** - Runtime color scheme selection
- **WiFi Setup** - No AP mode required, network scanning on display
- **OTA Updates** - Firmware updates with progress display

## Pin Configuration

**TFT Display (SPI):**
- MISO: 12, MOSI: 11, SCLK: 13
- CS: 10, DC: 14, RST: 21, BL: 48

**Resistive Touch:**
- X+: 4, X-: 5, Y+: 6, Y-: 7, CS: 33

**UART to Main Device:**
- RX: 16, TX: 17, Baud: 115200

## Manufacturing

**Single PCB Design:**
- Same hardware for both device types
- Firmware selection during manufacturing
- Cost optimization through shared design
- Simplified inventory and assembly