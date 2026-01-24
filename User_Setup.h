// ============================================================================
// TFT_eSPI User Setup for TTGO T-Display ESP32
// Board: TTGO T-Display / HiLetgo ESP32 LCD WiFi Kit
// Display: ST7789V 1.14" IPS TFT (135x240 pixels, portrait mode)
// ============================================================================

#define ST7789_DRIVER
#define TFT_WIDTH  135
#define TFT_HEIGHT 240

// Pin configuration for TTGO T-Display
#define TFT_MOSI 19
#define TFT_SCLK 18
#define TFT_CS    5
#define TFT_DC   16
#define TFT_RST  23
#define TFT_BL    4

// Display settings
#define TFT_RGB_ORDER TFT_RGB
#define TFT_INVERSION_ON

// SPI frequency
#define SPI_FREQUENCY  40000000
#define SPI_READ_FREQUENCY  20000000

// Font loading
#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF
#define SMOOTH_FONT
