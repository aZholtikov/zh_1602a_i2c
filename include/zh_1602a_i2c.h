#pragma once

#include "zh_pcf8574.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Initializes the LCD 1602A.
     *
     * @param[in] handle Pointer to unique PCF8574 handle.
     *
     * @return ESP_OK if success or an error code otherwise.
     */
    esp_err_t zh_1602a_init(zh_pcf8574_handle_t *handle);

    /**
     * @brief Clears the LCD screen.
     *
     * @param[in] handle Pointer to unique PCF8574 handle.
     * 
     * @return ESP_OK if success or an error code otherwise.
     */
    esp_err_t zh_1602a_lcd_clear(zh_pcf8574_handle_t *handle);

    /**
     * @brief Sets the cursor to a specific position on the LCD.
     *
     * @param[in] handle Pointer to unique PCF8574 handle.
     * @param[in] row The row number (0 or 1).
     * @param[in] col The column number (0–15).
     *
     * @return ESP_OK if success or an error code otherwise.
     */
    esp_err_t zh_1602a_set_cursor(zh_pcf8574_handle_t *handle, uint8_t row, uint8_t col);

    /**
     * @brief Prints a string to the LCD.
     *
     * @param[in] handle Pointer to unique PCF8574 handle.
     * @param[in] str Pointer to the string to be displayed.
     *
     * @return ESP_OK if success or an error code otherwise.
     */
    esp_err_t zh_1602a_print_char(zh_pcf8574_handle_t *handle, const char *str);

    /**
     * @brief Prints an integer to the LCD.
     *
     * @param[in] handle Pointer to unique PCF8574 handle.
     * @param[in] num The integer to be displayed.
     *
     * @return ESP_OK if success or an error code otherwise..
     */
    esp_err_t zh_1602a_print_int(zh_pcf8574_handle_t *handle, int num);

    /**
     * @brief Prints a floating-point number to the LCD.
     *
     * @param[in] handle Pointer to unique PCF8574 handle.
     * @param[in] num The floating-point number to be displayed.
     * @param[in] precision The number of decimal places to display.
     *
     * @return ESP_OK if success or an error code otherwise.
     */
    esp_err_t zh_1602a_print_float(zh_pcf8574_handle_t *handle, float num, uint8_t precision);

    /**
     * @brief Displays a progress bar on a specific row of the LCD.
     *
     * @param[in] handle Pointer to unique PCF8574 handle.
     * @param[in] row The row number (0 or 1).
     * @param[in] progress The progress percentage (0–100).
     *
     * @return ESP_OK if success or an error code otherwise.
     */
    esp_err_t zh_1602a_print_progress_bar(zh_pcf8574_handle_t *handle, uint8_t row, uint8_t progress);

    /**
     * @brief Clears a specific row on the LCD.
     *
     * @param[in] handle Pointer to unique PCF8574 handle.
     * @param[in] row The row number (0 or 1).
     *
     * @return ESP_OK if success or an error code otherwise.
     */
    esp_err_t zh_1602a_clear_row(zh_pcf8574_handle_t *handle, uint8_t row);

#ifdef __cplusplus
}
#endif