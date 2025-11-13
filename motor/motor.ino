#include "shared/address.h"
#include "shared/protocol.h"

namespace Pins {
    constexpr int kLeftPwm = 5;
    constexpr int kRightPwm = 6;
    constexpr int kLeftEnable = 7;
    constexpr int kRightEnable = 8;
    constexpr int kLeftIs = A0;
    constexpr int kRightIs = A1;
    constexpr int kDebugLed = LED_BUILTIN;
}

constexpr unsigned long kHeartbeatExpirationTime = 500ul;
volatile unsigned long heartbeatExpirationTimestamp;

void SetSpeed(float speed) {
    if (speed > 0.f) {
        int pwm = 255 * speed;
        digitalWrite(Pins::kLeftEnable, HIGH);
        digitalWrite(Pins::kRightEnable, HIGH);
        // analogWrite(Pins::kLeftPwm, 0);
        // analogWrite(Pins::kRightPwm, pwm);
        analogWrite(Pins::kLeftPwm, pwm);
        analogWrite(Pins::kRightPwm, 0);
    } else if (speed < 0.f) {
        int pwm = -255 * speed;
        digitalWrite(Pins::kLeftEnable, HIGH);
        digitalWrite(Pins::kRightEnable, HIGH);
        // analogWrite(Pins::kLeftPwm, pwm);
        // analogWrite(Pins::kRightPwm, 0);
        analogWrite(Pins::kLeftPwm, 0);
        analogWrite(Pins::kRightPwm, pwm);
    } else {
        digitalWrite(Pins::kLeftEnable, HIGH);
        digitalWrite(Pins::kRightEnable, HIGH);
        analogWrite(Pins::kLeftPwm, 0);
        analogWrite(Pins::kRightPwm, 0);
    }
}

void OnI2CReceive() {
    Protocol::MotorInstruction motorInstructionDto;
    static_assert(sizeof(motorInstructionDto) == 4);

    if (Protocol::ReadPackage(motorInstructionDto)) {
        SetSpeed(motorInstructionDto.throttle);
        heartbeatExpirationTimestamp = millis() + kHeartbeatExpirationTime;
    } else {
        SetSpeed(0.f);
    }
}

void setup() {
    Wire.begin(Address::kMotor);
    Wire.onReceive(OnI2CReceive);

    pinMode(Pins::kLeftPwm, OUTPUT);
    pinMode(Pins::kRightPwm, OUTPUT);
    pinMode(Pins::kLeftEnable, OUTPUT);
    pinMode(Pins::kRightEnable, OUTPUT);
    // pinMode(Pins::kLeftIs, OUTPUT);
    // pinMode(Pins::kRightIs, OUTPUT);

    pinMode(Pins::kDebugLed, OUTPUT);

    heartbeatExpirationTimestamp = 0ul;
}

void loop() {
    if (millis() > heartbeatExpirationTimestamp) {
        SetSpeed(0.f);
        digitalWrite(Pins::kDebugLed, HIGH);
    } else {
        digitalWrite(Pins::kDebugLed, LOW);
    }
}
