#ifndef MYDEVICE_MYLED_CONTROL_SERVICE_H
#define MYDEVICE_MYLED_CONTROL_SERVICE_H

#include <binder/IInterface.h>
#include <utils/Log.h>
#include <vector>
#include <mutex>
#include <fstream>

#include "mydevice/BnLedControlService.h"
#include "mydevice/ILedControlCallback.h"

namespace mydevice {

class MyLedControlService : public mydevice::BnLedControlService {
public:
    MyLedControlService();

    // Đăng ký callback để nhận sự kiện LED thay đổi
    ::android::binder::Status registerCallback(const android::sp<ILedControlCallback>& callback) override;

    // Hủy đăng ký callback
    ::android::binder::Status unregisterCallback(const android::sp<ILedControlCallback>& callback) override;

    // Đặt trạng thái LED
    ::android::binder::Status setLedState(int32_t state) override;

private:
    std::vector<android::sp<ILedControlCallback>> callbacks;
    std::mutex callbackMutex;

    void notifyCallbacks(bool state);
};

} // namespace mydevice

#endif // MYDEVICE_MYLED_CONTROL_SERVICE_H
