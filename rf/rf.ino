#include "shared/address.h"
#include "shared/protocol.h"

#include "pulse_signal_interpreter.h"

#include <util/atomic.h>

volatile float currentThrottle = 0.f;
volatile float currentWheel = 0.f;

namespace Pins {
    constexpr int kWheel = 5;
    constexpr int kThrottle = 6;
    constexpr int kToggle = 7;
    constexpr int kButton = 8;
    constexpr int kDebugLed = LED_BUILTIN;
} // namespace Pins

PulseSignalInterpreter steeringSignalInterpreter(
    Pins::kWheel,
    1632,
    50,
    400);
PulseSignalInterpreter throttleSignalInterpreter(
    Pins::kThrottle,
    1250,
    50,
    400);

void OnI2CRequest() {
    Protocol::Radio radioDto;
    static_assert(sizeof(radioDto) == 8);

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        radioDto.throttle = currentThrottle;
        radioDto.wheel = currentWheel;
    }

    Protocol::SendPackage(radioDto);
}

void Update() {
    int togglePulse = pulseIn(Pins::kToggle, HIGH);
    int buttonPulse = pulseIn(Pins::kButton, HIGH);

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        steeringSignalInterpreter.Update();
    }

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        throttleSignalInterpreter.Update();
    }

    bool validData =
        steeringSignalInterpreter.IsValid() &&
        throttleSignalInterpreter.IsValid();

    if (validData) {
        float wheelSignal = steeringSignalInterpreter.GetValue();
        float throttleSignal = throttleSignalInterpreter.GetValue();

        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            currentThrottle = throttleSignal;
            currentWheel = wheelSignal;
        }

        digitalWrite(Pins::kDebugLed, LOW);
    } else {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            currentThrottle = 0.f;
            currentWheel = 0.f;
        }

        digitalWrite(Pins::kDebugLed, HIGH);
    }
}

void setup() {
    Wire.begin(Address::kRadio);
    Wire.onRequest(OnI2CRequest);

    pinMode(Pins::kToggle, INPUT);
    pinMode(Pins::kButton, INPUT);

    steeringSignalInterpreter.Init();
    throttleSignalInterpreter.Init();

    pinMode(Pins::kDebugLed, OUTPUT);
}

void loop() {
    Update();
}
