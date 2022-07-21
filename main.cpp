#include <stdio.h>

#include "./lib/BMP280/bmp280.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"


int main() {
    stdio_init_all();


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

    BMP280 bmp280;
    bool success = bmp280.init();

    /*

    for(int i = 0; i < 10; i++){
        bmp280.get_data();
        printf("%d: %.3f", bmp280.temperature);
        printf(" - %.3f", bmp280.pressure);
        sleep_ms(500);
    }
    */
#endif

    while(1){
        printf("Hello World!");
    }

    return 0;
}






/*
Testing via Serial Monitor & 2.4GHz
1. Select Serial Monitor or 2.4GHz
2. Enter Module to Test

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

/*
#include <stdio.h>
#include "pico/stdlib.h"

int main() {
    char input;
    bool mode;

    stdio_init_all();

    printf("Select between Serial or 2.4GHz communication");
    input = getchar();
    mode = input ? 0 : 1;

        printf("Hello, world!\n");
        sleep_ms(1000);

    return 0;
}

*/











/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*
#include "pico/stdlib.h"

int main() {
    const uint GREEN = 14;
    const uint RED = 15;
    gpio_init(GREEN);
    gpio_init(RED);
    gpio_set_dir(GREEN, GPIO_OUT);
    gpio_set_dir(RED, GPIO_OUT);
    while (true) {
        gpio_put(GREEN, 1);
        gpio_put(RED, 0);
        sleep_ms(500);
        gpio_put(GREEN, 0);
        gpio_put(RED, 1);
        sleep_ms(500);
    }
}
*/

//RF24-Test
/*
 * See documentation at https://nRF24.github.io/RF24
 * See License information at root directory of this library
 * Author: Brendan Doherty (2bndy5)
 */

/**
 * A simple example of sending data from 1 nRF24L01 transceiver to another.
 *
 * This example was written to be used on 2 devices acting as "nodes".
 * Use the Serial Terminal to change each node's behavior.
 */

/*
#include "pico/stdlib.h"  // printf(), sleep_ms(), getchar_timeout_us(), to_us_since_boot(), get_absolute_time()
#include "pico/bootrom.h" // reset_usb_boot()
#include <tusb.h>         // tud_cdc_connected()
#include <RF24.h>         // RF24 radio object
#include "defaultPins.h"  // board presumptive default pin numbers for CE_PIN and CSN_PIN

// instantiate an object for the nRF24L01 transceiver
RF24 radio(CE_PIN, CSN_PIN);

// Used to control whether this node is sending or receiving
bool role = false; // true = TX role, false = RX role

// For this example, we'll be using a payload containing
// a single float number that will be incremented
// on every successful transmission
float payload = 0.0;

bool setup()
{
    // Let these addresses be used for the pair
    uint8_t address[][6] = {"1Node", "2Node"};
    // It is very helpful to think of an address as a path instead of as
    // an identifying device destination

    // to use different addresses on a pair of radios, we need a variable to
    // uniquely identify which address this radio will use to transmit
    bool radioNumber = 1; // 0 uses address[0] to transmit, 1 uses address[1] to transmit

    // wait here until the CDC ACM (serial port emulation) is connected
    while (!tud_cdc_connected()) {
        sleep_ms(10);
    }

    // initialize the transceiver on the SPI bus
    if (!radio.begin()) {
        printf("radio hardware is not responding!!\n");
        return false;
    }

    // print example's introductory prompt
    printf("RF24/examples_pico/gettingStarted\n");

    // To set the radioNumber via the Serial terminal on startup
    printf("Which radio is this? Enter '0' or '1'. Defaults to '0'\n");
    char input = getchar();
    radioNumber = input == 49;
    printf("radioNumber = %d\n", (int)radioNumber);

    // Set the PA Level low to try preventing power supply related problems
    // because these examples are likely run with nodes in close proximity to
    // each other.
    radio.setPALevel(RF24_PA_LOW); // RF24_PA_MAX is default.

    // save on transmission time by setting the radio to only transmit the
    // number of bytes we need to transmit a float
    radio.setPayloadSize(sizeof(payload)); // float datatype occupies 4 bytes

    // set the TX address of the RX node into the TX pipe
    radio.openWritingPipe(address[radioNumber]); // always uses pipe 0

    // set the RX address of the TX node into a RX pipe
    radio.openReadingPipe(1, address[!radioNumber]); // using pipe 1

    // additional setup specific to the node's role
    if (role) {
        radio.stopListening(); // put radio in TX mode
    }
    else {
        radio.startListening(); // put radio in RX mode
    }

    // For debugging info
    // radio.printDetails();       // (smaller) function that prints raw register values
    // radio.printPrettyDetails(); // (larger) function that prints human readable data

    // role variable is hardcoded to RX behavior, inform the user of this
    printf("*** PRESS 'T' to begin transmitting to the other node\n");

    return true;
} // setup

void loop()
{
    if (role) {
        // This device is a TX node

        uint64_t start_timer = to_us_since_boot(get_absolute_time()); // start the timer
        bool report = radio.write(&payload, sizeof(payload));         // transmit & save the report
        uint64_t end_timer = to_us_since_boot(get_absolute_time());   // end the timer

        if (report) {
            // payload was delivered; print the payload sent & the timer result
            printf("Transmission successful! Time to transmit = %llu us. Sent: %f\n", end_timer - start_timer, payload);

            // increment float payload
            payload += 0.01;
        }
        else {
            // payload was not delivered
            printf("Transmission failed or timed out\n");
        }

        // to make this example readable in the serial terminal
        sleep_ms(1000); // slow transmissions down by 1 second
    }
    else {
        // This device is a RX node

        uint8_t pipe;
        if (radio.available(&pipe)) {               // is there a payload? get the pipe number that recieved it
            uint8_t bytes = radio.getPayloadSize(); // get the size of the payload
            radio.read(&payload, bytes);            // fetch payload from FIFO

            // print the size of the payload, the pipe number, payload's value
            printf("Received %d bytes on pipe %d: %f\n", bytes, pipe, payload);
        }
    } // role

    char input = getchar_timeout_us(0); // get char from buffer for user input
    if (input != PICO_ERROR_TIMEOUT) {
        // change the role via the serial terminal

        if ((input == 'T' || input == 't') && !role) {
            // Become the TX node

            role = true;
            printf("*** CHANGING TO TRANSMIT ROLE -- PRESS 'R' TO SWITCH BACK\n");
            radio.stopListening();
        }
        else if ((input == 'R' || input == 'r') && role) {
            // Become the RX node

            role = false;
            printf("*** CHANGING TO RECEIVE ROLE -- PRESS 'T' TO SWITCH BACK\n");
            radio.startListening();
        }
        else if (input == 'b' || input == 'B') {
            // reset to bootloader
            radio.powerDown();
            reset_usb_boot(0, 0);
        }
    }
} // loop

int main()
{
    stdio_init_all(); // init necessary IO for the RP2040

    while (!setup()) { // if radio.begin() failed
        // hold program in infinite attempts to initialize radio
    }
    while (true) {
        loop();
    }
    return 0; // we will never reach this
}
*/






