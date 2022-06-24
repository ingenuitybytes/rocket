/*#include <stdio.h>
#include <iostream>
#include "pico/stdlib.h"

/*
States: IDLE, Tracking, STOP
IDLE: wait for Start Tracking-Command from Host
Tracking: read all Data from BMP280&MPU6050
STOP: write Data to SD-Card when Stop Tracking-Command is received


//Sensor Read Rate = 100Hz

//enum state {IDLE, Tracking, STOP};


using namespace std;

int main() {
    stdio_init_all();

    

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
    

    return 0;
}
*/

