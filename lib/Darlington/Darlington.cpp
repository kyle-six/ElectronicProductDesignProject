#include <Arduino.h>
#include <Darlington.h>

Darlington::Darlington(uint8_t _relayPin) : relayPin(_relayPin), state(DarlingtonState::OFF)
{
    pinMode(_relayPin, OUTPUT);
    set(_relayPin, DarlingtonState::OFF, false);
}
    
void Darlington::set(DarlingtonState _state, bool fast = true) 
{
    // Ignore repeated state changes
    if (state == _state)
        return;

    if (fast)
        digitalWriteFast(relayPin, _state);
    else
        digitalWrite(relayPin, _state);
    state = _state;
}

void Darlington::set(uint8_t _relayPin, DarlingtonState _state, bool fast = true) 
{
    if (fast)
        digitalWriteFast(_relayPin, _state);
    else
        digitalWrite(_relayPin, _state);
}

bool Darlington::getState(void)
{
    if (state == DarlingtonState::ON)
        return true;
    else
        return false;
}

uint8_t Darlington::getPin(void) {
    return relayPin;
}