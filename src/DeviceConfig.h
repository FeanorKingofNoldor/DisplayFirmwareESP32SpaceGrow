#ifndef DEVICE_CONFIG_H
#define DEVICE_CONFIG_H

// Device type validation
#if !defined(DEVICE_TYPE_ENVIRONMENT) && !defined(DEVICE_TYPE_LIQUID)
    #error "Device type must be defined: DEVICE_TYPE_ENVIRONMENT or DEVICE_TYPE_LIQUID"
#endif

#if defined(DEVICE_TYPE_ENVIRONMENT) && defined(DEVICE_TYPE_LIQUID)
    #error "Only one device type can be defined"
#endif

// Device-specific configuration
#ifdef DEVICE_TYPE_ENVIRONMENT
    #define DEVICE_NAME "AeroEnv Display"
    #define DEVICE_TYPE_STR "environment"
    
    // Sensor configuration
    #define SENSOR_COUNT 3
    #define SENSOR_1_NAME "Temperature"
    #define SENSOR_1_UNIT "°C"
    #define SENSOR_2_NAME "Humidity"
    #define SENSOR_2_UNIT "%"
    #define SENSOR_3_NAME "Air Pressure"
    #define SENSOR_3_UNIT "PSI"
    
    // Manual control configuration
    #define MANUAL_CONTROL_COUNT 2
    #define MANUAL_1_NAME "Lights"
    #define MANUAL_1_CMD "manual_lights"
    #define MANUAL_2_NAME "Spray Cycle"
    #define MANUAL_2_CMD "manual_spray"

#elif DEVICE_TYPE_LIQUID
    #define DEVICE_NAME "AeroLiquid Display"
    #define DEVICE_TYPE_STR "liquid"
    
    // Sensor configuration
    #define SENSOR_COUNT 3
    #define SENSOR_1_NAME "pH Level"
    #define SENSOR_1_UNIT "pH"
    #define SENSOR_2_NAME "EC Level"
    #define SENSOR_2_UNIT "mS/cm"
    #define SENSOR_3_NAME "Water Temp"
    #define SENSOR_3_UNIT "°C"
    
    // Manual control configuration
    #define MANUAL_CONTROL_COUNT 6
    #define MANUAL_1_NAME "Pump 1"
    #define MANUAL_1_CMD "manual_pump"
    #define MANUAL_2_NAME "Pump 2"
    #define MANUAL_2_CMD "manual_pump"
    #define MANUAL_3_NAME "Pump 3"
    #define MANUAL_3_CMD "manual_pump"
    #define MANUAL_4_NAME "Pump 4"
    #define MANUAL_4_CMD "manual_pump"
    #define MANUAL_5_NAME "Pump 5"
    #define MANUAL_5_CMD "manual_pump"
    #define MANUAL_6_NAME "pH/EC Check"
    #define MANUAL_6_CMD "manual_probe"
#endif

// Common configuration
#define UART_BAUD_RATE 115200
#define UART_TIMEOUT_MS 5000

// Display configuration
#define DISPLAY_WIDTH 480
#define DISPLAY_HEIGHT 320
#define TAB_COUNT 3

// Tab definitions
#define TAB_SENSORS 0
#define TAB_MANUAL 1
#define TAB_SETTINGS 2

// Color definitions - classic terminal colors
#define COLOR_GREEN 0x07E0    // Terminal green
#define COLOR_YELLOW 0xFFE0   // Terminal yellow
#define COLOR_RED 0xF800      // Error red
#define COLOR_BLUE 0x001F     // Info blue
#define COLOR_BLACK 0x0000    // Background
#define COLOR_WHITE 0xFFFF    // Text

#endif // DEVICE_CONFIG_H