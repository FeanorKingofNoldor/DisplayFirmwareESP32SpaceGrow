// TFT_eSPI User Setup for AeroDisplay ESP32-S3 with 4.3" resistive touch display

#define USER_SETUP_ID 301

// Driver selection - ILI9341 is common for 4.3" displays
#define ILI9341_DRIVER

// ESP32-S3 pin configuration
#define TFT_MISO 12
#define TFT_MOSI 11
#define TFT_SCLK 13
#define TFT_CS   10  // Chip select control pin
#define TFT_DC   14  // Data Command control pin
#define TFT_RST  21  // Reset pin (could connect to RST pin)

// Optional backlight control
#define TFT_BL   48  // LED back-light control pin
#define TFT_BACKLIGHT_ON HIGH  // Level to turn ON back-light (HIGH or LOW)

// Font definitions
#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel high font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel high font, needs ~2438 bytes in FLASH, only characters 1234567890:-.
#define LOAD_FONT8  // Font 8. Large 75 pixel high font needs ~3256 bytes in FLASH, only characters 1234567890:-.
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

// SPI frequency
#define SPI_FREQUENCY  40000000  // 40MHz for good performance
#define SPI_READ_FREQUENCY  20000000
#define SPI_TOUCH_FREQUENCY  2500000

// Touch screen calibration (these will need to be calibrated for your specific display)
#define TOUCH_CS 33     // Chip select pin (T_CS) of touch screen

// Resistive touch screen ADC pins for ESP32-S3
#define XP_PIN 4   // X+ pin
#define XM_PIN 5   // X- pin  
#define YP_PIN 6   // Y+ pin
#define YM_PIN 7   // Y- pin

// Touch pressure threshold
#define MINPRESSURE 10
#define MAXPRESSURE 1000

// For better terminal display
#define SMOOTH_FONT