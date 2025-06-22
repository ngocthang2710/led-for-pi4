#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/platform_device.h>
#include <linux/sysfs.h>
#include <linux/device.h>

static int led_gpio_pin = -1;
static struct class *led_class;
static struct device *led_device;

/* Sysfs show: đọc trạng thái GPIO */
static ssize_t led_gpio_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    int value = gpio_get_value(led_gpio_pin);
    return sprintf(buf, "%d\n", value);
}

/* Sysfs store: ghi trạng thái GPIO */
static ssize_t led_gpio_store(struct device *dev, struct device_attribute *attr,
                              const char *buf, size_t count)
{
    int value;
    if (sscanf(buf, "%d", &value) != 1)
        return -EINVAL;

    gpio_set_value(led_gpio_pin, !!value);  // Bật/tắt LED
    return count;
}

static DEVICE_ATTR(led, S_IRUGO | S_IWUSR, led_gpio_show, led_gpio_store);

/* Hàm probe: gọi khi device tree match */
static int led_gpio_probe(struct platform_device *pdev)
{
    int ret;

    pr_info("led-gpio: Probing LED driver\n");

    // Đọc GPIO từ device tree
    led_gpio_pin = of_get_named_gpio(pdev->dev.of_node, "gpios", 0);
    if (!gpio_is_valid(led_gpio_pin)) {
        dev_err(&pdev->dev, "Invalid GPIO pin from DT\n");
        return -EINVAL;
    }

    ret = gpio_request(led_gpio_pin, "led_gpio");
    if (ret) {
        dev_err(&pdev->dev, "Failed to request GPIO\n");
        return ret;
    }

    ret = gpio_direction_output(led_gpio_pin, 0);
    if (ret) {
        dev_err(&pdev->dev, "Failed to set GPIO as output\n");
        gpio_free(led_gpio_pin);
        return ret;
    }

    // Tạo class + device để hiện trong /sys/class
    led_class = class_create(THIS_MODULE, "led_gpio_class");
    if (IS_ERR(led_class)) {
        gpio_free(led_gpio_pin);
        return PTR_ERR(led_class);
    }

    led_device = device_create(led_class, NULL, 0, NULL, "led_gpio");
    if (IS_ERR(led_device)) {
        class_destroy(led_class);
        gpio_free(led_gpio_pin);
        return PTR_ERR(led_device);
    }

    // Gắn sysfs attribute
    ret = device_create_file(led_device, &dev_attr_led);
    if (ret) {
        device_destroy(led_class, 0);
        class_destroy(led_class);
        gpio_free(led_gpio_pin);
        return ret;
    }

    pr_info("led-gpio: Probe successful\n");
    return 0;
}

/* Hàm remove: gỡ bỏ khi rmmod */
static int led_gpio_remove(struct platform_device *pdev)
{
    device_remove_file(led_device, &dev_attr_led);
    device_destroy(led_class, 0);
    class_destroy(led_class);
    gpio_free(led_gpio_pin);

    pr_info("led-gpio: Removed driver\n");
    return 0;
}

/* Khai báo compatible name dùng trong device tree */
static const struct of_device_id led_gpio_of_match[] = {
    { .compatible = "led-gpio-device" },
    {},
};
MODULE_DEVICE_TABLE(of, led_gpio_of_match);

/* Cấu trúc platform_driver */
static struct platform_driver led_gpio_driver = {
    .probe = led_gpio_probe,
    .remove = led_gpio_remove,
    .driver = {
        .name = "led_gpio_driver",
        .of_match_table = led_gpio_of_match,
    },
};

module_platform_driver(led_gpio_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("LED GPIO Platform Driver using Device Tree");
