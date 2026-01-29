#include <Arduino.h>
#include <BleMouse.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <Preferences.h>

// Bluetooth Configs
#define X_RANDOM_RANGE 3
#define Y_RANDOM_RANGE 3
#define JIGGLE_STEP_INTERVAL 50
#define JIGGLE_MIN_DISTANCE 1        // Smaller movements (1-5 pixels)
#define JIGGLE_MAX_DISTANCE 5
#define JIGGLE_TIME_VARIANCE 20000   // +/- 20 seconds random timing variance (in milliseconds)
#define WHEEL_SCROLL_CHANCE 50      // 50% chance to include wheel scroll
#define WHEEL_MIN_SCROLL 1          // Minimum scroll amount
#define WHEEL_MAX_SCROLL 3          // Maximum scroll amount
#define INTERVAL_LIST { 60, 90, 180, 300, 600, 900 }
#define DEFAULT_INTERVAL 2
#define NUM_CHANNELS 3

// Display Configs
#define DISPLAY_UPDATE_INTERVAL 1000  // Display refresh rate (milliseconds)

// Button Configs
#define BUTTON_UP 0
#define BUTTON_DOWN 35
#define DEBOUNCE_DELAY 250
#define LONG_PRESS 1000

// Backlight pin for TTGO T-Display
#ifndef TFT_BL
#define TFT_BL 4
#endif

// Types
#define BUTTON_NONE 0
#define BUTTON_PRESS 1
#define BUTTON_LONGPRESS 2

struct ButtonState {
    bool init;
    u_int64_t pressed;
    u_int64_t longpress;
    u_int64_t released;
};

// Initialize Bluetooth
BleMouse bleMouse("Logitech M510", "Logitech", 100);

// Initialize Display
TFT_eSPI display;

// Initialize preferences from flash
Preferences preferences;

// --> Functions

void moveMouse()
{
    // Mouse cursor movement
    int distance = random(JIGGLE_MIN_DISTANCE, JIGGLE_MAX_DISTANCE);
    int x = random(X_RANDOM_RANGE) - 1;
    int y = random(Y_RANDOM_RANGE) - 1;

    for (int i = 0; i < distance; i++)
    {
        bleMouse.move(x, y);
        delay(JIGGLE_STEP_INTERVAL);
    }

    for (int i = 0; i < distance; i++)
    {
        bleMouse.move(0 - x, 0 - y);
        delay(JIGGLE_STEP_INTERVAL);
    }

    // Random wheel scroll (50% chance)
    if (random(100) < WHEEL_SCROLL_CHANCE)
    {
        int scrollAmount = random(WHEEL_MIN_SCROLL, WHEEL_MAX_SCROLL + 1);
        int scrollDirection = (random(2) == 0) ? 1 : -1;  // Random up or down
        int totalScroll = scrollAmount * scrollDirection;

        // Scroll in one direction
        for (int i = 0; i < scrollAmount; i++)
        {
            bleMouse.move(0, 0, scrollDirection);
            delay(JIGGLE_STEP_INTERVAL);
        }

        delay(100);  // Brief pause at scroll peak

        // Scroll back to original position
        for (int i = 0; i < scrollAmount; i++)
        {
            bleMouse.move(0, 0, -scrollDirection);
            delay(JIGGLE_STEP_INTERVAL);
        }
    }
}

short buttonState(int pin, unsigned long now, ButtonState *buttonState)
{
    if (buttonState->init == false && digitalRead(pin) == HIGH)
    {
        // first unpressed state registered
        buttonState->init = true;
    }
    else if(buttonState->init == false)
    {
        // button was pressed before firmware started, ignore
        return BUTTON_NONE;
    }

    if (buttonState->pressed == 0 && digitalRead(pin) == LOW)
    {
        // button pressed
        buttonState->pressed = millis();
    }
    else if (buttonState->pressed > 0 && buttonState->released == 0 && buttonState->longpress == 0 && digitalRead(pin) == LOW)
    {
        // button still pressed
        if (millis() - buttonState->pressed > LONG_PRESS)
        {
            // it was a long press
            buttonState->longpress = millis();
            return BUTTON_LONGPRESS;
        }
    }
    else if (buttonState->pressed > 0 && buttonState->released == 0 && digitalRead(pin) == HIGH)
    {
        // button was released
        buttonState->released = millis();
        if (buttonState->longpress == 0)
        {
            // no longpress recorded, so it's a short press
            return BUTTON_PRESS;
        }
    }
    else if (buttonState->pressed > 0 && buttonState->released > 0 && millis() - buttonState->released > DEBOUNCE_DELAY)
    {
        // button released, debounce time expired
        buttonState->pressed = 0;
        buttonState->released = 0;
        buttonState->longpress = 0;
    }

    return BUTTON_NONE;
}

