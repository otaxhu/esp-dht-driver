idf_component_register(SRCS "src/esp_dht.c"
                       INCLUDE_DIRS "include"
                       REQUIRES driver
                       PRIV_REQUIRES esp_timer)
