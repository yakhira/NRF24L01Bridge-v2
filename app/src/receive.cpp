#include <ctime>      
#include <iostream>   
#include <string>     
#include <time.h>
#include <RF24/RF24.h>

using namespace std;

void receive(RF24 radio, uint8_t *rx_address);

int main(int argc, char** argv)
{
    uint16_t spi_dev = atoi(getenv("SPI_DEV"));
    uint16_t ce = atoi(getenv("CE_PIN"));
    char *rx_address = getenv("RX_ADDRESS");

    uint8_t _rx_address[6];

    memcpy(_rx_address, rx_address, 6);

    RF24 radio(ce, spi_dev);

    if (!radio.begin()) {
        cout << "radio hardware is not responding!!" << endl;
        return 0;
    }

    radio.setPayloadSize(32);
    radio.setPALevel(RF24_PA_LOW);
    receive(radio, _rx_address);
    return 0;
}

void receive(RF24 radio, uint8_t *rx_address)
{
    radio.openReadingPipe(1, rx_address); 

    uint8_t bytes = radio.getPayloadSize();
    char payload[bytes];

    radio.startListening(); 
    radio.printDetails();

    cout << "----START----" << endl;

    while (true) {
        uint8_t pipe;
        if (radio.available(&pipe)) {
            radio.read(&payload, bytes);
            cout << payload << endl;
        }
    }
}