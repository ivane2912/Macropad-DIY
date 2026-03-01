// ============================================
// MACROPAD DIY V1 - Sketch finale con macro
// Arduino Pro Micro (ATmega32U4)
// ============================================
// PINOUT:
//   Encoder CLK  -> Pin 0 (TX)  (interrupt hardware)
//   Encoder DT   -> Pin 1 (RX)  (interrupt hardware)
//   Encoder SW   -> Pin 14
//   Display SDA  -> Pin 2
//   Display SCL  -> Pin 3
//   Tasti 1-9    -> Pin 4,5,6,7,8,9,15,A0,A1
// ============================================
// MACRO:
//   Tasto 1: Win+Shift+S        (Screenshot)
//   Tasto 2: Ctrl+Shift+Esc     (Task Manager)
//   Tasto 3: Win+L              (Blocca PC)
//   Tasto 4: git status         (+ Invio)
//   Tasto 5: git pull           (+ Invio)
//   Tasto 6: git commit & push  (con spazio per messaggio)
//   Tasto 7: Shift+Alt+F        (Formatta documento)
//   Tasto 8: Ctrl+Shift+F       (Cerca nei file)
//   Tasto 9: Ctrl+/             (Commenta/scommenta)
// ============================================

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <HID-Project.h>
#include <Encoder.h>

// --- SICUREZZA ---
#define STARTUP_DELAY 3000

// --- DISPLAY ---
#define I2C_ADDRESS 0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define HEADER_HEIGHT 12
#define BODY_Y (HEADER_HEIGHT + 2)
#define BODY_HEIGHT (SCREEN_HEIGHT - BODY_Y)

