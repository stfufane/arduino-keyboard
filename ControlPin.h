#pragma once

class ControlPin
{
    public:
        explicit ControlPin(int pin) : mPin(pin) {};
        virtual void setup() = 0;
        virtual void checkValue() = 0;
    protected:
        int mPin;
};
