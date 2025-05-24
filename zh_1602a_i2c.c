#include "zh_1602a_i2c.h"

static const char *TAG = "zh_1602a_i2c";

#define ZH_1602A_I2C_LOGI(msg, ...) ESP_LOGI(TAG, msg, ##__VA_ARGS__)
#define ZH_1602A_I2C_LOGW(msg, ...) ESP_LOGW(TAG, msg, ##__VA_ARGS__)
#define ZH_1602A_I2C_LOGE(msg, ...) ESP_LOGE(TAG, msg, ##__VA_ARGS__)
#define ZH_1602A_I2C_LOGE_ERR(msg, err, ...) ESP_LOGE(TAG, "[%s:%d:%s] " msg, __FILE__, __LINE__, esp_err_to_name(err), ##__VA_ARGS__)

#define ZH_1602A_I2C_CHECK(cond, err, msg, ...) \
    if (!(cond))                                \
    {                                           \
        ZH_1602A_I2C_LOGE_ERR(msg, err);        \
        return err;                             \
    }

#define LCD_1602A_PULSE                      \
    zh_pcf8574_write_gpio(handle, 2, true);  \
    vTaskDelay(10 / portTICK_PERIOD_MS);     \
    zh_pcf8574_write_gpio(handle, 2, false); \
    vTaskDelay(10 / portTICK_PERIOD_MS);

static void _zh_1602a_lcd_init(zh_pcf8574_handle_t *handle);
static void _zh_1602a_send_command(zh_pcf8574_handle_t *handle, uint8_t command);
static void _zh_1602a_send_data(zh_pcf8574_handle_t *handle, uint8_t data);

esp_err_t zh_1602a_init(zh_pcf8574_handle_t *handle)
{
    ZH_1602A_I2C_LOGI("1602A initialization started.");
    ZH_1602A_I2C_CHECK(handle != NULL, ESP_ERR_INVALID_ARG, "1602A initialization failed. Invalid argument.");
    ZH_1602A_I2C_CHECK(handle->is_initialized == true, ESP_ERR_INVALID_STATE, "1602A initialization failed. PCF8574 not initialized.");
    _zh_1602a_lcd_init(handle);
    ZH_1602A_I2C_LOGI("1602A initialization completed successfully.");
    return ESP_OK;
}

esp_err_t zh_1602a_lcd_clear(zh_pcf8574_handle_t *handle)
{
    ZH_1602A_I2C_LOGI("1602A display cleaning started.");
    ZH_1602A_I2C_CHECK(handle != NULL, ESP_ERR_INVALID_ARG, "1602A display cleaning failed. Invalid argument.");
    ZH_1602A_I2C_CHECK(handle->is_initialized == true, ESP_ERR_INVALID_STATE, "1602A display cleaning failed. PCF8574 not initialized.");
    _zh_1602a_send_command(handle, 0x01);
    ZH_1602A_I2C_LOGI("1602A display cleaning completed successfully.");
    return ESP_OK;
}

esp_err_t zh_1602a_set_cursor(zh_pcf8574_handle_t *handle, uint8_t row, uint8_t col)
{
    ZH_1602A_I2C_LOGI("1602A set cursor started.");
    ZH_1602A_I2C_CHECK(row < 2 && col < 16 && handle != NULL, ESP_ERR_INVALID_ARG, "1602A set cursor failed. Invalid argument.");
    ZH_1602A_I2C_CHECK(handle->is_initialized == true, ESP_ERR_INVALID_STATE, "1602A set cursor failed. PCF8574 not initialized.");
    uint8_t address = (row == 0) ? col : (0x40 + col);
    _zh_1602a_send_command(handle, 0x80 | address);
    ZH_1602A_I2C_LOGI("1602A set cursor completed successfully.");
    return ESP_OK;
}

esp_err_t zh_1602a_print_char(zh_pcf8574_handle_t *handle, const char *str)
{
    ZH_1602A_I2C_LOGI("1602A print char started.");
    ZH_1602A_I2C_CHECK(str != NULL && handle != NULL, ESP_ERR_INVALID_ARG, "1602A print char failed. Invalid argument.");
    ZH_1602A_I2C_CHECK(handle->is_initialized == true, ESP_ERR_INVALID_STATE, "1602A print char failed. PCF8574 not initialized.");
    while (*str != 0)
    {
        _zh_1602a_send_data(handle, (uint8_t)*str++);
    }
    ZH_1602A_I2C_LOGI("1602A print char completed successfully.");
    return ESP_OK;
}

