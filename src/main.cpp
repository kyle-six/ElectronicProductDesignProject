// Outside Libraries
#include <Arduino.h>

// Project Libraries
#include <EChimes.h>
#include <MIDIHandlers.h>

uint8_t midiChannel = 1;
uint8_t midiCable = 0;

// Create an eChimes object on specified MIDI channel and starting mode
EChimes eChimes(midiChannel, midiCable, ECHIMES_MODES::INSTRUMENT_ABSOLUTE_PITCH);

// Array that maps MIDI note nums -> solenoids 0-23
const int myNoteMapping[NUM_NOTES] = {
    92,
    115,
    93,
    113,
    95,
    111,
    97,
    109,
    99,
    107,
    101,
    105,
    103,
    104,
    102,
    106,
    100,
    108,
    98,
    110,
    96,
    112,
    94,
    114
};

void setup() 
{
    // Begin serial communication (debug)
    Serial.begin(115200);
    // Set linear [Midi Note -> Solenoid] mapping from a starting note
    //eChimes.setNoteMapping(startingNote);
    
    // Set custom [Midi Note -> Solenoid] mapping with pitch array
    eChimes.setNoteMapping(myNoteMapping, NUM_NOTES);

    // Set all the suppored MIDI message handlers
    eChimes.setHandleNoteOn(implementedNoteOn); // Only Note On has been implemented for now!
    eChimes.setHandleNoteOff(defaultNoteOff);
    eChimes.setHandleAfterTouchPoly(defaultAfterTouchPoly);
    eChimes.setHandleControlChange(defaultControlChange);
    eChimes.setHandleProgramChange(defaultProgramChange);
    eChimes.setHandleAfterTouchChannel(defaultAfterTouchChannel);
    eChimes.setHandlePitchChange(defaultPitchChange);
    eChimes.setHandleSystemExclusive(defaultSystemExclusiveChunk);
    eChimes.setHandleSystemExclusive(defaultSystemExclusive); 
    eChimes.setHandleTimeCodeQuarterFrame(defaultTimeCodeQuarterFrame);
    eChimes.setHandleSongPosition(defaultSongPosition);
    eChimes.setHandleSongSelect(defaultSongSelect);
    eChimes.setHandleTuneRequest(defaultTuneRequest);
    eChimes.setHandleClock(defaultClock);
    eChimes.setHandleStart(defaultStart);
    eChimes.setHandleContinue(defaultContinue);
    eChimes.setHandleStop(defaultStop);
    eChimes.setHandleActiveSensing(defaultActiveSensing);
    eChimes.setHandleSystemReset(defaultSystemReset);
    eChimes.setHandleRealTimeSystem(defaultRealTimeSystem);
}

void loop() 
{
    // Process MIDI messages
    eChimes.process();
}

