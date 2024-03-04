#include <esp_dht_driver.h>
#include <esp_timer.h>
#include <rom/ets_sys.h>

struct handle_impl {
    int64_t last_time_read;
    dht_data_t last_data;
    dht_config_t config;
};

struct dht22_handle {};

esp_err_t dht_new(dht_config_t *config, dht_handle_t *handle) {
    if (config == NULL || handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    switch (config->type) {
    case DHT_TYPE_11:
        *handle = malloc(sizeof(struct handle_impl));
        if (*handle == NULL) {
            return ESP_ERR_NO_MEM;
        }
        struct handle_impl *h_11 = (struct handle_impl *)(*handle);
        h_11->last_time_read = -2000000;
        h_11->config = *config;
        break;
    case DHT_TYPE_22:
        break;
    default:
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

esp_err_t dht_delete(dht_handle_t handle) {
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    struct handle_impl *h = (struct handle_impl *)handle;
    switch (h->config.type) {
    case DHT_TYPE_11:
    case DHT_TYPE_22:
        free(h);
        break;
    default:
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

// Function that can be used to wait the response signal after sending the start signal and
// get the bits data from the dht
//
// Returns microseconds waited succesfully or -1 if there was an error
static inline int16_t wait_response(int gpio_pin, int16_t micro_seconds, uint8_t level) {
    int16_t micros_res = 0;
    while (gpio_get_level(gpio_pin) == level) {
        if (micros_res++ > micro_seconds) {
            return -1;
        }
        ets_delay_us(1);
    }
    return micros_res;
}

esp_err_t dht_read(dht_handle_t handle, dht_data_t *out_data) {
    if (handle == NULL || out_data == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    int64_t temp_time = esp_timer_get_time();
    // Waiting one second so dht can startup
    if (temp_time < 1000000) {
        ets_delay_us(1000000 - temp_time);
        temp_time = esp_timer_get_time();
    }
    struct handle_impl *h = (struct handle_impl *)handle;
    if (h->last_time_read > temp_time - 2000000) {
        *out_data = h->last_data;
        return ESP_OK;
    }
    h->last_time_read = temp_time;

    // Send start signal
    gpio_set_direction(h->config.gpio_pin, GPIO_MODE_OUTPUT);
    gpio_set_level(h->config.gpio_pin, 0);
    ets_delay_us(20000);
    gpio_set_level(h->config.gpio_pin, 1);
    ets_delay_us(40);
    gpio_set_direction(h->config.gpio_pin, GPIO_MODE_INPUT);

    // Waiting the response signal
    if (wait_response(h->config.gpio_pin, 80, 0) == -1) {
        return ESP_FAIL;
    }
    if (wait_response(h->config.gpio_pin, 80, 1) == -1) {
        return ESP_FAIL;
    }

    uint8_t data[5] = { 0 };

    // 40 iterations for 40 bits
    for (uint8_t i = 0; i < 40; i++) {
        // Waiting 50us the low level voltage step
        if (wait_response(h->config.gpio_pin, 50, 0) == -1) {
            return ESP_FAIL;
        }

        // Getting the bit data from the high level voltage step
        int16_t bit_data = wait_response(h->config.gpio_pin, 70, 1);
        if (bit_data == -1) {
            return ESP_FAIL;
        }
        // Received 1 bit
        if (bit_data <= 27) {
            if (i < 8) {
                // humidity integral
                data[0] |= (1 << (7 - i));
            } else if (i < 16) {
                // humidity decimal
                data[1] |= (1 << (7 - (i - 8)));
            } else if (i < 24) {
                // temperature integral
                data[2] |= (1 << (7 - (i - 16)));
            } else if (i < 32) {
                // temperature decimal
                data[3] |= (1 << (7 - (i - 24)));
            } else {
                // checksum
                data[4] |= (1 << (7 - (i - 32)));
            }
        }
    }

    // Checksum
    if (data[4] != (data[0] + data[1] + data[2] + data[3])) {
        return ESP_FAIL;
    }

    out_data->humidity.integral = data[0];
    out_data->humidity.decimal = data[1];
    out_data->temperature.integral = data[2];
    out_data->temperature.decimal = data[3];

    return ESP_OK;
}