esp_err_t zh_1602a_print_int(zh_pcf8574_handle_t *handle, int num)
{
    ZH_1602A_I2C_LOGI("1602A print int started.");
    ZH_1602A_I2C_CHECK(handle != NULL, ESP_ERR_INVALID_ARG, "1602A print int failed. Invalid argument.");
    ZH_1602A_I2C_CHECK(handle->is_initialized == true, ESP_ERR_INVALID_STATE, "1602A print int failed. PCF8574 not initialized.");
    char buffer[12];
    sprintf(buffer, "%d", num);
    zh_1602a_print_char(handle, buffer);
    ZH_1602A_I2C_LOGI("1602A print int completed successfully.");
    return ESP_OK;
}

esp_err_t zh_1602a_print_float(zh_pcf8574_handle_t *handle, float num, uint8_t precision)
{
    ZH_1602A_I2C_LOGI("1602A print float started.");
    ZH_1602A_I2C_CHECK(handle != NULL, ESP_ERR_INVALID_ARG, "1602A print float failed. Invalid argument.");
    ZH_1602A_I2C_CHECK(handle->is_initialized == true, ESP_ERR_INVALID_STATE, "1602A print float failed. PCF8574 not initialized.");
    char buffer[16];
    sprintf(buffer, "%.*f", precision, num);
    zh_1602a_print_char(handle, buffer);
    ZH_1602A_I2C_LOGI("1602A print float completed successfully.");
    return ESP_OK;
}

esp_err_t zh_1602a_print_progress_bar(zh_pcf8574_handle_t *handle, uint8_t row, uint8_t progress)
{
    ZH_1602A_I2C_LOGI("1602A print progress bar started.");
    ZH_1602A_I2C_CHECK(row < 2 && progress <= 100 && handle != NULL, ESP_ERR_INVALID_ARG, "1602A print progress bar failed. Invalid argument.");
    ZH_1602A_I2C_CHECK(handle->is_initialized == true, ESP_ERR_INVALID_STATE, "1602A print progress bar failed. PCF8574 not initialized.");
    uint8_t blocks = (progress * 16) / 100;
    zh_1602a_set_cursor(handle, row, 0);
    for (uint8_t i = 0; i < 16; ++i)
    {
        if (i < blocks)
        {
            zh_1602a_print_char(handle, "\xFF");
        }
        else
        {
            zh_1602a_print_char(handle, " ");
        }
    }
    ZH_1602A_I2C_LOGI("1602A print progress bar completed successfully.");
    return ESP_OK;
}

esp_err_t zh_1602a_clear_row(zh_pcf8574_handle_t *handle, uint8_t row)
{
    ZH_1602A_I2C_LOGI("1602A clear row started.");
    ZH_1602A_I2C_CHECK(row < 2 && handle != NULL, ESP_ERR_INVALID_ARG, "1602A clear row failed. Invalid argument.");
    ZH_1602A_I2C_CHECK(handle->is_initialized == true, ESP_ERR_INVALID_STATE, "1602A clear row failed. PCF8574 not initialized.");
    zh_1602a_set_cursor(handle, row, 0);
    for (uint8_t i = 0; i < 16; ++i)
    {
        zh_1602a_print_char(handle, " ");
    }
    zh_1602a_set_cursor(handle, row, 0);
    ZH_1602A_I2C_LOGI("1602A clear row completed successfully.");
    return ESP_OK;
}

static void _zh_1602a_lcd_init(zh_pcf8574_handle_t *handle)
{
    vTaskDelay(20 / portTICK_PERIOD_MS);
    zh_pcf8574_write(handle, 0x30);
    LCD_1602A_PULSE;
    zh_pcf8574_write(handle, 0x30);
    LCD_1602A_PULSE;
    zh_pcf8574_write(handle, 0x30);
    LCD_1602A_PULSE;
    zh_pcf8574_write(handle, 0x20);
    LCD_1602A_PULSE;
    _zh_1602a_send_command(handle, 0x28);
    _zh_1602a_send_command(handle, 0x28);
    _zh_1602a_send_command(handle, 0x28);
    _zh_1602a_send_command(handle, 0x0C);
    _zh_1602a_send_command(handle, 0x01);
    _zh_1602a_send_command(handle, 0x06);
}

static void _zh_1602a_send_command(zh_pcf8574_handle_t *handle, uint8_t command)
{
    zh_pcf8574_write(handle, (command & 0xF0));
    LCD_1602A_PULSE;
    zh_pcf8574_write(handle, command << 4);
    LCD_1602A_PULSE;
}

static void _zh_1602a_send_data(zh_pcf8574_handle_t *handle, uint8_t data)
{
    zh_pcf8574_write(handle, (data & 0xF0) | 0x01);
    LCD_1602A_PULSE;
    zh_pcf8574_write(handle, (data << 4) | 0x01);
    LCD_1602A_PULSE;
}
