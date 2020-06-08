#pragma once

class LED
{
    public:
        LED(int pin, bool state) : mPin(pin), mState(state) {}
        void setup() { pinMode(mPin, OUTPUT); digitalWrite(mPin, mState); }
        void toggleLight() { mState = !mState; digitalWrite(mPin, mState); }
        int getPin() { return mPin; }
    private:
        int mPin;
        bool mState;
};