// --- BITMAP SFONDO 83x64 ---
const unsigned char epd_bitmap_logo [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x0e, 0x01, 0x07, 0xe3, 0xf8, 0xfc, 
  0x03, 0x8e, 0x1c, 0x00, 0x00, 0x0e, 0x04, 0x08, 0x12, 0x09, 0x02, 0x01, 0xc0, 0x38, 0x00, 0x00, 
  0x00, 0x00, 0x0f, 0xd3, 0xfd, 0x7a, 0x10, 0xc0, 0x30, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x73, 0xb9, 
  0xce, 0x18, 0xe0, 0x70, 0x40, 0x00, 0x00, 0x00, 0x0f, 0xd3, 0xfd, 0xfa, 0x1d, 0xe0, 0x38, 0xc0, 
  0x00, 0x00, 0x00, 0x09, 0x92, 0xe9, 0x32, 0x0f, 0xe0, 0x7f, 0x80, 0x0c, 0x00, 0x00, 0x07, 0xe1, 
  0xf0, 0xfc, 0x07, 0xf0, 0x7f, 0x80, 0x00, 0x00, 0x00, 0x07, 0xe1, 0xf0, 0x7c, 0x00, 0xf8, 0xff, 
  0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7d, 0xf0, 0x00, 0x00, 0x02, 0x03, 0x87, 
  0xe1, 0xf0, 0x7c, 0x00, 0x3b, 0xe0, 0x00, 0x0e, 0x09, 0x03, 0x88, 0x12, 0x09, 0x02, 0x18, 0x17, 
  0xc0, 0x00, 0x00, 0x3c, 0x83, 0x0b, 0xd3, 0xf9, 0x7a, 0x3c, 0x0f, 0x83, 0xc0, 0x00, 0x7e, 0x80, 
  0x0e, 0x73, 0xb9, 0xce, 0x18, 0x1f, 0x41, 0x80, 0x00, 0xfc, 0x00, 0x0f, 0xd3, 0xfd, 0xfa, 0x00, 
  0x3e, 0xe0, 0x00, 0x01, 0xf8, 0x00, 0x09, 0xd2, 0xe9, 0x32, 0x00, 0x7d, 0xf0, 0x00, 0x03, 0xf0, 
  0x00, 0x07, 0xe1, 0xf0, 0xfc, 0x03, 0xf8, 0xf8, 0x00, 0x07, 0xe0, 0x00, 0x07, 0xe1, 0xf0, 0xfc, 
  0x0f, 0xf0, 0x7f, 0x00, 0x0f, 0xc1, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xe0, 0x3f, 0x80, 0x1f, 
  0x87, 0x87, 0x00, 0x00, 0x00, 0x00, 0x19, 0xe0, 0x19, 0x80, 0x1f, 0x07, 0x87, 0x08, 0x12, 0x09, 
  0x02, 0x10, 0xe2, 0x18, 0xc0, 0x3e, 0x03, 0xff, 0x0f, 0xd3, 0xf9, 0x7a, 0x00, 0x67, 0x18, 0xc0, 
  0x1c, 0x00, 0xfe, 0x0e, 0x73, 0xbd, 0xce, 0x00, 0xc3, 0x0f, 0x80, 0x20, 0x00, 0x40, 0x0f, 0xd3, 
  0xf9, 0xfa, 0x01, 0x83, 0x07, 0x00, 0x40, 0x06, 0x04, 0x09, 0xd2, 0xe9, 0x32, 0x00, 0x03, 0x80, 
  0x00, 0x00, 0x02, 0x09, 0x03, 0xe1, 0xf0, 0xfc, 0x00, 0x01, 0x80, 0x00, 0x80, 0x00, 0x84, 0x07, 
  0xe3, 0xf8, 0xfc, 0x00, 0x01, 0xe0, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 
  0xf0, 0x00, 0x49, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x03, 0xf0, 0x00, 0x20, 0x07, 0xfe, 
  0x00, 0x00, 0x00, 0x00, 0x1c, 0x01, 0xf0, 0x00, 0x32, 0x0f, 0x0f, 0x0f, 0xff, 0x7d, 0xe6, 0x1c, 
  0x18, 0x00, 0x00, 0x00, 0x0f, 0x0f, 0x0f, 0xff, 0xfd, 0xfe, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x07, 
  0xfc, 0x0f, 0x87, 0x3b, 0x4e, 0x08, 0x66, 0x00, 0x00, 0x00, 0x01, 0xf0, 0x0f, 0x77, 0xdb, 0x2e, 
  0x00, 0x66, 0x02, 0x80, 0x02, 0x00, 0x04, 0x0e, 0x77, 0xba, 0xfe, 0x00, 0x7c, 0x07, 0x80, 0x00, 
  0x03, 0xf8, 0x0b, 0x77, 0xfd, 0xfe, 0x00, 0x3e, 0xd3, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x76, 0xff, 
  0xfe, 0x00, 0x03, 0xf0, 0x00, 0x00, 0x82, 0x00, 0x0f, 0xff, 0xff, 0xfe, 0x00, 0x03, 0x30, 0x00, 
  0x0d, 0x00, 0x04, 0x0b, 0xfe, 0xff, 0xf2, 0x18, 0x07, 0x38, 0x00, 0x00, 0x0c, 0x00, 0x0d, 0xdc, 
  0x7f, 0xfe, 0x24, 0x43, 0x30, 0x00, 0x00, 0x0c, 0x03, 0x0e, 0xff, 0xfb, 0x72, 0x18, 0xe3, 0xf0, 
  0x00, 0x00, 0x07, 0xfc, 0x0f, 0xff, 0xbb, 0x36, 0x01, 0xb0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x0f, 
  0xdf, 0xfb, 0x7e, 0x01, 0xa0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x7f, 0xfc, 0xfe, 0x00, 0xe0, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
#define LOGO_WIDTH 83
#define LOGO_HEIGHT 64

// --- ENCODER ---
Encoder myEnc(0, 1);
const int encoderButton = 14;
long oldPosition = 0;

// --- 9 TASTI ---
const int keyPins[9] = {4, 5, 6, 7, 8, 9, 15, A0, A1};
int lastKeyState[9] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};

// Etichette per il display (riga 1 e riga 2)
const char* keyLabel1[] = {"Screenshot", "Task Mgr", "Apri CMD", "git status", "git pull", "git commit", "Format Doc", "Cerca File", "Commenta"};
const char* keyLabel2[] = {"Win+Sh+S", "Ctrl+Sh+Esc", "Win+R > cmd", "", "", "& push", "Sh+Alt+F", "Ctrl+Sh+F", "Ctrl+/"};

