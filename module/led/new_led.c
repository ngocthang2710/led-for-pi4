#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>

#define LED_GPIO_PIN 23  // GPIO pin số 23 trên Raspberry Pi

static struct class *led_class;
static struct device *led_device;

static int __init led_gpio_init(void)
{
    int ret;

    pr_info("Initializing LED GPIO Driver\n");

    // Khởi tạo GPIO
    ret = gpio_request(LED_GPIO_PIN, "led_gpio");
    if (ret) {
        pr_err("Failed to request GPIO pin: %d\n", LED_GPIO_PIN);
        return ret;
    }

    // Set GPIO làm output
    ret = gpio_direction_output(LED_GPIO_PIN, 0); // Tắt LED lúc khởi tạo
    if (ret) {
        pr_err("Failed to set GPIO direction\n");
        gpio_free(LED_GPIO_PIN);
        return ret;
    }

    // Tạo class cho LED device
    led_class = class_create(THIS_MODULE, "led_gpio_class");
    if (IS_ERR(led_class)) {
        pr_err("Failed to create class for LED device\n");
        gpio_free(LED_GPIO_PIN);
        return PTR_ERR(led_class);
    }

    // Tạo device cho LED trong sysfs
    led_device = device_create(led_class, NULL, 0, NULL, "led_gpio");
    if (IS_ERR(led_device)) {
        pr_err("Failed to create device for LED in sysfs\n");
        class_destroy(led_class);
        gpio_free(LED_GPIO_PIN);
        return PTR_ERR(led_device);
    }

    pr_info("LED GPIO Driver Initialized\n");

    return 0;
}

static void __exit led_gpio_exit(void)
{
    pr_info("Exiting LED GPIO Driver\n");

    // Xóa device trong sysfs
    device_destroy(led_class, 0);
    class_destroy(led_class);

    // Giải phóng GPIO
    gpio_free(LED_GPIO_PIN);

    pr_info("LED GPIO Driver Unloaded\n");
}

static ssize_t led_gpio_store(struct device *dev, struct device_attribute *attr,
                              const char *buf, size_t count)
{
    int value;

    // Đọc giá trị từ user space
    if (sscanf(buf, "%d", &value) != 1)
        return -EINVAL;

    // Set giá trị GPIO tùy theo giá trị nhận được từ user space
    if (value)
        gpio_set_value(LED_GPIO_PIN, 1);  // Bật LED
    else
        gpio_set_value(LED_GPIO_PIN, 0);  // Tắt LED

    return count;
}

static ssize_t led_gpio_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    // Trả về trạng thái của GPIO (LED đang bật hay tắt)
    int value = gpio_get_value(LED_GPIO_PIN);
    return sprintf(buf, "%d\n", value);
}

static DEVICE_ATTR(led, S_IWUSR | S_IRUGO, led_gpio_show, led_gpio_store);

static int __init led_gpio_driver_init(void)
{
    int ret;

    // Khởi tạo driver LED GPIO
    ret = led_gpio_init();
    if (ret)
        return ret;

    // Tạo file sysfs cho việc điều khiển LED
    ret = device_create_file(led_device, &dev_attr_led);
    if (ret) {
        pr_err("Failed to create sysfs entry for LED\n");
        led_gpio_exit();
        return ret;
    }

    return 0;
}

static void __exit led_gpio_driver_exit(void)
{
    // Xóa file sysfs
    device_remove_file(led_device, &dev_attr_led);

    // Thoát driver
    led_gpio_exit();
}

module_init(led_gpio_driver_init);
module_exit(led_gpio_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("LED GPIO Driver for Raspberry Pi");
