#ifndef EChimes_h
#define EChimes_h
#include <Arduino.h>
#include <Settings.h>
#include <Darlington.h>

/**************************************************************************
 *** HARDWARE CONFIGURATION
 **************************************************************************/
static const int NUM_SOLENOIDS = 24;         // The count of connected darlington transistors && solenoids && notes
static const int NUM_NOTES = NUM_SOLENOIDS;
static const int MAX_MIDI_NOTES = 127 + 1;   // The highest MIDI note supported by the note/pitch mapping array 

/* Solenoids
 - static array declaring the darlington transistor object on the specified pin.
 - Note: The order of these pin assignments should be ascending according to chime pitch 
 */
// Darlinton Transistor Array, assigning pins to each [0-23]
static Darlington solenoids[NUM_NOTES] = 
{// Darlington(Pin#)
    Darlington(0), // solenoids[0] means Darlinton obj (transistor) on pin 0,
    Darlington(1),
    Darlington(2),
    Darlington(3),
    Darlington(4),
    Darlington(5),
    Darlington(6),
    Darlington(7),
    Darlington(8),
    Darlington(9),
    Darlington(10),
    Darlington(11),
    Darlington(12),
    Darlington(24),
    Darlington(25),
    Darlington(26),
    Darlington(27),
    Darlington(28),
    Darlington(29),
    Darlington(30),
    Darlington(31),
    Darlington(32),
    Darlington(34),
    Darlington(33),
};

/* NOTE MAPPING: MIDI Note Num -> Solenoid
 - static array using the MIDI note number
 as an index to a pointer to the correct solenoid
*/
static Darlington* NOTE_MAPPING[MAX_MIDI_NOTES] = {
    nullptr
    // Example when filled in...
    // [92] = &solenoids[0],  // MIDI Note 92, PITCH G6 @ 1661.22 HZ!!, first solenoid in array
    // ...
    // [116] = &solenoids[24],
};

enum ECHIMES_MODES {
    INSTRUMENT_ABSOLUTE_PITCH, // DEFAULT. Instrument Mode: Only accepts exactly mapped MIDI notes +/- a pitch shift
    INSTRUMENT_CLOSEST_OCTAVE, //          Instrument Mode: Accepts all notes and plays the note in the closest supported octave
    AMBIENT, // TODO
    MUSIC_BOX // TODO
};

enum ECHIMES_STATUS {
    UNINITIALIZED, 
    READY,
};

class EChimes : public usb_midi_class // inherits GLOBAL instance named "usbMIDI" for MIDIx4 over usb transport protocol
{
    private:
        u_int8_t MIDI_CHANNEL = 1;
        u_int8_t MIDI_CABLE = 0;
        
        ECHIMES_MODES mode = INSTRUMENT_ABSOLUTE_PITCH;

        bool LISTEN_ALL_CHANNELS = true;
        bool LISTEN_ALL_CABLES = true;
        const u_int8_t STARTING_NOTE = 92;
        int PITCH_SHIFT = 12; // Shifts the note range by given semitones, positive or negative
        u_int32_t STRIKE_PULSE_DURATION_US = 12000;

        ECHIMES_STATUS status = UNINITIALIZED;

        friend void implementedNoteOn(byte channel, byte note, byte velocity);

    public:
        EChimes() {
            MIDI_CHANNEL = 1;
            MIDI_CABLE = 0;
            mode = INSTRUMENT_ABSOLUTE_PITCH;
            setNoteMapping(STARTING_NOTE);
            status = READY;
        }
        EChimes(u_int8_t _MIDI_CHANNEL, uint8_t _MIDI_CABLE, ECHIMES_MODES _mode) : MIDI_CHANNEL{_MIDI_CHANNEL}, MIDI_CABLE{_MIDI_CABLE} {
            changeMode(_mode);
            status = READY;
        }

        void changeMode(ECHIMES_MODES newMode) {
            switch(newMode){
                default:
                case INSTRUMENT_ABSOLUTE_PITCH:
                    break;
                case INSTRUMENT_CLOSEST_OCTAVE:
                    break;
                case AMBIENT:
                    break;
                case MUSIC_BOX:
                    break;
            }
            mode = newMode;
        }

        void setNoteMapping(uint8_t startingNote = 92) {
            for (int i = 0; i < NUM_NOTES; i++){
                NOTE_MAPPING[startingNote + i] = &solenoids[i];
            }
        }
        void setNoteMapping(const int midiNotesAscendingOrder[], size_t arrSize) {
            for (size_t i = 0; i < arrSize; i++){
                NOTE_MAPPING[midiNotesAscendingOrder[i]] = &solenoids[i];
            }
        }

        void strikeChime(u_int8_t midiNote) {
            NOTE_MAPPING[midiNote]->set(DarlingtonState::ON, true);
            delayMicroseconds_discardingMIDI(STRIKE_PULSE_DURATION_US);
            NOTE_MAPPING[midiNote]->set(DarlingtonState::OFF, true);
        }

        void sustainChime(void) {} // TODO

        void delayMicroseconds_discardingMIDI(unsigned int usec) 
        {
            elapsedMicros t = 0;
            while (t < usec) {
                read(); // read and ignore incoming USB MIDI
            }
        }

        void process(void) {
            if (status == READY)
                if (!LISTEN_ALL_CHANNELS)
                    read(MIDI_CHANNEL);
                else
                    read();
            else
                Serial.printf("Error: eChimes is in an uninitialized state!");
        }
};
#endif