// --- TIMING ---
unsigned long lastVolumeChange = 0;
unsigned long lastMutePress = 0;
unsigned long lastButtonPress = 0;
unsigned long lastActivityTime = 0;
unsigned long lastDisplayRefresh = 0;

const unsigned long volumeCooldown = 50;
const unsigned long muteCooldown = 400;
const unsigned long buttonCooldown = 200;
const unsigned long idleTimeout = 2000;
const unsigned long displayRefreshInterval = 1000;

// --- ANTI-FLOOD ---
int volumeCommandCount = 0;
unsigned long volumeCountReset = 0;
const int maxVolPerSecond = 5;

// --- VOLUME ---
int virtualVolume = 50;
const int VOL_MIN = 0;
const int VOL_MAX = 100;
const int VOL_STEP = 2;

// --- MUTE ---
bool isMuted = false;

// --- OROLOGIO ---
int clockHours = 0;
int clockMinutes = 0;
int clockSeconds = 0;
unsigned long lastClockSync = 0;
bool clockValid = false;

// --- STATO DISPLAY ---
enum ScreenState {
  SCREEN_HOME,
  SCREEN_VOLUME,
  SCREEN_MUTE,
  SCREEN_BUTTON
};
ScreenState currentScreen = SCREEN_HOME;
bool screenNeedsUpdate = true;
int lastButtonIndex = -1;

// --- SERIAL BUFFER ---
char serialBuffer[20];
int serialIndex = 0;

// ============================================
// FUNZIONE SICURA: premi combo e rilascia tutto
// Ogni macro passa da qui per garantire il rilascio
// ============================================
void safeKeyCombo2(uint8_t k1, uint8_t k2) {
  Keyboard.press(k1);
  Keyboard.press(k2);
  delay(20);
  Keyboard.releaseAll();
  delay(20);
}

void safeKeyCombo3(uint8_t k1, uint8_t k2, uint8_t k3) {
  Keyboard.press(k1);
  Keyboard.press(k2);
  Keyboard.press(k3);
  delay(20);
  Keyboard.releaseAll();
  delay(20);
}

void safeTypeText(const char* text) {
  Keyboard.print(text);
  delay(20);
  Keyboard.releaseAll();
  delay(20);
}

void safeTypeEnter() {
  Keyboard.press(KEY_RETURN);
  delay(20);
  Keyboard.releaseAll();
  delay(20);
}

void safeTypeLeft() {
  Keyboard.press(KEY_LEFT_ARROW);
  delay(20);
  Keyboard.releaseAll();
  delay(20);
}

// ============================================
// SETUP
// ============================================
void setup() {
  delay(STARTUP_DELAY);

  for (int i = 0; i < 9; i++) {
    pinMode(keyPins[i], INPUT_PULLUP);
  }
  pinMode(encoderButton, INPUT_PULLUP);

  Consumer.begin();
  Keyboard.begin();

  myEnc.write(0);
  oldPosition = 0;

  delay(250);
  display.begin(I2C_ADDRESS, true);
  display.setRotation(2);
  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);
  display.display();

  Serial.begin(9600);
  Serial.println(F("MACROPAD_V1_READY"));

  lastActivityTime = millis();
  currentScreen = SCREEN_HOME;
  screenNeedsUpdate = true;
}

// ============================================
// LOOP
// ============================================
void loop() {
  receiveTime();
  updateClock();
  handleEncoder();
  handleEncoderButton();
  handleKeys();
  handleIdleScreen();
  handlePeriodicRefresh();
  updateDisplay();
}