// --> Global State Variables

// Bluetooth & Connection State
unsigned short bluetoothChannelOffset = 0;
bool running = true;
bool connected = false;
bool newConnectState = false;

// Timing & Jiggle State
unsigned long now = 0;
unsigned long lastJiggle;
int nextJiggleDiff;
int intervals[] = INTERVAL_LIST;
size_t numIntervals = sizeof(intervals) / sizeof(intervals[0]);
int current_interval;
int jiggle_interval;
unsigned long jiggleCount = 0;  // Total number of jiggles since boot

// Button State Variables
ButtonState buttonStateTop;
ButtonState buttonStateBottom;
short buttonResult;

// Display State Variables
unsigned long lastDisplayUpdate = 0;
bool dirty = true;
bool fullRedraw = true;  // Flag for full screen redraw vs partial update
int lastDisplayedSeconds = -1;  // Track last displayed countdown to avoid unnecessary redraws
int lastProgress = -1;  // Track last progress bar value
char s [22];  // String buffer for display formatting

// Display Animation Variables
char animation[] = { '◜', '◝', '◞', '◟' };  // Circular quadrant animation
size_t numAnimations = sizeof(animation) / sizeof(animation[0]);
int8_t i_animation = 0;
int8_t i_rainbow = 0;  // Rainbow color index for spinner
uint16_t rainbowColors[] = { TFT_RED, TFT_ORANGE, TFT_YELLOW, TFT_GREEN, TFT_CYAN, TFT_BLUE, TFT_MAGENTA };
size_t numRainbowColors = sizeof(rainbowColors) / sizeof(rainbowColors[0]);

void setup()
{
    // Serial
    Serial.begin(115200);
    Serial.println();
    Serial.println();

    // Preferences
    preferences.begin("app", false);
    current_interval = preferences.getShort("intv", DEFAULT_INTERVAL);
    jiggle_interval = intervals[current_interval] * 1000;
    running = preferences.getBool("isrunning", true);

    // mac address
    // https://generate.plus/en/address/mac
    // Logitech Inc
    bluetoothChannelOffset = preferences.getUShort("macoffset", 0);
    uint8_t macoffset = 0xAE + bluetoothChannelOffset;

    // Original mac configuration
    //uint8_t new_mac[6] = { 0xEC, 0x81, 0x93, 0x37, macoffset, 0xCB };
    
    // Matches Logitech M510
    uint8_t new_mac[6] = { 0x00, 0x1F, 0x20, 0x37, macoffset, 0xCB };
    esp_base_mac_addr_set(new_mac);

    // Button pins
    pinMode(BUTTON_UP, INPUT_PULLUP);
    pinMode(BUTTON_DOWN, INPUT_PULLUP);

    // Bluetooth
    bleMouse.begin();

    // Display backlight
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);  // Turn on backlight

    // Display
    display.init();
    display.setRotation(1);  // Rotate to landscape (becomes 240x135)
    display.fillScreen(TFT_BLACK);
    display.setTextSize(3);  // Bigger text size
    display.setTextColor(TFT_WHITE);

    // Boot message
    display.setCursor(5, 10);
    display.print("Mouse");
    display.setCursor(5, 40);
    display.print("Jiggler");
    display.setTextSize(2);
    display.setCursor(5, 80);
    display.print("Starting...");
    unsigned long bootStart = millis();
    int8_t bootAnim = 0;
    while (millis() - bootStart < 2000)
    {
        display.setTextSize(2);
        display.setCursor(200, 80);
        display.print(animation[bootAnim]);
        bootAnim = (bootAnim + 1) % numAnimations;
        delay(200);
    }

    // Initialize random seed for varied mouse movement patterns
    randomSeed(esp_random());

    lastJiggle = millis();
}

