/*
open terminal
enter command
save data in logfiles
*/

switch(flightState){
        case IDLE:
            if(terminal input == tracking){
                rfSend(radio, payload, role)
                flightState = Tracking;
            }
        break;
        case Tracking:
            rfReceive();
            writetofile()
            if(terminal input == stop){
                rfSend(radio, payload, role)
                flightState = STOP;
            }
        break;
        case STOP:
            //stop routine
    }


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
    ///////////////////char input = getchar();
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
            //Change to sdWrite Function
            fr = f_mount(&fs, "0:", 1);
            fr = f_open(&fil, filename, FA_WRITE | FA_CREATE_ALWAYS);
            ret = f_printf(&fil, "Hello\r\n");
            fr = f_close(&fil);
            if(rfReceive(radio, payload, role) == stop){
                flightState = STOP;
            }
            sleep_ms(250);
        break;
        case STOP:
            //Change to sdWrite Function
            fr = f_mount(&fs, "0:", 1);
            fr = f_open(&fil, filename, FA_WRITE | FA_CREATE_ALWAYS);
            ret = f_printf(&fil, "Hello\r\n");
            fr = f_close(&fil);
            f_unmount("0:");
            rfSend(radio, payload, role);
    }
}