// ============================================
// RICEZIONE ORA
// ============================================
void receiveTime() {
  while (Serial.available() > 0) {
    char c = Serial.read();

    if (c == '\n' || c == '\r') {
      serialBuffer[serialIndex] = '\0';

      // Parsing ora: T:HH:MM:SS
      if (serialIndex >= 9 && serialBuffer[0] == 'T' && serialBuffer[1] == ':') {
        int h = (serialBuffer[2] - '0') * 10 + (serialBuffer[3] - '0');
        int m = (serialBuffer[5] - '0') * 10 + (serialBuffer[6] - '0');
        int s = (serialBuffer[8] - '0') * 10 + (serialBuffer[9] - '0');

        if (h >= 0 && h <= 23 && m >= 0 && m <= 59 && s >= 0 && s <= 59) {
          clockHours = h;
          clockMinutes = m;
          clockSeconds = s;
          lastClockSync = millis();
          clockValid = true;
          screenNeedsUpdate = true;
        }
      }

      // Parsing volume: V:xx o V:xxx
      if (serialIndex >= 3 && serialBuffer[0] == 'V' && serialBuffer[1] == ':') {
        int vol = atoi(&serialBuffer[2]);
        if (vol >= 0 && vol <= 100) {
          virtualVolume = vol;
          screenNeedsUpdate = true;
        }
      }

      serialIndex = 0;
    } else {
      if (serialIndex < 19) {
        serialBuffer[serialIndex++] = c;
      }
    }
  }
}

// ============================================
// OROLOGIO INTERNO
// ============================================
void updateClock() {
  if (!clockValid) return;

  unsigned long elapsed = millis() - lastClockSync;
  unsigned long totalSeconds = (unsigned long)clockHours * 3600UL +
                               (unsigned long)clockMinutes * 60UL +
                               (unsigned long)clockSeconds +
                               (elapsed / 1000UL);

  totalSeconds = totalSeconds % 86400UL;

  clockHours = totalSeconds / 3600;
  clockMinutes = (totalSeconds % 3600) / 60;
  clockSeconds = totalSeconds % 60;

  if (elapsed >= 1000) {
    lastClockSync = millis();
  }
}

// ============================================
// ENCODER: VOLUME
// ============================================
void handleEncoder() {
  if (millis() - volumeCountReset > 1000) {
    volumeCommandCount = 0;
    volumeCountReset = millis();
  }

  long newPosition = myEnc.read();

  if (abs(newPosition) > 20) {
    myEnc.write(0);
    oldPosition = 0;
    return;
  }

  if (abs(newPosition - oldPosition) >= 4 &&
      millis() - lastVolumeChange > volumeCooldown &&
      volumeCommandCount < maxVolPerSecond) {

    if (newPosition > oldPosition) {
      Consumer.press(MEDIA_VOLUME_UP);
      delay(10);
      Consumer.release(MEDIA_VOLUME_UP);
      virtualVolume = min(virtualVolume + VOL_STEP, VOL_MAX);
    } else {
      Consumer.press(MEDIA_VOLUME_DOWN);
      delay(10);
      Consumer.release(MEDIA_VOLUME_DOWN);
      virtualVolume = max(virtualVolume - VOL_STEP, VOL_MIN);
    }

    isMuted = false;
    volumeCommandCount++;
    lastVolumeChange = millis();
    lastActivityTime = millis();
    currentScreen = SCREEN_VOLUME;
    screenNeedsUpdate = true;

    myEnc.write(0);
    oldPosition = 0;
  }
}

// ============================================
// MUTE TOGGLE
// ============================================
void handleEncoderButton() {
  if (digitalRead(encoderButton) == LOW &&
      millis() - lastMutePress > muteCooldown) {

    Consumer.press(MEDIA_VOLUME_MUTE);
    delay(10);
    Consumer.release(MEDIA_VOLUME_MUTE);

    isMuted = !isMuted;

    lastMutePress = millis();
    lastActivityTime = millis();
    currentScreen = SCREEN_MUTE;
    screenNeedsUpdate = true;

    while (digitalRead(encoderButton) == LOW) {
      delay(10);
    }
  }
}

