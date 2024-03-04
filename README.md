## Driver for controlling a DHT sensor. Compatible with ESP-IDF

### Installation:

1. Initialize a project:
```sh
$ mkdir project-name
$ cd project-name
$ mkdir main
$ touch main/main.c main/CMakeLists.txt
$ mkdir components
```

2. Go inside `components` directory and clone the repository:
```sh
$ cd components
$ git clone --depth 2 https://github.com/otaxhu/esp-dht-driver
```

3. Initialize the main's `CMakeLists.txt` file contents:
```cmake
# project-name/main/CMakeLists.txt
idf_component_register(SRCS "main.c"
                       REQUIRES esp-dht-driver)
```

4. Use the functions provided in your main component or whatever component you want :D

### Documentation
For more documentation you can see the [header file](/include/esp_dht_driver.h)
