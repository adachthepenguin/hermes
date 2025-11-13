#include "shared/address.h"
#include "shared/protocol.h"

#include <Servo.h>

namespace Pins {
    constexpr int kServo = 9;
    constexpr int kDebugLed = LED_BUILTIN;
}

constexpr unsigned long kHeartbeatExpirationTime = 500ul;
volatile unsigned long heartbeatExpirationTimestamp;
Servo steeringServo;

void OnI2CReceive() {
    Protocol::SteeringInstruction steeringInstructionDto;
    static_assert(sizeof(steeringInstructionDto) == 4);

    if (Protocol::ReadPackage(steeringInstructionDto)) {
        float angle = 90.f - steeringInstructionDto.steering * 40.f;
        steeringServo.write(angle);
        heartbeatExpirationTimestamp = millis() + kHeartbeatExpirationTime;
    }
}

void setup() {
    Wire.begin(Address::kSteering);
    Wire.onReceive(OnI2CReceive);

    steeringServo.attach(Pins::kServo);

    heartbeatExpirationTimestamp = 0ul;

    pinMode(Pins::kDebugLed, OUTPUT);
}

void loop() {
    if (millis() > heartbeatExpirationTimestamp) {
        digitalWrite(Pins::kDebugLed, HIGH);
    } else {
        digitalWrite(Pins::kDebugLed, LOW);
    }
}
