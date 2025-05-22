#include "MyLedControlService.h"

#include <binder/IServiceManager.h>
#include <utils/Log.h>
#include <utils/StrongPointer.h>
#include <fstream>
#include <mutex>
#include <algorithm> // std::remove_if

using android::status_t;

namespace mydevice {

MyLedControlService::MyLedControlService() {
    ALOGI("MyLedControlService created");
}

// Đăng ký callback để nhận sự kiện LED thay đổi
::android::binder::Status MyLedControlService::registerCallback(const android::sp<ILedControlCallback>& callback) {
    std::lock_guard<std::mutex> lock(callbackMutex);
    for (const auto& cb : callbacks) {
        if (IInterface::asBinder(cb) == IInterface::asBinder(callback)) {
            ALOGI("Callback already registered");
            return ::android::binder::Status::ok();
        }
    }
    callbacks.push_back(callback);
    ALOGI("Callback registered, total: %zu", callbacks.size());
    return ::android::binder::Status::ok();
}

// Hủy đăng ký callback
::android::binder::Status MyLedControlService::unregisterCallback(const android::sp<ILedControlCallback>& callback) {
    std::lock_guard<std::mutex> lock(callbackMutex);
    auto it = std::remove_if(callbacks.begin(), callbacks.end(),
        [&](const android::sp<ILedControlCallback>& cb) {
            return IInterface::asBinder(cb) == IInterface::asBinder(callback);
        });

    if (it != callbacks.end()) {
        callbacks.erase(it, callbacks.end());
        ALOGI("Callback unregistered, remaining: %zu", callbacks.size());
    } else {
        ALOGI("Callback not found");
    }

    return ::android::binder::Status::ok();
}

// Thiết lập trạng thái LED
::android::binder::Status MyLedControlService::setLedState(int32_t state) {
    std::ofstream ledFile("/sys/devices/virtual/led_gpio_class/led_gpio/led");

    if (!ledFile.is_open()) {
        ALOGE("Failed to open LED control file");
        return ::android::binder::Status::fromExceptionCode(::android::binder::Status::EX_ILLEGAL_STATE);
    }

    if (state == 1) {
        ledFile << "1";
        ALOGI("LED is turned ON");
        notifyCallbacks(true);
    } else if (state == 0) {
        ledFile << "0";
        ALOGI("LED is turned OFF");
        notifyCallbacks(false);
    } else {
        ALOGE("Invalid state value: %d", state);
        return ::android::binder::Status::fromExceptionCode(::android::binder::Status::EX_ILLEGAL_ARGUMENT);
    }

    ledFile.close();
    return ::android::binder::Status::ok();
}

// Gửi thông báo đến tất cả callback đã đăng ký
void MyLedControlService::notifyCallbacks(bool state) {
    std::lock_guard<std::mutex> lock(callbackMutex);
    for (auto it = callbacks.begin(); it != callbacks.end(); ) {
        auto cb = *it;
        auto status = cb->onLedStateChanged(state);
        if (!status.isOk()) {
            ALOGW("Callback failed, removing...");
            it = callbacks.erase(it);
        } else {
            ++it;
        }
    }
}

} // namespace mydevice
