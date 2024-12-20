#ifndef Darlington_h
#define Darlington_h
#include <Arduino.h>

enum DarlingtonState 
{
    OFF = 0,
    ON = 1
};

class Darlington
{
    public:
        Darlington(uint8_t _relayPin);

        void set(DarlingtonState state, bool fast = true);
        static void set(uint8_t _relayPin, DarlingtonState state, bool fast = true);

        bool getState(void);

        uint8_t getPin(void);

    private:
        uint8_t relayPin;
        DarlingtonState state;
};
#endif