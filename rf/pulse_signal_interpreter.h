#pragma once

#include <stdint.h>

class PulseSignalInterpreter {
public:
    PulseSignalInterpreter(
        uint8_t signalPin,
        int base,
        int deadZone,
        int range);

    float GetValue() const;

    bool IsValid() const;

    void Init();

    void Update();

private:
    int GetPulse() const;

    uint8_t mSignalPin;
    int mBase;
    int mDeadZone;
    int mRange;

    float mValue;
    bool mIsValid;
};
