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
#include "sd_card.h"
#include "ff.h"


//Change to bool later on, to check if writing to file was successful
void sdWrite(char* filename, char* data){
    FRESULT fr;
    FATFS fs;
    FIL fil;
    int ret;
    char buf[100];

    fr = f_mount(&fs, "0:", 1);
    fr = f_open(&fil, filename, FA_WRITE | FA_CREATE_ALWAYS);
    ret = f_printf(&fil, "Hello\r\n");
    fr = f_close(&fil);
    f_unmount("0:");
}


bool rfSetup(RF24 radio, char* payload){
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
