# ESP32 ESP-IDF and ESP8266 RTOS SDK component for liquid crystal display module 1602(4)A via I2C connection (PCF8574)

## Tested on

1. [ESP8266 RTOS_SDK v3.4](https://docs.espressif.com/projects/esp8266-rtos-sdk/en/latest/index.html#)
2. [ESP32 ESP-IDF v5.4](https://docs.espressif.com/projects/esp-idf/en/release-v5.4/esp32/index.html)

## Features

1. Support of 16 LCD 160X on one bus.

## Connection

| 1602(4)A | PCF8574 |
| -------- | ------- |
|    RS    |   P0    |
|    E     |   P2    |
|    D4    |   P4    |
|    D5    |   P5    |
|    D6    |   P6    |
|    D7    |   P7    |

## Dependencies

1. [zh_vector](http://git.zh.com.ru/alexey.zholtikov/zh_vector)
2. [zh_pcf8574](http://git.zh.com.ru/alexey.zholtikov/zh_pcf8574)

## Using

In an existing project, run the following command to install the components:

```text
cd ../your_project/components
git clone http://git.zh.com.ru/alexey.zholtikov/zh_160x_i2c
git clone http://git.zh.com.ru/alexey.zholtikov/zh_pcf8574
git clone http://git.zh.com.ru/alexey.zholtikov/zh_vector
```

In the application, add the component:

```c
#include "zh_160x_i2c.h"
```

## Examples

One LCD on bus:

```c
#include "zh_160x_i2c.h"

#define I2C_PORT (I2C_NUM_MAX - 1)

#ifndef CONFIG_IDF_TARGET_ESP8266
i2c_master_bus_handle_t i2c_bus_handle = NULL;
#endif

zh_pcf8574_handle_t lcd_160x_handle = {0};

void app_main(void)
{
    esp_log_level_set("zh_160x_i2c", ESP_LOG_NONE); // For ESP8266 first enable "Component config -> Log output -> Enable log set level" via menuconfig.
    esp_log_level_set("zh_pcf8574", ESP_LOG_NONE);  // For ESP8266 first enable "Component config -> Log output -> Enable log set level" via menuconfig.
    esp_log_level_set("zh_vector", ESP_LOG_NONE);   // For ESP8266 first enable "Component config -> Log output -> Enable log set level" via menuconfig.
#ifdef CONFIG_IDF_TARGET_ESP8266
    i2c_config_t i2c_config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = GPIO_NUM_4, // In accordance with used chip.
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = GPIO_NUM_5, // In accordance with used chip.
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
    };
    i2c_driver_install(I2C_PORT, i2c_config.mode);
    i2c_param_config(I2C_PORT, &i2c_config);
#else
    i2c_master_bus_config_t i2c_bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_PORT,
        .scl_io_num = GPIO_NUM_22, // In accordance with used chip.
        .sda_io_num = GPIO_NUM_21, // In accordance with used chip.
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    i2c_new_master_bus(&i2c_bus_config, &i2c_bus_handle);
#endif
    zh_pcf8574_init_config_t pcf8574_init_config = ZH_PCF8574_INIT_CONFIG_DEFAULT();
#ifdef CONFIG_IDF_TARGET_ESP8266
    pcf8574_init_config.i2c_port = I2C_PORT;
#else
    pcf8574_init_config.i2c_handle = i2c_bus_handle;
#endif
    pcf8574_init_config.i2c_address = 0x27;
    zh_pcf8574_init(&pcf8574_init_config, &lcd_160x_handle);
    zh_160x_init(&lcd_160x_handle, ZH_LCD_16X2); // For LCD 16X2.
    // zh_160x_init(&lcd_160x_handle, ZH_LCD_16X4); // For LCD 16X4.
    for (;;)
    {
        zh_160x_set_cursor(&lcd_160x_handle, 0, 0);
        zh_160x_print_char(&lcd_160x_handle, "LCD 160X");
        zh_160x_set_cursor(&lcd_160x_handle, 1, 0);
        zh_160x_print_char(&lcd_160x_handle, "Hello World!");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        zh_160x_set_cursor(&lcd_160x_handle, 0, 0); // For LCD 16X2.
        // zh_160x_set_cursor(&lcd_160x_handle, 2, 0); // For LCD 16X4.
        zh_160x_print_char(&lcd_160x_handle, "Progress: ");
        for (uint8_t i = 0; i <= 100; ++i)
        {
            zh_160x_set_cursor(&lcd_160x_handle, 0, 10); // For LCD 16X2.
            // zh_160x_set_cursor(&lcd_160x_handle, 2, 10); // For LCD 16X4.
            zh_160x_print_int(&lcd_160x_handle, i);
            zh_160x_print_char(&lcd_160x_handle, "%");
            zh_160x_print_progress_bar(&lcd_160x_handle, 1, i); // For LCD 16X2.
            // zh_160x_print_progress_bar(&lcd_160x_handle, 3, i); // For LCD 16X4.
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        zh_160x_lcd_clear(&lcd_160x_handle);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}
```
