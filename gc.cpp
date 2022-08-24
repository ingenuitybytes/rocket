/*
Code Debugging Problems:
User Cant RF Send&Receive --> getchar() blocking rf communication
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


//void core1_entry();
void sdWrite(char* filename, char* data);
bool rfSetup(RF24& radio, bool radioName, float payload);
void rfSend(RF24& radio, float payload);
float rfReceive(RF24& radio, float payload);
//static char * getLine(bool fullDuplex = true, char lineBreak = '\n');


//GC
int main() {

    int32_t sensorData;
    enum flightStates {IDLE, TRACKING, STOP};
    flightStates flightState = IDLE;
    char filename[] = "flightData.txt";
    char command = '3';
    float payload = 0.0;
    bool radioNumber = 0;

    //Use Dual Core
    //multicore_launch_core1(core1_entry);

    //Basic Inits
    stdio_init_all();
    i2cInit();

    //Create Module Instances
    RF24 radio(CE_PIN, CSN_PIN);
    
    //Init Modules
    while(!rfSetup(radio, radioNumber, payload)){}

    sleep_ms(5000);

    while(true) {
        printf("Enter a number: ");
        command = getchar();//multicore_fifo_pop_blocking();
        printf("\n");

        switch (flightState) {
            case IDLE:
                printf("0");
                if(command == '1'){
                    payload = 1.0;
                    rfSend(radio, payload);
                    flightState = TRACKING;
                }
                break;
                sleep_ms(100);

            case TRACKING:
                printf("1");

                for(int i=0; i<10; i++){
                    payload = rfReceive(radio, payload);
                    printf("%f", payload);
                    sleep_ms(1200);
                }

                //fileWrite();
                if(command == '2'){
                    payload = 2.0;
                    rfSend(radio, payload);
                    flightState = STOP;
                }
                sleep_ms(1000);

            case STOP:
                printf("2\n");
                if(command == '0'){
                    payload = 0.0;
                    rfSend(radio, payload);
                    flightState = IDLE;
                }   
                sleep_ms(1000);  
        }
    }
}


/*
void core1_entry() {
    while(true){
        char *pLine = getLine(true, '\r');
        std::string instruction = pLine;
        multicore_fifo_push_blocking(pLine);
        free(pLine); // dont forget freeing buffer !!
    }
}*/



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
            printf("Transmission successful! Time to transmit = %llu us. Sent: %f\n", end_timer - start_timer, payload);
        }
        else {
            // payload was not delivered
            printf("Transmission failed or timed out\n");
        }
        sleep_ms(1000); // slow transmissions down by 1 second
}

float rfReceive(RF24& radio, float payload){
    // Become the RX node
    radio.startListening();
    
    // This device is a RX node

        uint8_t pipe;
        if (radio.available(&pipe)) {  
            printf("\nPayload available!\n");    
            // is there a payload? get the pipe number that received it
            uint8_t bytes = radio.getPayloadSize(); // get the size of the payload
            radio.read(&payload, bytes);            // fetch payload from FIFO

            // print the size of the payload, the pipe number, payload's value
            printf("Received %d bytes on pipe %d: %f\n", bytes, pipe, payload);

            return payload;
        }
        else{  
            return 0.0;
        }
    
}


/*
static char * getLine(bool fullDuplex, char lineBreak) {
    const uint startLineLength = 8; // the linebuffer will automatically grow for longer lines
    const char eof = 255;           // EOF in stdio.h -is -1, but getchar returns int 255 to avoid blocking

    // th line buffer
    // will allocated by pico_malloc module if <cstdlib> gets included
    char * pStart = (char*)malloc(startLineLength); 
    char * pPos = pStart;  // next character position
    size_t maxLen = startLineLength; // current max buffer size
    size_t len = maxLen; // current max length
    int c;

    if(!pStart) {
        return NULL; // out of memory or dysfunctional heap
    }

    while(1) {
        c = getchar(); // expect next character entry
        if(c == eof || c == lineBreak) {
            break;     // non blocking exit
        }

        if (fullDuplex) {
            putchar(c); // echo for fullDuplex terminals
        }

        if(--len == 0) { // allow larger buffer
            len = maxLen;
            // double the current line buffer size
            char *pNew  = (char*)realloc(pStart, maxLen *= 2);
            if(!pNew) {
                free(pStart);
                return NULL; // out of memory abort
            }
            // fix pointer for new buffer
            pPos = pNew + (pPos - pStart);
            pStart = pNew;
        }

        // stop reading if lineBreak character entered 
        if((*pPos++ = c) == lineBreak) {
            break;
        }
    }

    *pPos = '\0';   // set string end mark
    return pStart;
}
*/


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