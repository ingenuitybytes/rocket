#include <stdio.h>
#include <iostream>
#include "pico/stdlib.h"

#define GPIO_ON 1
#define GPIO_OFF 0
#define LED_PIN 25

using namespace std;

int main() {
    stdio_init_all();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    
    while (true) {
        cout << "hi" << endl;
//        printf("Hello, world!\n");
        gpio_put(LED_PIN, GPIO_ON);
        sleep_ms(500);
        gpio_put(LED_PIN, GPIO_OFF);
        sleep_ms(500);
    }
    return 0;
}