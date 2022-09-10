/*
Code Debugging Problems:
FC stuck when writing to SD Card --> rfSetup is a problem when mounting sd card (probably because SPI Pin mismatch or misconfiguration)
*/



/*Concerns
one 3 color led
MPU6050 not accurate but BNO055 too expensive
right clock speed
PFW or PWM mode
use external storage chip
continuity check for pyros
Later maybe one core for calculation and one for communication
pico examples sources: pwm-servos/led, adc-battery meter
*/

/*
Flight Code
States: IDLE, TRACKING, STOP
IDLE: wait for Start TRACKING-Command from Host
TRACKING: read all Data from BMP280&MPU6050
STOP: write Data to SD-Card when Stop TRACKING-Command is received

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
#include <iostream>
#include <fstream>
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
#include "sd_card.h"
#include "ff.h"
#include "pico/multicore.h"
#include <cstdlib>          // need this include, if you need heap management (malloc, realloc, free)
#include "hardware/gpio.h"
#include "hardware/adc.h"

void sdWrite(char* filename, char* data);
bool rfSetup(RF24& radio, bool radioName, float payload);
void rfSend(RF24& radio, float payload);
float rfReceive(RF24& radio, float payload);
void core1_entry();

static void sd_spi_deselect() {
    gpio_put(17, 1);
    LED_OFF();
    /*
    MMC/SDC enables/disables the DO output in synchronising to the SCLK. This
    means there is a posibility of bus conflict with MMC/SDC and another SPI
    slave that shares an SPI bus. Therefore to make MMC/SDC release the MISO
    line, the master device needs to send a byte after the CS signal is
    deasserted.
    */
    uint8_t fill = SPI_FILL_CHAR;
    #define 	spi0   ((spi_inst_t *)spi0_hw)
    spi_write_blocking(spi0, &fill, 1);
}

//FC

int main() {
    enum flightStates {
        IDLE, 
        TRACKING, 
        STOP
    };
    flightStates flightState = TRACKING;
    
    int sensorReadRate = 400; //Hz
    char sensorData[10];
    bool radioNumber = 1;
    char filename[] = "flightData.txt";
    float payload = 0.0;

    //Basic Inits
    stdio_init_all();
    i2cInit();

    //Create Module Instances
    BMP280 bmp280;
    MPU6050 mpu6050;
    RF24 radio(CE_PIN, CSN_PIN);

    //Reset Modules
    mpu6050.reset();
    
    //Init Modules
    bmp280.init(); //include in getData Function

    while(!rfSetup(radio, radioNumber, payload)){}
    sd_init_driver();
    
    //Use Dual Core
    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);
    multicore_launch_core1(core1_entry);

    sleep_ms(5000);
    
    while(true) {
        switch (flightState) {
            case IDLE:
                printf("IDLE\n");
                payload = rfReceive(radio, payload);
                if(payload == 1){
                    flightState = TRACKING;
                }
                sleep_ms(100);
                break;

            case TRACKING:
                //printf("TRACKING\n");
                
                bmp280.getData();
                mpu6050.getData();
                //printf("Pressure = %.3f kPa\n", 26.687);//bmp280.pressure / 1000.f);
                //printf("Acc. X = %d, Y = %d, Z = %d\n", mpu6050.rawAccel[0], mpu6050.rawAccel[1], mpu6050.rawAccel[2]);
                sprintf(sensorData, "%d", bmp280.pressure);
                //printf("\n---%s---\n", sensorData);
                sdWrite(filename, (char*)"fw");
                //sleep_ms(1000);
                payload = (float)bmp280.pressure;
                rfSend(radio, payload);
                
                payload = rfReceive(radio, payload);
                if(payload == 2) {
                    flightState = STOP;
                }
                //sleep_ms(1000);
                break;

            case STOP:
                printf("STOP\n");
                payload = rfReceive(radio, payload);
                if(payload == 0) {
                    flightState = IDLE;
                }

                sleep_ms(1000);
                break;
            default:
                break;
        }
    }
}

////////////////////////////////////////////////////////////////


void core1_entry() {
    while(true){
        const float conversion_factor = 3.3f / (1 << 12);
        uint16_t result = adc_read();
        printf("%f\n", result, result * conversion_factor);
    }
}


//Change to bool later on, to check if writing to file was successful
void sdWrite(char* filename, char* data){
    FRESULT fr;
    FATFS fs;
    FIL fil;
    int ret;
    char buf[128];
    fr = f_mount(&fs, "0:", 1);
    fr = f_open(&fil, filename, FA_OPEN_APPEND | FA_WRITE);
    ret = f_printf(&fil, data, buf);
    fr = f_close(&fil);
    f_unmount("0:");
}

bool rfSetup(RF24& radio, bool radioNumber, float payload){
    uint8_t address[][6] = {"1Node", "2Node"};
    while (!tud_cdc_connected()) {
        sleep_ms(10);
    }
    if (!radio.begin()) {
        printf("radio hardware is not responding!!\n");
        return false;
    }
    radio.setPALevel(RF24_PA_LOW); // RF24_PA_MAX is default.
    radio.setPayloadSize(sizeof(payload)); // float datatype occupies 4 bytes
    radio.openWritingPipe(address[radioNumber]);
    radio.openReadingPipe(1, address[!radioNumber]); // using pipe 1
    radio.startListening(); // put radio in RX mode
    return true;
}

void rfSend(RF24& radio, float payload){
    // Become the TX node
    radio.stopListening();
        
    // This device is a TX node
    uint64_t start_timer = to_us_since_boot(get_absolute_time()); // start the timer
    bool report = radio.write(&payload, sizeof(payload));         // transmit & save the report
    uint64_t end_timer = to_us_since_boot(get_absolute_time());   // end the timer



    if (report) {
            // payload was delivered; print the payload sent & the timer result
            //printf("Transmission successful! Time to transmit = %llu us. Sent: %f\n", end_timer - start_timer, payload);
        }
        else {
            // payload was not delivered
            //printf("Transmission failed or timed out\n");
        }
        sleep_ms(1000); // slow transmissions down by 1 second
}

float rfReceive(RF24& radio, float payload){
    // Become the RX node
    radio.startListening();
    
    // This device is a RX node

        uint8_t pipe;
        if (radio.available(&pipe)) {      
            // is there a payload? get the pipe number that recieved it
            uint8_t bytes = radio.getPayloadSize(); // get the size of the payload
            radio.read(&payload, bytes);            // fetch payload from FIFO

            // print the size of the payload, the pipe number, payload's value
            //printf("Received %d bytes on pipe %d: %f\n", bytes, pipe, payload);

            return payload;
        }
        else{
            return 0.0;
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