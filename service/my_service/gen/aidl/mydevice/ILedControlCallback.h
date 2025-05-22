#pragma once

#include <binder/IBinder.h>
#include <binder/IInterface.h>
#include <binder/Status.h>
#include <cstdint>
#include <utils/StrongPointer.h>

namespace mydevice {
class ILedControlCallback : public ::android::IInterface {
public:
  DECLARE_META_INTERFACE(LedControlCallback)
  virtual ::android::binder::Status onLedStateChanged(int32_t state) = 0;
};  // class ILedControlCallback

class ILedControlCallbackDefault : public ILedControlCallback {
public:
  ::android::IBinder* onAsBinder() override {
    return nullptr;
  }
  ::android::binder::Status onLedStateChanged(int32_t /*state*/) override {
    return ::android::binder::Status::fromStatusT(::android::UNKNOWN_TRANSACTION);
  }
};  // class ILedControlCallbackDefault
}  // namespace mydevice
