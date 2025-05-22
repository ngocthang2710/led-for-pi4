#pragma once

#include <binder/IBinder.h>
#include <binder/IInterface.h>
#include <binder/Status.h>
#include <cstdint>
#include <mydevice/ILedControlCallback.h>
#include <utils/StrongPointer.h>

namespace mydevice {
class ILedControlService : public ::android::IInterface {
public:
  DECLARE_META_INTERFACE(LedControlService)
  virtual ::android::binder::Status registerCallback(const ::android::sp<::mydevice::ILedControlCallback>& callback) = 0;
  virtual ::android::binder::Status unregisterCallback(const ::android::sp<::mydevice::ILedControlCallback>& callback) = 0;
  virtual ::android::binder::Status setLedState(int32_t state) = 0;
};  // class ILedControlService

class ILedControlServiceDefault : public ILedControlService {
public:
  ::android::IBinder* onAsBinder() override {
    return nullptr;
  }
  ::android::binder::Status registerCallback(const ::android::sp<::mydevice::ILedControlCallback>& /*callback*/) override {
    return ::android::binder::Status::fromStatusT(::android::UNKNOWN_TRANSACTION);
  }
  ::android::binder::Status unregisterCallback(const ::android::sp<::mydevice::ILedControlCallback>& /*callback*/) override {
    return ::android::binder::Status::fromStatusT(::android::UNKNOWN_TRANSACTION);
  }
  ::android::binder::Status setLedState(int32_t /*state*/) override {
    return ::android::binder::Status::fromStatusT(::android::UNKNOWN_TRANSACTION);
  }
};  // class ILedControlServiceDefault
}  // namespace mydevice
