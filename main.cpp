/* Flight Code
States: IDLE, Tracking, STOP
IDLE: wait for Start Tracking-Command from Host
Tracking: read all Data from BMP280&MPU6050
STOP: write Data to SD-Card when Stop Tracking-Command is received

/*
Access via Serial Monitor or 2.4GHz

Modules
- Switch
- Buttons 1&2
- LEDs 1&2
- Buzzer
- Servo 1&2&3
- BMP280
- MPU 6050
- SD
- NRF24L01
- Pyros 1&2
*/


#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h" // printf(), sleep_ms(), getchar_timeout_us(), to_us_since_boot(), get_absolute_time()
#include "hardware/i2c.h"
#include "./lib/BMP280/bmp280.h"
#include "./lib/MPU6050/mpu6050.h"
#include "pico/binary_info.h"
#include "./src/bus_init.cpp"
#include "pico/bootrom.h" // reset_usb_boot()
#include <tusb.h>         // tud_cdc_connected()
#include <RF24.h>         // RF24 radio object
#include "defaultPins.h"  // board presumptive default pin numbers for CE_PIN and CSN_PIN
#include <stdio.h>
#include "pico/stdlib.h"
#include "sd_card.h"
#include "ff.h"



int main() {

    int sensorReadRate = 400;
    int32_t sensorData;
    enum flightStates {IDLE, Tracking, STOP};
    int flightState;
    char filename[] = "flightData.txt";
    FRESULT fr;
    FATFS fs;
    FIL fil;
    int ret;
    char buf[100];
    bool role = false; // true = TX role, false = RX role
    char payload[200];
    char tracking[] = "Tracking";
    char stop[] = "Stop";

    //Basic Inits
    stdio_init_all();
    i2cInit();
    //spiInit();

    //Create Module Instances
    BMP280 bmp280;
    MPU6050 mpu6050;
    RF24 radio(CE_PIN, CSN_PIN);

    //Reset Modules
    //bmp280.reset(); needed?
    mpu6050.reset();
    
    //Init Modules
    bmp280.init(); //include in getData Function
    //mpu6050.init(); why none needed?
    sd_init_driver();

    //Get Sensor Data
    bmp280.getData();
    //mpu6050.getData();


    switch(flightState){
        case IDLE:
            if(rfReceive(radio, payload, role) == tracking){
                flightState = Tracking;
            }
        break;
        case Tracking:
            sensorData = bmp280.pressure;
            rfSend(radio, payload, role);
            //flashWrite(sensorData)
            if(rfReceive(radio, payload, role) == stop){
                flightState = STOP;
            }
            sleep_ms(250);
        break;
        case STOP:
            //flightData = flashRead();
            //Change to sdWrite Function
            fr = f_mount(&fs, "0:", 1);
            fr = f_open(&fil, filename, FA_WRITE | FA_CREATE_ALWAYS);
            ret = f_printf(&fil, "Hello\r\n");
            fr = f_close(&fil);
            f_unmount("0:");
            rfSend(radio, payload, role);
    }
}

//printf("Pressure = %.3f kPa\n", bmp280.pressure / 1000.f);
//printf("Temp. = %.2f C\n", bmp280.temperature / 100.f);

// Note this is chip temperature.
//printf("Temp. = %f\n", (mpu6050.rawTemp / 340.0) + 36.53);
// These are the raw numbers from the chip, so will need tweaking to be really useful.
// See the datasheet for more information
//printf("Acc. X = %d, Y = %d, Z = %d\n", mpu6050.rawAccel[0], mpu6050.rawAccel[1], mpu6050.rawAccel[2]);
//printf("Gyro. X = %d, Y = %d, Z = %d\n", mpu6050.rawGyro[0], mpu6050.rawGyro[1], mpu6050.rawGyro[2]);


// reset to bootloader - RF24: why is this needed?
//radio.powerDown();
//reset_usb_boot(0, 0);







bool setup(RF24 radio, char payload)
{
    uint8_t address[][6] = {"1Node", "2Node"};
    bool radioNumber = 1; // 0 uses address[0] to transmit, 1 uses address[1] to transmit
    while (!tud_cdc_connected()) {
        sleep_ms(10);
    }
    radio.begin();
    radio.setPALevel(RF24_PA_LOW); // RF24_PA_MAX is default.
    radio.setPayloadSize(sizeof(payload)); // float datatype occupies 4 bytes
    radio.openWritingPipe(address[radioNumber]);
    radio.openReadingPipe(1, address[!radioNumber]); // using pipe 1
    radio.startListening(); // put radio in RX mode
    return true;
}


char* rfReceive(RF24 radio, char* payload, bool role){
    // Become the RX node
        role = false;
        radio.startListening();
    
    // This device is a RX node

        uint8_t pipe;
        if (radio.available(&pipe)) {               // is there a payload? get the pipe number that recieved it
            uint8_t bytes = radio.getPayloadSize(); // get the size of the payload
            radio.read(&payload, bytes);            // fetch payload from FIFO

        }
    
    return payload;
}


int rfSend(RF24 radio, char* payload, bool role){
    // Become the TX node
        role = true;
        radio.stopListening();
        
    // This device is a TX node
    uint64_t start_timer = to_us_since_boot(get_absolute_time()); // start the timer
    bool report = radio.write(&payload, sizeof(payload));         // transmit & save the report
    uint64_t end_timer = to_us_since_boot(get_absolute_time());   // end the timer
}

