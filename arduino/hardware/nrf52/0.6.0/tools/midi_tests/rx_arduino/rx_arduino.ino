/*********************************************************************
 This is an example for our nRF52 based Bluefruit LE modules

 Pick one up today in the adafruit shop!

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/
#include <bluefruit.h>
#include <MIDI.h>

BLEMidi blemidi = BLEMidi(256);

// Create a new instance of the Arduino MIDI Library,
// and attach BluefruitLE MIDI as the transport.
MIDI_CREATE_BLE_INSTANCE(blemidi);

int on_count = 0;
int off_count = 0;

void setup()
{
  Serial.begin(115200);

  Serial.println("Bluefruit52 BLEMIDI RX Test");

  Bluefruit.autoConnLed(true);

  Bluefruit.begin();
  Bluefruit.setName("Bluefruit52 MIDI");

  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);

  // Set General Discoverable Mode flag
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);

  // Advertise BLE MIDI Service
  Bluefruit.Advertising.addService(blemidi);

  // Advertise device name in the Scan Response
  Bluefruit.ScanResponse.addName();
  
  // Start Advertising
  Bluefruit.Advertising.start();
  
  Scheduler.startLoop(midiRead);

}

void handleNoteOn(byte channel, byte pitch, byte velocity)
{
  on_count++;
  Serial.print("note on count: ");
  Serial.println(on_count, DEC);
  
}

void handleNoteOff(byte channel, byte pitch, byte velocity)
{
  off_count++;
  Serial.print("note off count: ");
  Serial.println(off_count, DEC);
}

void loop() {
}

void midiRead()
{
  // Don't continue if we aren't connected.
  if (! Bluefruit.connected()) {
    return;
  }

  // Don't continue if the connected device isn't ready to receive messages.
  if (! blemidi.notifyEnabled()) {
    return;
  }

  // read any new MIDI messages
  MIDI.read();
}
