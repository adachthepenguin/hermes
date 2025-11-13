#pragma once

#include <Wire.h>

namespace Protocol {
    // Version 2.0.0
    struct Radio {
        float wheel;
        float throttle;
    };

    struct MotorInstruction {
        float throttle;
    };

    struct SteeringInstruction {
        float steering;
    };

    template <typename T>
    void SendPackage(const T& protocol) {
        byte protocolBytes[sizeof(T)];
        memcpy(protocolBytes, &protocol, sizeof(T));
        for (int i = 0; i < sizeof(T); ++i) {
            Wire.write(protocolBytes[i]);
        }
    }

    template <typename T>
    int SendPackage(int address, const T& protocol) {
        Wire.beginTransmission(address);
        SendPackage(protocol);
        int result =  Wire.endTransmission();
        #ifdef PROTOCOL_DEBUGGING
            if (result != 0) {
                Serial.print("Failed to send ");
                Serial.print(sizeof(T));
                Serial.print("B to address ");
                Serial.println(address);
            }
        #endif
        return result;
    }

    template <typename T>
    bool ReadPackage(T& protocol) {
        if (Wire.available() == sizeof(T)) {
            byte protocolData[sizeof(T)];
            for (int i = 0; i < sizeof(T); ++i) {
                protocolData[i] = Wire.read();
            }
            memcpy(&protocol, protocolData, sizeof(T));
            return true;
        } else {
            return false;
        }
    }

    template <typename T>
    bool RequestPackage(int address, T& protocol) {
        #ifdef PROTOCOL_DEBUGGING
            Serial.print("Requesting ");
            Serial.print(sizeof(T));
            Serial.print("B from address ");
            Serial.print(address);
        #endif
        Wire.requestFrom(address, static_cast<int>(sizeof(T)));
        return ReadPackage(protocol);
    }
} // namespace Protocol
