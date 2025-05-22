#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <utils/Log.h>
#include <utils/RefBase.h>
#include "mydevice/ILedControlService.h"
#include "mydevice/ILedControlCallback.h"
#include <mydevice/BnLedControlCallback.h> 

using namespace android;
using mydevice::ILedControlService;
using mydevice::ILedControlCallback;
using mydevice::BnLedControlCallback;

class LedControlCallback : public BnLedControlCallback {
    public:
        android::binder::Status onLedStateChanged(int32_t state) override {
            ALOGI("LED state changed to: %d", state);
            return android::binder::Status::ok();
        }
    };

int main() {
    sp<IBinder> binder = defaultServiceManager()->getService(String16("my.ledcontrol"));
    if (binder == nullptr) {
        ALOGE("Service not found");
        return 1;
    }

    sp<ILedControlService> service = interface_cast<ILedControlService>(binder);
    if (service == nullptr) {
        ALOGE("Service cast failed");
        return 1;
    }

    sp<ILedControlCallback> callback = new LedControlCallback();
    service->registerCallback(callback);

    service->setLedState(1);
    sleep(2);
    service->setLedState(0);
    sleep(2);

    service->unregisterCallback(callback);

    return 0;
}
