package mydevice;

import mydevice.ILedControlCallback;

interface ILedControlService {
    void registerCallback(in ILedControlCallback callback);
    void unregisterCallback(in ILedControlCallback callback);
    void setLedState(int state);
}