// ============================================
// 9 TASTI CON MACRO
// ============================================
void handleKeys() {
  for (int i = 0; i < 9; i++) {
    int reading = digitalRead(keyPins[i]);

    if (reading == LOW && lastKeyState[i] == HIGH &&
        millis() - lastButtonPress > buttonCooldown) {

      executeKey(i);

      lastButtonIndex = i;
      lastButtonPress = millis();
      lastActivityTime = millis();
      currentScreen = SCREEN_BUTTON;
      screenNeedsUpdate = true;
    }
    lastKeyState[i] = reading;
  }
}

void executeKey(int key) {
  Keyboard.releaseAll();
  delay(30);

  switch (key) {
    case 0: // Win + Shift + S
      Keyboard.press(KEY_LEFT_GUI);
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.press('s');
      delay(50);
      Keyboard.releaseAll();
      break;

    case 1: // Ctrl + Shift + Esc
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.press(KEY_ESC);
      delay(50);
      Keyboard.releaseAll();
      break;

    case 2: // Win+R -> cmd -> Invio
      Keyboard.press(KEY_LEFT_GUI);
      Keyboard.press('r');
      delay(50);
      Keyboard.releaseAll();
      delay(500); // aspetta che si apra Esegui
      Keyboard.print("cmd");
      delay(50);
      Keyboard.press(KEY_RETURN);
      delay(30);
      Keyboard.releaseAll();
      break;

    case 3: // git status + Invio
      Keyboard.print("git status");
      delay(50);
      Keyboard.press(KEY_RETURN);
      delay(30);
      Keyboard.releaseAll();
      break;

    case 4: // git pull + Invio
      Keyboard.print("git pull");
      delay(50);
      Keyboard.press(KEY_RETURN);
      delay(30);
      Keyboard.releaseAll();
      break;

    case 5: // git commit & push con cursore tra virgolette
      Keyboard.print("git add . && git commit -m \"\" && git push");
      delay(100);
      // 13 posizioni a sinistra per arrivare tra le virgolette
      for (int j = 0; j < 13; j++) {
        Keyboard.press(KEY_LEFT_ARROW);
        delay(30);
        Keyboard.releaseAll();
        delay(30);
      }
      break;

    case 6: // Shift + Alt + F
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.press(KEY_LEFT_ALT);
      Keyboard.press('f');
      delay(50);
      Keyboard.releaseAll();
      break;

    case 7: // Ctrl + Shift + F
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.press('f');
      delay(50);
      Keyboard.releaseAll();
      break;

    case 8: // Ctrl + /
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press('/');
      delay(50);
      Keyboard.releaseAll();
      break;
  }

  delay(30);
  Keyboard.releaseAll();
}

// ============================================
// IDLE -> HOME
// ============================================
void handleIdleScreen() {
  if (currentScreen != SCREEN_HOME &&
      millis() - lastActivityTime > idleTimeout) {
    currentScreen = SCREEN_HOME;
    screenNeedsUpdate = true;
  }
}

// ============================================
// REFRESH PERIODICO
// ============================================
void handlePeriodicRefresh() {
  if (millis() - lastDisplayRefresh >= displayRefreshInterval) {
    lastDisplayRefresh = millis();
    screenNeedsUpdate = true;
  }
}

// ============================================
// DISPLAY
// ============================================
void updateDisplay() {
  if (!screenNeedsUpdate) return;
  screenNeedsUpdate = false;

  display.clearDisplay();
  drawHeader();
  display.drawLine(0, HEADER_HEIGHT, SCREEN_WIDTH - 1, HEADER_HEIGHT, SH110X_WHITE);

  switch (currentScreen) {
    case SCREEN_HOME:
      drawHomeScreen();
      break;
    case SCREEN_VOLUME:
      drawVolumeBody();
      break;
    case SCREEN_MUTE:
      drawMuteBody();
      break;
    case SCREEN_BUTTON:
      drawButtonBody();
      break;
  }

  display.display();
}

// --- HEADER ---
void drawHeader() {
  display.setTextSize(1);

  display.setCursor(0, 2);
  display.print(F("MACROPAD V1"));

  if (clockValid) {
    char timeStr[6];
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d", clockHours, clockMinutes);
    display.setCursor(SCREEN_WIDTH - 30, 2);
    display.print(timeStr);
  } else {
    display.setCursor(SCREEN_WIDTH - 30, 2);
    display.print(F("--:--"));
  }
}