void loop()
{
    now = millis();

    if (connected && running && now - lastDisplayUpdate > DISPLAY_UPDATE_INTERVAL)
    {
        dirty = true;
    }

    buttonResult = buttonState(BUTTON_UP, now, &buttonStateTop);
    if (buttonResult == BUTTON_PRESS)
    {
        running = !running;
        dirty = true;
        fullRedraw = true;  // State changed, need full redraw
        lastJiggle = now;

        preferences.putBool("isrunning", running);
    }

    buttonResult = buttonState(BUTTON_DOWN, now, &buttonStateBottom);
    if (buttonResult == BUTTON_PRESS)
    {
        dirty = true;
        fullRedraw = true;  // Interval changed, need full redraw
        current_interval = (current_interval + 1) % numIntervals;
        jiggle_interval = intervals[current_interval] * 1000;
        lastJiggle = now;
        preferences.putShort("intv", current_interval);
    }
    else if (buttonResult == BUTTON_LONGPRESS)
    {
        preferences.putUShort("macoffset", (preferences.getUShort("macoffset", 0) + 1) % NUM_CHANNELS);

        // Restart required to apply new Bluetooth MAC address
        ESP.restart();
    }

    newConnectState = bleMouse.isConnected();
    if (newConnectState != connected)
    {
        connected = newConnectState;
        jiggle_interval = intervals[current_interval] * 1000;
        dirty = true;
        fullRedraw = true;  // Connection state changed, need full redraw

        if (!connected)
        {
            // Restart ESP to reliably reconnect BLE
            ESP.restart();
        }
    }

    nextJiggleDiff = jiggle_interval - (now - lastJiggle);

    if (dirty)
    {
        display.setTextSize(3);  // Bigger text throughout

        if (fullRedraw)
        {
            // Full screen redraw - clear everything
            display.fillScreen(TFT_BLACK);

            // Draw status
            display.setCursor(5, 5);
            if (connected)
            {
                if (running)
                {
                    display.setTextColor(TFT_GREEN);
                    display.print("Jiggle");
                }
                else
                {
                    display.setTextColor(TFT_YELLOW);
                    display.print("Paused");
                }
            }
            else
            {
                display.setTextColor(TFT_RED);
                display.print("Wait");
            }

            // Draw static labels and jiggle count
            display.setTextColor(TFT_WHITE);
            display.setCursor(5, 105);
            sprintf (s, "J:%-3lu I:%-3d C:%d", jiggleCount, intervals[current_interval], bluetoothChannelOffset);
            display.print(s);

            fullRedraw = false;
            lastDisplayedSeconds = -1;  // Reset to force countdown update
            lastProgress = -1;  // Reset to force progress bar update
        }

        if (connected && running)
        {
            // Update dynamic content only (no screen clear)

            // Rainbow Spinner - cycles through colors
            i_rainbow = (i_rainbow + 1) % numRainbowColors;
            i_animation = (i_animation + 1) % numAnimations;
            display.setTextColor(rainbowColors[i_rainbow], TFT_BLACK);
            display.setCursor(200, 5);
            display.print(animation[i_animation]);
            display.print(" ");  // Print space to clear any trailing pixels

            // Countdown - only update when seconds change, with dynamic color
            int currentSeconds = nextJiggleDiff / 1000;
            if (currentSeconds != lastDisplayedSeconds)
            {
                // Calculate percentage of time remaining
                int percentRemaining = (nextJiggleDiff * 100) / jiggle_interval;

                // Choose color based on time remaining
                uint16_t countdownColor;
                if (percentRemaining > 50)
                    countdownColor = TFT_GREEN;      // Plenty of time
                else if (percentRemaining > 25)
                    countdownColor = TFT_YELLOW;     // Getting close
                else
                    countdownColor = TFT_ORANGE;     // About to jiggle!

                display.setTextColor(countdownColor, TFT_BLACK);  // Dynamic color with background
                display.setCursor(5, 40);
                sprintf (s, "%3ds", currentSeconds);  // Fixed width with space padding
                display.print(s);
                lastDisplayedSeconds = currentSeconds;
            }

            // Progress bar - only update when progress changes, with color gradient
            int barWidth = 220;
            int barHeight = 12;
            int barX = 10;
            int barY = 75;
            long elapsed = jiggle_interval - nextJiggleDiff;
            int progress = (elapsed * barWidth) / jiggle_interval;
            progress = constrain(progress, 0, barWidth);

            if (progress != lastProgress)
            {
                // Calculate percentage complete
                int percentComplete = (elapsed * 100) / jiggle_interval;

                // Choose bar color based on progress
                uint16_t barColor;
                if (percentComplete < 50)
                    barColor = TFT_GREEN;      // First half - green
                else if (percentComplete < 75)
                    barColor = TFT_YELLOW;     // Third quarter - yellow
                else
                    barColor = TFT_ORANGE;     // Final quarter - orange (almost done!)

                // Only redraw bar when progress changes
                display.fillRect(barX, barY, barWidth, barHeight, TFT_BLACK);
                display.drawRect(barX, barY, barWidth, barHeight, TFT_WHITE);
                display.fillRect(barX, barY, progress, barHeight, barColor);
                lastProgress = progress;
            }
        }

        dirty = false;
        lastDisplayUpdate = now;
    }

    if (connected && running && nextJiggleDiff <= 0)
    {
        // Add random timing variance (+/- 5 seconds) to make timing less predictable
        int timeVariance = random(-JIGGLE_TIME_VARIANCE, JIGGLE_TIME_VARIANCE + 1);
        lastJiggle = now - timeVariance;
        moveMouse();
        jiggleCount++;
        fullRedraw = true;  // Force redraw to update jiggle count
        dirty = true;
    }
}
