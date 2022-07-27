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
States: IDLE, Tracking, STOP
IDLE: wait for Start Tracking-Command from Host
Tracking: read all Data from BMP280&MPU6050
STOP: write Data to SD-Card when Stop Tracking-Command is received

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
#include "sd_card.h"
#include "ff.h"

void sdWrite(char* filename, char* data);
bool rfSetup(RF24 radio, char* payload);
char* rfReceive(RF24 radio, char* payload);
bool rfSend(RF24 radio, char* payload);

//FC
int main() {

    int sensorReadRate = 400;
    char sensorData[200];
    //enum flightStates {IDLE, Tracking, STOP};
    int flightState = 0;
    char filename[] = "flightData.txt";
    //char payload[200];
    //char tracking[] = "Tracking";
    //char stop[] = "Stop";
    int i = 0;
    int j = 0;
    char command[100];

    //Basic Inits
    stdio_init_all();
    i2cInit();
    //spiInit();

    //Find better way for entering strings
    /*
    j = 0;
    while(1){
        i = 0;
        do{
            command[i] = getchar();
            i++;
        }while(command[i-1] != 0x0D);
        command[i+1] = '\0';
        printf("%d%s", j, command);
        j++;
    }*/

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
    //rfSetup(radio, payload);
    
    //Replace with switch and enum
    while(1){
        printf("-----\n");
        printf("%d\n", flightState);
        if(flightState == 0){
            printf("Hi\n");

            if(i == 100){
                flightState = 1;
            }
            //printf("%d", i);
            i++;
        }
        sleep_ms(100);

        if(flightState == 1){
            bmp280.getData();
            mpu6050.getData();
            printf("Pressure = %.3f kPa\n", bmp280.pressure / 1000.f);
            // add convert to string
            //data = sensorData;
            rfSend(radio, payload);
            sprintf(sensorData, "%d", bmp280.pressure);
            sdWrite(filename, sensorData);
            //strcpy(command, rfReceive(radio));
            if(j == 10){
                flightState = 2;
            }
            //if(rfReceive(radio, payload) == stop){
            //    flightState = STOP;
            //}
            sleep_ms((int)1000/sensorReadRate);
            //printf("%d", j);
            j++;
        }

        if(flightState == 2){
            //sdWrite(filename, sensorData);
            //rfSend(radio, payload);
            break;
        }
    }
}

/*
    switch(flightState){
        case 0:
            //if(rfReceive(radio, payload) == tracking){
            //    flightState = Tracking;
            //}

            printf("Hi\n");

            if(i == 100){
                flightState = 1;
            }
            printf("%d", i);
            i++;
        break;
    }
    */

/*case 1:
            bmp280.getData();
            mpu6050.getData();
            sensorData = bmp280.pressure;
            // add convert to string
            //data = sensorData;
            //rfSend(radio, payload);
            sdWrite(filename, sensorData);
            //strcpy(command, rfReceive(radio));
            if(j == 10){
                flightState = STOP;
            }
            //if(rfReceive(radio, payload) == stop){
            //    flightState = STOP;
            //}
            sleep_ms((int)1000/sensorReadRate);
            printf("%d", j);
            j++;
        break;
        case 2:
            //sdWrite(filename, sensorData);
            //rfSend(radio, payload);
            break;
        */

////////////////////////////////////////////////////////////////

/*
//User
int main() {

    int32_t sensorData;
    enum flightStates {IDLE, Tracking, STOP};
    int flightState;
    char filename[] = "flightData.txt";
    char payload[200];
    char tracking[] = "Tracking";
    char stop[] = "Stop";
    char data[200];

    //Basic Inits
    stdio_init_all();
    i2cInit();
    //spiInit();

    //Create Module Instances
    RF24 radio(CE_PIN, CSN_PIN);
    
    //Init Modules
    rfSetup(radio, payload);

    switch(flightState){
        case IDLE:
            if(getchar() == 1){
                rfSend(radio, payload)
                flightState = Tracking;
            }
        break;
        case Tracking:
            strcpy(data, rfReceive(radio, payload))
            fileWrite();
            if(getchar() == 2){
                rfSend(radio, payload)
                flightState = STOP;
            }
        break;
        case STOP:
        break;
    }
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





//Change to bool later on, to check if writing to file was successful
void sdWrite(char* filename, char* data){
    FRESULT fr;
    FATFS fs;
    FIL fil;
    int ret;
    char buf[100];
    fr = f_mount(&fs, "0:", 1);
    fr = f_open(&fil, filename, FA_WRITE | FA_CREATE_ALWAYS);
    ret = f_printf(&fil, data);
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


char* rfReceive(RF24 radio, char* payload){
    // Become the RX node
    radio.startListening();
    
    // This device is a RX node

        uint8_t pipe;
        if (radio.available(&pipe)) {               // is there a payload? get the pipe number that recieved it
            uint8_t bytes = radio.getPayloadSize(); // get the size of the payload
            radio.read(&payload, bytes);            // fetch payload from FIFO

        }
    
    return payload;
}


bool rfSend(RF24 radio, char* payload){
    // Become the TX node
    radio.stopListening();
        
    // This device is a TX node
    //uint64_t start_timer = to_us_since_boot(get_absolute_time()); // start the timer
    bool report = radio.write(&payload, sizeof(payload));         // transmit & save the report
    //uint64_t end_timer = to_us_since_boot(get_absolute_time());   // end the timer
    return report;
}
