#include <Arduino.h>
#include <Settings.h>
#include <Darlington.h>
#include <EChimes.h>

extern EChimes eChimes;

/*********************************************
 * MIDI Handlers
 ******************************************/

/* Implemented Handlers:
- NoteOn: Decodes MIDI note according to eChimes mode, and strikes appropriate solenoid
*/
void implementedNoteOn(byte channel, byte note, byte velocity) {
    // When using MIDIx4 or MIDIx16, usbMIDI.getCable() can be used
    // to read which of the virtual MIDI cables received this message.
    uint8_t cable = eChimes.getCable();
    Serial.printf("NOTE ON -> Cable: %d, Channel: %d, Note: %d, Velocity: %d\n", cable, channel, note, velocity);

    // Map to correct solenoid based on eChimes mode
    Darlington* solenoid = nullptr;
    switch (eChimes.mode)
    {
    case INSTRUMENT_ABSOLUTE_PITCH:
        note += eChimes.PITCH_SHIFT;
        if (note < MAX_MIDI_NOTES && note >= 0)
            solenoid = NOTE_MAPPING[note];
        break;
    case INSTRUMENT_CLOSEST_OCTAVE:
        note += eChimes.PITCH_SHIFT;
        int higherOrLower = (note <= eChimes.STARTING_NOTE) ? 1 : -1;
        int semitones = (12 * higherOrLower);
        int count = 0;
        do {
            if (NOTE_MAPPING[note] != nullptr || count > NUM_NOTES){
                solenoid = NOTE_MAPPING[note];
                break;
            }
            note += semitones;
            count += 12;
        } while (note < eChimes.STARTING_NOTE || note > eChimes.STARTING_NOTE + NUM_NOTES);
        break;
    }

    if (solenoid == nullptr)
        return;
    else {
        // play note
        Serial.printf("Playing note: %d, Pin: %d\n", note, solenoid->getPin());
        eChimes.strikeChime(note);
    }
}

/* Default Handlers:
- The rest of them largely just print out the incoming bytes in the appropriate formats to the Serial port
*/

void printBytes(const byte *data, unsigned int size) {
  while (size > 0) {
    byte b = *data++;
    if (b < 16) Serial.print('0');
    Serial.print(b, HEX);
    if (size > 1) Serial.print(' ');
    size = size - 1;
  }
}

void defaultNoteOn(byte channel, byte note, byte velocity) {
  // When using MIDIx4 or MIDIx16, usbMIDI.getCable() can be used
  // to read which of the virtual MIDI cables received this message.
  Serial.print("Note On, ch=");
  Serial.print(channel, DEC);
  Serial.print(", note=");
  Serial.print(note, DEC);
  Serial.print(", velocity=");
  Serial.println(velocity, DEC);
}

void defaultNoteOff(byte channel, byte note, byte velocity) {
  Serial.print("Note Off, ch=");
  Serial.print(channel, DEC);
  Serial.print(", note=");
  Serial.print(note, DEC);
  Serial.print(", velocity=");
  Serial.println(velocity, DEC);
}

void defaultAfterTouchPoly(byte channel, byte note, byte velocity) {
  Serial.print("AfterTouch Change, ch=");
  Serial.print(channel, DEC);
  Serial.print(", note=");
  Serial.print(note, DEC);
  Serial.print(", velocity=");
  Serial.println(velocity, DEC);
}

void defaultControlChange(byte channel, byte control, byte value) {
  Serial.print("Control Change, ch=");
  Serial.print(channel, DEC);
  Serial.print(", control=");
  Serial.print(control, DEC);
  Serial.print(", value=");
  Serial.println(value, DEC);
}

void defaultProgramChange(byte channel, byte program) {
  Serial.print("Program Change, ch=");
  Serial.print(channel, DEC);
  Serial.print(", program=");
  Serial.println(program, DEC);
}

void defaultAfterTouchChannel(byte channel, byte pressure) {
  Serial.print("After Touch, ch=");
  Serial.print(channel, DEC);
  Serial.print(", pressure=");
  Serial.println(pressure, DEC);
}

void defaultPitchChange(byte channel, int pitch) {
  Serial.print("Pitch Change, ch=");
  Serial.print(channel, DEC);
  Serial.print(", pitch=");
  Serial.println(pitch, DEC);
}


// This 3-input System Exclusive function is more complex, but allows you to
// process very large messages which do not fully fit within the usbMIDI's
// internal buffer.  Large messages are given to you in chunks, with the
// 3rd parameter to tell you which is the last chunk.  This function is
// a Teensy extension, not available in the Arduino MIDI library.
//
void defaultSystemExclusiveChunk(const byte *data, uint16_t length, bool last) {
  Serial.print("SysEx Message: ");
  printBytes(data, length);
  if (last) {
    Serial.println(" (end)");
  } else {
    Serial.println(" (to be continued)");
  }
}

// This simpler 2-input System Exclusive function can only receive messages
// up to the size of the internal buffer.  Larger messages are truncated, with
// no way to receive the data which did not fit in the buffer.  If both types
// of SysEx functions are set, the 3-input version will be called by usbMIDI.
//
void defaultSystemExclusive(byte *data, unsigned int length) {
  Serial.print("SysEx Message: ");
  printBytes(data, length);
  Serial.println();
}

void defaultTimeCodeQuarterFrame(byte data) {
  static char SMPTE[8]={'0','0','0','0','0','0','0','0'};
  static byte fps=0;
  byte index = data >> 4;
  byte number = data & 15;
  if (index == 7) {
    fps = (number >> 1) & 3;
    number = number & 1;
  }
  if (index < 8 || number < 10) {
    SMPTE[index] = number + '0';
    Serial.print("TimeCode: ");  // perhaps only print when index == 7
    Serial.print(SMPTE[7]);
    Serial.print(SMPTE[6]);
    Serial.print(':');
    Serial.print(SMPTE[5]);
    Serial.print(SMPTE[4]);
    Serial.print(':');
    Serial.print(SMPTE[3]);
    Serial.print(SMPTE[2]);
    Serial.print('.');
    Serial.print(SMPTE[1]);  // perhaps add 2 to compensate for MIDI latency?
    Serial.print(SMPTE[0]);
    switch (fps) {
      case 0: Serial.println(" 24 fps"); break;
      case 1: Serial.println(" 25 fps"); break;
      case 2: Serial.println(" 29.97 fps"); break;
      case 3: Serial.println(" 30 fps"); break;
    }
  } else {
    Serial.print("TimeCode: invalid data = ");
    Serial.println(data, HEX);
  }
}

void defaultSongPosition(uint16_t beats) {
  Serial.print("Song Position, beat=");
  Serial.println(beats);
}

void defaultSongSelect(byte songNumber) {
  Serial.print("Song Select, song=");
  Serial.println(songNumber, DEC);
}

void defaultTuneRequest() {
  Serial.println("Tune Request");
}

void defaultClock() {
  Serial.println("Clock");
}

void defaultStart() {
  Serial.println("Start");
}

void defaultContinue() {
  Serial.println("Continue");
}

void defaultStop() {
  Serial.println("Stop");
}

void defaultActiveSensing() {
  Serial.println("Actvice Sensing");
}

void defaultSystemReset() {
  Serial.println("System Reset");
}

void defaultRealTimeSystem(uint8_t realtimebyte) {
  Serial.print("Real Time Message, code=");
  Serial.println(realtimebyte, HEX);
}

