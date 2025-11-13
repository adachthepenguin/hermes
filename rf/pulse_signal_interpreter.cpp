#include "pulse_signal_interpreter.h"

#include "shared/helpers.h"

#include <Arduino.h>

PulseSignalInterpreter::PulseSignalInterpreter(
    uint8_t signalPin,
    int base,
    int deadZone,
    int range)
    : mSignalPin(signalPin)
    , mBase(base)
    , mDeadZone(deadZone)
    , mRange(range)
    , mValue(0.f)
    , mIsValid(false) {
}

float PulseSignalInterpreter::GetValue() const {
    return mValue;
}

bool PulseSignalInterpreter::IsValid() const {
    return mIsValid;
}

void PulseSignalInterpreter::Init() {
    pinMode(mSignalPin, INPUT);
}

void PulseSignalInterpreter::Update() {
    auto pulse = GetPulse();
    if (pulse > 0) {
        if (pulse > mBase + mDeadZone) {
            auto clampedPulse = Math::Clamp(pulse, mBase + mDeadZone, mBase + mDeadZone + mRange);
            mValue = static_cast<float>(clampedPulse - mBase - mDeadZone) / mRange;
        } else if (pulse < mBase - mDeadZone) {
            auto clampedPulse = Math::Clamp(pulse, mBase - mDeadZone - mRange, mBase - mDeadZone);
            mValue = static_cast<float>(clampedPulse - mBase + mDeadZone) / mRange;
        } else {
            mValue = 0.f;
        }
        mIsValid = true;
    } else {
        mIsValid = false;
    }
}

int PulseSignalInterpreter::GetPulse() const {
    return pulseIn(mSignalPin, HIGH);
}
