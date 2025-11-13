#pragma once

namespace Plotter {
    template <typename T>
    void AddItem(const char* name, T value) {
        Serial.print(" ");
        Serial.print(name);
        Serial.print(":");
        Serial.print(value);
    }

    void Init() {
        Serial.begin(9600);
    }
    
    void Submit() {
        Serial.println("");
    }
} // namespace Plotter