//SD-Test
/*
#include <stdio.h>
#include "pico/stdlib.h"
#include "sd_card.h"
#include "ff.h"

int main() {

    FRESULT fr;
    FATFS fs;
    FIL fil;
    int ret;
    char buf[100];
    char filename[] = "test02.txt";

    // Initialize chosen serial port
    stdio_init_all();

    // Wait for user to press 'enter' to continue
    printf("\r\nSD card test. Press 'enter' to start.\r\n");
    while (true) {
        buf[0] = getchar();
        if ((buf[0] == '\r') || (buf[0] == '\n')) {
            break;
        }
    }

    // Initialize SD card
    if (!sd_init_driver()) {
        printf("ERROR: Could not initialize SD card\r\n");
        while (true);
    }

    // Mount drive
    fr = f_mount(&fs, "0:", 1);
    if (fr != FR_OK) {
        printf("ERROR: Could not mount filesystem (%d)\r\n", fr);
        while (true);
    }

    // Open file for writing ()
    fr = f_open(&fil, filename, FA_WRITE | FA_CREATE_ALWAYS);
    if (fr != FR_OK) {
        printf("ERROR: Could not open file (%d)\r\n", fr);
        while (true);
    }

    // Write something to file
    ret = f_printf(&fil, "This is another test\r\n");
    if (ret < 0) {
        printf("ERROR: Could not write to file (%d)\r\n", ret);
        f_close(&fil);
        while (true);
    }
    ret = f_printf(&fil, "of writing to an SD card.\r\n");
    if (ret < 0) {
        printf("ERROR: Could not write to file (%d)\r\n", ret);
        f_close(&fil);
        while (true);
    }

    // Close file
    fr = f_close(&fil);
    if (fr != FR_OK) {
        printf("ERROR: Could not close file (%d)\r\n", fr);
        while (true);
    }

    // Open file for reading
    fr = f_open(&fil, filename, FA_READ);
    if (fr != FR_OK) {
        printf("ERROR: Could not open file (%d)\r\n", fr);
        while (true);
    }

    // Print every line in file over serial
    printf("Reading from file '%s':\r\n", filename);
    printf("---\r\n");
    while (f_gets(buf, sizeof(buf), &fil)) {
        printf(buf);
    }
    printf("\r\n---\r\n");

    // Close file
    fr = f_close(&fil);
    if (fr != FR_OK) {
        printf("ERROR: Could not close file (%d)\r\n", fr);
        while (true);
    }

    // Unmount drive
    f_unmount("0:");

    // Loop forever doing nothing
    while (true) {
        sleep_ms(1000);
    }
}

*/








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
