#define LOG_TAG "my_simple_service"

#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <utils/Log.h>
#include <thread>
#include <chrono>

#include "MyLedControlService.h"

using namespace android;
using namespace mydevice;

int main() {
    ALOGI("my_simple_service: Starting MyLedControlService...");

    // Lấy ServiceManager
    sp<IServiceManager> sm = defaultServiceManager();
    if (sm == nullptr) {
        ALOGE("my_simple_service: Failed to get defaultServiceManager");
        return 1;
    }

    // Tạo service instance
    sp<IBinder> service = new MyLedControlService();

    // Đăng ký service thủ công và log status trả về
    status_t status = sm->addService(String16("my.ledcontrol"), service);
    if (status != OK) {
        ALOGE("my_simple_service: Failed to register service, status=%d", status);
        return 1;
    }

    ALOGI("my_simple_service: Service registered successfully.");

    // Chạy IPC thread pool
    ProcessState::self()->startThreadPool();
    IPCThreadState::self()->joinThreadPool();

    return 0;
}
