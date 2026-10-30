#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <esp_system.h>
#include "ota_support.h"

constexpr const char *FIRMWARE_VERSION = "v1-0.2.3";

// V1: classic ESP-WROOM-32. Not the incoming round ESP32-S3.
Adafruit_SSD1306 display(128, 64, &Wire, -1);
constexpr uint8_t BUTTON_PIN = 0;
bool displayReady = false;
struct Card { const char *title; const char *body; };
const Card cards[] = {
  {"SCOUT", "Ask AI for three\nvery different\napproaches."},
  {"FOG AHEAD", "Ask AI to ask you\none clarifying\nquestion at a time."},
  {"CHECK BEARINGS", "Which claims need\nchecking? Verify\none independently."},
  {"SMALL STEPS", "Ask AI for a test\nyou can finish in\nten minutes."},
  {"NEW PERSPECTIVE", "Ask AI to critique\nthe idea as its\nintended user."},
  {"FALSE SUMMIT", "Ask: How could\nthis fail? Test\none failure mode."},
  {"TRAVEL LIGHT", "Try a fresh chat\nwith only the\nessential context."},
  {"LEAVE MARKERS", "Save a useful\nprompt. Explain\nwhy it helped."},
  {"MAP THE GAPS", "Ask AI to separate\nknown facts from\nits assumptions."},
  {"SHOW THE TARGET", "Give AI an example\nof what a good\nanswer looks like."},
  {"MAKE CAMP", "Summarize progress,\nopen questions,\nand the next step."},
  {"CHANGE COURSE", "Ask AI for the\nstrongest argument\nagainst your plan."}
};
constexpr size_t CARD_COUNT = sizeof(cards) / sizeof(cards[0]);
uint8_t deck[CARD_COUNT];
size_t nextCard = CARD_COUNT;
int lastCard = -1;
bool rawButton = HIGH, stableButton = HIGH;
uint32_t changedAt = 0;

void shuffleDeck() {
  for (size_t i = 0; i < CARD_COUNT; ++i) deck[i] = i;
  for (size_t i = CARD_COUNT - 1; i > 0; --i) {
    size_t j = random(i + 1);
    uint8_t swap = deck[i]; deck[i] = deck[j]; deck[j] = swap;
  }
  if (deck[0] == lastCard) {
    uint8_t swap = deck[0]; deck[0] = deck[1]; deck[1] = swap;
  }
  nextCard = 0;
}

void drawCard() {
  if (nextCard == CARD_COUNT) shuffleDeck();
  lastCard = deck[nextCard++];
  if (displayReady) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print(cards[lastCard].title);
    display.drawLine(0, 14, 127, 14, SSD1306_WHITE);
    display.setCursor(0, 20);
    display.print(cards[lastCard].body);
    display.setCursor(0, 56);
    display.print("BOOT: next bearing");
    display.display();
  }
  Serial.println(cards[lastCard].title);
  Serial.println(cards[lastCard].body);
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  randomSeed(esp_random());
  Serial.println("TrailGuide: press BOOT to draw advice.");
  Serial.printf("Firmware: %s\n", FIRMWARE_VERSION);
  beginUpdates();
  Wire.begin(21, 22);
  uint8_t address = 0;
  const uint8_t addresses[] = {0x3C, 0x3D};
  for (uint8_t candidate : addresses) {
    Wire.beginTransmission(candidate);
    if (Wire.endTransmission() == 0) { address = candidate; break; }
  }
  if (!address) {
    Serial.println("OLED not found at 0x3C or 0x3D. Check wiring. Serial-only mode.");
    return;
  }
  if (!display.begin(SSD1306_SWITCHCAPVCC, address, true, false)) {
    Serial.println("OLED framebuffer allocation failed. Serial-only mode.");
    return;
  }
  displayReady = true;
  Serial.printf("OLED address: 0x%02X\n", address);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setTextWrap(false);
  display.clearDisplay();
  display.setCursor(0, 0); display.print("FRONTIER OPS");
  display.setCursor(0, 20); display.print("Trail Guide's Oracle");
  display.setCursor(0, 36); display.print("Find your next\nbearing.");
  display.setCursor(0, 56); display.print("Press BOOT to begin");
  display.display();
}

void loop() {
  serviceUpdates();
  bool reading = digitalRead(BUTTON_PIN);
  if (reading != rawButton) { rawButton = reading; changedAt = millis(); }
  if (millis() - changedAt >= 35 && stableButton != rawButton) {
    stableButton = rawButton;
    if (stableButton == LOW) drawCard();
  }
  delay(1);
}
