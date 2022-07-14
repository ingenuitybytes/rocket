// Example Code
#include <stdio.h>
#include "pico/stdlib.h"
#include "./lib/BMP280/bmp280.h"

int main() {
    stdio_init_all();
/*
#if !defined(i2c_default) || !defined(PICO_DEFAULT_I2C_SDA_PIN) || !defined(PICO_DEFAULT_I2C_SCL_PIN)
    #warning i2c / bmp280_i2c example requires a board with I2C pins
        puts("Default I2C pins were not defined");
#else
    // useful information for picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));
    bi_decl(bi_program_description("BMP280 I2C example for the Raspberry Pi Pico"));

    printf("Hello, BMP280! Reading temperaure and pressure values from sensor...\n");

    // I2C is "open drain", pull ups to keep signal high when no data is being sent
    i2c_init(i2c_default, 100 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

    // configure BMP280
    bmp280_init();

    // retrieve fixed compensation params
    struct bmp280_calib_param params;
    bmp280_get_calib_params(&params);

    int32_t raw_temperature;
    int32_t raw_pressure;

    sleep_ms(250); // sleep so that data polling and register update don't collide
    while (1) {
        bmp280_read_raw(&raw_temperature, &raw_pressure);
        int32_t temperature = bmp280_convert_temp(raw_temperature, &params);
        int32_t pressure = bmp280_convert_pressure(raw_pressure, raw_temperature, &params);
        printf("Pressure = %.3f kPa\n", pressure / 1000.f);
        printf("Temp. = %.2f C\n", temperature / 100.f);
        // poll every 500ms
        sleep_ms(500);
    }

#endif
*/
printf("Hello World");
    return 0;
}





//#include <stdio.h>
//#include <iostream>
//#include "pico/stdlib.h"

/*
States: IDLE, Tracking, STOP
IDLE: wait for Start Tracking-Command from Host
Tracking: read all Data from BMP280&MPU6050
STOP: write Data to SD-Card when Stop Tracking-Command is received


//Sensor Read Rate = 100Hz

//enum state {IDLE, Tracking, STOP};
*/

//using namespace std;


    
    /*
    sensor_data = bmp_data & mpu_data

    IDLE
        rf_send(sensor_data)
        if(rf_receive = "Tracking"){
            state = Tracking
        }

    Tracking
        rf_send(sensor_data)
        flash_write(sensor_data)
        if(rf_receive = "STOP"){
            state = STOP
        }
    
    STOP
        data = flash_data()
        sd_write(data)
        rf_send("Tracking stopped")
    

    */