// --- HOME ---
void drawHomeScreen() {
  int imgX = (SCREEN_WIDTH - LOGO_WIDTH) / 2;
  int imgY = BODY_Y;
  display.drawBitmap(imgX, imgY, epd_bitmap_logo, LOGO_WIDTH, LOGO_HEIGHT, SH110X_WHITE);
}

// --- VOLUME ---
void drawVolumeBody() {
  int bodyCenter = BODY_Y + BODY_HEIGHT / 2;

  display.setTextSize(1);
  display.setCursor(0, BODY_Y + 4);
  display.print(F("Volume: "));
  display.print(virtualVolume);
  display.print(F("%"));

  int barX = 4;
  int barY = bodyCenter - 4;
  int barWidth = 120;
  int barHeight = 16;

  display.drawRect(barX, barY, barWidth, barHeight, SH110X_WHITE);

  int fillWidth = map(virtualVolume, 0, 100, 0, barWidth - 4);
  if (fillWidth > 0) {
    display.fillRect(barX + 2, barY + 2, fillWidth, barHeight - 4, SH110X_WHITE);
  }

  if (virtualVolume >= VOL_MAX) {
    display.setTextSize(1);
    display.setCursor(52, BODY_Y + BODY_HEIGHT - 12);
    display.print(F("MAX"));
  } else if (virtualVolume <= VOL_MIN) {
    display.setTextSize(1);
    display.setCursor(52, BODY_Y + BODY_HEIGHT - 12);
    display.print(F("MIN"));
  }
}

// --- MUTE / UNMUTE ---
void drawMuteBody() {
  int bodyCenter = BODY_Y + BODY_HEIGHT / 2;

  display.setTextSize(2);
  int16_t x1, y1;
  uint16_t w, h;

  if (isMuted) {
    display.getTextBounds("MUTE", 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH - w) / 2, bodyCenter - h - 2);
    display.println(F("MUTE"));

    int cx = SCREEN_WIDTH / 2;
    int cy = bodyCenter + 10;
    display.drawLine(cx - 10, cy - 6, cx + 10, cy + 6, SH110X_WHITE);
    display.drawLine(cx - 10, cy + 6, cx + 10, cy - 6, SH110X_WHITE);
  } else {
    display.getTextBounds("UNMUTE", 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH - w) / 2, bodyCenter - h - 2);
    display.println(F("UNMUTE"));

    int cx = SCREEN_WIDTH / 2;
    int cy = bodyCenter + 10;
    display.fillRect(cx - 8, cy - 4, 6, 8, SH110X_WHITE);
    display.fillTriangle(cx - 2, cy - 8, cx - 2, cy + 8, cx + 8, cy, SH110X_WHITE);
  }
}

// --- TASTO PREMUTO ---
void drawButtonBody() {
  if (lastButtonIndex < 0 || lastButtonIndex > 8) return;

  int bodyCenter = BODY_Y + BODY_HEIGHT / 2;

  // Nome macro grande
  display.setTextSize(2);
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(keyLabel1[lastButtonIndex], 0, 0, &x1, &y1, &w, &h);

  // Se troppo largo per textSize 2, usa textSize 1
  if (w > SCREEN_WIDTH) {
    display.setTextSize(1);
    display.getTextBounds(keyLabel1[lastButtonIndex], 0, 0, &x1, &y1, &w, &h);
  }

  int y_label1 = bodyCenter - 6;
  if (strlen(keyLabel2[lastButtonIndex]) > 0) {
    y_label1 = bodyCenter - 12;
  }

  display.setCursor((SCREEN_WIDTH - w) / 2, y_label1);
  display.print(keyLabel1[lastButtonIndex]);

  // Seconda riga (shortcut) piccola
  if (strlen(keyLabel2[lastButtonIndex]) > 0) {
    display.setTextSize(1);
    display.getTextBounds(keyLabel2[lastButtonIndex], 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH - w) / 2, bodyCenter + 8);
    display.print(keyLabel2[lastButtonIndex]);
  }
}
