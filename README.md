# ESP32 Bluetooth Mouse Jiggler

A stealthy ESP32-based Bluetooth mouse jiggler that keeps your screen alive with subtle, randomized movements. Features a vibrant 1.14" color TFT display showing real-time status, countdown timers, and activity tracking.

**Hardware:** Designed for TTGO T-Display ESP32 boards (and compatibles like HiLetgo ESP32 LCD WiFi Kit) with integrated ST7789 1.14" IPS display and built-in buttons - no soldering required!


## Features

### Display
- **1.14" Color TFT Display** with real-time information:
  - Connection status (Jiggling/Standby/Waiting) with color indicators
  - Dynamic countdown timer with color coding (Green → Yellow → Orange as jiggle approaches)
  - Animated progress bar that changes color as it fills
  - Jiggle counter (total jiggles since boot)
  - Current interval setting
  - Bluetooth channel/MAC offset
  - Activity spinner animation

### Controls
- **Left Button (GPIO 0)**: Start/Pause jiggler (short press)
- **Right Button (GPIO 35)**:
  - Short press: Cycle through intervals (60s, 90s, 180s, 300s, 600s, 900s)
  - Long press: Change Bluetooth channel (0-2) for multi-device support

### Mouse Movement
- **Subtle random movements**: 1-5 pixels in random directions
- **Returns to original position**: Cursor ends up exactly where it started
- **Random wheel scrolling** (50% chance): Scrolls 1-3 notches up/down, then returns
- **Timing variance**: ±20 seconds random variance per interval for unpredictable timing
- **Multiple movement patterns**: 9 possible directions (including diagonals)

### Other Features
- Saves all settings to flash (persists across reboots)
- Supports up to 3 different devices with separate Bluetooth MAC addresses
- No soldering required - uses built-in buttons and display
- Undetectable mouse movements
- Simple and reliable

## Known limitations

- If bluetooth connection is lost the ESP is restarted because the BLE library does not perform a proper re-init
- Same for changing the bluetooth MAC address

## Parts

- TTGO T-Display ESP32 (or compatible board with ST7789 1.14" 240x135 TFT display)
  - Example: HiLetgo ESP32 LCD WiFi Kit ESP-32 1.14 Inch LCD
  - Built-in buttons on GPIO 0 and GPIO 35
  - Built-in ST7789V display
  - No additional components needed!

## Setup

- Clone the repo.
- Open the repo with VisualStudio Code with PlatformIO installed.
- Press upload button and wait for upload to finish.
- Power your board on
- Connect to bluetooth device

## Usage

### Pairing
1. Power on the device
2. Look for "ESP32 Bluetooth Mouse" in your computer/phone's Bluetooth settings
3. Pair with the device
4. The display will show "Jiggling" in green when connected and active

### Operation
- **Green status**: Jiggling actively at set interval
- **Yellow status**: Paused (standby mode)
- **Red status**: Waiting for Bluetooth connection

### Display Information
```
┌─────────────────────────────┐
│ Jiggling              /     │  ← Status + Spinner
│ Next:  45s                  │  ← Countdown (color changes)
│ ▓▓▓▓▓▓▓▓▓▓░░░░░░░░░░        │  ← Progress bar (color gradient)
│ J:127  Int:90s  Ch:0        │  ← Jiggle count, Interval, Channel
└─────────────────────────────┘
```

### Multi-Device Setup
If you want to use the jiggler with multiple computers:
1. Connect to first device (Channel 0)
2. Long-press right button to switch to Channel 1
3. Device restarts with new MAC address
4. Connect to second device
5. Repeat for third device (Channel 2)

Each device remembers which channel it's paired with!

## Configuration

You can customize the jiggler behavior by editing values in `src/main.cpp`:

```cpp
// Mouse movement settings
#define JIGGLE_MIN_DISTANCE 1        // Minimum pixels to move (1-5)
#define JIGGLE_MAX_DISTANCE 5        // Maximum pixels to move
#define JIGGLE_TIME_VARIANCE 20000   // ±20 seconds timing randomness (ms)

// Wheel scroll settings
#define WHEEL_SCROLL_CHANCE 50       // 50% chance to scroll each jiggle
#define WHEEL_MIN_SCROLL 1           // Minimum scroll notches (1-3)
#define WHEEL_MAX_SCROLL 3           // Maximum scroll notches

// Available intervals (in seconds)
#define INTERVAL_LIST { 60, 90, 180, 300, 600, 900 }

// Display update rate (in milliseconds)
#define DISPLAY_UPDATE_INTERVAL 1000  // 1 second
```

### Customization Examples:
- **More aggressive movement**: Increase `JIGGLE_MAX_DISTANCE` to 10
- **No wheel scrolling**: Set `WHEEL_SCROLL_CHANCE` to 0
- **Exact timing**: Set `JIGGLE_TIME_VARIANCE` to 0
- **Different intervals**: Modify `INTERVAL_LIST` array

## Credits

- Cloned from https://github.com/perryflynn/mouse-jiggler
- Original code by https://github.com/tornado67/DroChill


## License

[MIT](https://choosealicense.com/licenses/mit/)
