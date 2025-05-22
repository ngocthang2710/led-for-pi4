adb root
adb remount
adb push out/target/product/rpi4/symbols/system/lib64/libled_aidl.so /vendor/lib64/
adb push out/target/product/rpi4/vendor/bin/my_simple_service /vendor/bin/
adb shell
cd /vendor/bin
./my_simple_service



adb push new_led.ko /vendor_dlkm 


cp rpi4-gpio23-led-overlay.dts /media/thangnn/boot/overlays/
disable_fw_kms_setup=1
dtoverlay=vc4-kms-v3d
dtoverlay=cma,cma-512
dtoverlay=rpi4-gpio23-led
dtoverlay=dht11

out/target/product/rpi4/system/priv-app/my_led_control_service_app/my_led_control_service_app.apk

echo 1 > /sys/devices/virtual/led_gpio_class/led_gpio/led
echo 0 > /sys/devices/virtual/led_gpio_class/led_gpio/led

adb shell am start -n com.mydevice.app/.MainActivity
adb logcat -s AndroidRuntime
adb logcat -s LedServiceConnection


adb shell strace -tt -f /vendor/bin/my_simple_service


adb logcat -v threadtime *:E | grep -iE "ServiceManager|my_simple_service|denied"


adb logcat | grep my_simple_service


adb shell dumpsys activity activities | grep com.mydevice.app