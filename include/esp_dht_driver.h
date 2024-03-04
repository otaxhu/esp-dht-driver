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
 * Type of dht
*/
typedef enum {
    DHT_TYPE_11,
    DHT_TYPE_22
} dht_type_t;

/**
 * Config type to be passed to dht_new() function
 * 
 * @note Config gets copied in dht_new() function so you don't have to worry about it being on the
 *       heap or the stack
*/
typedef struct dht_config {
    gpio_num_t gpio_pin;
    dht_type_t type;
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
 * Creates a new instance of dht sensor, 
*/
esp_err_t dht_new(dht_config_t *config, dht_handle_t *handle);

esp_err_t dht_delete(dht_handle_t handle);

esp_err_t dht_read(dht_handle_t handle, dht_data_t *out_data);

#ifdef __cplusplus
}
#endif
#endif
