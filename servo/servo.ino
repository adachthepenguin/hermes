#include <Servo.h>

namespace Pins {
    const int kServoSignal = 9;
    const int kDebugLed = LED_BUILTIN;
}

Servo servo;

void setup() {
    servo.attach(Pins::kServoSignal);

    pinMode(Pins::kDebugLed, OUTPUT);
}

void loop() {
    digitalWrite(Pins::kDebugLed, HIGH);

    for (int i = 0; i <= 180; i += 5) {
        servo.write(i);
        delay(100);
    }

    for (int i = 180; i >= 0; i -= 5) {
        servo.write(i);
        delay(100);
    }

    digitalWrite(Pins::kDebugLed, LOW);
    servo.write(90);
    delay(5000);
}
