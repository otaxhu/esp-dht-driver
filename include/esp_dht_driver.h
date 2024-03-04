#ifndef __ESP_DHT_DRIVER_HEADER
#define __ESP_DHT_DRIVER_HEADER

#include <esp_err.h>
#include <stdint.h>
#include <driver/gpio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Opaque type for handlers of dht's
*/
typedef void *dht_handle_t;

/**
 * Config type to be passed to dht_new() function
 * 
 * @note Config gets copied in dht_new() function so you don't have to worry about it being on the
 *       heap or the stack
*/
typedef struct dht_config {
    gpio_num_t gpio_pin;
} dht_config_t;

/**
 * Data structure that retrieves dht_read() function
*/
typedef struct dht_data {
    struct {
        int8_t integral; // Integral part of temperature
        uint8_t decimal; // Decimal part of temperature
    } temperature;
    struct {
        int8_t integral; // Integral part of humidity
        uint8_t decimal; // Decimal part of humidity
    } humidity;
} dht_data_t;

/**
 * Creates a new instance of dht sensor.
 * 
 * @param config Pointer to structure of configuration, must not be NULL
 * @param handle Pointer to handle type, must not be NULL
 * 
 * @returns
 *  - ESP_OK on success.
 *  - ESP_ERR_INVALID_ARG if the parameters are NULL.
 *  - ESP_ERR_NO_MEM if there was no memory to allocate "handle" data structure
*/
esp_err_t dht_new(dht_config_t *config, dht_handle_t *handle);

/**
 * Frees up dht_handle_t created by dht_new() function.
 * 
 * @param handle Handle created by successful call to dht_new() function
 * 
 * @note Is undefined behaviour if "handle" was already freed up by this function
*/
void dht_delete(dht_handle_t handle);

/**
 * Reads humidity and temperature from dht sensor.
 * 
 * @param handle Handle to dht sensor, created by successful call to dht_new() function
 * @param[out] out_data Data structure to write with temperature and humidity data
 * 
 * @returns
 *  - ESP_OK on success
 *  - ESP_ERR_INVALID_ARG if the arguments are NULL
 *  - ESP_FAIL if ocurred an error reading dht sensor (data could be written to "out_data")
*/
esp_err_t dht_read(dht_handle_t handle, dht_data_t *out_data);

#ifdef __cplusplus
}
#endif
#endif
