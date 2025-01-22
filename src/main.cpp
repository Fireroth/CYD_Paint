#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>

#define XPT2046_IRQ 36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK 25
#define XPT2046_CS 33

// You might have to change these values depending on your device
#define TS_MINX 135
#define TS_MAXX 3730
#define TS_MINY 220
#define TS_MAXY 3830

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

SPIClass mySpi(VSPI);
TFT_eSPI tft = TFT_eSPI();
XPT2046_Touchscreen ts(XPT2046_CS, XPT2046_IRQ);

uint16_t colors[] = {TFT_RED, TFT_GREEN, TFT_BLUE, TFT_YELLOW, TFT_CYAN, TFT_MAGENTA, TFT_WHITE, TFT_BLACK};
int currentColorIndex = 0;

#define BUTTON_WIDTH 60
#define BUTTON_HEIGHT 30
#define BUTTON_X 0
#define BUTTON_Y (SCREEN_HEIGHT - BUTTON_HEIGHT)

void setup() {
  Serial.begin(115200);

  // Initialize SPI for touchscreen
  mySpi.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  ts.begin(mySpi);
  ts.setRotation(1);

  // Initialize TFT
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  // Draw color selector boxes
  for (int i = 0; i < 8; i++) {
    tft.fillRect(i * (SCREEN_WIDTH / 8), 0, SCREEN_WIDTH / 8, 20, colors[i]);
  }

  // Draw the "Clear" button
  tft.fillRect(BUTTON_X, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, TFT_WHITE);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("CLEAR", BUTTON_X + BUTTON_WIDTH / 2, BUTTON_Y + BUTTON_HEIGHT / 2);
}

void loop() {
  TS_Point p;

  if (ts.tirqTouched() && ts.touched()) {
    p = ts.getPoint();
    if (p.z < 380) return;

    // Map touch coordinates to screen dimensions
    int x = map(p.x, TS_MINX, TS_MAXX, 0, SCREEN_WIDTH);
    int y = map(p.y, TS_MINY, TS_MAXY, 0, SCREEN_HEIGHT);

    if (y < 20) {
      // Color selector area
      currentColorIndex = x / (SCREEN_WIDTH / 8);
    } else if (x >= BUTTON_X && x <= BUTTON_X + BUTTON_WIDTH && y >= BUTTON_Y && y <= BUTTON_Y + BUTTON_HEIGHT) {
      // Clear screen button area
      tft.fillScreen(TFT_BLACK);

      // Redraw color selector boxes
      for (int i = 0; i < 8; i++) {
        tft.fillRect(i * (SCREEN_WIDTH / 8), 0, SCREEN_WIDTH / 8, 20, colors[i]);
      }

      // Redraw the "Clear Screen" button
      tft.fillRect(BUTTON_X, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, TFT_WHITE);
      tft.setTextColor(TFT_BLACK, TFT_WHITE);
      tft.setTextDatum(MC_DATUM);
      tft.drawString("CLEAR", BUTTON_X + BUTTON_WIDTH / 2, BUTTON_Y + BUTTON_HEIGHT / 2);
    } else {
      // Draw on the screen
      tft.fillCircle(x, y, 3, colors[currentColorIndex]);
    }
  }
}

