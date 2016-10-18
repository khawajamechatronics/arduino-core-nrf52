#include <BLEHIDPeripheral.h>
#include <BLEMultimedia.h>

// http://www.pjrc.com/teensy/td_libs_Encoder.html
#include <Encoder.h>

#define BUTTON_PIN 5

#define ENC_RIGHT_PIN 3
#define ENC_LEFT_PIN  4

#define INPUT_POLL_INTERVAL 100

#define ANDROID_CENTRAL

BLEHIDPeripheral bleHIDPeripheral = BLEHIDPeripheral();
BLEMultimedia bleMultimedia;

Encoder encoder(ENC_RIGHT_PIN, ENC_LEFT_PIN);

int buttonState;
unsigned long lastInputPollTime = 0;

void setup() {
  Serial.begin(9600);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  buttonState = digitalRead(BUTTON_PIN);

  if (buttonState == LOW) {
    Serial.println(F("BLE HID Peripheral - clearing bond data"));

    // clear bond store data
    bleHIDPeripheral.clearBondStoreData();
  }

  encoder.write(0);

#ifdef ANDROID_CENTRAL
  bleHIDPeripheral.setReportIdOffset(1);
#endif

  bleHIDPeripheral.setLocalName("HID Volume");
  bleHIDPeripheral.addHID(bleMultimedia);

  bleHIDPeripheral.begin();

  Serial.println(F("BLE HID Volume Knob"));
}

void loop() {
  BLECentral central = bleHIDPeripheral.central();

  if (central) {
    // central connected to peripheral
    Serial.print(F("Connected to central: "));
    Serial.println(central.address());

    while (bleHIDPeripheral.connected()) {
      pollInputs();
    }

    // central disconnected
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}

void pollInputs() {
  // only poll the input every 100ms
  if (millis() - lastInputPollTime > INPUT_POLL_INTERVAL) {
    pollButton();

    pollEncoder();

    lastInputPollTime = millis();
  }
}

void pollButton() {
  // check the button
  int tempButtonState = digitalRead(BUTTON_PIN);

  if (tempButtonState != buttonState) {
    buttonState = tempButtonState;

    if (buttonState == LOW) {
      Serial.println(F("Mute"));
      bleMultimedia.write(MMKEY_MUTE);
    }
  }
}

void pollEncoder() {
  // check the encoder
  int encoderState = encoder.read();

  if (encoderState != 0) {
    if (encoderState > 0) {
      Serial.println(F("Volume up"));
      bleMultimedia.write(MMKEY_VOL_UP);
    } else {
      Serial.println(F("Volume down"));
      bleMultimedia.write(MMKEY_VOL_DOWN);
    }

    encoder.write(0);
  }
}