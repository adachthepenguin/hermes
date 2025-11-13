#include "shared/address.h"
#include "shared/plotter.h"
#include "shared/protocol.h"

#include <LiquidCrystal_I2C.h>

#include <util/atomic.h>

namespace Pins {
    constexpr int kDebugLed = LED_BUILTIN;
} // namespace Pins

LiquidCrystal_I2C lcd(Address::kLCD, 16, 2);

void setup() {
    Plotter::Init();

    Wire.begin();
    Wire.setClock(100000);

    pinMode(Pins::kDebugLed, OUTPUT);

    lcd.init();
    lcd.backlight();
}

void loop() {
    Protocol::Radio radioDto;
    static_assert(sizeof(radioDto) == 8);

    Protocol::MotorInstruction motorInstructionDto;
    static_assert(sizeof(motorInstructionDto) == 4);

    Protocol::SteeringInstruction steeringInstructionDto;
    static_assert(sizeof(steeringInstructionDto) == 4);

    if (Protocol::RequestPackage(Address::kRadio, radioDto)) {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            motorInstructionDto.throttle = radioDto.throttle;
            steeringInstructionDto.steering = radioDto.wheel;
        }

        Protocol::SendPackage(Address::kMotor, motorInstructionDto);
        Protocol::SendPackage(Address::kSteering, steeringInstructionDto);
    } else {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            motorInstructionDto.throttle = 0.f;
            steeringInstructionDto.steering = 0.f;
        }
        
        Protocol::SendPackage(Address::kMotor, motorInstructionDto);
        Protocol::SendPackage(Address::kSteering, steeringInstructionDto);
    }

    String speedText(radioDto.throttle, 3);
    String steeringText(radioDto.wheel, 3);
    lcd.setCursor(0, 0);
    lcd.print(speedText.c_str());
    lcd.setCursor(8, 0);
    lcd.print(steeringText.c_str());

    Plotter::AddItem("radio.throttle", radioDto.throttle);
    Plotter::AddItem("radio.wheel", radioDto.wheel);
    Plotter::Submit();

    delay(30);
}
